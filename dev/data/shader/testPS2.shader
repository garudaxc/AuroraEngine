#include "stdafx.shader"

#include "ParameterBuffers"
#include "CommonPS.shader"


struct PS_INPUT
{
    float4 pos : SV_POSITION; // World position
	float3 view		: TEXCOORD0;
	float3 normal	: TEXCOORD1;
	float2 tex		: TEXCOORD2;
};

Texture2D    g_txDiffuse;
TextureCube    g_txEnv;
SamplerState g_samLinear;

float3 Color_ = {1.0, 1.0, 1.0};


struct PS_OUTPUT
{
	half4 c0	: SV_Target0;
};



float3 SpecularIBL( float3 SpecularColor , float Roughness , float3 N, float3 V )
{
	float3 SpecularLighting = 0;
	const uint NumSamples = 1024;
	for( uint i = 0; i < NumSamples; i++ )
	{
		float2 Xi = Hammersley( i, NumSamples );
		float3 H = ImportanceSampleGGX( Xi, Roughness , N );
		float3 L = 2 * dot( V, H ) * H - V;
		float NoV = saturate( dot( N, V ) );
		float NoL = saturate( dot( N, L ) );
		float NoH = saturate( dot( N, H ) );
		float VoH = saturate( dot( V, H ) );

		if( NoL > 0 )
		{
			float3 SampleColor = g_txEnv.SampleLevel( g_samLinear , L, 0 ).rgb;
			float G = G_GGX( Roughness , NoV, NoL );
			float Fc = pow( 1 - VoH, 5 );
			float3 F = (1 - Fc) * SpecularColor + Fc;
			// Incident light = SampleColor * NoL
			// Microfacet specular = D*G*F / (4*NoL*NoV)
			// pdf = D * NoH / (4 * VoH)
			SpecularLighting += SampleColor * F * G * VoH / (NoH * NoV);
		}
	}
	return SpecularLighting / NumSamples;
}



PS_OUTPUT Main(PS_INPUT input)
{
	PS_OUTPUT output = (PS_OUTPUT)0;

    float4 vDiffuse = g_txDiffuse.Sample( g_samLinear, input.tex );

	float3 c = input.normal * 0.5 + 0.5;
	
	float3 SpecularColor_ = float3(0.8f, 0.8f, 0.8f);

	float ndl = max(0, dot(input.normal, DirectionLightDirection0));
	float ndv = max(0, dot(input.normal, input.view));

	float3 ibl = SpecularIBL(SpecularColor_, 0.1, input.normal, input.view);

	c = vDiffuse.xyz * DirectionLightColor0.xyz * ndl + ibl;

	output.c0 = float4(input.normal, 1.0);

	return output;
}