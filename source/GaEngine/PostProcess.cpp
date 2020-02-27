#include "stdheader.h"
#include "PostProcess.h"
#include "Material.h"


namespace Aurora
{

	PostProcessRendering::PostProcessRendering()
	{

	}

	PostProcessRendering::~PostProcessRendering()
	{

	}

	void PostProcessRendering::Init()
	{
		//m_pMatInst = Global::GetResourceManager()->CreateMaterialInst("MtlPostProcess");
		//m_pBlurHInst = Global::GetResourceManager()->CreateMaterialInst("MtlBlurH");
		//m_pBlurVInst = Global::GetResourceManager()->CreateMaterialInst("MtlBlurV");

		////m_pMatInst->SetTexture("texUI", Global::GetResourceManager()->GetTexture(m_strTexture));

		//m_VertBuff.resize(4);
		//m_Index.resize(3 * 2);

		//m_VertBuff[0].pos.Set(-1.0f,  1.0f, 0.0f);
		//m_VertBuff[1].pos.Set(-1.0f, -1.0f, 0.0f);
		//m_VertBuff[2].pos.Set( 1.0f,  1.0f, 0.0f);
		//m_VertBuff[3].pos.Set( 1.0f, -1.0f, 0.0f);

		//m_VertBuff[0].uv.Set(0.0f, 0.0f);
		//m_VertBuff[1].uv.Set(0.0f, 1.0f);
		//m_VertBuff[2].uv.Set(1.0f, 0.0f);
		//m_VertBuff[3].uv.Set(1.0f, 1.0f);

		//m_Index[0] = 0;
		//m_Index[1] = 1;
		//m_Index[2] = 2;

		//m_Index[3] = 2;
		//m_Index[4] = 1;
		//m_Index[5] = 3;

		//m_op.pSystemVertBuff = (float*)&m_VertBuff[0];
		//m_op.pSystemIndexBuff = (ushort*)&m_Index[0];
		//m_op.pVertDesc = Global::GetResourceManager()->GetVertexDesc(ResourceManager::VEXDESC_PT).Get();
		//m_op.nPrimType = RenderOperator::PT_TRIANGLELIST;
		//m_op.nBaseVertexIndex = 0;
		//m_op.nMinVertexIndex = 0;
		//m_op.nNumVertices = (uint)m_VertBuff.size();
		//m_op.nStartIndex = 0;;
		//m_op.nPrimitiveCount = 2;
		//m_op.nVertexStride = sizeof(VertexPT);

	}

	void PostProcessRendering::Render(RenderingContext* pContex)
	{
		//HALRestoreFrameBuffer();
		//HALClear(IRenderDevice::CLEAR_FRAME_BUFFER, Color::BLACK);	


		//m_op.pMaterial = m_pMatInst->GetMaterialTemplate();
		//m_op.pMtlInst = m_pMatInst.get();
		//m_op.pMaterial->BeginPass(0);
		//HALExecuteOperator(m_op);



		////m_op.pMaterial = m_pBlurHInst->GetMaterialTemplate();
		////m_op.pMtlInst = m_pBlurHInst.get();
		////m_op.pMaterial->BeginPass(0);
		////HALExecuteOperator(m_op);

		////m_op.pMaterial = m_pBlurVInst->GetMaterialTemplate();
		////m_op.pMtlInst = m_pBlurVInst.get();
		////m_op.pMaterial->BeginPass(0);
		////HALExecuteOperator(m_op);

	}

	


}