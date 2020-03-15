
static const int MAX_MATRICES = 18;
float4x4 amPalette[MAX_MATRICES];

float4x4 matWorld	: WORLD;
float4x4 matWorldInv : WORLDINVERSE;
float4x4 matVP : VIEWPROJECTION;
float4x4 matVPLight;

float3 eyePos;
float3 lightPos;
float ambientLight = 0.4;
float3 lightAtten = {0.0, 0.002, 0.0};


texture txDiffuse;
texture txNormal;
texture txSpec;

sampler DiffuseSampler = 
sampler_state
{
    Texture = <txDiffuse>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

sampler NormalSampler = 
sampler_state
{
    Texture = <txNormal>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

sampler SpecSampler = 
sampler_state
{
    Texture = <txSpec>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

struct VS_INPUT
{
    float4 pos		: POSITION;
    uint4 blendIndex : BLENDINDICES;
    float3 normal	: NORMAL;
    float3 tangent	: TANGENT;
    float3 binormal	: BINORMAL;
    float2 tex0		: TEXCOORD0;
    float2 tex1		: TEXCOORD1;
};

struct PS_INPUT
{
	float4 pos		: POSITION;
	float2 tex0		: TEXCOORD0;
	float2 diffuse	: TEXCOORD1;
	float3 lightDir	: TEXCOORD2;
};



/*
=================

=================
*/
PS_INPUT VS( VS_INPUT input )
{
	PS_INPUT output;
	// ÃÉÆ¤
	float4x4 matBone = amPalette[input.blendIndex.x];
	float4 posModel = mul(input.pos, matBone);	
	float3 norModel = mul(input.normal, (float3x3)matBone);
	float4 posWorld = mul(posModel, matWorld);
	float3 norWorld = mul(norModel, (float3x3)matWorld);
	
	output.pos = mul(posWorld, matVP);
		
	float3 lightDir = lightPos - posWorld.xyz;
	float dist = length(lightDir);
	lightDir = lightDir / dist;
	
	float3 eyeDir = normalize(eyePos - posWorld.xyz);
	float3 halfVect = normalize(lightDir + eyeDir);
	float SpecularAttn = pow(clamp(0, 1, dot(norWorld, halfVect)), 64);
	
	float DistAttn = clamp( 0, 1, 1 / ( lightAtten.x + lightAtten.y * dist + lightAtten.z * dist * dist ));	
	float AngleAttn = clamp( 0, 1, dot(norWorld, lightDir) );
	
	// Âþ·´ÉäÓë¾µÃæ·´ÉäË¥¼õÏµÊý
	output.diffuse.x = DistAttn * AngleAttn;
	output.diffuse.y = SpecularAttn;
	
	// tangent space light direction
	float3 lightDirInModel = mul(lightDir, (float3x3)matWorldInv);
	output.lightDir.x = dot(lightDirInModel, input.tangent);
	output.lightDir.y = dot(lightDirInModel, input.binormal);
	output.lightDir.z = dot(lightDirInModel, input.normal);
	
	output.tex0 = input.tex0;
    return output;
}


float4 PS(PS_INPUT input) : COLOR
{
	float4 texel = tex2D(NormalSampler, input.tex0) * 2 - 1;
	float3 normal;
	normal.xy = texel.ar;
	normal.z = sqrt(1.0 - normal.x * normal.x - normal.y * normal.y);
	// material map
	float4 spec = tex2D(SpecSampler, input.tex0);
	
	// °¼Í¹Ó³Éä + Âþ·´Éä
	float d = (clamp(0, 1, dot(normal, input.lightDir)) + input.diffuse.x);
	// ¸ß¹â
	float s = input.diffuse.y * spec.r;
	
    float4 diffuse = saturate(tex2D(DiffuseSampler, input.tex0) * (d + s + ambientLight) + spec.b);
    return float4(diffuse.xyz, 1.0f);
}



struct PS_INPUT_SHADOW
{
	float4 pos		: POSITION;
	float2 depth	: TEXCOORD0;
	
};


PS_INPUT_SHADOW VS_SHADOW(float4 pos : POSITION, uint4 blendIndex : BLENDINDICES)
{
	PS_INPUT_SHADOW output;
	// ÃÉÆ¤
	float4x4 matBone = amPalette[blendIndex.x];
	float4 posModel = mul(pos, matBone);	
	float4 posWorld = mul(posModel, matWorld);
	
	output.pos = mul(posWorld, matVPLight);
	output.depth = output.pos.zw;
	return output;
}


float4 PS_SHADOW(PS_INPUT_SHADOW input) : COLOR
{
	// save the depth value to render target
	float d = input.depth.x / input.depth.y;
	return float4(d, d, d, d);
}



//--------------------------------------------------------------------------------------
technique Render
{
    pass P0
    {
		VertexShader = compile vs_2_0 VS();
        PixelShader  = compile ps_2_0 PS(); 
    }
}

technique Shadow
{
	pass P0
	{
		VertexShader = compile vs_2_0 VS_SHADOW();
        PixelShader  = compile ps_2_0 PS_SHADOW(); 
	}
}

