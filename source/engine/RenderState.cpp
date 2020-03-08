#include "RenderState.h"
#include "Log.h"

#include <d3d11.h>
#include <DXGI.h>
#include <D3Dcompiler.h>

#include "Renderer.h"

namespace Aurora
{


	extern ID3D11Device* D3D11Device;
	extern ID3D11DeviceContext* ImmediateContext;



	

	D3D11_BLEND MapBlendFactor(BlendFactor factor)
	{
		static D3D11_BLEND table[] = { D3D11_BLEND_ZERO, D3D11_BLEND_ONE, D3D11_BLEND_SRC_COLOR,
			D3D11_BLEND_INV_SRC_COLOR,D3D11_BLEND_DEST_COLOR, D3D11_BLEND_INV_DEST_COLOR,
			D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_DEST_ALPHA,
			D3D11_BLEND_INV_DEST_ALPHA, D3D11_BLEND_BLEND_FACTOR, D3D11_BLEND_INV_BLEND_FACTOR };

		return table[(int)factor];
	}


	D3D11_BLEND_OP MapBlendOp(BlendOp op)
	{
		static D3D11_BLEND_OP table[] = { D3D11_BLEND_OP_ADD, D3D11_BLEND_OP_SUBTRACT,
			D3D11_BLEND_OP_REV_SUBTRACT, D3D11_BLEND_OP_MIN, D3D11_BLEND_OP_MAX };

		return table[(int)op];
	}


	D3D11_CULL_MODE MapCullMode(CullMode mode)
	{
		static D3D11_CULL_MODE table[] = { D3D11_CULL_FRONT, D3D11_CULL_BACK, D3D11_CULL_NONE };
		return table[(int)mode];
	}


	D3D11_COMPARISON_FUNC MapCmpFunc(CompareFunc fuc)
	{
		static D3D11_COMPARISON_FUNC table[] = { D3D11_COMPARISON_NEVER,
			D3D11_COMPARISON_EQUAL, D3D11_COMPARISON_NOT_EQUAL, D3D11_COMPARISON_LESS,
			D3D11_COMPARISON_LESS_EQUAL, D3D11_COMPARISON_GREATER,
			D3D11_COMPARISON_GREATER_EQUAL, D3D11_COMPARISON_ALWAYS };
		return table[(int)fuc];
	}

	D3D11_FILL_MODE MapFillMode(FillMode mode)
	{
		static D3D11_FILL_MODE table[] = { D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID };
		return table[(int)mode];
	}


	D3D11_STENCIL_OP MapStencilOp(StencilOp op)
	{
		static D3D11_STENCIL_OP table[] = { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_ZERO,
			D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_INCR_SAT, D3D11_STENCIL_OP_DECR_SAT,
			D3D11_STENCIL_OP_INVERT, D3D11_STENCIL_OP_INCR, D3D11_STENCIL_OP_DECR };

		return table[(int)op];
	}


	enum class RenderStateFlag : uint8
	{
		BLEND = 0x1,
		DEPTH_STENCIL = 0x2,
		RASTERIZER = 0x4
	};

	struct RenderStateObject
	{
		int32	Flag = 0;
		union {
			ID3D11BlendState* BlendState;
			ID3D11DepthStencilState* DepthStencilState;
			ID3D11RasterizerState* RasterizerState;
		};
	};




	static vector<RenderStateObject*>	RenderStateObjectList;

	Handle	CreateBlendState(bool BlendEnable, BlendFactor SrcBlend, BlendFactor DestBlend,
		BlendOp  op, uint8 WriteMask, bool AlphaToCoverageEnable)
	{
		D3D11_BLEND_DESC desc = { 0 };
		desc.AlphaToCoverageEnable		= AlphaToCoverageEnable;
		desc.IndependentBlendEnable		= FALSE;
		desc.RenderTarget[0].BlendEnable	= (BOOL)BlendEnable;
		desc.RenderTarget[0].SrcBlend		= MapBlendFactor(SrcBlend);
		desc.RenderTarget[0].DestBlend		= MapBlendFactor(DestBlend);
		desc.RenderTarget[0].BlendOp		= MapBlendOp(op);

		// 暂时不支持alpha通道单独混合方式
		desc.RenderTarget[0].SrcBlendAlpha	= MapBlendFactor(SrcBlend);
		desc.RenderTarget[0].DestBlendAlpha	= MapBlendFactor(DestBlend);
		desc.RenderTarget[0].BlendOpAlpha	= MapBlendOp(op);
		desc.RenderTarget[0].RenderTargetWriteMask = WriteMask & D3D11_COLOR_WRITE_ENABLE_ALL;

		ID3D11BlendState* blendState = nullptr;
		if (FAILED(D3D11Device->CreateBlendState(&desc, &blendState))) {
			GLog->Error("create d3d11 blend state object failed!");
			return -1;
		}

		RenderStateObject* obj = new RenderStateObject();
		obj->Flag = (int)RenderStateFlag::BLEND;
		obj->BlendState = blendState;

		int slot = FindAvailableSlot(RenderStateObjectList);
		RenderStateObjectList[slot] = obj;

		return slot;
	}


	void BindBlendState(Handle handle, const Color& BlendFactor)
	{
		RenderStateObject* obj = RenderStateObjectList[handle];
		assert(obj->Flag == (int)RenderStateFlag::BLEND);
		ImmediateContext->OMSetBlendState(obj->BlendState, BlendFactor.Ptr(), 0xffffffff);
	}

	Handle	CreateRasterizerState(bool FrontCounterClockwise, CullMode cullMode,
		FillMode fillMode, int DepthBias, int SlopeScaledDepthBias, bool AntialiasedLineEnable)
	{
		D3D11_RASTERIZER_DESC desc = { 0 };
		desc.FillMode	= MapFillMode(fillMode);
		desc.CullMode = MapCullMode(cullMode);
		desc.FrontCounterClockwise = (BOOL)FrontCounterClockwise;
		desc.DepthBias = DepthBias;
		desc.DepthBiasClamp		= 0.0f;
		desc.SlopeScaledDepthBias = SlopeScaledDepthBias * 0.001f;
		desc.DepthClipEnable	= TRUE;
		desc.ScissorEnable		= FALSE;
		desc.MultisampleEnable	= FALSE;
		desc.AntialiasedLineEnable = (BOOL)AntialiasedLineEnable;

		ID3D11RasterizerState* rasterizerState = nullptr;
		if (FAILED(D3D11Device->CreateRasterizerState(&desc, &rasterizerState))) {
			GLog->Error("create d3d11 Rasterizer state object failed!");
			return -1;
		}

		RenderStateObject* obj = new RenderStateObject();
		obj->Flag = (int)RenderStateFlag::RASTERIZER;
		obj->RasterizerState = rasterizerState;

		int slot = FindAvailableSlot(RenderStateObjectList);
		RenderStateObjectList[slot] = obj;

		return slot;
	}

	void BindRasterizerState(Handle handle)
	{
		RenderStateObject* obj = RenderStateObjectList[handle];
		assert(obj->Flag == (int)RenderStateFlag::RASTERIZER);
		ImmediateContext->RSSetState(obj->RasterizerState);
	}


}