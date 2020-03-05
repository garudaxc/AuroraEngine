#include "stdheader.h"
#include "Pipeline.h"
#include "Renderer.h"
#include "Camera.h"
#include "Util.h"
#include "PostProcess.h"
#include "OcclusionQuery.h"
#include "HelperRenderer.h"
#include "Entity.h"
#include "Renderer.h"
#include "SceneView.h"
#include "Material.h"
#include "Model.h"

namespace Aurora
{

	struct MaterialCmp : public binary_function<Renderable*, Renderable*, bool>
	{	// functor for operator<
		bool operator()(const Renderable* _Left, const Renderable* _Right) const;
	};


	struct CullDistCmp : public binary_function<Renderable*, Renderable*, bool>
	{	// functor for operator<
		bool operator()(const Renderable* _Left, const Renderable* _Right) const;
	};

	
	bool MaterialCmp::operator()(const Renderable* _Left, const Renderable* _Right) const
	{
		/*return _Left->GetMaterialInst()->GetMaterialTemplate()->GetPriority()
			< _Right->GetMaterialInst()->GetMaterialTemplate()->GetPriority();*/
		return _Left < _Right;
	}

	bool CullDistCmp::operator()(const Renderable* _Left, const Renderable* _Right) const
	{
		// order distance from back to front
		return _Left->GetCullDist() > _Right->GetCullDist();
	}




	struct RenderingStatistics
	{
		uint nNumObj;
		uint nNumLights;
		uint nNumPrimitive;

		void Clear()
		{
			memset(this, 0, sizeof(RenderingStatistics));
		}
	};



extern IRenderDevice* GRenderDevice;


class RenderInfo
{
public:
	RenderTarget*	pRTGbuffer0;
	RenderTarget*	pRTGbuffer1;
	RenderTarget*	pRTLighting;
};

RenderInfo renderInfo;

Pipeline GPipeline;



Pipeline::Pipeline(void):m_bOcclusionQuerying(false)
{
}

Pipeline::~Pipeline(void)
{
}

void Pipeline::BeginPipe()
{
	//m_Context.GetStatistic()->Clear();
	GRenderDevice->BeginScene();
	//m_Context.PrepareRendering();
}

void Pipeline::Render()
{
	//g_pShadowRendering->Render(&m_Context);
	//g_pGBufferRendering->Render(&m_Context);
	//g_pLightRendering->Render(&m_Context);
	//g_pShadingRendering->Render(&m_Context, m_bOcclusionQuerying ? m_pOcclusionQuery : NULL);
	//g_pPostProcessRendering->Render(&m_Context);
}

void Pipeline::EndPipe()
{
	GRenderDevice->EndScene();
	Clear();

	GRenderDevice->Present();
}

void Pipeline::Clear()
{
}

void Pipeline::Initialize(int width, int height)
{
	RenderTarget::Desc desc;

	desc.nWidth		= width;
	desc.nHeight	= height;
	desc.nUsage		= Surface::USAGE_RENDERTARGET;
	desc.nFormat	= Surface::FMT_R8G8B8A8;

	renderInfo.pRTGbuffer0 = GRenderDevice->CreateRenderTarget(desc);
	renderInfo.pRTGbuffer1 = GRenderDevice->CreateRenderTarget(desc);

	desc.nWidth		= width;
	desc.nHeight	= height;
	desc.nUsage		= Surface::USAGE_RENDERTARGET;
	desc.nFormat	= Surface::FMT_A16B16G16R16F;

	renderInfo.pRTLighting = GRenderDevice->CreateRenderTarget(desc);



	//GHelperRenderer.Init();


	/*uint width = 0, height = 0;
	RenderTarget::Desc desc;
	GRenderDevice->GetFrameBufferSize(width, height);

	RenderTarget* pRT = NULL;

	desc.nWidth		= RenderTarget::DS_FRAMEBUFFER_WIDTH;
	desc.nHeight	= RenderTarget::DS_FRAMEBUFFER_HEIGHT;
	desc.nUsage		= Surface::USAGE_RENDERTARGET;
	desc.nFormat	= Surface::FMT_A16B16G16R16F;

	pRT = ->CreateRenderTarget(desc);
	Global::GetResourceManager()->AddRenderTarget("ColorRT", pRT);


	desc.nWidth		= 1024;
	desc.nHeight	= 1024;
	desc.nUsage		= Surface::USAGE_DEPTHSTENCIL;
	desc.nFormat	= Surface::FMT_D16;

	pRT =->CreateRenderTarget(desc);
	Global::GetResourceManager()->AddRenderTarget("ShadowMapRT", pRT);


	desc.nWidth		= 1024;
	desc.nHeight	= 1024;
	desc.nUsage		= Surface::USAGE_RENDERTARGET;
	desc.nFormat	= Surface::FMT_R8G8B8A8;

	pRT = ->CreateRenderTarget(desc);
	Global::GetResourceManager()->AddRenderTarget("ShadowMapColorRT", pRT);*/
	

	//g_pPostProcessRendering.reset(new PostProcessRendering());
	//g_pPostProcessRendering->Init();

	//m_pHelperRenderer = make_unique<HelperRenderer>();
	//m_pHelperRenderer->Init();
}


void Pipeline::Finalize()
{

}

void Pipeline::OnReset()
{
	GRenderDevice->OnReset();
	//m_pOcclusionQuery.reset(new OcclusionQuery());
	//m_pOcclusionQuery->Init();
}

void Pipeline::OnLost()
{
	//m_pOcclusionQuery.reset();
	GRenderDevice->OnLost();
}

void Pipeline::SetupShaderVariables(SceneView* view)
{

	//GShaderManager.SetMatrix(SV_MATVIEWPROJ, pView->matViewProj);

	//// x : 1 / back buffer width
	//// y : 1 / back buffer height
	//// z : back buffer width
	//// w : back buffer height
	//uint nWidth = 0, nHeight = 0;
	//GRenderDevice->GetFrameBufferSize(nWidth, nHeight);
	//float fBackBufferSize[] = {1.0f / (float)nWidth,1.0f / (float)nHeight,
	//							(float)nWidth,(float)nHeight};
	//GShaderManager.SetVector(SV_BACKBUFFERSIZE, fBackBufferSize);

	//float fCameraParam[] = {pView->fNearPlane, pView->fFarPlane,
	//				1.0f / pView->matProj._11, 1.0f / pView->matProj._22};

	//GShaderManager.SetVector(SV_CAMERAVECX, pView->vCameraVec[0].Ptr());
	//GShaderManager.SetVector(SV_CAMERAVECY, pView->vCameraVec[1].Ptr());
	//GShaderManager.SetVector(SV_CAMERAVECZ, pView->vCameraVec[2].Ptr());

	//GShaderManager.SetVector(SV_CAMERAPARAM, fCameraParam);
	//GShaderManager.SetVector(SV_CAMERAPOS, pView->vEyePos.Ptr());
	//GShaderManager.SetVector(SV_CAMERALOOKDIR, pView->vCameraVec[0].Ptr());
}


static void RenderEntity(DrawingEntity* pEntity, uint nTechnique)
{
	const Matrix4f& matWorld = pEntity->mWorld;
	shaderManager.SetWorldMatrix(matWorld);

	const Entity* pRenderEntity = pEntity->pEntity;
	uint nNumRenderable = pRenderEntity->GetNumRenderable();
	for (uint i = 0; i < nNumRenderable; i++)
	{
		const RenderOperator& op = pRenderEntity->GetRenderable(i)->GetRenderOperator();
		//op.pMtlInst->Apply(nTechnique);
		//op.pMtlInst->Commit();
		GRenderDevice->ExecuteOperator(op);
	}
}


void RenderGeomPass(SceneView* pView)
{
	RenderTarget* rt[] = {renderInfo.pRTGbuffer0, renderInfo.pRTGbuffer1};
	RenderTarget* pDepthBuffer = GRenderDevice->GetDepthStecil();
	GRenderDevice->SetRenderTarget(2, rt, pDepthBuffer);

	//GShaderManager.BindGPassShader();
	DrawingEntity* pEntity = pView->pDrawingEntity;
	while (pEntity)
	{
		RenderEntity(pEntity, 0);
		pEntity = pEntity->pNext;
	}

	GRenderDevice->RestoreFrameBuffer();
}

void RenderLightingPass(SceneView* pView)
{
	RenderTarget* rt[] = {renderInfo.pRTLighting};
	RenderTarget* pDepthBuffer = GRenderDevice->GetDepthStecil();
	GRenderDevice->SetRenderTarget(1, rt, pDepthBuffer);
	GRenderDevice->Clear(IRenderDevice::CLEAR_FRAME_BUFFER, Color::ZERO);

	GShaderManager.SetTexture(SV_GBUFFER0, renderInfo.pRTGbuffer0);
	GShaderManager.SetTexture(SV_GBUFFER1, renderInfo.pRTGbuffer1);

	//GShaderManager.BindDirLightShader();
	DrawingLight* pLight = pView->pDrawingLight;
	while (pLight)
	{
		Model* pModel = NULL;
		if (pLight->Type == LT_DIRECTIONAL)
		{
			shaderManager.SetWorldMatrix(pLight->mWorld);
			pModel = GModelManager.GetDirLight();
			pModel->GetRenderable(0)->GetMaterialInst()->SetVariableValue("vLightColor", pLight->cColor.Ptr());
		}
		else if (pLight->Type == LT_POINT)
		{
			shaderManager.SetWorldMatrix(pLight->mWorld);
			pModel = GModelManager.GetPointLight();
			float param[] = {pLight->fRange, 0.f, 0.f, 0.f};
			pModel->GetRenderable(0)->GetMaterialInst()->SetVariableValue("vLightColor", pLight->cColor.Ptr());
			pModel->GetRenderable(0)->GetMaterialInst()->SetVariableValue("vLightParam", param);			
		}
		else if (pLight->Type == LT_SPOT)
		{
			shaderManager.SetWorldMatrix(pLight->mWorld);
			pModel = GModelManager.GetSpotLight();

			float param[4];
			// x : tangent of half outter angle
			// y : cosine of half outter angle
			// z : cosine of half inner angle
			// w : 1 / (z - y)
			param[0] = Mathf::Tan(pLight->fInnerCone * .5f * Mathf::DEG_TO_RAD);
			param[1] = Mathf::Cos(pLight->fOutterCone * .5f * Mathf::DEG_TO_RAD);
			param[2] = Mathf::Cos(pLight->fInnerCone * .5f * Mathf::DEG_TO_RAD);
			param[3] = 1.0f / (param[2] - param[1]);
			pModel->GetRenderable(0)->GetMaterialInst()->SetVariableValue("vLightColor", pLight->cColor.Ptr());
			pModel->GetRenderable(0)->GetMaterialInst()->SetVariableValue("vLightParam", param);

		}

		const RenderOperator& op = pModel->GetRenderable(0)->GetRenderOperator();
		op.pMtlInst->Apply(0);
		GRenderDevice->ExecuteOperator(op);

		pLight = pLight->pNext;
	}

	GRenderDevice->RestoreFrameBuffer();
	GShaderManager.SetTexture(SV_GBUFFER0, NULL);
	GShaderManager.SetTexture(SV_GBUFFER1, NULL);
}

void RenderShadingPass(SceneView* pView)
{
	GRenderDevice->RestoreFrameBuffer();
	GShaderManager.SetTexture(SV_LIGHTINGBUFFER, renderInfo.pRTLighting);

	//GShaderManager.BindShadingPassShader();
	DrawingEntity* pEntity = pView->pDrawingEntity;
	while (pEntity)
	{
		RenderEntity(pEntity, 1);
		pEntity = pEntity->pNext;
	}

	GShaderManager.SetTexture(SV_LIGHTINGBUFFER, NULL);
}



//
//
//Vector4Transform(vec[0], vec[0], pView->matProj);	
//Vector4Transform(vec[1], vec[1], pView->matProj);
//
//
//GHelperRenderer.AddLine(vec[0].ProjectToVector3(), vec[1].ProjectToVector3(), color);





void Pipeline::RenderSceneView(SceneView* pView)
{
	SetupShaderVariables(pView);

	RenderGeomPass(pView);

	RenderLightingPass(pView);

	RenderShadingPass(pView);

	//RenderDebugPass(pView);
	
	GRenderDevice->RestoreFrameBuffer();
}


void Pipeline::BeginOcclusionQuery(int x0, int y0, int x1, int y1)
{
	m_pOcclusionQuery->BeginQuery(x0, y0, x1, y1);
	m_bOcclusionQuerying = true;
}

int Pipeline::EndOcclusionQuery()
{
	m_bOcclusionQuerying = false;
	return m_pOcclusionQuery->EndQuery();
}

void Pipeline::DrawLine(const Vector3f& a, const Vector3f& b, const Color& color, int pickupId)
{
	//m_pHelperRenderer->DrawLine(a, b, color, pickupId,
	//	m_bOcclusionQuerying ? m_pOcclusionQuery.get() : NULL);
}

void Pipeline::DrawLineList(const Vector3f* pPos, uint nNumPoint, const Color& color, int pickupId)
{
	//m_pHelperRenderer->DrawLineList(pPos, nNumPoint, color, pickupId,
	//	m_bOcclusionQuerying ? m_pOcclusionQuery.get() : NULL);
}

void Pipeline::DrawCicle(const Vector3f& center, float radius, const Vector3f& dir, const Color& color, int pickupId)
{
	//m_pHelperRenderer->DrawCicle(center, radius, dir, color, pickupId,
	//				m_bOcclusionQuerying ? m_pOcclusionQuery.get() : NULL);
}

void Pipeline::DrawAABB(const Vector3f& a, const Vector3f& b, const Color& color, int pickupId)
{
	//m_pHelperRenderer->DrawAABB(a, b, color, pickupId, NULL);
}

}
