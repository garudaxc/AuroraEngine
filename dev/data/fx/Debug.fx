#include "GlobalShaderVariables.fx"

float4x4 mMatWorld;

struct VSIn
{
	float4 pos		: POSITION;
};


struct VSOut
{
	float4 pos		: SV_POSITION;
};


VSOut VSShading(VSIn vsin)
{
	VSOut output = (VSOut)0;
	float4 worldPos = mul(float4(vsin.pos.xyz, 1), mMatWorld);
	output.pos = mul(worldPos, MatViewProj);
	return output;
}

float4 PSShading(VSOut psin) : SV_Target0
{
	return float4(1, 0, 0, 1);
}

DepthStencilState EnableDepth
{
    DepthEnable = TRUE;
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

        SetDepthStencilState( EnableDepth, 0 );
        SetRasterizerState(CullNone);
    }
}