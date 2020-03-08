#include "PrePassShadingCommon.fx"


float4x4 mMatWorld;




cbuffer CBufferPS
{
	float3 vDiffuse = {0, 0, 0};
	float fSpecularLevel = 1.0;
	float3 vAmbient = {0, 0, 0};
	float fSpecularPower = 32.0;
	float3 vSpecular = {1, 0, 1};
	float3 vEmissive = {0, 0, 0};
};	

Texture2D tDiffuseMap;
Texture2D tBumpMap;



struct VSOutPrePass
{
	float4 pos		: SV_POSITION;
	float3 normal	: NORMAL;
	float3 tangent	: TANGENT;
	float3 binormal	: BINORMAL;
	float2 tex		: TEXCOORD0;
	float depth		: TEXCOORD1;
};


VSOutPrePass VSPrePass(GeomVertexInput vsin)
{
	VertexShaderData vsData = (VertexShaderData)0;
	PrepareVertexShaderData(vsData, vsin, mMatWorld);
	
	VSOutPrePass output;
	output.pos = vsData.hPos;
	output.normal = vsData.normal;
	output.tangent = vsData.tangent;
	output.binormal = vsData.binormal;
	output.tex = vsData.texcoord;
	output.depth = CalcDepth(vsData);

	return output;
}



struct PSOutPrePass
{
	float4 c0 : SV_Target0;
	float4 c1 : SV_Target1;
};


SamplerState MeshTextureSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};



PSOutPrePass PSPrePass(VSOutPrePass input)
{
	PSOutPrePass output = (PSOutPrePass)0;

	float3 texNorm = tBumpMap.Sample(MeshTextureSampler, input.tex).rgb;
	float3 normMap = texNorm.rgb * 2.0 - 1.0;
	float3 normal = normalize(input.normal + input.tangent * normMap.x - input.binormal * normMap.y);
	
	PackGBuffer(output.c0, output.c1, normal, input.depth, fSpecularLevel, fSpecularPower);
		
	return output;
}

//===================================================
struct VSIn
{
	float4 pos		: POSITION;
	float3 normal	: NORMAL;
	float2 tex		: TEXCOORD0;
	float4 tangent	: TANGENT;
};


struct VSOutShading
{
	float4 pos	: SV_POSITION;
	float2 tex	: TEXCOORD0;
};


struct PSInShading
{
	float4 vpos		: SV_POSITION;
	float2 tex		: TEXCOORD0;
};


VSOutShading VSShading(GeomVertexInput input)
{
	VSOutShading output = (VSOutShading)0;
	output.pos = GetHomogeneousCoord(input, mMatWorld);
	
	//output.normal = GetNormal(input);
	output.tex = input.tex;
	
	//float4 worldPos = mul(float4(input.pos.xyz, 1.0), mMatWorld);
	//output.worldPos = worldPos.xyz;
	
	//output.eyeDir = GetEyeDir(output.pos);
	
  return output;
}

float4 PSShading(PSInShading input) : SV_Target0
{
	float2 srCoord = input.vpos * BackBufferSize.xy;
	float4 l = tLightingBuffer.Sample(LPPSampler, srCoord);
	
	float4 albedo = tDiffuseMap.Sample(MeshTextureSampler, input.tex);
	
	float3 diffuse = l.rgb * albedo.rgb * vDiffuse;
	float3 spec = l.a * l.rgb;

	float3 c = diffuse + spec + vGlobalAmbient + vEmissive;
	
	//output.c0 = float4(lightingInfo.rgb, 1);

	return float4(c, albedo.a);
}


DepthStencilState DSPrePass
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
    DepthFunc = LESS_EQUAL;
};

DepthStencilState DSShading
{
    DepthEnable = TRUE;
    DepthWriteMask = ZERO;
    DepthFunc = EQUAL;
};

RasterizerState RSState
{
	FillMode = SOLID;
	CullMode = BACK;
	FrontCounterClockwise = TRUE;
};


//===================================================

technique10 PrePass
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VSPrePass() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSPrePass() ) );

        SetDepthStencilState( DSPrePass, 0 );
        SetRasterizerState(RSState);
        
    }
}


technique10 ShadingPass
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VSShading() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSShading() ) );

        SetDepthStencilState( DSShading, 0 );
        SetRasterizerState(RSState);
    }
}