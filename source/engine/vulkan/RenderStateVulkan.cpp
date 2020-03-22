#include "RenderState.h"
#include "Log.h"
#include "Renderer.h"

namespace Aurora
{

	

	Handle	CreateBlendState(bool BlendEnable, BlendFactor SrcBlend, BlendFactor DestBlend,
		BlendOp  op, uint8 WriteMask, bool AlphaToCoverageEnable)
	{
		return 0;
	}


	void BindBlendState(Handle handle, const Color& BlendFactor)
	{
	}

	Handle	CreateRasterizerState(bool FrontCounterClockwise, CullMode cullMode,
		FillMode fillMode, int DepthBias, int SlopeScaledDepthBias, bool AntialiasedLineEnable)
	{
		return 0;
	}

	void BindRasterizerState(Handle handle)
	{
	}


}