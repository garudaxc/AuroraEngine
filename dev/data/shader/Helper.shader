
#include "stdafx.shader"

struct VS_INPUT
{
    float4 Position		: SV_POSITION;
	float4 Color		: COLOR0;

};

struct VS_OUTPUT
{
	float4 Position		: SV_POSITION;
	float4 Color		: TEXCOORD0;
};

#ifdef VERTEX_SHADER
#include "ParameterBuffers"

VS_OUTPUT Main( VS_INPUT input )
{
	VS_OUTPUT output;
	output.Position = mul(float4(input.Position.xyz, 1), mViewProjectionMatrix);
	output.Color = input.Color;
    return output;
}

#endif


#ifdef PIXEL_SHADER

float4 Main(VS_OUTPUT input) : SV_Target0
{
	return input.Color;
}

#endif