
#define PI 3.14159265
#define HALF_PI 1.570796325?
#define INV_PI 0.3183098865

// x : near plane
// y : far plane
// z : 1 / _11 of proj matrix
// w : 1 / _22 of proj matrix
float4 CameraParam : register(c0);
float3 CameraPos : register(c1);
float3 CameraLookDir : register(c2);

// x : 1 / back buffer width
// y : 1 / back buffer height
// z : back buffer width
// w : back buffer height
float4 BackBufferSizeParam : register(c3);

float3 GlobalAmbient : register(c4);


sampler2D NormalRT : register(s0);
sampler2D DepthRT : register(s1);
sampler2D LightingRT : register(s0);



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


void PackGBuffer(out half4 c0, out half4 c1,
half3 normal, float depth,
float gloss, float specularLevel)
{
float3 n = PackNormal(normal);
c0 = half4(n, 0.f);

/*float d = input.dep * 16777215.0 / 65536.0;
float fr = frac(d);
float r = (d - fr) / 255.0;
d = fr * 65536.0 / 256.0;
fr = frac(d);
float g = (d - fr) / 255.0;
float b = fr * 256.0 / 255.0;*/


float d = depth * 65536.0 / 256.0;
float fr = frac(d);
float r = (d - fr) / 255.0;
float g = fr * 256.0 / 255.0;

c1.rgba = half4(r, g, gloss, specularLevel * 0.33333);
}


struct LightPixelShaderMidst
{
float2 vPos;
float2 srCoord;
half4 texNorm;
half4 texDepth;
half3 normal;
half depth;
half specStrength;
half specPower;
};


half3 GetNormal(LightPixelShaderMidst input)
{
return normalize(UnpackNormal(input.texNorm.rgb));
}

half GetSpecularStrength(LightPixelShaderMidst input)
{
return input.texDepth.a * 3.0;
}

half GetSpecularPower(LightPixelShaderMidst input)
{
return input.texDepth.b * 100.0;
}

half4 GetLightBufferColor(LightPixelShaderMidst input)
{
return tex2D(LightingRT, input.srCoord);
}


float GetDepth(LightPixelShaderMidst input)
{
/*float3 rawval = floor( 255.0 * input.texDepth.rgb + 0.5);

return dot( rawval, float3(0.996093809371817670572857294849,
0.0038909914428586627756752238080039,
1.5199185323666651467481343000015e-5) *
0.003921568627450980392156862745098);*/


/*float3 rawval = input.texDepth.rgb;

return dot( rawval, float3(0.996093809371817670572857294849,
0.0038909914428586627756752238080039,
1.5199185323666651467481343000015e-5));*/


float2 rawval = input.texDepth.rg;

return dot(rawval, float2(0.99610894941634241245136186770428,
0.0038910505836575875486381322957198));
}


void FillLightPSMidst(out LightPixelShaderMidst output, float2 vPos)
{
output.vPos = vPos;
output.srCoord = (vPos + 0.5) * BackBufferSizeParam.xy;
output.texNorm = tex2D(NormalRT, output.srCoord);
output.texDepth = tex2D(DepthRT, output.srCoord);

output.normal = half3(0, 0, 0);
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



float3 BRDF_Diffuse(float3 inDiff)
{
return inDiff * INV_PI;
}


float NDF_GGX(float Roughness, float NdotH)
{
float alpha = Roughness * Roughness;
float beta = NdotH * NdotH * (alpha - 1) + 1;
float ndf = alpha / (PI * (beta * beta));

return ndf;
}

float G_GGX(float Roughness, float NdotV, float NdotL)
{
float k = (Roughness + 1) * (Roughness + 1) * 0.125;
float G1 = NdotV / (NdotV * (1 - k) + k);
float G2 = NdotL / (NdotL * (1 - k) + k);
return G1 * G2;
}

float3 ImportanceSampleGGX(float2 Xi, float Roughness, float3 N)
{
float a = Roughness * Roughness;
float Phi = 2 * PI * Xi.x;
float CosTheta = sqrt((1 - Xi.y) / (1 + (a * a - 1) * Xi.y));
float SinTheta = sqrt(1 - CosTheta * CosTheta);
float3 H;
H.x = SinTheta * cos(Phi);
H.y = SinTheta * sin(Phi);
H.z = CosTheta;
float3 UpVector = abs(N.z) < 0.999 ? float3(0, 0, 1) : float3(1, 0, 0);
float3 TangentX = normalize(cross(UpVector, N));
float3 TangentY = cross(N, TangentX);
// Tangent to world space
return TangentX * H.x + TangentY * H.y + N * H.z;
}