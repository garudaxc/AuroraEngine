#include "PrePassShadingCommon.fx"


float3 vDiffuse = {0, 0, 0};
float3 vAmbient = {0, 0, 0};
float3 vSpecular = {1, 0, 1};
float3 vEmissive = {0, 0, 0};
float fSpecularLevel = 1.0;
float fSpecularPower = 32.0;
Texture2D tDiffuseMap;
Texture2D tBumpMap;

// volumn texture parameter
float3 vScale;
float3 vOffset;

float3 vInvS;

Texture3D tVoxelMap;


float4x4 mMatWorld;


SamplerState MeshTextureSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState VoxelTextureSampler
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = BORDER;
    AddressV = BORDER;
    AddressW = BORDER;
};



float4 CubicNTreeLookup(Texture3D tex3D, float3 invS, float N, float3 M)
{
	float4 I = 0.f;
	float3 MND = M;
	
	for (float i = 0; i < 10; i++)
	{
		float3 P = (floor(I.xyz * 255.0) + frac(MND)) * invS;
		
		if (I.w < 0.9)
		{
			I = tex3D.Sample(VoxelTextureSampler, P);
		}
		
		if (I.w > 0.9)
			break;
			
		if (I.w < 0.1)
			clip(-1);
		
		MND = MND * N;		
	}
	
	return I;
}

float4 N3TreeLookupLerp(float3 pos)
{
	float3 voxelSize = 0.5f * invS;
	
	//float3 offset[] = 
	
	return float3(0, 0, 0);
}




struct VSIn
{
	float4 pos		: POSITION;
	float3 normal	: NORMAL;
	float2 tex		: TEXCOORD0;
	float4 tangent	: TANGENT;
};


struct VSOut
{
	float4 pos		: SV_POSITION;
	float3 normal	: NORMAL;
	float2 tex		: TEXCOORD0;
	float3 VoxelPos : TEXCOORD1;
};


VSOut VSShading(VSIn vsin)
{
	VSOut output = (VSOut)0;

	float4 worldPos = mul(float4(vsin.pos.xyz, 1), mMatWorld);
	output.pos = mul(worldPos, MatViewProj);
	output.normal = vsin.normal;
	output.tex = vsin.tex;
	
	output.VoxelPos = (vsin.pos.xyz + vOffset) * vScale;

	return output;
}

float4 PSShading(VSOut psin) : SV_Target0
{
	float4 tex = tDiffuseMap.Sample(MeshTextureSampler, psin.tex);
	
	float3 lightDir = float3(1, -1, 1);
	lightDir = normalize(lightDir);
	float dnl = dot(lightDir, psin.normal);
	
	float3 c = tex.rgb * dnl * vDiffuse;
	
	float3 vc = 0;
	vc.x = psin.VoxelPos.x;
	vc.y = 1 - psin.VoxelPos.z;
	vc.z = psin.VoxelPos.y;
	
	//vc += float3(0.5 / 64.0, 0.5 / 64.0, 0.5 / 64.0);
	/*
	if (vc.x > 1.0f)
		return float4(1, 0, 0, 1);
	if (vc.y > 1.0f)
		return float4(0, 1, 0, 1);		
	if (vc.z > 1.0f)
		return float4(0, 0, 1, 1);
	return float4(0, 0, 0, 1);*/
	
	//c = tVoxelMap.Sample(VoxelTextureSampler, psin.VoxelPos);

	c = CubicNTreeLookup(tVoxelMap, vInvS, 2.f, psin.VoxelPos);
	
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