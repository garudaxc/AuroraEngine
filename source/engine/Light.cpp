#include "stdHeader.h"
#include "Light.h"
#include "CGeometry.h"
#include "Renderer.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "Model.h"
#include "LightNode.h"
#include "Config.h"

namespace Aurora
{

	extern IRenderDevice* GRenderDevice;
	static IRenderDevice* GetRenderer()
	{
		return GRenderDevice;
	}


	Model* CreateLightObject(LightData* pLightData)
	{
		Model* pLightObj = nullptr;
	/*	if (pLightData->lightType == LIGHTTYPE_DIRECTIONAL)
		{
			pLightObj = new Model(string("DirectionalLight"));
			string path = "asset:ScreenQuad.mesh";

			CGeometry* pGeometry = GMeshManager.GetMesh(path);
			pLightObj->SetMesh(pGeometry);


			MaterialTemplate* pMtlTemplate = MaterialTemplateManager::Get()->GetMaterialTemplate("DirectionalLight.fx");
			assert(pMtlTemplate);
			MaterialInstance* pMtlInstance = MaterialInstanceManager::Get()->CreateInstanceFromTemplate(pMtlTemplate);
			assert(pMtlInstance);

			pLightObj->SetMaterial(0, pMtlInstance);
		}*/

		return pLightObj;
	}


	Light::Light(ELightType lightType)
	{
		Type = lightType;

		cBaseColor = Color::WHITE;
		fMultiplier = 1.f;

		cShadowColor = Color::BLACK;

		fRange = 20.f;

		fInnerCone = 0.f;
		fOutterCone = 30.f;

		bCastShadow = 1;
	}



	//void LoadXmlLight(TiXmlNode* pXmlLight, SceneNode* pSceneNode)
	//{
	//	//LightObjectPtr pObject;

	//	//float fMultiplayer = 1.0f;
	//	//Color color = Color::WHITE;

	//	//TiXmlNode* pElem = pXmlLight->FirstChild();
	//	//while (pElem)
	//	//{
	//	//	if (strcmp(pElem->Value(), "Color") == 0)
	//	//	{
	//	//		string text = pElem->FirstChild()->ToText()->Value();
	//	//		color = ParseColor(text);
	//	//	}
	//	//	else if (strcmp(pElem->Value(), "Multiplier") == 0)
	//	//	{
	//	//		string text = pElem->FirstChild()->ToText()->Value();
	//	//		fMultiplayer = (float)atof(text.c_str());
	//	//	}

	//	//	pElem = pElem->NextSibling();
	//	//}

	//	//const char* type = pXmlLight->ToElement()->Attribute("Type");
	//	//if (strcmp(type, "Directional") == 0)
	//	//{
	//	//	pObject.reset(new DirectionalLight(color, fMultiplayer));
	//	//}
	//	//else if (strcmp(type, "Point") == 0)
	//	//{
	//	//	float attenStart = 20.0f, attenEnd = 80.0f;

	//	//	pElem = pXmlLight->FirstChild();
	//	//	while (pElem)
	//	//	{
	//	//		if (strcmp(pElem->Value(), "AttenStart") == 0)
	//	//		{
	//	//			string text = pElem->FirstChild()->ToText()->Value();
	//	//			attenStart = (float)atof(text.c_str());
	//	//		}
	//	//		else if (strcmp(pElem->Value(), "AttenEnd") == 0)
	//	//		{
	//	//			string text = pElem->FirstChild()->ToText()->Value();
	//	//			attenEnd = (float)atof(text.c_str());
	//	//		}

	//	//		pElem = pElem->NextSibling();
	//	//	}

	//	//	pObject.reset(new PointLight(color, fMultiplayer, attenStart, attenEnd));
	//	//}
	//	//else if (strcmp(type, "Spot") == 0)
	//	//{
	//	//	float hotSpot = 20.0f, fallOff = 40.0f;

	//	//	TiXmlNode* pElem = pXmlLight->FirstChild();
	//	//	while (pElem)
	//	//	{
	//	//		if (strcmp(pElem->Value(), "HotSpot") == 0)
	//	//		{
	//	//			string text = pElem->FirstChild()->ToText()->Value();
	//	//			hotSpot = (float)atof(text.c_str());
	//	//		}
	//	//		else if (strcmp(pElem->Value(), "FallOff") == 0)
	//	//		{
	//	//			string text = pElem->FirstChild()->ToText()->Value();
	//	//			fallOff = (float)atof(text.c_str());
	//	//		}

	//	//		pElem = pElem->NextSibling();
	//	//	}

	//	//	pObject.reset(new SpotLight(color, fMultiplayer, 
	//	//		hotSpot * Mathf::DEG_TO_RAD, fallOff * Mathf::DEG_TO_RAD));

	//	//}

	//	//const char* pAttr = pXmlLight->ToElement()->Attribute("CastShadow");
	//	//if (pAttr && atoi(pAttr) == 1)
	//	//{
	//	//	pObject->SetCastShadow(true);
	//	//}

	//	///*pAttr = pXmlLight->ToElement()->Attribute("ProjTex");
	//	//if (pAttr)
	//	//{
	//	//	pObject->SetProjTexture(ConvertFileName(pAttr));
	//	//}


	//	//pElem = pXmlLight->FirstChild("Controller");
	//	//if (pElem)
	//	//{
	//	//	LoadXmlLightController(pElem->ToElement(), pObject.get());
	//	//}*/

	//	//pObject->Init();

	//	//pSceneNode->Attach(pObject);
	//}

//
//
//LightObject::LightObject(Type lightType, const Color& baseColor, const float multiplier):
//Attachable(Attachable::TYPE_LIGHT),m_nType(lightType),m_cBaseColor(baseColor),m_bCastShadow(false),
//m_fMultiplier(multiplier),m_pOP(new RenderOperator())
//{
//	m_cFinalColor = m_cBaseColor * m_fMultiplier;
//}
//
//
//
//LightObject::~LightObject()
//{
//}
//
//
//void LightObject::CreateRenderable(const string& geomName, const string& mtlName)
//{
//	//ResourcePtr<CGeometry> pGeomData = Global::GetResourceManager()->GetGeometryData(geomName);
//	//m_pVertDesc = Global::GetResourceManager()->GetVertexDesc(ResourceManager::VEXDESC_P).Get();
//
//	//assert(0);
//	//// �޸�ʵ��
//	////m_pVertexBuffer = pGeomData->GetVertexBuffer();
//	//m_pIndexBuffer = pGeomData->GetIndexBuffer();
//
//	//m_pMatInst = Global::GetResourceManager()->GetMaterial(mtlName)->CreateMaterialInstance();
//
//	//BuildRenderOperator(pGeomData.Get());
//
//	//m_ShaderConstLightColor = m_pMatInst->CreateShaderConstParam("LightColor");
//
//	//if (m_strProjTexName == "")
//	//{
//	//	m_strProjTexName = GetResourcePath() + "asset/default_albedo.dds";
//	//}
//	//m_pMatInst->SetTexture("TexProj", Global::GetResourceManager()->GetTexture(m_strProjTexName));
//	//m_pTexProjParam = m_pMatInst->CreateShaderConstParam("MatTexProj");
//}
//
//
//
//void LightObject::BuildRenderOperator(CGeometry* pGeom)
//{
//	m_pOP->nBaseVertexIndex = 0;
//	m_pOP->nNumVertices = pGeom->GetNumVertex();
//	m_pOP->nPrimitiveCount = pGeom->GetNumTri();
//	m_pOP->nPrimType = RenderOperator::PT_TRIANGLELIST;
//	m_pOP->nStartIndex = 0;
//	m_pOP->pIndexBuffer = m_pIndexBuffer;
//	m_pOP->pVertexBuffer = m_pVertexBuffer;
//	m_pOP->pVertDesc = m_pVertDesc;
//}
//
//
//const RenderOperator& LightObject::GetRenderOperator() const
//{
//	//m_pOP->pMaterial = m_pMatInst->GetMaterialTemplate();
//	//m_pOP->pMtlInst = m_pMatInst.get();
//	return *m_pOP;
//}
//
//void LightObject::SetProjTexture(const string& projTexName)
//{
//	m_strProjTexName = projTexName;
//}
//
//void LightObject::AddBaseColorController(const ControllerPtr& pController)
//{
//	m_Controllers.push_back(pController);
//
//	pController->BindFloats(m_cBaseColor.Ptr(), 4);
//}
//
//
//void LightObject::AddMultiplierController(const ControllerPtr& pController)
//{
//	m_Controllers.push_back(pController);
//
//	pController->BindFloats(&m_fMultiplier, 1);
//}
//
//void LightObject::UpdateController(float dt)
//{
//	for (vector<ControllerPtr>::iterator it = m_Controllers.begin();
//		it != m_Controllers.end(); ++it)
//	{
//		(*it)->AdvanceTime(dt);
//	}
//
//}
//
//
//bool LightObject::TestIlluminating() const
//{
//	return m_fMultiplier > 0.01f;
//}
//
//
////////////////////////////////////////////////////////////////////////////
//
//DirectionalLight::DirectionalLight(const Color& color, float multiplier):
//LightObject(LightObject::LT_DIRECTIONAL, color, multiplier)
//{
//}
//
//DirectionalLight::~DirectionalLight()
//{
//}
//
//bool DirectionalLight::Init()
//{
//	//CreateRenderable("ScreenQuad", "MatDirectionalLight");
//
//	//m_pShadowMapParam = m_pMatInst->CreateShaderConstParam("MatShadowProj");
//
//
//	return true;
//}
//
//
//bool DirectionalLight::TestInside(const Vector3f& pos)
//{
//	return true;
//}
//
//void DirectionalLight::UpdateFromParentNode()
//{
//	//GetMaterialInst()->UpdateShaderConst((ShaderConstProvider*)GetParentNode());
//
//	//m_cFinalColor = m_cBaseColor * m_fMultiplier;
//	//*m_ShaderConstLightColor = m_cFinalColor;
//
//	//Quaternionf qRot = GetParentNode()->GetWorldRotation();
//	//Vector3Rotate(m_vDirection, Vector3f::UNIT_Z, qRot);
//}
//
//Matrix4f DirectionalLight::GetViewProjMatrix() const
//{
//	//// right hand coord
//	//Vector3f z, x, y;
//	//Vector3f vEye = m_vPosition;
//
//	//Vector3Rotate(x, Vector3f::UNIT_X, qRot);
//	//Vector3Rotate(y, Vector3f::UNIT_Y, qRot);
//	//Vector3Rotate(z, Vector3f::UNIT_Z, qRot);
//
//	//Matrix4f matView;
//	//matView.Set(x.x, y.x, z.x, 0.0f,
//	//	x.y, y.y, z.y, 0.0f,
//	//	x.z, y.z, z.z, 0.0f,
//	//	-DotProduct(x, vEye), -DotProduct(y, vEye), -DotProduct(z, vEye), 1.0f);
//
//	//Matrix4f matProj;
//	//MatrixPerspectiveFovRH(matProj, m_fOutterCone, 1.0f, 1.0f, 500.0f);
//
//	//MatrixMultiply(m_matViewProj, matView, matProj);
//
//	return Matrix4f::IDENTITY;
//}
//
//void DirectionalLight::ApplyShadow()
//{
//	//*m_pShadowMapParam = m_matShadowReceive;
//}
//
//
////////////////////////////////////////////////////////////////////////////
//
//
//PointLight::PointLight(const Color& color, float multiplier, float fAttenStart, float fAttenEnd):
//LightObject(LightObject::LT_POINT, color, multiplier),m_fAttenStart(fAttenStart),m_fAttenEnd(fAttenEnd)
//{
//
//}
//
//PointLight::~PointLight()
//{
//
//}
//
//bool PointLight::Init()
//{
//	//CreateRenderable("PointLight", "MatPointLight");
//
//	//Vector3f param(m_fAttenStart, m_fAttenEnd, 0.0f);
//	//m_pMatInst->SetValueF("PointLightParam", param.Ptr(), 3);
//
//	return true;
//}
//
//bool PointLight::TestInside(const Vector3f& pos)
//{
//	float dist = (pos - m_vPosition).Length();
//
//	return dist < m_fAttenEnd;
//}
//
//void PointLight::UpdateFromParentNode()
//{
//	//GetMaterialInst()->UpdateShaderConst((ShaderConstProvider*)GetParentNode());
//	//m_cFinalColor = m_cBaseColor * m_fMultiplier;
//	//*m_ShaderConstLightColor = m_cFinalColor;
//
//	//m_vPosition = GetParentNode()->GetWorldTranslation();
//
//	//Quaternionf qRot = GetParentNode()->GetWorldRotation();
//
//	//Matrix4f matTexProj;
//	//MatrixFromQuaternion(matTexProj, qRot);
//	//*m_pTexProjParam = matTexProj;
//}
//
//
////////////////////////////////////////////////////////////////////////////
//
//SpotLight::SpotLight(const Color& color, float multiplier, float fInnerCone, float fOutterCone):
//LightObject(LightObject::LT_SPOT, color, multiplier),m_fInnerCone(fInnerCone),
//m_fOutterCone(fOutterCone),m_fCosHalfOutAngle(0.0f)
//{
//}
//
//SpotLight::~SpotLight()
//{
//
//}
//
//
//bool SpotLight::Init()
//{
//	//CreateRenderable("cone", "MatSpotLight");
//
//	//float param[4];
//	//float halfInner = m_fInnerCone * 0.5f;
//	//float halfOutter = m_fOutterCone * 0.5f;
//	//// x : tangent of half outter angle
//	//// y : cosine of half outter angle
//	//// z : cosine of half inner angle
//	//// w : 1 / (z - y)
//	//param[0] = Mathf::Tan(halfOutter);
//	//param[1] = Mathf::Cos(halfOutter);
//	//param[2] = Mathf::Cos(halfInner);
//	//param[3] = 1.0f / (param[2] - param[1]);
//	//m_pMatInst->SetValueF("SpotLightParam", param, 4);
//
//	//m_fCosHalfOutAngle = Mathf::Cos(halfOutter);
//
//	//m_pShadowMapParam = m_pMatInst->CreateShaderConstParam("MatShadowProj");
//
//	return true;
//}
//
//bool SpotLight::TestInside(const Vector3f& pos)
//{
//	Vector3f dir = m_vPosition - pos;
//	dir.Normalize();
//
//	return (DotProduct(dir, m_vDirection) > m_fCosHalfOutAngle);
//}
//
//void SpotLight::UpdateFromParentNode()
//{
//	//GetMaterialInst()->UpdateShaderConst((ShaderConstProvider*)GetParentNode());
//	//m_cFinalColor = m_cBaseColor * m_fMultiplier;
//	//*m_ShaderConstLightColor = m_cFinalColor;
//
//
//	//m_vPosition = GetParentNode()->GetWorldTranslation();
//
//	//Quaternionf qRot = GetParentNode()->GetWorldRotation();
//	//Vector3Rotate(m_vDirection, Vector3f::UNIT_Z, qRot);
//
//
//	//// right hand coord
//	//Vector3f z, x, y;
//	//Vector3f vEye = m_vPosition;
//
//	//Vector3Rotate(x, Vector3f::UNIT_X, qRot);
//	//Vector3Rotate(y, Vector3f::UNIT_Y, qRot);
//	//Vector3Rotate(z, Vector3f::UNIT_Z, qRot);
//
//	//Matrix4f matView;
//	//matView.Set(x.x, y.x, z.x, 0.0f,
//	//	x.y, y.y, z.y, 0.0f,
//	//	x.z, y.z, z.z, 0.0f,
//	//	-DotProduct(x, vEye), -DotProduct(y, vEye), -DotProduct(z, vEye), 1.0f);
//
//	//Matrix4f matProj;
//	//MatrixPerspectiveFovRH(matProj, m_fOutterCone, 1.0f, 1.0f, 500.0f);
//
//	//MatrixMultiply(m_matViewProj, matView, matProj);
//
//	//static Matrix4f matScale(  0.5f,	0.0f,	0.0f,	0.0f,
//	//							0.0f,	-0.5f,	0.0f,	0.0f,
//	//							0.0f,	0.0f,	1.0f,	0.0f,
//	//							0.5f,	0.5f,	0.0f,	1.0f);
//
//	//Matrix4f matTexProj;
//	//MatrixMultiply(matTexProj, m_matViewProj, matScale);
//
//	//*m_pTexProjParam = matTexProj;
//
//	//if (m_bCastShadow)
//	//{
//	//	*m_pShadowMapParam = matTexProj;
//	//}
//}
//
//Matrix4f SpotLight::GetViewProjMatrix() const
//{
//	return m_matViewProj;
//}
//




LightRendering::LightRendering()
{
}

LightRendering::~LightRendering()
{

}

void LightRendering::Initialize()
{
}

void LightRendering::Render(LightRenderingInfo* renderInfo)
{
	//RenderTarget* rt[] = {renderInfo->pLightingRT};
	//RenderTarget* pDepthBuffer = GetRenderer()->GetDepthStencil();
	//GetRenderer()->SetRenderTarget(1, rt, pDepthBuffer);

	//GetRenderer()->Clear(IRenderDevice::CLEAR_FRAME_BUFFER, Color::ZERO);


	//const vector<RenderStrategy*>& lights = *renderInfo->lights;
	//for (vector<RenderStrategy*>::const_iterator it = lights.begin();
	//	it != lights.end(); ++it)
	//{
	//	RenderStrategy* pRenderStrategy = *it;

	//	uint nNumRenderable = pRenderStrategy->GetNumRenderable();
	//	for (uint i = 0; i < nNumRenderable; i++)
	//	{
	//		const RenderOperator& op = pRenderStrategy->GetRenderable(i)->GetRenderOperator();
	//		op.pMtlInst->Apply(0);
	//		GetRenderer()->ExecuteOperator(op);
	//	}
	//}




	//HALSetRenderTarget(0, m_pRTLighting.Get());
	//HALClear(IRenderDevice::CLEAR_FRAME_BUFFER, Color::ZERO);

	//*StateValuePair renderState;
	//renderState.AddStateValue(m_nCullState, m_nCullCCW);
	//renderState.AddStateValue(m_nZFunc, m_nCmpGreater);
	//HALSetRenderState(renderState);*/
	//

	//RenderingStatistics* pStatic = pContex->GetStatistic();
	//LightObject* pLight = (LightObject*)pContex->BeginAttachable(RenderingContext::AT_LIGHT);
	//while (pLight)
	//{
	//	if (!pLight->TestIlluminating())
	//	{
	//		continue;
	//	}

	//	pStatic->nNumLights++;

	//	pLight->GetMaterialInst()->GetMaterialTemplate()->BeginPass(0);

	//	if (pLight->IsCastShadow())
	//	{
	//		pLight->ApplyShadow();
	//	}

	//	LightObject::Type type = pLight->GetLightType();


	//	

	//	RenderOperator op = pLight->GetRenderOperator();
	//	HALExecuteOperator(op);

	//	pLight = (LightObject*)pContex->NextAttachable();
	//}

	
}



}