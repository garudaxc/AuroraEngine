#include "stdheader.h"
#include "SimpleRendering.h"
#include "Renderer.h"
#include "Camera.h"
#include "Util.h"
#include "Entity.h"
#include "Renderer.h"
#include "SceneView.h"
#include "Material.h"
#include "Model.h"
#include "Texture.h"
#include "RenderState.h"

namespace Aurora
{

	ModelShaderVS modelvs_;
	ModelShaderPS modelps_;

	RasterizerState<true> rasterState;


	SimpleRendering GSimpleRendering;




	Texture* pTexture = nullptr; 
	
	void SimpleRendering::Initialize()
	{
		modelvs_.Initialize();
		modelps_.Initialize();
		rasterState.CreateDeviceObject();


		pTexture =  GTextureManager.GetTexture("resource:old/c_08.bmp");
	}

	void SimpleRendering::RenderSceneView(SceneView* view)
	{

		GRenderDevice->Clear(IRenderDevice::CLEAR_FRAME_BUFFER | IRenderDevice::CLEAR_DEPTH_BUFFER, Color(0.2f, 0.2f, 0.2f, 0.2f));


		modelvs_.matViewProj = view->matViewProj;
		modelvs_.matViewProj.TransposeSelf();

		modelvs_.BindShader();
		modelps_.BindShader();
		modelps_.BindTextureToRender(pTexture);
		rasterState.BindToDevice();

		DrawingEntity* pEntity = view->pDrawingEntity;
		while (pEntity)
		{
			modelvs_.matWorld = pEntity->mWorld;
			modelvs_.matWorld.TransposeSelf();

			modelvs_.CommitShaderParameter();
			modelps_.CommitShaderParameter();

			const Entity* pRenderEntity = pEntity->pEntity;
			uint nNumRenderable = pRenderEntity->GetNumRenderable();
			for (uint i = 0; i < nNumRenderable; i++)
			{
				const RenderOperator& op = pRenderEntity->GetRenderable(i)->GetRenderOperator();
				//op.pMtlInst->Apply(nTechnique);
				//op.pMtlInst->Commit();
				GRenderDevice->ExecuteOperator(op);
			}

			pEntity = pEntity->pNext;
		}

		GRenderDevice->Present();

	}



}