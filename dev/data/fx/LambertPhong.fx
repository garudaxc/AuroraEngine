#include "PrePassShadingCommon.fx"


float3 vDiffuse = {0, 0, 0};
float3 vAmbient = {0, 0, 0};
float3 vSpecular = {1, 0, 1};
float3 vEmissive = {0, 0, 0};
float fSpecularLevel = 1.0;
float fSpecularPower = 32.0;
Texture2D tDiffuseMap;
Texture2D tBumpMap;



float4x4 mMatWorld;


SamplerState MeshTextureSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};


struct VSIn
{
	float4 pos					: POSITION;
	float3 normal				: NORMAL;
	float2 tex					: TEXCOORD0;
	float4 tangent			: TANGENT;
#if _SKIN_
	int4 	 blendIndex		: BLENDINDICES;
	float3 blendWeight	: BLENDWEIGHT;	
#endif
};


struct VSOut
{
	float4 pos		: SV_POSITION;
	float3 normal	: NORMAL;
	float2 tex		: TEXCOORD0;
};


VSOut VSShading(VSIn vsin)
{
	VSOut output = (VSOut)0;

	float4 worldPos = mul(float4(vsin.pos.xyz, 1), mMatWorld);
	output.pos = mul(worldPos, MatViewProj);
	output.normal = mul(vsin.normal, mMatWorld);
	output.tex = vsin.tex;
	
	return output;
}

float4 PSShading(VSOut psin) : SV_Target0
{
	float4 tex = tDiffuseMap.Sample(MeshTextureSampler, psin.tex);
	
	float3 lightDir = float3(1, -1, 1);
	lightDir = normalize(lightDir);
	float dnl = dot(lightDir, psin.normal);
	
	//float3 halfView = normalize(
	//float spec = pow(
	
	float3 c = tex.rgb * dnl * vDiffuse;	
	
	return float4(c, 1);
}


DepthStencilState EnableDepth
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
    DepthFunc = LESS_EQUAL;
};

RasterizerState CullCC
{
	CullMode = Back;
	FrontCounterClockwise = True;
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
        SetRasterizerState(CullCC);
    }
}