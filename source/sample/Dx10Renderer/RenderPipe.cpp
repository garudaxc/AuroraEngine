#include "stdheader.h"
#include "GaRenderer.h"
#include "GaCamera.h"
#include "GaUtil.h"
#include "GaPostProcess.h"
#include "GaDeferredLightingRender.h"
#include "GaShadowRender.h"
#include "GaOcclusionQuery.h"
#include "GaHelperRenderer.h"
#include "GaEntity.h"
#include "GaRendererDX10.h"
#include "GaSceneView.h"
#include "GaMaterial.h"

#include "RenderPipe.h"



	static Renderer* GetRenderer()
	{
		return Global::GetRenderer();
	}


	RenderPipe::RenderPipe(void)
	{
	}

	RenderPipe::~RenderPipe(void)
	{
	}

	void RenderPipe::BeginPipe()
	{
		GetRenderer()->BeginScene();
	}

	void RenderPipe::EndPipe()
	{
		GetRenderer()->EndScene();

		GetRenderer()->Present();
	}


	void RenderPipe::Init()
	{
		m_pShaderVarManager = new GlobalShaderVarManager();
		m_pShaderVarManager->Initialize();

		m_pSVCameraParam = m_pShaderVarManager->FindShaderVariable("CameraParam");
		assert(m_pSVCameraParam);

		m_pSVMatrixViewProj = m_pShaderVarManager->FindShaderVariable("MatViewProj");
		assert(m_pSVMatrixViewProj);

	}


	void RenderPipe::SetupShaderVariables(Camera* pCamera)
	{
		m_pSVMatrixViewProj->SetValue((void*)pCamera->GetViewProjMatrix().Ptr());

		const Matrix4f& matProj = pCamera->GetProjMatrix();
		float fCameraParam[] = {pCamera->GetNearPlane(), pCamera->GetFarPlane(),
			1.0f / matProj._11, 1.0f / matProj._22};
		m_pSVCameraParam->SetValue(fCameraParam);
	}

	void RenderPipe::RenderEntity(RenderStrategy* pRenderStrategy, uint nTechnique)
	{
		const Matrix4f& matWorld = pRenderStrategy->GetWorldTransform();
		MaterialInstanceManager::Get()->GetShaderVariableStrategy()->SetWorldMatrix(matWorld);

		uint nNumRenderable = pRenderStrategy->GetNumRenderable();
		for (uint i = 0; i < nNumRenderable; i++)
		{
			const RenderOperator& op = pRenderStrategy->GetRenderable(i)->GetRenderOperator();
			op.pMtlInst->Apply(nTechnique);
			GetRenderer()->ExecuteOperator(op);
		}
	}

	void RenderPipe::Render(RenderingContext* pContex)
	{
		const vector<RenderStrategy*>& models = pContex->GetModels();

		for (vector<RenderStrategy*>::const_iterator it = models.begin();
			it != models.end(); ++it)
		{
			RenderEntity(*it, 0);
		}
	}


	void RenderPipe::RenderSceneView(SceneView* pView)
	{
		RenderingContext context;

		pView->CollectRenderingObjects(&context);

		Camera* pCamera = context.GetCamera();

		SetupShaderVariables(pCamera);

		Render(&context);

	}

