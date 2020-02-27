#include "ShadowStrategyX52.h"
#include "SubEntity.h"
#include "H3DMaterial.h"
#include "Entity.h"
#include "SceneManager.h"
#include "SingletonManager.h"
#include "MaterialLayer.h"
#include "PrePassLight.h"
#include "def_process.h"
#include "GlobalConfig.h"
#include "MeshData.h"
#include "RenderStateNew.h"
#include "Camera.h"
#include "FxGroup.h"
#include "TextureD3D9.h"
#include "RenderTargetD3D9.h"
#include "RenderDeviceD3D9.h"
#include "GlobalShaderConst.h"
#include "RenderTargetManager.h"
#include "../postprocess/PostProcessManager.h"
#include "EntityFactory.h"
#include "Frustum.h"
#include "ShaderGenFrameWork.h"
#include "ShaderGenOperation.h"
#include "RenderPipe.h"
#include "RenderTarget.h"
#include "mmgrInclude.h"
namespace H3D
{


// 有效，但是永远不会投射出阴影的投影矩阵
// 用于shadow map texture不够用的情况，光源直接使用这个矩阵不会出现artifact
static H3DMat4 s_DummyShadowMatrix(0.0f, 0.0f, 0.0f, 0.0f,
								   0.0f, 0.0f, 0.0f, 0.0f,
								   0.0f, 0.0f, 0.0f, 0.0f,
								   0.0f, 0.0f, -1.0f, 1.0f);


static bool TestAABBInSideCullingPlanes(const AABB& aabb, const H3DVector<Plane>& planes)
{
	for (H3DVector<Plane>::const_iterator it = planes.begin();
		it != planes.end(); ++it) // for each plane
	{
		const Plane& plane = *it;
		EPlaneSide test = TestAABBPlane(aabb, plane);

		// 在某一平面外，则在外部
		if (test == PS_Negative)
		{
			return false;
		}
	}

	return true;
}

static void CalculateSplitPositions(float* splitPosition, int nSplit, float fNear, float fFar)
{
	float* pWeight = NULL;

	float weight3[] = {0.05f, 0.2f, 0.75f};
	float weight2[] = {0.2f, 0.8f};

	if (nSplit == 2)
	{
		pWeight = weight2;
	}
	else if (nSplit == 3)
	{
		pWeight = weight3;
	}
	else
	{
		assert(0);
	}

	// make sure border values are accurate
	splitPosition[0] = fNear;
	splitPosition[nSplit] = fFar;

	for(int i = 0; i < nSplit; i++)
	{
		splitPosition[i + 1] = splitPosition[i] + (fFar - fNear) * pWeight[i];
	}
}

void ShadowParameter::SetQualityLevel(int nLevel)
{
	nQualityLevel = nLevel;
	if (nLevel == SHADOW_QUALITY_HIGH)
	{
		bUseSoftShadow = 1;
		bUseCharacterShadow = 0;
		nNumPass = 2;
		nAtlasSize = GlobalConfig::GetShadowMapSize();
		nShadowMapWidth = nAtlasSize * nNumPass;
		nShadowMapHeight = nAtlasSize;	
	}

	bShadowEnable = (g_Caps.m_bSupportSampleDepthBuffer)&&GlobalConfig::GetCastShadow();

	fShadowStart = 1.f;
	fShadowEnd = GlobalConfig::GetShadowFadeoutEnd();
	fFadeoutStart = fShadowEnd*0.9f;
	CalculateSplitPositions(fSplitPos, nNumPass, fShadowStart, fShadowEnd);
	fDepthBias = GlobalConfig::GetShadowDepthBias();
	fSlopDepthBias = GlobalConfig::GetShadowSlopDepthBias();
}

ShadowStrategyX52::ShadowStrategyX52():
m_pScreenSpaceShadowRT(NULL),m_pEntitySSShadow(NULL),
m_pEntityCharacterCollecter(NULL),
m_pEntityShadowMask(NULL),m_pShadowMapColor(NULL),m_pShadowMapDepth(NULL)
{
	memset(&m_ShadowParameter, 0, sizeof(m_ShadowParameter));
	Init();
}

ShadowStrategyX52::~ShadowStrategyX52()
{
	m_pEntitySSShadow->Release();
	m_pEntitySSShadow = NULL;

	FxGroupMapItr itr=m_FxGroupMap.begin();
	while(itr!=m_FxGroupMap.end())
	{
		SingletonManager::GetSingleton().m_RenderingContex.DeleteFxGroup(itr->second);
		//delete itr->second;
		itr->second = NULL;
		++itr;
	}
	m_FxGroupMap.clear();
}

bool ShadowStrategyX52::Init()
{
	m_pEntitySSShadow = EngineLoadHelper::LoadDMLFromFile(Runtime::GetDataAssetDir()+"ScreenSpaceShadow.dml");
	assert(m_pEntitySSShadow);
	if (g_Caps.ShouldUseRawZTexture())
	{
		Renderable* pRenderable = m_pEntitySSShadow->GetSubEntityByIndex(0);
		pRenderable->GetMaterialLayer()->GetNewMaterial()->SetMacro(RAWZ, "1");
	}
	//设置软阴景级别
	if(GlobalConfig::UseSoftShadow())
	{

		SetSoftShadowLevel(1);
	}
	else
	{
		SetSoftShadowLevel(0);
	}


	//m_pEntityCharacterCollecter = EngineLoadHelper::LoadDMLFromFile( Runtime::GetDataAssetDir()+"CharacterShadowCollect.dml" );
	//assert(m_pEntityCharacterCollecter);
	//m_pEntityShadowMask = EngineLoadHelper::LoadDMLFromFile( Runtime::GetDataAssetDir()+"CharacterShadowMask.dml" );
	//assert(m_pEntityShadowMask);

	SetQualityLevel(ShadowParameter::SHADOW_QUALITY_HIGH);

	return true;
}

void ShadowStrategyX52::SetQualityLevel(int nLevel)
{
	m_ShadowParameter.SetQualityLevel(nLevel);
	//ReleaseResources();
	//AllocResources();
}

void ShadowStrategyX52::ReturnRenderTarget()
{
	if (m_pShadowMapColor)
	{
		SceneManager::GetSingleton().GetRenderTargetManager()->ReturnRenderTarget(m_pShadowMapColor);
	}
	m_pShadowMapColor = 0;

	if (m_pShadowMapDepth)
	{
		SceneManager::GetSingleton().GetRenderTargetManager()->ReturnRenderTarget(m_pShadowMapDepth);
	}
	m_pShadowMapDepth = 0;
}

/// 修改为rendertargetmanager管理，因尺寸及格式特殊，目前不会被复用
RenderTarget*	ShadowStrategyX52::GetShadowMap()
{
	if (m_pShadowMapColor == 0)
	{
		int nWidth = m_ShadowParameter.nShadowMapWidth;
		int nHeight = m_ShadowParameter.nShadowMapHeight;

		RenderTargetInfo info(nWidth, nHeight, PF_NULL); 
		if (!g_Caps.IsSupportsRenderTarget(DT_SUPPORT_NULL))
		{
			info.pfFormat = PF_ARGB8;
			if (!g_Caps.IsSupportsRenderTarget(DT_SUPPORT_ARGB8))
			{
				LogErr( "error ShadowMap: 创建color RenderTarget失败!，不支持NULL/ARGB8格式");
				//return 0;
			}
		}
		
		m_pShadowMapColor = SceneManager::GetSingleton().GetRenderTargetManager()->GetRenderTarget(info, false);
		m_pShadowMapColor->SetUseRTSizeVP(true);
		//m_pShadowMapColor->SetTextureName("PrelightingShadowMapColor");
		//LogInfo(RENDER_INFO,OutPut_File, "ShadowMap: 创建%d格式的 RenderTarget	%s	成功!", int(info.pfFormat), "PrelightingShadowMapColor");
	}
	return m_pShadowMapColor;
}

RenderTarget*	ShadowStrategyX52::GetShadowDepthBuffer()
{
	if (m_pShadowMapDepth == 0)
	{
		int nWidth = m_ShadowParameter.nShadowMapWidth;
		int nHeight = m_ShadowParameter.nShadowMapHeight;

		RenderTargetInfo info(nWidth, nHeight, g_Caps.m_DepthFormat); 
		m_pShadowMapDepth = SceneManager::GetSingleton().GetRenderTargetManager()->GetRenderTarget(info);
		//m_pShadowMapDepth->SetTextureName("PrelightingShadowMapDepth");
		//LogInfo(RENDER_INFO,OutPut_File, "ShadowMap: 创建%d格式的 RenderTarget	%s	成功!", int(info.pfFormat), "PrelightingShadowMapDepth");
	}
	return m_pShadowMapDepth;
}

void ShadowStrategyX52::AllocResources()
{
	//assert(m_pShadowMapColor == NULL);
	//assert(m_pShadowMapDepth == NULL);

	//int nWidth = m_ShadowParameter.nShadowMapWidth;
	//int nHeight = m_ShadowParameter.nShadowMapHeight;

	//// 创建rt
	//// dummy frame buffer, use intz depth buffer for depth compartion
	//m_pShadowMapColor = TextureManager::GetSingleton().CreateRenderTarget(RT_Color, H3DString("PrelightingShadowMapColor"));
	//if (m_pShadowMapColor->Create(nWidth, nHeight, 1, TEX_TYPE_2D, PF_NULL, false))
	//{
	//	LogInfo(RENDER_INFO,OutPut_File, "ShadowMap: 创建NULL RenderTarget成功!");

	//}
	//else if (m_pShadowMapColor->Create(nWidth, nHeight, 1, TEX_TYPE_2D, PF_ARGB8, false))
	//{
	//	LogInfo(RENDER_INFO,OutPut_File, "ShadowMap: 创建ARGB8 RenderTarget成功!");
	//}
	//else
	//{
	//	LogErr( "error ShadowMap: 创建color RenderTarget失败!，不支持NULL格式");
	//}
	//m_pShadowMapColor->AddRef();

	//m_pShadowMapDepth = TextureManager::GetSingleton().CreateRenderTarget(RT_Depth, H3DString("PrelightingShadowMapDepth"));
	//if (g_Caps.ShouldUseRawZTexture())
	//{
	//	m_pShadowMapDepth->Create(nWidth, nHeight, 1, TEX_TYPE_2D, PF_RAWZ, false); //for nvidia
	//}
	//else
	//{
	//	if (m_pShadowMapDepth->Create(nWidth, nHeight, 1, TEX_TYPE_2D, PF_INTZ, false))
	//	{
	//		LogInfo(RENDER_INFO,OutPut_File, "ShadowMap: 创建INTZ depth buffer成功");
	//	}
	//	else if (m_pShadowMapDepth->Create(nWidth, nHeight, 1, TEX_TYPE_2D, PF_ATI_DF24, false))
	//	{
	//		LogInfo(RENDER_INFO,OutPut_File, "ShadowMap: 创建ATI_DF24 depth buffer成功");
	//	}
	//	else
	//	{
	//		LogErr( "error ShadowMap: 创建depth buffer失败，显卡不支持RAWZ/INTZ/ATI_DF24");
	//	}
	//}
	//m_pShadowMapDepth->AddRef();
}


void ShadowStrategyX52::Cleanup()
{
	m_Lights.clear();
	m_CasterObjs.clear();
	m_CharacterSet.clear();
}

void ShadowStrategyX52::PrepareRender()
{
	/// 更新配置信息
	m_ShadowParameter.SetQualityLevel(ShadowParameter::SHADOW_QUALITY_HIGH);

	PrepareLight();

	//CullShadowCasters();

	SetupParameter();
}


void ShadowStrategyX52::SetupParameter()
{

	// 设置一些与阴影相关的shader const
	GlobalShaderConst* pManager = SingletonManager::GetSingleton().m_RenderingContex.GetGlobalShaderConstManager();

	// 全局方向光划分cascaded的距离
	float* splitPos = m_ShadowParameter.fSplitPos;
	H3DVec4 DirShadowSplitPos(splitPos[0], splitPos[1], splitPos[2], splitPos[3]);
	pManager->SetVector("ShadowSplitPosition",DirShadowSplitPos);

	// 阴影淡入的始末距离
	H3DVec4 shadowFade(m_ShadowParameter.fFadeoutStart, m_ShadowParameter.fShadowEnd, (float)m_ShadowParameter.nShadowMapWidth, (float)m_ShadowParameter.nShadowMapHeight);
	pManager->SetVector("ShadowParam2",shadowFade);
}

bool ShadowStrategyX52::NeedRenderShadowmap()
{
	return true;
	//uint32 frame = SceneManager::GetSingleton().GetFrameCount();
	//if (frame % 2 == 0)
	//{
	//	return true;
	//}
	//else
	//{
	//	return false;
	//}
}

int ShadowStrategyX52::Render()
{
	PrepareRender();

	RenderSystem* pRender = SceneManager::GetSingleton().GetRenderSystem();

	//保存原来的viewPort，设置新的VP
	m_SavedVP = SceneManager::GetSingleton().GetRenderSystem()->GetViewPort();

	// 对于每个投射阴影的光源，绘制其对应的shadow map atlas
	int nDrawedAtlas = 0;
	for (H3DVector<PrePassLight*>::iterator it = m_Lights.begin(); it != m_Lights.end(); ++it)
	{
		PrePassLight* pLight = *it;

		if (pLight->GetLightType() == LIGHT_Directional)
		{
			if (nDrawedAtlas == 0)
			{
				// 隔一帧投射一次shadow map
				if (NeedRenderShadowmap())
				{
					nDrawedAtlas += RenderLightImplDirectional(nDrawedAtlas, (PrePassLightDirectional*)pLight);
				}
				CollectSceneShadow(pLight, m_dirProjMatrix);
			}
		}
		else if (pLight->GetLightType() == LIGHT_Spot)
		{
			nDrawedAtlas += RenderLightImplSpot(nDrawedAtlas, (PrePassLightSpot*)pLight);
		}
	}

	//if (m_ShadowParameter.bUseCharacterShadow)
	//{
	//	P_BEGIN(456);
	//	RenderCharacterShadow();
	//	P_END(456);
	//}

	//pRender->SetViewPort(m_SavedVP);

	// 将ScreenSpaceShadow Buffer作为贴图待用
	GlobalShaderConst* pManager = SingletonManager::GetSingleton().m_RenderingContex.GetGlobalShaderConstManager();
	pManager->SetTexture("ScreenSpaceShadowRT0", ((RenderTargetD3D9*)m_pScreenSpaceShadowRT));

	Cleanup();
	return 1;
}



H3DMat4 ShadowStrategyX52::CalcTexProjMatrix(const H3DMat4& matProj, int nAtlas)
{
	// setup unproject matrix
	float scale = 1.0f / (float)m_ShadowParameter.nNumPass;
	float scaleX = 0.5f * scale;

	// 计算贴图空间的位移
	// x : 当前atlas位置 + 半个像素
	float offsetX = scale * nAtlas + 0.5f * scale + 0.5f / (float)(m_ShadowParameter.nShadowMapWidth);
	// y : 移动到贴图空间 + 半个像素
	float offsetY = 0.5f + 0.5f / (float)m_ShadowParameter.nShadowMapHeight;								
	H3DMat4 matScale(scaleX,	0.0f,	0.0f,	0.0f,
		0.0f,	-0.5f,	0.0f,	0.0f,
		0.0f,	0.0f,	1.0f,	0.0f,
		offsetX,	offsetY,	0.0f,	1.0f);

	return matProj * matScale;
}


void ShadowStrategyX52::RenderOneAtlas(PrePassLight* pLight, const H3DVector<Plane>& cullingPlanes)
{
	// 剔除测试所有的renderable，通过测试的激活cache，然后渲染
	H3DVector<PrePassLight*>::iterator itPrinLight = m_PrincipalLight.begin();
	for (H3DVector<Renderable*>::iterator it = m_CasterObjs.begin(); 
		it != m_CasterObjs.end(); ++it, ++itPrinLight)
	{
		Renderable* pRenderable = *it;

		if (/**itPrinLight == pLight && */TestAABBInSideCullingPlanes(pRenderable->GetParent()->GetWorldAABB(), cullingPlanes))
		{
			ActiveRenderableToPass(pRenderable);
		}
	}

	RenderRenderable();
}

void ShadowStrategyX52::CullShadowCasters()
{
	// 根据阴影淡出距离再对投射物体进行一次剔除
	Camera* pCamera = SceneManager::GetSingleton().GetRenderCamera();
	H3DVec3 dir = -pCamera->GetDirection();
	H3DVec3 eyePos = pCamera->GetWorldPosition();

	Plane plane;
	plane.m_Normal = dir;
	plane.m_fDistance = -(eyePos.Dot(dir) - m_ShadowParameter.fShadowEnd);

	H3DVector<Renderable*> casters;
	casters.reserve(m_CasterObjs.size());
	for (H3DVector<Renderable*>::iterator it = m_CasterObjs.begin(); it != m_CasterObjs.end(); ++it)
	{
		Renderable* pRenderable = *it;
		EPlaneSide test = TestAABBPlane(pRenderable->GetParent()->GetWorldAABB(), plane);
		// 在某一平面外，则在外部
		if (test != PS_Negative)
		{
			casters.push_back(pRenderable);
		}
	}
	if (casters.size() < m_CasterObjs.size())
	{
		casters.swap(m_CasterObjs);
	}
}

void ShadowStrategyX52::PrepareLight()
{
	// 做一些初始化工作
	for (H3DVector<PrePassLight*>::iterator it = m_Lights.begin(); it != m_Lights.end(); ++it)
	{
		PrePassLight* pLight = *it;

		if (pLight->GetLightType() == LIGHT_Directional)
		{
			// 初始化为缺省投影矩阵
			for (int i = 0; i < 3; i++)
			{
				pLight->SetShadowMapProjMatrix(i, s_DummyShadowMatrix);
			}
		}
		else if (pLight->GetLightType() == LIGHT_Spot)
		{
			// 初始化为缺省投影矩阵
			pLight->SetShadowMapProjMatrix(0, s_DummyShadowMatrix);
		}
	}


	// 计算每个物体接受到的，优先级最高的光源
	m_PrincipalLight.resize(m_CasterObjs.size());
	for (H3DVector<PrePassLight*>::iterator it = m_PrincipalLight.begin(); it != m_PrincipalLight.end(); ++it)
	{
		(*it) = NULL;
	}

	// 只保留一个优先级最高的方向光
	{
		int nHighPriority = -1;
		PrePassLight* pCasterLight = NULL;
		for (H3DVector<PrePassLight*>::iterator itLight = m_Lights.begin(); itLight != m_Lights.end(); ++itLight)
		{
			PrePassLight* pLight = *itLight;

			if (pLight->GetLightType() == LIGHT_Directional && pLight->GetPriority() > nHighPriority)
			{
				pCasterLight = pLight;
				nHighPriority = pLight->GetPriority();
			}
		}

		m_Lights.clear();
		if (pCasterLight != NULL)
		{
			m_Lights.push_back(pCasterLight);
		}
	}

	//for (H3DVector<PrePassLight*>::iterator itLight = m_Lights.begin(); itLight != m_Lights.end(); ++itLight)
	//{
	//	PrePassLight* pLight = *itLight;

	//	int nCasterIndex = 0;
	//	for (H3DVector<Renderable*>::iterator it = m_CasterObjs.begin(); it != m_CasterObjs.end(); ++it)
	//	{
	//		Renderable* pRenderable = *it;

	//		// 光源与物体的剔除测试,有剔除平面和光源自身剔除两种方法，都要进行测试
	//		// 目前方向光实现了剔除平面，聚光灯实现了自身的剔除算法
	//		if (pLight->TestAABB(pRenderable->GetWorldAABB()) != IT_Outside)
	//		{
	//			if (m_PrincipalLight[nCasterIndex] == NULL ||
	//				m_PrincipalLight[nCasterIndex]->GetPriority() < pLight->GetPriority())
	//			{
	//				m_PrincipalLight[nCasterIndex] = pLight;
	//			}
	//		}

	//		nCasterIndex++;
	//	}
	//}
}


/////////////////////////////////////////////////////////////////////////////
// 方向光阴影实现


// 计算frustum的一个slice，返回八个顶点及其aabb
static void CalculateSplitFrustum(/*out*/ H3DVec3* pPos, /*out*/ AABB& aabb, float fNear, float fFar, Camera* pCamera)
{
	//float fNear = m_SplitPosition[nSplite];
	//float fFar = m_SplitPosition[nSplite + 1];

	H3DVec3 eyePos = pCamera->GetWorldPosition();

	H3DVec3 vZ = pCamera->GetDirection();
	H3DVec3 vX = pCamera->GetRight();
	H3DVec3 vY = pCamera->GetUp();


	float fAspect = pCamera->GetAspect();
	float fFov = D3DXToRadian(pCamera->GetFovInDegree());
	float tanHalfFov = tanf(fFov * 0.5f);

	float fNearPlaneHalfHeight = tanHalfFov * fNear;
	float fNearPlaneHalfWidth = fNearPlaneHalfHeight * fAspect;

	float fFarPlaneHalfHeight = tanHalfFov * fFar;
	float fFarPlaneHalfWidth = fFarPlaneHalfHeight * fAspect;

	H3DVec3 vNearPlaneCenter = eyePos + vZ * fNear;
	H3DVec3 vFarPlaneCenter = eyePos + vZ * fFar;

	pPos[0] = H3DVec3(vNearPlaneCenter - vX*fNearPlaneHalfWidth - vY*fNearPlaneHalfHeight);
	pPos[1] = H3DVec3(vNearPlaneCenter - vX*fNearPlaneHalfWidth + vY*fNearPlaneHalfHeight);
	pPos[2] = H3DVec3(vNearPlaneCenter + vX*fNearPlaneHalfWidth + vY*fNearPlaneHalfHeight);
	pPos[3] = H3DVec3(vNearPlaneCenter + vX*fNearPlaneHalfWidth - vY*fNearPlaneHalfHeight);

	pPos[4] = H3DVec3(vFarPlaneCenter - vX*fFarPlaneHalfWidth - vY*fFarPlaneHalfHeight);
	pPos[5] = H3DVec3(vFarPlaneCenter - vX*fFarPlaneHalfWidth + vY*fFarPlaneHalfHeight);
	pPos[6] = H3DVec3(vFarPlaneCenter + vX*fFarPlaneHalfWidth + vY*fFarPlaneHalfHeight);
	pPos[7] = H3DVec3(vFarPlaneCenter + vX*fFarPlaneHalfWidth - vY*fFarPlaneHalfHeight);

	aabb.SetNull(true);
	for (int i = 0; i < 8; i++)
	{
		aabb.MergeWithPoint(pPos[i]);
	}
}

static void SmoothShadowFlickering(H3DVec3& projMin, H3DVec3& projMax, int shadowmapSize)
{
	float unitPerPixel = (projMax.x - projMin.x) / (float)shadowmapSize;
	projMin /= unitPerPixel;
	projMin.x = H3DMath::Floor(projMin.x);
	projMin.y = H3DMath::Floor(projMin.y);
	projMin *= unitPerPixel;

	projMax /= unitPerPixel;
	projMax.x = H3DMath::Floor(projMax.x);
	projMax.y = H3DMath::Floor(projMax.y);
	projMax *= unitPerPixel;
}

static void CalcLightSpaceViewPort(H3DVec3& projMin, H3DVec3& projMax, const H3DVec3* pCorners, int nNumPos,
								   const H3DVec3& lispRight, const H3DVec3& lispUp)
{
	// project caster's aabb to light space
	projMin.Set(H3DMath::INFINITY, H3DMath::INFINITY, H3DMath::INFINITY);
	projMax.Set(-H3DMath::INFINITY, -H3DMath::INFINITY, -H3DMath::INFINITY);

	for (int i = 0; i < nNumPos; i++)
	{
		float proj = 0.0f;
		proj = pCorners[i].Dot(lispRight);
		if (proj < projMin.x)
			projMin.x = proj;

		if (proj > projMax.x)
			projMax.x = proj;

		proj = pCorners[i].Dot(lispUp);
		if (proj < projMin.y)
			projMin.y = proj;

		if (proj > projMax.y)
			projMax.y = proj;
	}

	// 将投影空间变为正方形
	H3DVec3 projSize = projMax - projMin;
	float projViewSize = 0.f;
	if (projSize.x > projSize.y)
	{
		float halfSize = (projSize.x - projSize.y) * 0.5f;
		projMin.y -= halfSize;
		projMax.y += halfSize;
		projViewSize = projSize.x;
	}
	else
	{
		float halfSize = (projSize.y - projSize.x) * 0.5f;
		projMin.x -= halfSize;
		projMax.x += halfSize;
		projViewSize = projSize.y;
	}

	H3DVec3 vDiagonal = pCorners[0] - pCorners[6];
	float fCascadeBound = vDiagonal.Length();
	vDiagonal = pCorners[4] - pCorners[6];
	if (vDiagonal.Length() > fCascadeBound)
	{
		fCascadeBound = vDiagonal.Length();
	}

	float fBoderOffset = (fCascadeBound - projViewSize)* 0.5f;
	assert(fBoderOffset >= 0.f);

	H3DVec3 vBoarderOffset(fBoderOffset, fBoderOffset, 0.f);
	projMin -= vBoarderOffset;
	projMax += vBoarderOffset;
}

static void CalcCullingPlanes(Plane* pPlanes, const H3DVec3& projMin, const H3DVec3& projMax, 
							  const H3DVec3& lispRight, const H3DVec3& lispUp)
{
	Plane plane;
	plane.m_Normal = -lispUp;
	plane.m_fDistance = projMax.y;
	pPlanes[0] = plane;

	plane.m_Normal = lispUp;
	plane.m_fDistance = -projMin.y;
	pPlanes[1] = plane;

	// 构造四个裁剪平面
	plane.m_Normal = lispRight;
	plane.m_fDistance = -projMin.x;
	pPlanes[2] = plane;

	plane.m_Normal = -lispRight;
	plane.m_fDistance = projMax.x;
	pPlanes[3] = plane;
}

static void CalcLightSpaceNearFar(float& projNear, float& projFar, const H3DVec3& eyePos, const H3DVec3& vLightDir)
{
	projNear = H3DMath::INFINITY;
	projFar = -H3DMath::INFINITY;

	float proj = eyePos.Dot(vLightDir);
	if (proj < projNear)
		projNear = proj;

	if (proj > projFar)
		projFar = proj;

	//projNear -= GlobalConfig::m_sShadowFadeoutEnd;
	//projFar += GlobalConfig::m_sShadowFadeoutEnd;

	projNear -= 100.f;
	projFar += 100.f;
}

static void CalcLightSpaceNearFar(float& projNear, float& projFar, const AABB& sceneAABB, const H3DVec3& vLightDir)
{
	projNear = H3DMath::INFINITY;
	projFar = -H3DMath::INFINITY;

	const H3DVec3* pSceneCorner = sceneAABB.GetCorners();

	for (int i = 0; i < 8; i++)
	{
		float proj = pSceneCorner[i].Dot(vLightDir);
		if (proj < projNear)
			projNear = proj;

		if (proj > projFar)
			projFar= proj;
	}

	projNear -= 10.f;
	projFar += 10.f;
}

static void CalcShadowViewProjMatrix(H3DMat4& out, const H3DVec3& projMin, const H3DVec3& projMax, 
									 const H3DVec3& vLightDir, const H3DVec3& vLightUp)
{

	D3DXMATRIX matLightView, matLightProj, matLightViewProj;

	H3DVec3 vEyePos = vLightDir * projMax.z;
	H3DVec3 vLookAt = vLightDir * projMin.z;
	D3DXMatrixLookAtRH(&matLightView, (CONST D3DXVECTOR3*)vEyePos.ToFloatPtr(),
		(CONST D3DXVECTOR3*)vLookAt.ToFloatPtr(), (CONST D3DXVECTOR3*)vLightUp.ToFloatPtr());

	float farPlane = projMax.z - projMin.z;

	D3DXMatrixOrthoOffCenterRH(&matLightProj, projMin.x, projMax.x, 
		projMin.y, projMax.y, 0.0f, farPlane);

	D3DXMatrixMultiply(&matLightViewProj, &matLightView, &matLightProj);

	out = H3DMat4(matLightViewProj._11, matLightViewProj._21, matLightViewProj._31, matLightViewProj._41,
		matLightViewProj._12, matLightViewProj._22, matLightViewProj._32, matLightViewProj._42,
		matLightViewProj._13, matLightViewProj._23, matLightViewProj._33, matLightViewProj._43,
		matLightViewProj._14, matLightViewProj._24, matLightViewProj._34, matLightViewProj._44);

}

static void CalcLightSpaceCoord(const H3DVec3& lightDir, const H3DVec3& lookDir, Camera* pCam,
								H3DVec3& lightRight, H3DVec3& lightUp)
{
	// to do: 光线与视线方向几乎一致时
	lightRight = lookDir.Cross(lightDir);
	if (lightRight.LengthSqr() < 0.01f)
	{
		// 光线与视线方向几乎相反时
		lightRight = pCam->GetRight();
	}

	lightRight.Normalize();

	lightUp = lightDir.Cross(lightRight);
	lightUp.Normalize();
}

/*
static void ComputeCasterAABB(AABB* pAABB, Camera* pCam, float sliceNear, float sliceFar, 
							  const H3DVec3& lightDir, const H3DVector<Renderable*>& casters)
{
	Frustum frustum;
	frustum.SetAspect(pCam->GetAspect());
	frustum.SetFov(pCam->GetFovInDegree());
	frustum.SetNearDist(sliceNear);
	frustum.SetFarDist(sliceFar);

	//GetWorldPosition(), GetWorldOrientation(), SceneManager::GetSingleton().GetRenderSystem(),bRightHand
	frustum.UpdateFrustumParams(pCam->GetWorldPosition(), pCam->GetWorldOrientation(),
	 SceneManager::GetSingleton().GetRenderSystem(), true);

	LightWrappingVolume lightVolume;
	lightVolume.Update(&frustum, lightDir);

	//AABB aabb;
	//aabb.SetNull(true);

	pAABB->SetNull(true);

	EIntersectionType lvTest, dummyTest;
	lvTest = dummyTest = IT_Contain;
	for (H3DVector<Renderable*>::const_iterator it = casters.begin(); it != casters.end(); ++it)
	{
		const AABB& casterAABB = (*it)->GetWorldAABB();
		lightVolume.TestAABB(&casterAABB, lvTest, dummyTest);
		if (lvTest != IT_Outside)
		{
			//aabb.MergeWithAABB(casterAABB);
			pAABB->MergeWithAABB(casterAABB);
		}
	}
}

static AABB CullCasters(const AABB& cullingAABB, const H3DVector<Renderable*>& casters, H3DVector<Renderable*>& cullingRes)
{
	AABB castersAABB;
	for (H3DVector<Renderable*>::const_iterator it = casters.begin(); it != casters.end(); ++it)
	{
		const AABB& objAABB = (*it)->GetWorldAABB();
		EIntersectionType test = cullingAABB.TestAABB(&objAABB);
		if (test != IT_Outside)
		{
			cullingRes.push_back(*it);
			castersAABB.MergeWithAABB(objAABB);
		}
	}
	return castersAABB;
}
*/

int ShadowStrategyX52::RenderLightImplDirectional(int nBeginAtlas, PrePassLightDirectional* pLight)
{
	// 如果atlas不够用了，跳过该光源的阴影计算
	if (nBeginAtlas + m_ShadowParameter.nNumPass > m_ShadowParameter.nNumPass)
	{
		return 0;
	}

	H3DQuat qRot		= pLight->GetWorldOrientation();
	H3DVec3 vLightDir	= qRot * SceneNode::m_sInitDirection;
	Camera* pCam		= SceneManager::GetSingleton().GetRenderCamera();
	H3DVec3 vEyePos		= pCam->GetWorldPosition();
	H3DVec3 lookDir		= pCam->GetDirection();
	H3DVec3 lightLookAt	= vEyePos - vLightDir;

	
	H3DVec3 vLightRight, vLightUp;
	// calc light space up direction
	CalcLightSpaceCoord(vLightDir, lookDir, pCam, vLightRight, vLightUp);

	AABB currSceneAABB;
	currSceneAABB.SetNull(true);
	for (H3DVector<Renderable*>::iterator it = m_CasterObjs.begin(); 
		it != m_CasterObjs.end(); ++it)
	{
		Renderable* pRenderable = *it;
		const AABB& aabb = pRenderable->GetWorldAABB();
		currSceneAABB.MergeWithAABB(aabb);
		/*H3DVec3 center, minPos, maxPos;
		aabb.GetCenterPoint(center);
		minPos = aabb.GetMin();
		maxPos = aabb.GetMax();

		if (center.z > 1000.f || center.z < -1000.f || maxPos.z - minPos.z > 1500.f)
		{
			if (pRenderable->GetMaterialLayer())
			{
				if (pRenderable->GetMaterialLayer()->GetNewMaterial())
				{
					const char* name = pRenderable->GetNewMaterial()->GetMatInsName().c_str();
					LogInfo(RENDER_INFO, OutPut_Console ,"cast shadow obj %s\n", name);
				}
			}

			LogInfo(RENDER_INFO, OutPut_Console ,"large shadow caster\n");			
		}*/
	}

	float projNear, projFar;
	CalcLightSpaceNearFar(projNear, projFar, currSceneAABB, vLightDir);
	//LogInfo(RENDER_INFO, OutPut_Console ,"projNear %.3f, projFar %.3f\n", projNear, projFar);

	float scale = 50.f * GlobalConfig::GetShadowDepthBias();
	m_ShadowParameter.fDepthBias = scale / (projFar - projNear);
	ApplyShadowParamBase(m_ShadowParameter.nShadowMapWidth, m_ShadowParameter.nShadowMapHeight, m_ShadowParameter.fDepthBias);
	//LogInfo(RENDER_INFO, OutPut_Console, "shadow bias %.8f\n", m_ShadowParameter.fDepthBias);


	//CalcLightSpaceNearFar(projNear, projFar, vEyePos, vLightDir);

	RenderSystem* pRender = SceneManager::GetSingleton().GetRenderSystem();
	RenderDevice* pRenderDevice = pRender->GetRenderDevice();
	//pRenderDevice->SetDepthBias(m_ShadowParameter.fDepthBias);
	pRenderDevice->SetSlopScaleDepthBias(m_ShadowParameter.fSlopDepthBias);
	pRenderDevice->SetColorMask(false, false, false, false);

	//激活2个RT
	
	GetShadowMap()->Activate(0);
	GetShadowDepthBuffer()->Activate(0);

	pRender->Clear(CF_Color|CF_Depth, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

	AABB m_FrustumAABB[3];
	for (int i = 0; i < m_ShadowParameter.nNumPass; i++)
	{
		int m_nCurPass = i;

		int nCurrentAtlas = nBeginAtlas + i;
		SetupViewPort(nCurrentAtlas);

		H3DVec3 projMin, projMax;

		H3DVec3 worldFrustum[8];
		CalculateSplitFrustum(worldFrustum, m_FrustumAABB[i], 
							m_ShadowParameter.fSplitPos[i],
							m_ShadowParameter.fSplitPos[i+1], pCam);

		//H3DVector<Renderable*> culledCasters;
		//AABB castersAABB = CullCasters(m_FrustumAABB[i], m_CasterObjs, culledCasters);


		CalcLightSpaceViewPort(projMin, projMax, worldFrustum, 8, vLightRight, vLightUp);

		SmoothShadowFlickering(projMin, projMax, m_ShadowParameter.nAtlasSize);

		projMax.z = projFar;
		projMin.z = projNear;//projFar - GlobalConfig::m_sShadowFadeoutEnd;

		H3DMat4 matLightVP;
		CalcShadowViewProjMatrix(matLightVP, projMin, projMax, vLightDir, vLightUp);

		// 设置阴影投影矩阵
		GlobalShaderConst* pManager = SingletonManager::GetSingleton().m_RenderingContex.GetGlobalShaderConstManager();
		pManager->SetMatrixTranspose("ShadowmapViewProjMatrix", matLightVP);

		H3DVector<Plane> cullingPlanes(4);
		CalcCullingPlanes(&cullingPlanes[0], projMin, projMax, vLightRight, vLightUp);

		RenderOneAtlas(pLight, cullingPlanes);

		matLightVP.TransposeSelf();
		H3DMat4 unProjMat = CalcTexProjMatrix(matLightVP, nCurrentAtlas);
		pLight->SetShadowMapProjMatrix(i, unProjMat);

		m_dirProjMatrix[i] = unProjMat;
	}

	//pRenderDevice->SetDepthBias(0.0f);
	pRenderDevice->SetSlopScaleDepthBias(0.0f);
	pRenderDevice->SetColorMask(true, true, true, true);

	GetShadowMap()->Deactivate();
	GetShadowDepthBuffer()->Deactivate();



	// 如果整个阴影管线只有一个方向光，在这里就应用投影矩阵,不用等到shding管线在设
	// 用于赛马的项目
	if (m_Lights.size() == 1)
	{
		pLight->ApplyShadow();
	}

	pRender->SetViewPort(m_SavedVP);

	return 2;
}

void ShadowStrategyX52::CollectSceneShadow(PrePassLight* pLight, const H3DMat4* pMatDirProj)
{	
	RenderTarget* shadowMap = GetShadowDepthBuffer();
	GlobalShaderConst* pManager = SingletonManager::GetSingleton().m_RenderingContex.GetGlobalShaderConstManager();
	pManager->SetTexture("ShadowMapTexture", (RenderTargetD3D9*)shadowMap);

	m_pScreenSpaceShadowRT->Activate(0);

	Renderable* pRenderable = m_pEntitySSShadow->GetSubEntityByIndex(0);

	RenderWindow *pWindow = SceneManager::GetSingleton().GetRenderSystem()->GetCurrentRenderWindow(SceneManager::GetSingleton().GetRenderSystem()->GetActiveRenderWindow()->GetID() );
	pRenderable->GetMaterialLayer()->SetNodeVarValue(CS_CACHE, H3DString("SceneDepthRT"),(void*)(pWindow->GetDepthRT()->GetTextureName().c_str()));
	const Color& shadowColor = pLight->GetShadowColor();
	pRenderable->GetMaterialLayer()->SetNodeVarValue(CS_CACHE, H3DString("ShadowColor"),(void*)&shadowColor);

	pManager->SetMatrixArray("ShadowmapProjMatrix", pMatDirProj, 3);

	ActiveRenderableToPass(pRenderable);
	RenderRenderable();

	m_pScreenSpaceShadowRT->Deactivate();

	/// 释放阴影深度图
	pManager->ResetTexture("ShadowMapTexture");	
	ReturnRenderTarget();
}

static void CalcShadowProjMatrix(H3DMat4& out, const H3DVec3* pCorners, int nNumPos, const H3DVec3& lightDir)
{
	H3DVec3 vLightDir = lightDir;
	vLightDir.Normalize();

	H3DVec3 vLightCamUp(0.0f, 0.0f, 1.0f);
	H3DVec3 vLightCamRight = vLightCamUp.Cross(vLightDir);

	// if light dir very close to up direction
	// force right direction to x axis
	if (vLightCamRight.LengthSqr() < 0.01f)
	{
		vLightCamRight.Set(1.0f, 0.0f, 0.0f);
	}
	else
	{
		vLightCamRight.Normalize();
	}

	vLightCamUp = vLightDir.Cross(vLightCamRight);
	vLightCamUp.Normalize();

	// project caster's aabb to light space
	H3DVec3 projMin(H3DMath::INFINITY, H3DMath::INFINITY, H3DMath::INFINITY);
	H3DVec3 projMax(-H3DMath::INFINITY, -H3DMath::INFINITY, -H3DMath::INFINITY);

	for (int i = 0; i < nNumPos; i++)
	{
		float proj = pCorners[i].Dot(vLightDir);
		if (proj < projMin.z)
			projMin.z = proj;

		if (proj > projMax.z)
			projMax.z = proj;

		proj = pCorners[i].Dot(vLightCamRight);
		if (proj < projMin.x)
			projMin.x = proj;

		if (proj > projMax.x)
			projMax.x = proj;

		proj = pCorners[i].Dot(vLightCamUp);
		if (proj < projMin.y)
			projMin.y = proj;

		if (proj > projMax.y)
			projMax.y = proj;
	}

	D3DXMATRIX matLightView, matLightProj, matLightViewProj;

	H3DVec3 vEyePos = vLightDir * projMax.z;
	H3DVec3 vLookAt = vLightDir * projMin.z;
	D3DXMatrixLookAtRH(&matLightView, (CONST D3DXVECTOR3*)vEyePos.ToFloatPtr(),
		(CONST D3DXVECTOR3*)vLookAt.ToFloatPtr(), (CONST D3DXVECTOR3*)vLightCamUp.ToFloatPtr());

	static float fShadowmapMargin = 0.2f;
	// enlarge shadow map with a margin to ensure all needed meshes are included
	// remove this when actor's aabb is correctly updated
	projMin.x -= fShadowmapMargin;
	projMin.y -= fShadowmapMargin;
	projMax.x += fShadowmapMargin;
	projMax.y += fShadowmapMargin;

	//float compensate = CalcCompensateDepth(casterAABB, lightDir);
	float farPlane = projMax.z - projMin.z;// + compensate;


	D3DXMatrixOrthoOffCenterRH(&matLightProj, projMin.x, projMax.x, 
		projMin.y, projMax.y, -10.0f, farPlane+20.0f);

	D3DXMatrixMultiply(&matLightViewProj, &matLightView, &matLightProj);

	// transpose d3d matrix to an engine one 
	out = H3DMat4(matLightViewProj._11, matLightViewProj._12, matLightViewProj._13, matLightViewProj._14,
		matLightViewProj._21, matLightViewProj._22, matLightViewProj._23, matLightViewProj._24,
		matLightViewProj._31, matLightViewProj._32, matLightViewProj._33, matLightViewProj._34,
		matLightViewProj._41, matLightViewProj._42, matLightViewProj._43, matLightViewProj._44);

}


void ShadowStrategyX52::RenderCharacterShadow()
{
	// find a directional light
	PrePassLight* pDirLight = NULL;
	for (H3DVector<PrePassLight*>::iterator it = m_Lights.begin(); it != m_Lights.end(); ++it)
	{
		PrePassLight* pLight = (*it);
		if (pLight->GetLightType() == LIGHT_Directional && 
			(pLight->GetAffectParam() == H3DI::AFFECT_ACTOR_ONLY || pLight->GetAffectParam() == H3DI::AFFECT_ALL))
		{
			if (pDirLight == NULL || pLight->GetPriority() > pDirLight->GetPriority())
			{
				pDirLight = pLight;
			}
		}
	}
	if (pDirLight == NULL)
	{
		return;
	}
	H3DQuat qRot		= pDirLight->GetWorldOrientation();
	H3DVec3 vLightDir	= qRot * SceneNode::m_sInitDirection;

	for (H3DSet<Entity*>::iterator itChar = m_CharacterSet.begin(); itChar != m_CharacterSet.end(); ++itChar)
	{
		// calculate shadow matrix
		AABB aabb = (*itChar)->GetWorldAABB();

		H3DMat4 mat;
		CalcShadowProjMatrix(mat, aabb.GetCorners(), 8, vLightDir);
		GlobalShaderConst* pManager = SingletonManager::GetSingleton().m_RenderingContex.GetGlobalShaderConstManager();
		// 设置阴影投影矩阵
		pManager->SetMatrix("ShadowmapViewProjMatrix", mat);

		// 渲染shadow map
		H3DVector<Renderable*>* pAllRenderables = (*itChar)->GetAllRenderable();
		for (H3DVector<Renderable*>::iterator it = pAllRenderables->begin(); it != pAllRenderables->end(); ++it)
		{
			ActiveRenderableToPass(*it);
		}

		RenderTarget*  ShadowMapColor= GetShadowMap();
		RenderTarget*  ShadowMapdepth= GetShadowDepthBuffer();

		//激活2个RT
		ShadowMapColor->Activate(0);
		ShadowMapdepth->Activate(0);

		RenderSystem* pRender = SceneManager::GetSingleton().GetRenderSystem();
		pRender->Clear(CF_Color|CF_Depth, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
		SetupViewPort(0);
		pRender->GetRenderDevice()->SetColorMask(false, false, false, false);

		RenderRenderable();

		H3DMat4 unProjMat = CalcTexProjMatrix(mat, 0);
		// 在shader里暂时复用这个矩阵
		pManager->SetMatrix("ShadowmapViewProjMatrix", unProjMat);

		ShadowMapColor->Deactivate();
		ShadowMapdepth->Deactivate();
		pRender->GetRenderDevice()->SetColorMask(true, true, true, true);

		CollectCharacterShadow(pDirLight, mat);
	}
}


void ShadowStrategyX52::CollectCharacterShadow(PrePassLight* pLight, const H3DMat4& matShadowViewProj)
{
	H3DMat4 matShadowViewProjInv = matShadowViewProj.Inverse();
	GlobalShaderConst* pManager = SingletonManager::GetSingleton().m_RenderingContex.GetGlobalShaderConstManager();
	pManager->SetMatrix("ShadowmapViewProjInvMatrix", matShadowViewProjInv);

	IDirect3DDevice9* pDevice = ((RenderDeviceD3D9*)SceneManager::GetSingleton().GetRenderSystem()->GetRenderDevice())->GetD3D9Device();

	m_pScreenSpaceShadowRT->Activate(0);

	SceneManager::GetSingleton().GetRenderSystem()->Clear(CF_Stencil);

	{
		// 渲染蒙版
		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0);
		Renderable* pRenderable = m_pEntityShadowMask->GetSubEntityByIndex(0);
		RenderDevice* pRenderDevice = SceneManager::GetSingleton().GetRenderSystem()->GetRenderDevice();
		pRenderDevice->SetStencilTest(true);

		pDevice->SetRenderState(D3DRS_STENCILMASK, 0xFFFFFFFF);
		pDevice->SetRenderState(D3DRS_STENCILWRITEMASK, 0xFFFFFFFF);
		pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
		pDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
		pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
		pDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_DECR);


		pDevice->SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, TRUE);
		pDevice->SetRenderState(D3DRS_CCW_STENCILPASS, D3DSTENCILOP_KEEP);
		pDevice->SetRenderState(D3DRS_CCW_STENCILFAIL, D3DSTENCILOP_KEEP);
		pDevice->SetRenderState(D3DRS_CCW_STENCILFUNC, D3DCMP_ALWAYS);
		pDevice->SetRenderState(D3DRS_CCW_STENCILZFAIL, D3DSTENCILOP_INCR);

		ActiveRenderableToPass(pRenderable);
		RenderRenderable();

		pDevice->SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, FALSE);
		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED|
														D3DCOLORWRITEENABLE_GREEN|
														D3DCOLORWRITEENABLE_BLUE|
														D3DCOLORWRITEENABLE_ALPHA);

	}


	pDevice->SetRenderState(D3DRS_STENCILREF, 1);
	pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);

	{
		pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_MIN);
		// 使用蒙版,抠出角色阴影
		Renderable* pRenderable = m_pEntityCharacterCollecter->GetSubEntityByIndex(0);

		RenderWindow *pWindow = SceneManager::GetSingleton().GetRenderSystem()->GetCurrentRenderWindow(SceneManager::GetSingleton().GetPostProcessManager()->GetRenderWindowId());
		pRenderable->GetMaterialLayer()->SetNodeVarValue(CS_CACHE, H3DString("SceneDepthRT"),(void*)(pWindow->GetDepthRT()->GetTextureName().c_str()));

		const Color& shadowColor = pLight->GetShadowColor();
		pRenderable->GetMaterialLayer()->SetNodeVarValue(CS_CACHE, H3DString("ShadowColor"),(void*)&shadowColor);

		ActiveRenderableToPass(pRenderable);
		RenderRenderable();
		pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	}

	m_pScreenSpaceShadowRT->Deactivate();

	RenderDevice* pRenderDevice = SceneManager::GetSingleton().GetRenderSystem()->GetRenderDevice();
	pRenderDevice->SetStencilTest(false);
}

void ShadowStrategyX52::PreSSShadow()
{
	if(GlobalConfig::UseSoftShadow() != (m_nSoftShadowLevel == 1))
	{
		SetSoftShadowLevel(!!GlobalConfig::UseSoftShadow());
	}
	int w = 0, h = 0;
	SceneManager::GetSingleton().GetRenderSystem()->GetCurrentReslution(w, h);

	RenderTargetManager* pRTManager = SceneManager::GetSingleton().GetRenderTargetManager();
	RenderTargetInfo rtInfo(w, h, PF_ARGB8);
	m_pScreenSpaceShadowRT = pRTManager->GetRenderTarget(rtInfo);

	RenderSystem* pRender = SceneManager::GetSingleton().GetRenderSystem();
	m_pScreenSpaceShadowRT->Activate(0);
	pRender->Clear(CF_Color, 1.f, 1.f, 1.f, 1.f);
	m_pScreenSpaceShadowRT->Deactivate();
}

void ShadowStrategyX52::PostSSShadow()
{
	GlobalShaderConst* pManager = SingletonManager::GetSingleton().m_RenderingContex.GetGlobalShaderConstManager();

	pManager->ResetTexture("ScreenSpaceShadowRT0" );
	RenderTargetManager* pRTManager = SceneManager::GetSingleton().GetRenderTargetManager();
	pRTManager->ReturnRenderTarget(m_pScreenSpaceShadowRT);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////

int ShadowStrategyX52::PushRenderable(Renderable* pRenderable)
{
	if (!m_ShadowParameter.bShadowEnable)
	{
		return 0;
	}

	if (m_ShadowParameter.bUseCharacterShadow)
	{
		// find all characters
		Entity* pEntity = pRenderable->GetParent();
		//if (pEntity->GetEntityType() == Entity_CHR_Actor)
		if (pEntity->GetEntityType() == Entity_Actor ||
			pEntity->GetEntityType() == Entity_CHR_Actor)
		{
			m_CharacterSet.insert(pEntity);
		}
		else
		{
			m_CasterObjs.push_back(pRenderable);

		}
	}
	else
	{
		m_CasterObjs.push_back(pRenderable);
	}

	return 1;
}

int ShadowStrategyX52::PushShadowLight(Renderable* p)
{
	if (!m_ShadowParameter.bShadowEnable)
	{
		return 0;
	}

	PrePassLight* pLight = (PrePassLight*)(p->GetParent());
	if (pLight->GetLightType() == LIGHT_Spot || pLight->GetLightType() == LIGHT_Point)
	{
		return 0;
	}

	m_Lights.push_back(pLight);
	return 1;
}

void ShadowStrategyX52::SetupViewPort(int nAtlas)
{
	// setup viewport
	ViewPort vp;
	vp.x = nAtlas * m_ShadowParameter.nAtlasSize;
	vp.y = 0;
	vp.width = m_ShadowParameter.nAtlasSize;
	vp.height = m_ShadowParameter.nAtlasSize;
	SceneManager::GetSingleton().GetRenderSystem()->SetViewPort(vp);
}

///放置Renderable
void ShadowStrategyX52::ActiveRenderableToPass(Renderable* pRenderable)
{
	//获得frame id
	unsigned long lastframeID=SceneManager::GetSingleton().GetLastFrameCount();
	MaterialLayer* pLayer=pRenderable->GetMaterialLayer();
	H3DMaterial* pMat=pLayer->GetNewMaterial();

	Entity* pEntity=((SubEntity*)(pRenderable))->GetParent(); //特效咋办?

	if (!IsCached(pRenderable))
	{
		ShaderGenFrameWork* pShaderGen = SingletonManager::GetSingleton().m_RenderingContex.GetShaderGen(pMat,CS_CACHE);
		ShaderGenOperation op;
		op.render_tech=CastShadow_Shader;
		op.pipe_type=ST_CASTSHADOW;
		op.fx_tech=CASTSHADOW_TECHNIQUE;
		op.pSrcMat=pMat;
		op.pRendable=pRenderable;
		op.pDstMat=pMat;
		op.pShader=pMat->GetLastFrameShader(CS_CACHE);

		FxHelpInfo* pHelpInfo=&(op.m_macros);

		///// !!! 此Macro只跟castshadow、lppPrepass管线相关...要么写在GetMacros中，暂特化于此
		//bool bHasAlphaTest = pMat->HasAlphaTest(CS_CACHE);
		//if (bHasAlphaTest)
		//{
		//	H3DShaderMacro* pATestMacro=SingletonManager::GetSingleton().m_H3DShaderMacroManager.GetMacro(RCATEST);
		//	pHelpInfo->m_fx_marcos.push_back(pATestMacro);
		//}
		pMat->GetMacros(CastShadow_Shader,pRenderable,pHelpInfo);//获取模板中定义的宏，不包括渲染环境
		SingletonManager::GetSingleton().m_H3DShaderMacroManager.GetDefautMacro(ST_CASTSHADOW,CastShadow_Shader,&(op.m_macros.m_fx_enumMacros));
		pShaderGen->GenerateH3DShader(op);
		//GetLppShaderGen()->GenerateSingleShader(Lpp_PreLight_Shader,ST_LPP,SHG_NOSKL_NOBUMP, pRenderable, pMat);
		//m_ShaderGen.GenerateSingleShader(CastShadow_Shader,ST_CASTSHADOW,SHG_NOSKL_NOBUMP,pRenderable,pMat,false);

		//按名字放到相应的FxGroup中
		AssignFxGroup(pRenderable,pMat);
	}
}

void ShadowStrategyX52::RenderRenderable()
{
	FxGroupMapItr itr = m_FxGroupMap.begin();
	while(itr != m_FxGroupMap.end())
	{
		FxGroup* p = itr->second;
		SingletonManager::GetSingleton().m_RenderProcessor.RenderFxGroup(ST_CASTSHADOW,p);
		p->MoveAllToUnUsedMap();
		++itr;
	}
}

void ShadowStrategyX52::AssignFxGroup(Renderable* pRenderable,H3DMaterial* pMat)
{
	//取出fx的名字
	H3DShader* pShader=pMat->GetLastFrameShader(CS_CACHE);
	H3DString& fxname=pShader->GetFxFileName();
	FxGroup* pFxGroup=GetFxGroup(fxname);
	pFxGroup->PushWrapper(CS_CACHE,pRenderable,pMat);
}


FxGroup* ShadowStrategyX52::GetFxGroup(H3DString& fxName)
{
	FxGroup* pNewGroup=0;
	FxGroupMapItr itr=m_FxGroupMap.find(fxName);
	if (itr!=m_FxGroupMap.end())
		return itr->second;
	else
	{
		pNewGroup=SingletonManager::GetSingleton().m_RenderingContex.CreateFxGroup(fxName,0,false); 
		m_FxGroupMap.insert(std::make_pair(fxName,pNewGroup));
	}
	return pNewGroup;
}


bool ShadowStrategyX52::IsCached(Renderable* pSubEntity)
{
	if(SceneManager::GetSingleton().GetNeedReFlashMaterial())
	{
		return false;
	}

	MaterialLayer* pLayer=pSubEntity->GetMaterialLayer();
	H3DMaterial* pMat=pLayer->GetNewMaterial();
	SHADER_CACHE_RESULT _state=pLayer->GetNewMaterial()->GetReCreateState(CS_CACHE);

	//判断自身是否被改变
	bool RCS=(_state==SCR_RECREATE_ALL); 

	if (!RCS)
	{
		//判断是否已经有Cache的 RenderOp

		NewRenderOperation* op=pMat->GetCacheOp(CS_CACHE);

		if (!op)
			return false;

		H3DString& fxName=pMat->GetLastFrameShaderName(CS_CACHE);
		H3DString& matBallName=pMat->GetLastFrameShaderMaterialBallName(CS_CACHE);
		MAT_ID id=pLayer->GetLayerID();
		FxGroup* pFxGroup=pMat->GetLastFrameFxGroup(CS_CACHE);
		MaterialBallGroup* pMatBallGroup=pMat->GetLastFrameMatBallGroup(CS_CACHE);
		bool res=ActiveCacheOp(fxName,matBallName,id,op,pFxGroup,pMatBallGroup);
		return res;
	}

	return false;
}

///查找cache的render op,如果成功 直接使用cache的op
bool ShadowStrategyX52::ActiveCacheOp(H3DString& fxPath,H3DString& matName,MAT_ID id,NewRenderOperation* op,FxGroup* pFxGroup,MaterialBallGroup* pMatBallGroup)
{
	if(pFxGroup && pMatBallGroup)
	{
		pMatBallGroup->ActiveCacheOp(id,op);
		return true;
	}

	FxGroup* _pFxGroup=GetFxGroup(fxPath);

	if(!_pFxGroup)
		return false;
	_pFxGroup->ActiveCacheOp(matName,id,op);
	return true;
}


int ShadowStrategyX52::RenderLightImplSpot(int nBeginAtlas, PrePassLightSpot* pLight)
{
	// 如果atlas不够用了，跳过该光源的阴影计算
	if (nBeginAtlas + 1 > m_ShadowParameter.nNumPass)
	{
		return 0;
	}

	// right hand coord
	H3DVec3 vEye = pLight->GetWorldPosition();
	H3DQuat qRot = pLight->GetWorldOrientation();

	float innerAngle = 0.0f, outterAngle = 0.0f;
	pLight->GetAngle(innerAngle, outterAngle);

	// 由光源的默认朝向构造光源的本地坐标系
	H3DVec3 x = qRot * vec3_unitX;
	H3DVec3 y = qRot * vec3_unitZ;
	H3DVec3 z = qRot * SceneNode::m_sInitDirection;

	H3DMat4 matView(x.x, y.x, z.x, 0.0f,
		x.y, y.y, z.y, 0.0f,
		x.z, y.z, z.z, 0.0f,
		-x.Dot(vEye), -y.Dot(vEye), -z.Dot(vEye), 1.0f);

	float halfOutter = outterAngle * H3DMath::M_DEG2RAD * 0.5f;
	float scale = 1.0f / H3DMath::Tan(halfOutter);

	float zf = 2000.0f;
	float zn = 10.0f;

	float zScale = zf / (zn - zf);

	// target viewport's aspect is 1
	H3DMat4 matProj(scale, 0.0f, 0.0f, 0.0f,
		0.0f, scale, 0.0f, 0.0f,
		0.0f, 0.0f, zScale, -1.0f,
		0.0f, 0.0f, zn * zScale, 0.0f);

	H3DMat4 matViewProj = matView * matProj;

	GlobalShaderConst* pManager = SingletonManager::GetSingleton().m_RenderingContex.GetGlobalShaderConstManager();
	// 设置阴影投影矩阵
	pManager->SetMatrix("ShadowmapViewProjMatrix", matViewProj);

	SetupViewPort(nBeginAtlas);

	// 聚光灯不使用剔除平面，直接使用自己的剔除算法
	H3DVector<Plane> dummyCullingPlanes;

	// render shadow map atlas
	RenderOneAtlas(pLight, dummyCullingPlanes);

	// 计算贴图空间投影矩阵
	// 为了解决自阴影的问题，将远截面移近一些，重新计算投影矩阵
	//matProj[3][2] = matProj[3][2] - 0.008f;

	/*zf *= 1.6f;
	zScale = zf / (zn - zf);
	matProj = H3DMat4(scale, 0.0f, 0.0f, 0.0f,
	0.0f, scale, 0.0f, 0.0f,
	0.0f, 0.0f, zScale, -1.0f,
	0.0f, 0.0f, zn * zScale, 0.0f);

	matViewProj = matView * matProj;*/
	H3DMat4 unProjMat = CalcTexProjMatrix(matViewProj, nBeginAtlas);
	pLight->SetShadowMapProjMatrix(0, unProjMat);

	//SceneManager::GetSingleton().ConsolOutput(false,"Cast Spot Shadow!");
	return 1;
}

void ShadowStrategyX52::SetSoftShadowLevel( int nLevel )
{
	m_nSoftShadowLevel = nLevel;
	Renderable* pRenderable = m_pEntitySSShadow->GetSubEntityByIndex(0);
	if(nLevel == 0)
	{
		pRenderable->GetMaterialLayer()->GetNewMaterial()->SetMacro(S_S, "0");
	}
	else if(nLevel == 1)
	{
		pRenderable->GetMaterialLayer()->GetNewMaterial()->SetMacro(S_S, "1");
	}
}




}