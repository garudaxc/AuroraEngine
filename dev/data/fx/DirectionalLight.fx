#include "PrePassShadingCommon.fx"

float4x4 mMatWorld;
float3 vLightColor = {1, 1, 1};

//#define _SHADOW_

#ifdef _SHADOW_
float4x4 MatShadowProj;
sampler2D ShadowMapRT;
#endif



struct VS_INPUT
{
	float4 pos		: POSITION;
};

struct VS_OUTPUT
{
	float4 pos			: SV_POSITION;
	float3 eyeDir		: TEXCOORD0;
	float3 direction	: TEXCOORD1;
};



VS_OUTPUT VSMain( VS_INPUT input )
{
	VS_OUTPUT output;	
	output.pos = float4(input.pos.xyz, 1);
	
	// objec's z axis
	output.direction = mMatWorld._31_32_33;
		
	output.eyeDir = GetEyeDir(output.pos);
	return output;
}


struct PS_INPUT
{
	float4 vpos		: SV_POSITION;
	float3 eyeDir		: TEXCOORD0;
	float3 direction	: TEXCOORD1;
};



float4 PSMain(PS_INPUT input) : SV_Target0
{
	LightPixelShaderMidst midst = (LightPixelShaderMidst)0;
	FillLightPSMidst(midst, input.vpos);
		
	float3 pixelWorldPos = GetWorldFromEyeDir(midst, input.eyeDir);

	float illum = 1.0f;
#ifdef _SHADOW_
	float4 shadowPos = mul(float4(pixelWorldPos, 1), MatShadowProj);
	illum = tex2Dproj(ShadowMapRT, shadowPos);
#endif

	float dnl = saturate(dot(input.direction, midst.normal));
	float3 c0 = dnl * vLightColor * illum;

	float spec = ComputeSpec(midst, input.direction, pixelWorldPos, 1);
		
	return float4(c0, spec);

	//return float4(input.pos.xyz, 1);
}


DepthStencilState EnableDepth
{
    DepthEnable = FALSE;
    DepthWriteMask = ZERO;
    DepthFunc = LESS_EQUAL;
};

RasterizerState RSState
{
	FillMode = SOLID;
	CullMode = BACK;
	FrontCounterClockwise = TRUE;
};


technique10 T0
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VSMain() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSMain() ) );

        SetDepthStencilState( EnableDepth, 0 );
        SetRasterizerState(RSState);
    }
}