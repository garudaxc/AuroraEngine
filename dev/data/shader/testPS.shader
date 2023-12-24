#include "stdafx.shader"


samplerCUBE texCube;

struct PS_INPUT
{
	float2 vpos		: VPOS;
	float3 normal	: TEXCOORD0;
	float2 tex		: TEXCOORD1;
};


struct PS_OUTPUT
{
	half4 c0	: COLOR0;
};



PS_OUTPUT Main(PS_INPUT input)
{
	PS_OUTPUT output = (PS_OUTPUT)0;

	float4 tex = texCUBE(texCube, float3(input.normal.x, input.normal.z, -input.normal.y));
	
	float3 LightDirection =  float3(0, 1, 0);
	
	float dnl = max(0, dot(LightDirection, input.normal));
	
	//float d = 0;
	//if (ceil(tex.r * 256.0) == ceil(tex.g))
	//	d = 1;

	output.c0 = float4(1, dnl, dnl, dnl);
	
	return output;
}