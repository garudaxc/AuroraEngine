
#ifndef PRE_LIGHT_H
#define PRE_LIGHT_H


//#define _LINEAR_DEPTH_
#define _SPOTLIGHT_TEX_PROJ_
//#define _POINTLIGHT_TEX_PROJ_


sampler2D sampler_PrePassRT0 = sampler_state
{
	Texture=<PrePassRT0>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = None;
	AddressU  = WRAP;
	AddressV  = WRAP;
	AddressW  = WRAP;
  SRGBTEXTURE = False;
};

sampler2D sampler_PrePassRT1 = sampler_state
{
	Texture=<PrePassRT1>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = None;
	AddressU  = WRAP;
	AddressV  = WRAP;
	AddressW  = WRAP;
  SRGBTEXTURE = False;
};

sampler2D sampler_LightingRT = sampler_state
{
	Texture=<LightingRT>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = None;
	AddressU  = WRAP;
	AddressV  = WRAP;
	AddressW  = WRAP;
  SRGBTEXTURE = False;
};


half4 SamplePrePassRT0(float2 uv)
{
	return tex2D(sampler_PrePassRT0, uv);
}


float3 PackNormal(half3 normal)
{
	return normal * 0.5 + 0.5;
}

float3 UnpackNormal(half3 packed)
{
	return packed * 2.0f - 1.0f;
}

half3 Reflect(half3 normal, half3 light)
{
	return 2.0 * dot(normal, light) * normal - light;
}

float3 GetEyeDir(float4 hPos)
{
	float2 uv = hPos.xy * CameraParam.zw;
	return CameraVec[0] * hPos.w + CameraVec[1] * uv.x + CameraVec[2] * uv.y;
}


float2 CartesianToLambertAzimuthal(float3 cartesian)
{
	float f = sqrt(8 * cartesian.z + 8);
	return cartesian.xy / f + 0.5;
}

float3 LambertAzimuthalToCartesian(float2 la)
{
	float2 fenc = la * 4 - 2;
	float f = dot(fenc , fenc);
	float g = sqrt(1 - f / 4);
	float3 n;
	n.xy = fenc * g;
	n.z = 1 - f / 2;
	return n;
}


// 将法线，高光等信息打包到两个float4中(对应两张render target)
void PackGBuffer(float3 normal, float depth, float specPower, 
								float specStrength, float transmission,
									out float4 c0, out float4 c1)
{
	float3 norm = PackNormal(normal);
	
	//float2 norm = CartesianToLambertAzimuthal(normal);
	 
	/*float d = depth * 65536.0 / 256.0;
	float fr = frac(d);
	float r = (d - fr) / 255.0;
	float g = fr * 256.0 / 255.0;*/
	
	float d = depth * 16777215.0 / 65536.0;
	float fr = frac(d);
	float r = (d - fr) / 255.0;
	d = fr * 65536.0 / 256.0;
	fr = frac(d);
	float g = (d - fr) / 255.0;
	float b = fr * 256.0 / 255.0;
	
	c0 = float4(norm, specStrength * 0.33333);
	c1 = float4(r, g, b, specPower/128.0);
}





//////////////////////////////////////////////////////////////////////////






struct LightPixelShaderMidst
{
	float2		vPos;
	float2		srCoord;
	half4			prepassRT0;
	half4			prepassRT1;
	half3			normal;
	half			depth;
	half			viewZ;							// 视空间中的z轴坐标
	half			specStrength;
	half			specPower;
	half			transmission;
	
};


half3 GetNormal(LightPixelShaderMidst input)
{
	//return UnpackNormal(input.prepassRT0.rgb);
	return normalize(UnpackNormal(input.prepassRT0.rgb));
	//return normalize(LambertAzimuthalToCartesian(input.prepassRT0.rg));
}

half GetSpecularPower(LightPixelShaderMidst input)
{
	return input.prepassRT1.a * 128.0;
}

half GetSpecularStrength(LightPixelShaderMidst input)
{
	return input.prepassRT0.a * 3.0;
}

half GetTransmission(LightPixelShaderMidst input)
{
	return 0;
	return input.prepassRT1.a;
}

half4 GetLightBufferColor(LightPixelShaderMidst input)
{
	return tex2D(sampler_LightingRT, input.srCoord);
}

half GetViewZ(LightPixelShaderMidst input)
{
	return lerp(CameraParam.x, CameraParam.y, input.depth);
}


float GetDepth(LightPixelShaderMidst input)
{	
	float3 rawval = floor( 255.0 * input.prepassRT1.rgb + 0.5);
	
	return dot( rawval, float3(0.996093809371817670572857294849,
	0.0038909914428586627756752238080039,
	1.5199185323666651467481343000015e-5) *
	0.003921568627450980392156862745098);
	
	
	/*float3 rawval = input.prepassRT1.rgb;
	
	return dot( rawval, float3(0.996093809371817670572857294849,
	0.0038909914428586627756752238080039,
	1.5199185323666651467481343000015e-5));
	
	
	float2 rawval = input.prepassRT1.rg;
	
	return dot( rawval, float2(0.99610894941634241245136186770428,
							0.0038910505836575875486381322957198));*/
}


void FillLightPSMidst(out LightPixelShaderMidst output, float2 vPos)
{
	output.vPos = vPos;
	output.srCoord = (vPos + 0.5) * BackBufferSizeParam.xy;
	output.prepassRT0 = tex2D(sampler_PrePassRT0, output.srCoord);
	output.prepassRT1 = tex2D(sampler_PrePassRT1, output.srCoord);
	
	output.normal = half3(0, 0, 0);
	output.depth = 0;
	output.viewZ = 0;
	output.specStrength = 0;
	output.specPower = 0;
	output.transmission = 0;

	output.normal = GetNormal(output);
	output.depth = GetDepth(output);
	output.viewZ = GetViewZ(output);
	output.specStrength = GetSpecularStrength(output);
	output.specPower = GetSpecularPower(output);
	output.transmission = GetTransmission(output);
}


float3 ComputeWorldPos(LightPixelShaderMidst input)
{
	float xPos = (input.vPos.x * BackBufferSizeParam.x) * 2.0f - 1.0f;
	float yPos = ((BackBufferSizeParam.w - input.vPos.y) * BackBufferSizeParam.y) * 2.0f - 1.0f;

	float2 uv = float2(xPos, yPos) * CameraParam.zw;

	float3 dir = CameraVec[0] + CameraVec[1] * uv.x + CameraVec[2] * uv.y;

	return CameraPos + dir * input.viewZ;
}



float3 GetWorldFromEyeDir(LightPixelShaderMidst input, float3 eyeDir)
{
	float depth = GetDepth(input);	
	float dist = lerp(CameraParam.x, CameraParam.y, depth);
	eyeDir /= dot(eyeDir, CameraLookDir);
	return CameraPos + eyeDir * dist;
}


half ComputeSpec(LightPixelShaderMidst input, float3 lightDir, float3 pixelWorldPos, float atten)
{
	float3 eyeDir = normalize(CameraPos - pixelWorldPos);
	float3 refDir = Reflect(input.normal, lightDir);
	half spec = pow(saturate(dot(eyeDir, refDir)), input.specPower) * atten * input.specStrength;
	
	return spec;
}

half3 TextureProjection2D(float3 worldPos, float4x4 matTexProj, sampler2D texSampler)
{
	float4 projPos = mul(float4(worldPos, 1), matTexProj);
	half3 projTexColor = tex2Dproj(texSampler, projPos).rgb;
	return projTexColor;
}



half3 TextureProjectionCUBE(float3 lightDir, float4x4 matWorld, samplerCUBE texSampler)
{
	// from world space to obj space
	float3 texProjDir = mul((float3x3)matWorld, -lightDir);
	half3 texProjColor = texCUBE(texSampler, texProjDir).rgb;
	return texProjColor;
}


#endif