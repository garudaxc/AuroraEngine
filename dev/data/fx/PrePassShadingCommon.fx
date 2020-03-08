#include "GlobalShaderVariables.fx"

struct GeomVertexInput
{
	float4 pos		: POSITION0;
	float3 normal	: NORMAL;
	float2 tex		: TEXCOORD0;
	float4 tangentEx: TANGENT;
#if _SKIN_
	int4 	 blendIndex		: BLENDINDICES;
	float3 blendWeight	: BLENDWEIGHT;	
#endif
};


struct VertexShaderData
{
	float3 objPos;
	float3 worldPos;
	float4 hPos;
	float3 normal;
	float3 tangent;
	float3 binormal;
	float2 texcoord;
};

void PrepareVertexShaderData(out VertexShaderData vsData, GeomVertexInput input, float4x4 mWorld)
{
	vsData = (VertexShaderData)0;
	vsData.objPos = input.pos.xyz;
	vsData.worldPos = mul(float4(input.pos.xyz, 1.0), mWorld).xyz;
	
	vsData.hPos = mul(float4(vsData.worldPos, 1.0), MatViewProj);
	
	vsData.normal	= normalize(mul(input.normal, (float3x3)mWorld));
	vsData.tangent	= normalize(mul(input.tangentEx.xyz, (float3x3)mWorld));
	vsData.binormal	= cross(vsData.normal, vsData.tangent) * input.tangentEx.w;
	
	vsData.texcoord = input.tex;
}

float CalcDepth(VertexShaderData vsData)
{
	return (vsData.hPos.z / GetFarPlane());
}


void GetHomogeneousCoordAndDepth(GeomVertexInput input, float4x4 matWorld, out float4 pos, out float depth)
{
	float4 worldPos = mul(float4(input.pos.xyz, 1.0), matWorld);
	pos = mul(worldPos, MatViewProj);
	depth = pos.z / vCameraParam.y;
}

float4 GetHomogeneousCoord(GeomVertexInput input, float4x4 matWorld)
{
	float4 worldPos = mul(float4(input.pos.xyz, 1.0), matWorld);
	float4 pos = mul(worldPos, MatViewProj);
	return pos;	
}


float3 GetEyeDir(float4 hPos)
{
	float2 uv = hPos.xy * vCameraParam.zw;
	return vCameraVecX * hPos.w + vCameraVecY * uv.x + vCameraVecZ * uv.y;
}



//=================================================================



float3 PackNormal(float3 normal)
{
	return normal * 0.5 + 0.5;
}

float3 UnpackNormal(float3 packed)
{
	return packed * 2.0f - 1.0f;
}

float3 Reflect(float3 normal, float3 light)
{
	return 2.0 * dot(normal, light) * normal - light;
}


void PackGBuffer(out float4 c0, out float4 c1, float3 normal, float depth, 
				 float fSpecularStrength, float fSpecularPower)
{
	float3 n = PackNormal(normal);
	c0 = float4(n, fSpecularStrength * 0.33333);
	
	float d = depth * 16777215.0 / 65536.0;
	float fr = frac(d);
	float r = (d - fr) / 255.0;
	d = fr * 65536.0 / 256.0;
	fr = frac(d);
	float g = (d - fr) / 255.0;
	float b = fr * 256.0 / 255.0;
	
	
	/*float d = depth * 65536.0 / 256.0;
	float fr = frac(d);
	float r = (d - fr) / 255.0;
	float g = fr * 256.0 / 255.0;	*/
	
	c1.rgba = float4(r, g, b, fSpecularPower / 128.0);
}

/////////////////////////////////////////////////////////////////////////////



struct LightPixelShaderMidst
{
	float2		vPos;
	float2		srCoord;
	float4		buffer0;
	float4		buffer1;
	float3		normal;
	float		depth;
	float		specStrength;
	float		specPower;
};

SamplerState LPPSampler
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = CLAMP;
    AddressV = CLAMP;
};


float3 GetNormal(LightPixelShaderMidst input)
{
	return normalize(UnpackNormal(input.buffer0.rgb));
}

float GetSpecularStrength(LightPixelShaderMidst input)
{
	return input.buffer0.a * 3.0;
}

float GetSpecularPower(LightPixelShaderMidst input)
{
	return input.buffer1.a * 128.0;
}

float4 GetLightBufferColor(LightPixelShaderMidst input)
{
	float4 tex = tLightingBuffer.Sample(LPPSampler, input.srCoord);
	return tex;
}


float GetDepth(LightPixelShaderMidst input)
{	
	/*float3 rawval = floor( 255.0 * input.buffer1.rgb + 0.5);
	
	return dot( rawval, float3(0.996093809371817670572857294849,
	0.0038909914428586627756752238080039,
	1.5199185323666651467481343000015e-5) *
	0.003921568627450980392156862745098);*/
	
	
	float3 rawval = input.buffer1.rgb;
	
	return dot( rawval, float3(0.996093809371817670572857294849,
	0.0038909914428586627756752238080039,
	1.5199185323666651467481343000015e-5));
	
	
	/*float2 rawval = input.buffer1.rg;
	
	return dot( rawval, float2(0.99610894941634241245136186770428,
							0.0038910505836575875486381322957198));*/
}


void FillLightPSMidst(out LightPixelShaderMidst output, float2 vPos)
{
	output.vPos = vPos;
	output.srCoord = vPos * BackBufferSize.xy;
	output.buffer0 = tGbuffer0.Sample(LPPSampler, output.srCoord);
	output.buffer1 = tGbuffer1.Sample(LPPSampler, output.srCoord);

	output.normal = float3(0, 0, 0);
	output.depth = 0;
	output.specStrength = 0;
	output.specPower = 0;

	output.normal = GetNormal(output);
	output.depth = GetDepth(output);
	output.specStrength = GetSpecularStrength(output);
	output.specPower = GetSpecularPower(output);
}


float3 GetWorldFromEyeDir(LightPixelShaderMidst input, float3 eyeDir)
{
	float nearPlane = GetNearPlane();
	float farPlane = GetFarPlane();
	float depth = GetDepth(input);	
	
	float dist = lerp(nearPlane, farPlane, depth);
	eyeDir /= dot(eyeDir, vCameraLookDir);
	return vCameraPos + eyeDir * dist;
}


float ComputeSpec(LightPixelShaderMidst input, float3 lightDir, float3 pixelWorldPos, float atten)
{
	float3 eyeDir = normalize(vCameraPos - pixelWorldPos);
	float3 halfDir = normalize(eyeDir + lightDir);
	
	// todo : normalize light power
	float spec = pow(saturate(dot(halfDir, input.normal)), input.specPower) * atten * input.specStrength;

	return spec;
}