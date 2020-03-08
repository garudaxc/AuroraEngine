#include "PrePassShadingCommon.fx"

float4x4 mMatWorld;
float3 vLightColor = {1, 1, 1};
float4 vLightParam;

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
	float3 lightPos	: TEXCOORD1;
};



VS_OUTPUT VSMain( VS_INPUT input )
{
	VS_OUTPUT output;
	
	float4 worldPos = mul(float4(input.pos.xyz * vLightParam.x, 1.0), mMatWorld);
	output.pos = mul(worldPos, MatViewProj);
		
	output.lightPos = mMatWorld._41_42_43;
	
	//output.direction = float3(0, 0, 1);
	
	output.eyeDir = GetEyeDir(output.pos);
	return output;
}


struct PS_INPUT
{
	float4 vpos		: SV_POSITION;
	float3 eyeDir		: TEXCOORD0;
	float3 lightPos	: TEXCOORD1;
};



float4 PSMain(PS_INPUT input) : SV_Target0
{
	LightPixelShaderMidst midst = (LightPixelShaderMidst)0;
	FillLightPSMidst(midst, input.vpos);
		
	float3 pixelWorldPos = GetWorldFromEyeDir(midst, input.eyeDir);
	float3 l = input.lightPos - pixelWorldPos;
	float dist = length(l);
	l /= dist;

	float illum = 1.0f;
#ifdef _SHADOW_
	float4 shadowPos = mul(float4(pixelWorldPos, 1), MatShadowProj);
	illum = tex2Dproj(ShadowMapRT, shadowPos);
#endif

	
	float dnl = saturate(dot(l, midst.normal));
	float fRange = vLightParam.x;
	float atten = saturate((fRange - dist) / fRange);
	atten = atten * atten;
	float3 c0 = dnl * vLightColor * illum * atten;

	float spec = ComputeSpec(midst, l, pixelWorldPos, 1);
		
	return float4(c0, spec);

	//return float4(input.pos.xyz, 1);
}


DepthStencilState EnableDepth
{
    DepthEnable = False;
    DepthWriteMask = ZERO;
    DepthFunc = LESS_EQUAL;
};

RasterizerState RSState
{
	FillMode = SOLID;
	CullMode = None;
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