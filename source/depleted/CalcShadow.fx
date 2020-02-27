#ifndef CALC_SHADOW
#define CALC_SHADOW

#include "GlobalShaderDefine.fx"

sampler3D JitterSampler = sampler_state
{
 Texture=<JitterSampleTexture>;
 MinFilter = Linear;
 MagFilter = Linear;
 MipFilter = Linear;
 AddressU  = WRAP;
 AddressV  = WRAP;
 AddressW  = WRAP;
	SRGBTEXTURE = False;
	MipLODBias = 0; 
            MaxAnisotropy = 0; 
};

sampler2D sampler_ShadowMap=sampler_state
{
 Texture=<ShadowMapTexture>;
 MinFilter = Point;
 MagFilter = Point;
 MipFilter = None;
 AddressU  = Border;
 AddressV  = Border;
 AddressW  = Border;
 BorderColor = 0xffffffff;
	SRGBTEXTURE = False;
	MipLODBias = 0; 
            MaxAnisotropy = 0; 
};




void CalcShadowProj(out float4 pos0, out float4 pos1, 
										out float4 pos2, 
										float3 worldPos, float4x4 worldMatrix, 
										float4x4 shadowVPMatrix[4])
{
	float4 pos = mul(float4(worldPos, 1), worldMatrix);
	pos0 = mul(pos, shadowVPMatrix[0]);
	pos1 = mul(pos, shadowVPMatrix[1]);
	pos2 = mul(pos, shadowVPMatrix[2]);
}




shared float4x4 shadowVPMatrixArray[4];

float PCFKernel(sampler2D map, float3 uv)
{
	float2 halfSize = 0.5f * ShadowParam.zw;
	float fShadow[4] = {1, 1, 1, 1};
	fShadow[0] = (uv.z < tex2D(map, uv.xy + float2(-halfSize.x, -halfSize.y)).r);
	fShadow[1] = (uv.z < tex2D(map, uv.xy + float2(halfSize.x, -halfSize.y)).r);
	fShadow[2] = (uv.z < tex2D(map, uv.xy + float2(-halfSize.x, halfSize.y)).r);
	fShadow[3] = (uv.z < tex2D(map, uv.xy + float2(halfSize.x, halfSize.y)).r);

	float2 vLerpFactor = frac(ShadowParam.zw * (uv.xy + float2(-halfSize.x, -halfSize.y)));
	return lerp( lerp(fShadow[0], fShadow[1], vLerpFactor.x),
					lerp(fShadow[2], fShadow[3], vLerpFactor.x),
					vLerpFactor.y);
}

/*
float HardwarePCF4(float3 texcoord)
{
	static int nSamples = 4;

	static float2 offset[] = {{-0.5, -0.5}, {-0.5, 0.5}, {0.5, -0.5}, {0.5, 0.5}};

	float shadow = 0;

	for (int i = 0; i < nSamples; i++)
	{

		float2 off = offset[i] / (float)_SHADOWMAPSIZE_;

		//shadow += (texcoord.z < tex2D(sampler_ShadowMap, texcoord.xy + offset).r) * (1.0 / (float)nSamples);
		shadow += tex2Dproj(sampler_ShadowMap, float4(texcoord.xy + off, texcoord.z, 1.0)).r * (1.0 / (float)nSamples);
	}

	return shadow;
}

float CommonPCF(float3 texcoord)
{
	//float shadow = PCFKernel(sampler_ShadowMap, float3(texcoord.xy, texcoord.z));
	return shadow;
}


*/


#define SOFT_SHADOW_SAMPLE 2
float SoftShadowTermJitter(float3 vShadowTexCoord, float2 vPos)
{
	float2 diskSize = 1.0 * ShadowParam.zw;			// 圆盘半径1个像素
	float3 jcoord = float3(vPos / 16.0, 0);
	
	float shadow = 0;
	
	[unroll]
	for (int i = 0; i < SOFT_SHADOW_SAMPLE; i++)
	{
		float4 jitter = (tex3D(JitterSampler, jcoord) * 2.0 - 1.0) * diskSize.xyxy;
		jcoord.z += 1.0 / (float)SOFT_SHADOW_SAMPLE;
		
		float2 smCoord = vShadowTexCoord.xy + jitter.xy;
		shadow += (vShadowTexCoord.z < SampleDepth(sampler_ShadowMap, smCoord));
		
		smCoord = vShadowTexCoord.xy + jitter.zw;
		shadow += (vShadowTexCoord.z < SampleDepth(sampler_ShadowMap, smCoord));
	}
	shadow /= (SOFT_SHADOW_SAMPLE * 2.f);

	return shadow;
}

float2 GetShadowMapSize()
{
	return ShadowParam2.zw;
}

static const float2 SHADOWMAP_SIZE = {2048.f, 1024.f};
// Calculates the shadow term using PCF soft-shadowing
float SoftShadowTermPCF(float fLightDepth, float2 vShadowTexCoord, int iSqrtSamples)
{
	float fShadowTerm = 0.0f; 
	float fRadius = (iSqrtSamples - 1.0f) / 2.f;
	
	float2 shadowMapSize = GetShadowMapSize();
	
	for (float y = -fRadius; y <= fRadius; y++)
	{
		for (float x = -fRadius; x <= fRadius; x++)
		{
			float2 vOffset = float2(x, y) * ShadowParam.zw;
			float2 vSamplePoint = vShadowTexCoord + vOffset;			
			float fDepth = SampleDepth(sampler_ShadowMap, vSamplePoint).x;
			float fSample = (fLightDepth < fDepth);
			
			// Edge tap smoothing
			float xWeight = 1;
			float yWeight = 1;
			
			if (x == -fRadius)
				xWeight = 1 - frac(vShadowTexCoord.x * shadowMapSize.x);
			else if (x == fRadius)
				xWeight = frac(vShadowTexCoord.x * shadowMapSize.x);
				
			if (y == -fRadius)
				yWeight = 1 - frac(vShadowTexCoord.y * shadowMapSize.y);
			else if (y == fRadius)
				yWeight = frac(vShadowTexCoord.y * shadowMapSize.y);
				
			fShadowTerm += fSample * xWeight * yWeight;
		}											
	}		
	
	fShadowTerm /= (iSqrtSamples * iSqrtSamples);
	fShadowTerm *= 2.5f;	
	
	return fShadowTerm;
}

// 展开9次pcf采样，无动态分支，intel hd上效果正确
// 114 instructions
half SoftShadowTermPCF9(float fLightDepth, float2 vShadowTexCoord)
{
	half fShadowTerm = 0.0f; 
	half2 shadowMapSize = GetShadowMapSize();
	
	// 0
	{
		half2 vOffset = float2(-1, -1) * ShadowParam.zw;
		half2 vSamplePoint = vShadowTexCoord + vOffset;			
		half fDepth = SampleDepth(sampler_ShadowMap, vSamplePoint).x;
		half fSample = (fLightDepth < fDepth);
			
		half xWeight = 1 - frac(vShadowTexCoord.x * shadowMapSize.x);
		half yWeight = 1 - frac(vShadowTexCoord.y * shadowMapSize.y);
		fShadowTerm += fSample * xWeight * yWeight;
	}
		
	// 1
	{
		half2 vOffset = float2(0, -1) * ShadowParam.zw;
		half2 vSamplePoint = vShadowTexCoord + vOffset;			
		half fDepth = SampleDepth(sampler_ShadowMap, vSamplePoint).x;
		half fSample = (fLightDepth < fDepth);
			
		half xWeight = 1;
		half yWeight = 1 - frac(vShadowTexCoord.y * shadowMapSize.y);
		fShadowTerm += fSample * xWeight * yWeight;
	}
	
	// 2
	{
		half2 vOffset = float2(1, -1) * ShadowParam.zw;
		half2 vSamplePoint = vShadowTexCoord + vOffset;			
		half fDepth = SampleDepth(sampler_ShadowMap, vSamplePoint).x;
		half fSample = (fLightDepth < fDepth);
			
		half xWeight = frac(vShadowTexCoord.x * shadowMapSize.x);
		half yWeight = 1 - frac(vShadowTexCoord.y * shadowMapSize.y);
		fShadowTerm += fSample * xWeight * yWeight;
	}
	
	// 3
	{
		half2 vOffset = float2(-1, 0) * ShadowParam.zw;
		half2 vSamplePoint = vShadowTexCoord + vOffset;			
		half fDepth = SampleDepth(sampler_ShadowMap, vSamplePoint).x;
		half fSample = (fLightDepth < fDepth);
			
		half xWeight = 1 - frac(vShadowTexCoord.x * shadowMapSize.x);
		half yWeight = 1;
		fShadowTerm += fSample * xWeight * yWeight;
	}
		
	// 4
	{
		half2 vOffset = float2(0, 0) * ShadowParam.zw;
		half2 vSamplePoint = vShadowTexCoord + vOffset;			
		half fDepth = SampleDepth(sampler_ShadowMap, vSamplePoint).x;
		half fSample = (fLightDepth < fDepth);
			
		half xWeight = 1;
		half yWeight = 1;
		fShadowTerm += fSample * xWeight * yWeight;
	}
	
	// 5
	{
		half2 vOffset = float2(1, 0) * ShadowParam.zw;
		half2 vSamplePoint = vShadowTexCoord + vOffset;			
		half fDepth = SampleDepth(sampler_ShadowMap, vSamplePoint).x;
		half fSample = (fLightDepth < fDepth);
			
		half xWeight = frac(vShadowTexCoord.x * shadowMapSize.x);
		half yWeight = 1;
		fShadowTerm += fSample * xWeight * yWeight;
	}
	
	// 6
	{
		half2 vOffset = float2(-1, 1) * ShadowParam.zw;
		half2 vSamplePoint = vShadowTexCoord + vOffset;			
		half fDepth = SampleDepth(sampler_ShadowMap, vSamplePoint).x;
		half fSample = (fLightDepth < fDepth);
			
		half xWeight = 1 - frac(vShadowTexCoord.x * shadowMapSize.x);
		half yWeight = frac(vShadowTexCoord.y * shadowMapSize.y);
		fShadowTerm += fSample * xWeight * yWeight;
	}
		
	// 7
	{
		half2 vOffset = float2(0, 1) * ShadowParam.zw;
		half2 vSamplePoint = vShadowTexCoord + vOffset;			
		half fDepth = SampleDepth(sampler_ShadowMap, vSamplePoint).x;
		half fSample = (fLightDepth < fDepth);
			
		// Edge tap smoothing
		half xWeight = 1;
		half yWeight = frac(vShadowTexCoord.y * shadowMapSize.y);
		fShadowTerm += fSample * xWeight * yWeight;
	}
	
	// 8
	{
		half2 vOffset = float2(1, 1) * ShadowParam.zw;
		half2 vSamplePoint = vShadowTexCoord + vOffset;			
		half fDepth = SampleDepth(sampler_ShadowMap, vSamplePoint).x;
		half fSample = (fLightDepth < fDepth);
			
		half xWeight = frac(vShadowTexCoord.x * shadowMapSize.x);
		half yWeight = frac(vShadowTexCoord.y * shadowMapSize.y);
		fShadowTerm += fSample * xWeight * yWeight;
	}
	
	fShadowTerm *= 2.5 / 9.0f;	
	
	return fShadowTerm;
}


#define SOFT_SHADOW S_S

//by sssa2000 2010 0623
//over load the function for the forward rendering
float3 CalcShadowDirectional(float3 worldPos, float viewZ, float2 vPos,in float4x4 _ShadowmapTexProjMatrix[3])
{
	float4 pos = 0;
	if (viewZ > ShadowSplitPosition.z)
	{
		// 剔除远处不必要的像素				
		// 	clip(-1);
	}
	else if (viewZ > ShadowSplitPosition.y)
	{
		pos = mul(float4(worldPos, 1), _ShadowmapTexProjMatrix[1]);
	}
	else
	{
		pos = mul(float4(worldPos, 1), _ShadowmapTexProjMatrix[0]);
	}
								
	pos /= pos.w;
	
	// 做一些修正
	pos.z -= ShadowParam.x;		// depthBias
	pos.z = min(pos.z, 0.999);
	
	float shadow = 0;
	
#if SOFT_SHADOW == 1
	//shadow = SoftShadowTermJitter(pos, vPos);
	//shadow = SoftShadowTermPCF(pos.z, pos.xy, 3);
	shadow = SoftShadowTermPCF9(pos.z, pos.xy);
#else
	shadow = (pos.z < SampleDepth(sampler_ShadowMap, pos.xy));
#endif
	
	float factor = smoothstep(ShadowParam2.x, ShadowParam2.y, viewZ);
	float3 s = lerp(shadow.xxx * shadow.xxx, float3(1, 1, 1), factor);

	return s;
}




float DiskSample(float4 hPos, float2 vPos, float radius, float depthBias)
{
	float4 pos = hPos;

	pos.z -= depthBias;									// depth bias
	pos.z = min(pos.z, 0.999);
	
	float2 diskSize = float2(radius, radius) * ShadowParam.zw;			// 圆盘半径两个像素
	float3 jcoord = float3(vPos / 16.0, 0);
	
	float3 shadow = 0;
	for (int i = 0; i < 8; i++)
	{
		float4 jitter = tex3D(JitterSampler, jcoord) * 2.0 - 1.0;
		jcoord.z += 1.0 / 8.0;
		
		float2 smCoord = pos.xy + jitter.xy * diskSize;
		shadow += (pos.z < SampleDepth(sampler_ShadowMap, smCoord)) / 16.0;
		
		smCoord = pos.xy + jitter.zw * diskSize;
		shadow += (pos.z < SampleDepth(sampler_ShadowMap, smCoord)) / 16.0;
	}
	
	return shadow;
}



float4 CalcShadowX52(float3 worldPos, float viewZ, float2 vPos)
{
	float shadow = 0.0f;
	float3 c = float3(1, 1, 0.3);
	
	float4 pos0 = mul(float4(worldPos, 1), ShadowmapProjMatrix[0]);
	float4 texcoord = pos0 / pos0.w;
	if (max(abs(texcoord.x - 0.25), abs(texcoord.y - 0.25)) >= 0.25)
	{
	  c = float3(1, 0.7, 0.6);
	  float4 pos1 = mul(float4(worldPos, 1), ShadowmapProjMatrix[1]);
	  texcoord = pos1 / pos1.w;
	  if (max(abs(texcoord.x - 0.75), abs(texcoord.y - 0.25)) >= 0.25)
	  {
	    c = float3(1, 0.3, 2.0);
			float4 pos2 = mul(float4(worldPos, 1), ShadowmapProjMatrix[2]);
	    texcoord = pos2 / pos2.w;
	    if (max(abs(texcoord.x - 0.25), abs(texcoord.y - 0.75)) >= 0.25)
	    {
		      c = float3(1, 1, 1);
	        shadow = 1;
	    }
	  }
	}
	
	if (shadow < 0.5)
	{
		shadow = DiskSample(texcoord, vPos, 2.0, ShadowParam.x * c.z);
	}
		
	float factor = smoothstep(ShadowParam2.x, ShadowParam2.y, viewZ);
	float3 s = lerp(c * shadow, float3(1, 1, 1), factor);

	return float4(s, 1);
}




float3 CalcCharacterShadow(float3 worldPos, float viewZ, float2 vPos,in float4x4 _ShadowmapTexProjMatrix[3])
{
	float2 uv = (vPos + 0.5) * BackBufferSizeParam.xy;
	float4 shadow = tex2D(Sampler_SSShadowRT0, uv);
	return shadow.xyz;
}

#endif