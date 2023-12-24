#include "stdafx.shader"

#include "Parameters"

#include "ParameterBuffers"
#include "CommonVS.shader"


struct VS_OUTPUT
{
    float4 pos : SV_POSITION; // World position
	float3 view		: TEXCOORD0;
	float3 normal	: TEXCOORD1;
	float2 tex		: TEXCOORD2;
};

VS_OUTPUT Main( GeomVertexInput input )
{
	VS_OUTPUT output;
	   
	output.pos = GetHomogeneousCoord(input);
	//output.pos = input.pos;
	
	output.normal = mul(input.normal, (float3x3)matWorld);
	output.normal = input.normal;
	
	float4 worldPos = mul(input.pos, matWorld);
	float3 eye = mViewMatrixInverse._41_42_43;

	output.view = normalize(eye - worldPos.xyz);
	output.tex = input.tex;
    return output;
}