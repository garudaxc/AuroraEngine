#include "GlobalShaderVariables.fx"


Texture2D tTexture;
SamplerState PointSampler
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = BORDER;
    AddressV = BORDER;
    AddressW = BORDER;
};

struct VSIn
{
	float4 pos	: POSITION;
	float2 uv	: Texcoord0;
};

struct VSOut
{
	float4 pos	: SV_POSITION;
	float2 uv	: Texcoord0;
};


VSOut VSShading(VSIn vsin)
{
	VSOut output = (VSOut)0;
	output.pos = float4(vsin.pos.xyz, 1);
	output.uv = vsin.uv;
	return output;
}

float4 PSShading(VSOut psin) : SV_Target0
{
	float4 tex = tTexture.Sample(PointSampler, psin.uv);
	
	return tex;
}

DepthStencilState DisableDepth
{
    DepthEnable = False;
    DepthWriteMask = ZERO;
    DepthFunc = LESS_EQUAL;
};

RasterizerState CullNone
{
	CullMode = None;
};

//===================================================

technique10 Shading
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VSShading() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSShading() ) );

        SetDepthStencilState( DisableDepth, 0 );
        SetRasterizerState(CullNone);
    }
}