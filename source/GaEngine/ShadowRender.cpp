#include "stdHeader.h"
#include "ShadowRender.h"
#include "RendererObject.h"
#include "Material.h"
#include "Renderer.h"
#include "MaterialInstance.h"
#include "Shader.h"
#include "Light.h"

namespace Aurora
{

	//void CalcDirectionalLightShadowMatrix(DirectionalLight* pShadowCastLight, Matrix4f& matCast, Matrix4f& matReceive)
	//{
	//	Quaternionf qRot = pShadowCastLight->GetParentNode()->GetWorldRotation();

	//	// right hand coord
	//	Vector3f z, x, y;
	//	Vector3f vEye = pShadowCastLight->GetParentNode()->GetWorldTranslation();

	//	Vector3Rotate(x, Vector3f::UNIT_X, qRot);
	//	Vector3Rotate(y, Vector3f::UNIT_Y, qRot);
	//	Vector3Rotate(z, Vector3f::UNIT_Z, qRot);

	//	Matrix4f matView;
	//	matView.Set(x.x, y.x, z.x, 0.0f,
	//		x.y, y.y, z.y, 0.0f,
	//		x.z, y.z, z.z, 0.0f,
	//		-DotProduct(x, vEye), -DotProduct(y, vEye), -DotProduct(z, vEye), 1.0f);

	//	Matrix4f matProj;
	//	MatrixOrthoRH(matProj, 100.f, 100.f, 0.01f, 500.f);
	//	//MatrixPerspectiveFovRH(matProj, m_fOutterCone, 1.0f, 1.0f, 500.0f);

	//	MatrixMultiply(matCast, matView, matProj);

	//	static Matrix4f matScale(0.5f,	0.0f,	0.0f,	0.0f,
	//							0.0f,	-0.5f,	0.0f,	0.0f,
	//							0.0f,	0.0f,	1.0f,	0.0f,
	//							0.5f,	0.5f,	0.0f,	1.0f);

	//	MatrixMultiply(matReceive, matCast, matScale);
	//}


	ShadowRendering::ShadowRendering()
	{
	}

	ShadowRendering::~ShadowRendering()
	{

	}

	void ShadowRendering::Init()
	{
		//m_pRTShadowMap = Global::GetResourceManager()->GetRenderTarget("ShadowMapRT");
		//m_pRTDummy = Global::GetResourceManager()->GetRenderTarget("ShadowMapColorRT");
		//m_pShadowMaterial = Global::GetResourceManager()->CreateMaterialInst("MtlShadowCast");
		//m_pMatShadowCastParam = m_pShadowMaterial->CreateShaderConstParam("matShadowViewProj");
	}

	void ShadowRendering::Render(RenderingContext* pContex)
	{
		//HALSetRenderTarget(0, m_pRTDummy.Get());
		//HALSetDepthStencil(m_pRTShadowMap.Get());
		//HALClear(IRenderDevice::CLEAR_DEPTH_BUFFER | IRenderDevice::CLEAR_FRAME_BUFFER, Color::BLACK);

		//m_pShadowMaterial->GetMaterialTemplate()->BeginPass(0);

		///**/StateValuePair renderState;
		//float bias = 0.001f;
		//uint ubias = *((uint*)&bias);
		//renderState.AddStateValue(D3DRS_DEPTHBIAS, ubias);
		//HALSetRenderState(renderState);



		//DirectionalLight* pShadowCastLight = NULL;

		//LightObject* pLight = (LightObject*)pContex->BeginAttachable(RenderingContext::AT_SHADOWCASTER);
		//while (pLight)
		//{
		//	LightObject::Type type = pLight->GetLightType();
		//	if (type == LightObject::LT_DIRECTIONAL)
		//	{
		//		pShadowCastLight = (DirectionalLight*)pLight;
		//		break;
		//	}

		//	pLight = (LightObject*)pContex->NextAttachable();
		//}

		//if (pShadowCastLight != NULL)
		//{
		//	Matrix4f matCast, matReceive;
		//	CalcDirectionalLightShadowMatrix(pShadowCastLight, matCast, matReceive);

		//	*m_pMatShadowCastParam = matCast;//pLight->GetViewProjMatrix();
		//	pShadowCastLight->SetShadowReceiveMatrix(matReceive);

		//	Renderable* pObj = (Renderable*)pContex->BeginAttachable(RenderingContext::AT_ORDINARY);
		//	while (pObj)
		//	{
		//		m_pShadowMaterial->UpdateShaderConst((ShaderConstProvider*)pObj->GetParentNode());
		//		RenderOperator op = pObj->GetRenderOperator();
		//		op.pMaterial = m_pShadowMaterial->GetMaterialTemplate();
		//		op.pMtlInst = m_pShadowMaterial.get();

		//		HALExecuteOperator(op);

		//		pObj = (Renderable*)pContex->NextAttachable();
		//	}
		//}




		/*vector<LightObject*>::iterator it = m_pContex->GetShadowCaster().begin();
		vector<LightObject*>::iterator itEnd = m_pContex->GetShadowCaster().end();

		Attachable* pLight = m_pContex

		for (; it != itEnd; ++it)
		{
		LightObject* pLight = (*it);
		*m_pLightViewProjParam = pLight->GetViewProjMatrix();

		set<Renderable*, MaterialCmp>::iterator it = m_pContex->GetMeshes().begin();
		set<Renderable*, MaterialCmp>::iterator itEnd = m_pContex->GetMeshes().end();

		for (; it != itEnd; ++it)
		{
		m_pShadowMaterial->UpdateShaderConst((ShaderConstProvider*)(*it)->GetParentNode());
		RenderOperator op = (*it)->GetRenderOperator();
		op.pMaterial = m_pShadowMaterial->GetMaterialTemplate();
		op.pMtlInst = m_pShadowMaterial.get();

		HALExecuteOperator(op);
		}

		}


		HALRestoreFrameBuffer();*/
	}
}