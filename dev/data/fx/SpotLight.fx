#include "PrePassShadingCommon.fx"

float4x4 mMatWorld;
float3 vLightColor = {1, 1, 1};

// x : tangent of half outter angle
// y : cosine of half outter angle
// z : cosine of half inner angle
// w : 1 / (z - y)
float4 vLightParam;



struct VS_INPUT
{
	float4 pos		: POSITION;
};

struct VS_OUTPUT
{
	float4 pos			: SV_POSITION;
	float3 eyeDir		: TEXCOORD0;
	float3 lightDir	: TEXCOORD1;
	float3 lightPos	: TEXCOORD2;
};



VS_OUTPUT VSMain( VS_INPUT input )
{
	VS_OUTPUT output;
	
	
	float3 objPos = input.pos.xyz;
	
	float len = 500;
	objPos = float3(objPos.xy * vLightParam.x * len, objPos.z * len);
	float4 worldPos = mul(float4(objPos, 1.0), mMatWorld);
	output.pos = mul(worldPos, MatViewProj);
	
	// objec's z axis
	output.lightDir = mMatWorld._31_32_33;
	output.lightPos = mMatWorld._41_42_43;
	output.eyeDir = GetEyeDir(output.pos);
	
	return output;
}


struct PS_INPUT
{
	float4 vpos		: SV_POSITION;
	float3 eyeDir		: TEXCOORD0;
	float3 lightDir	: TEXCOORD1;
	float3 lightPos	: TEXCOORD2;
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
	
	float dnl = saturate(dot(l, midst.normal));
	float fRange = vLightParam.x;
	float atten = saturate((fRange - dist) / fRange);
	atten = atten * atten;
	
	float angleCos = dot(l, input.lightDir);
	float spotFactor  = saturate((angleCos - vLightParam.y) * vLightParam.w);
	
	float3 c0 = dnl * vLightColor * illum * atten * spotFactor;

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