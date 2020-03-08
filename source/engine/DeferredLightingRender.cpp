#include "stdHeader.h"
#include "DeferredLightingRender.h"
#include "RendererObject.h"
#include "Material.h"
#include "Renderer.h"
#include "MaterialInstance.h"
#include "Shader.h"
#include "Camera.h"
#include "Renderer.h"
#include "OcclusionQuery.h"
#include "Entity.h"
#include "Light.h"

namespace Aurora
{


	GBufferRendering::GBufferRendering()
	{
		
	}

	GBufferRendering::~GBufferRendering()
	{

	}

	void GBufferRendering::Init()
	{
		//m_pRTNormal = Global::GetResourceManager()->GetRenderTarget("NormalRT");
		//m_pDepth = Global::GetResourceManager()->GetRenderTarget("DepthRT");
	}

	void GBufferRendering::Render(RenderingContext* pContex)
	{
		//HALSetRenderTarget(0, m_pRTNormal.Get());
		//HALSetRenderTarget(1, m_pDepth.Get());
		//HALClear(IRenderDevice::CLEAR_DEPTH_BUFFER | IRenderDevice::CLEAR_FRAME_BUFFER, Color::BLACK);

		//{
		//	Renderable* pObj = (Renderable*)pContex->BeginAttachable(RenderingContext::AT_ORDINARY);
		//	while (pObj)
		//	{
		//		pObj->GetMaterialInst()->GetMaterialTemplate()->BeginPass(0);

		//		RenderOperator op = pObj->GetRenderOperator();
		//		HALExecuteOperator(op);

		//		pObj = (Renderable*)pContex->NextAttachable();
		//	}
		//}

		//{
		//	Renderable* pObj = (Renderable*)pContex->BeginAttachable(RenderingContext::AT_TRANSPARENT);
		//	while (pObj)
		//	{
		//		pObj->GetMaterialInst()->GetMaterialTemplate()->BeginPass(0);

		//		RenderOperator op = pObj->GetRenderOperator();
		//		HALExecuteOperator(op);

		//		pObj = (Renderable*)pContex->NextAttachable();
		//	}

		//}

		//HALRestoreFrameBuffer();
	}



//////////////////////////////////////////////////////////////////////////
	ShadingRendering::ShadingRendering()
	{

	}

	ShadingRendering::~ShadingRendering()
	{

	}

	void ShadingRendering::Init()
	{
		//m_pRTColor = Global::GetResourceManager()->GetRenderTarget("ColorRT");
	}

	void ShadingRendering::Render(RenderingContext* pContex, OcclusionQuery* pQuery)
	{
		//HALSetRenderTarget(0, m_pRTColor.Get());
		//HALClear(IRenderDevice::CLEAR_FRAME_BUFFER, Color::BLACK);
		//pContex->GetStatistic()->nNumObj += pContex->GetNumAttachables(RenderingContext::AT_ORDINARY);
		//pContex->GetStatistic()->nNumObj += pContex->GetNumAttachables(RenderingContext::AT_TRANSPARENT);


		//RenderingStatistics* pStatic = pContex->GetStatistic();
		//{
		//	Renderable* pObj = (Renderable*)pContex->BeginAttachable(RenderingContext::AT_ORDINARY);
		//	while (pObj)
		//	{
		//		pObj->GetMaterialInst()->GetMaterialTemplate()->BeginPass(1);

		//		RenderOperator op = pObj->GetRenderOperator();

		//		if (pQuery)
		//		{
		//			pQuery->BeginObject(pObj->GetOcclusionId());
		//			HALExecuteOperator(op);
		//			pQuery->EndObject();
		//		}
		//		else
		//		{
		//			HALExecuteOperator(op);
		//		}

		//		pStatic->nNumPrimitive += op.nPrimitiveCount;

		//		pObj = (Renderable*)pContex->NextAttachable();
		//	}
		//}

		//{
		//	Renderable* pObj = (Renderable*)pContex->BeginAttachable(RenderingContext::AT_TRANSPARENT);
		//	while (pObj)
		//	{
		//		pObj->GetMaterialInst()->GetMaterialTemplate()->BeginPass(1);

		//		RenderOperator op = pObj->GetRenderOperator();
		//		HALExecuteOperator(op);
		//		pStatic->nNumPrimitive += op.nPrimitiveCount;

		//		pObj = (Renderable*)pContex->NextAttachable();
		//	}

		//}

		//HALRestoreFrameBuffer();
	}



//////////////////////////////////////////////////////////////////////////


	static IRenderDevice* GetRenderer()
	{
		return GRenderDevice;
	}


	PPLRendering::PPLRendering()
	{

	}

	PPLRendering::~PPLRendering()
	{

	}

	void PPLRendering::Initialize(ShaderManager* pSVManager)
	{
		RenderTarget::Desc desc;

		desc.nWidth		= RenderTarget::DS_FRAMEBUFFER_WIDTH;
		desc.nHeight	= RenderTarget::DS_FRAMEBUFFER_HEIGHT;
		desc.nUsage		= Surface::USAGE_RENDERTARGET;
		desc.nFormat	= Surface::FMT_R8G8B8A8;

		m_pRTGbuffer0 = GetRenderer()->CreateRenderTarget(desc);
		m_pRTGbuffer1 = GetRenderer()->CreateRenderTarget(desc);

		desc.nWidth		= RenderTarget::DS_FRAMEBUFFER_WIDTH;
		desc.nHeight	= RenderTarget::DS_FRAMEBUFFER_HEIGHT;
		desc.nUsage		= Surface::USAGE_RENDERTARGET;
		desc.nFormat	= Surface::FMT_A16B16G16R16F;

		m_pRTLighting = GetRenderer()->CreateRenderTarget(desc);

		m_pLightRendering = new LightRendering();
		m_pLightRendering->Initialize();
	}

	void PPLRendering::Finalize()
	{

	}

	void RenderEntity(RenderStrategy* pRenderStrategy, uint nTechnique)
	{
		//const Matrix4f& matWorld = pRenderStrategy->GetWorldTransform();
		//MaterialInstanceManager::Get()->GetShaderVariableStrategy()->SetWorldMatrix(matWorld);

		//uint nNumRenderable = pRenderStrategy->GetNumRenderable();
		//for (uint i = 0; i < nNumRenderable; i++)
		//{
		//	const RenderOperator& op = pRenderStrategy->GetRenderable(i)->GetRenderOperator();
		//	op.pMtlInst->Apply(nTechnique);
		//	GetRenderer()->ExecuteOperator(op);
		//}
	}

	void PPLRendering::Render(RenderingContext* pContex)
	{
		RenderGBufferPass(pContex);
		RenderLightingPass(pContex);
		RenderShadingPass(pContex);
	}

	void PPLRendering::RenderGBufferPass(RenderingContext* pContex)
	{
		//RenderTarget* rt[] = {m_pRTGbuffer0, m_pRTGbuffer1};
		//RenderTarget* pDepthBuffer = GetRenderer()->GetDepthStecil();
		//GetRenderer()->SetRenderTarget(2, rt, pDepthBuffer);

		//const vector<RenderStrategy*>& models = pContex->GetModels();

		//for (vector<RenderStrategy*>::const_iterator it = models.begin();
		//	it != models.end(); ++it)
		//{
		//	RenderEntity(*it, 0);
		//}
	}

	void PPLRendering::RenderLightingPass(RenderingContext* pContex)
	{
		//LightRenderingInfo info;
		//info.pLightingRT = m_pRTLighting;
		//info.lights = &pContex->GetLights();

		//m_pLightRendering->Render(&info);


	/*	RenderTarget* rt[] = {m_pRTLighting};
		RenderTarget* pDepthBuffer = GetRenderer()->GetDepthStecil();
		GetRenderer()->SetRenderTarget(1, rt, pDepthBuffer);

		const vector<RenderStrategy*>& lights = pContex->GetLights();

		for (vector<RenderStrategy*>::const_iterator it = lights.begin();
			it != lights.end(); ++it)
		{
			RenderEntity(*it, 0);
		}*/

	}

	void PPLRendering::RenderShadingPass(RenderingContext* pContex)
	{
		//RenderTarget* rt[] = {GetRenderer()->GetFrameBuffer()};
		//RenderTarget* pDepthBuffer = GetRenderer()->GetDepthStecil();
		//GetRenderer()->SetRenderTarget(1, rt, pDepthBuffer);

		//const vector<RenderStrategy*>& models = pContex->GetModels();

		//for (vector<RenderStrategy*>::const_iterator it = models.begin();
		//	it != models.end(); ++it)
		//{
		//	RenderEntity(*it, 1);
		//}

	}



}