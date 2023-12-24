
#ifndef USE_TANGENT
#define USE_TANGENT 0
#endif

#ifndef USE_VERTEX_COLOR
#define USE_VERTEX_COLOR 0
#endif



cbuffer StaticBuffer: register(b1)	
{									
	float2	HammersleySample[1024];	
};									



float2 Hammersley(int i, int NumSamples)
{
    return HammersleySample[i];
}



/*


//////////////////////////////////////////
// Engine Variables
/////////////////////////////
    float4x4 matWorldViewProj : WorldViewProjection;
    float4x4 matWorld         : World;
    float4x4 matView          : View;

    float3 EyePos             : CameraPosition;
    float  TanHalfFOV         : TanHalfFOV;
    float  ViewAspect         : ViewAspect;
    float2 InvScreenDim       : InvScreenDimensions;

    texture tRT_Depths : RT_Depths;
    sampler RT_Depths = sampler_state { 
                            texture = <tRT_Depths>; 
                            MIPFILTER = POINT;    MINFILTER = POINT;    MAGFILTER = POINT;
                        };

struct VS_INPUT
{
    float4 Position    : POSITION;
};

struct VS_OUTPUT
{
    float4 Position    : POSITION;
    float3 LightPos    : TEXCOORD0;
    float4 vPos        : TEXCOORD1;
    float3 vEyeRay     : TEXCOORD2;
};

float4 ConvertToVPos( float4 p )
{
	return float4( 0.5*( float2(p.x + p.w, p.w - p.y) + p.w*InvScreenDim.xy), p.zw);
}

float3 CreateEyeRay( float4 p )
{
	float3 ViewSpaceRay = float3( p.x*TanHalfFOV*ViewAspect, p.y*TanHalfFOV, p.w);

      return mul( matView, v ); // or multiply by the ViewInverse in the normal orden
}

VS_OUTPUT vs_main(VS_INPUT Input)
{ 
    VS_OUTPUT Output;   

    Output.Position = mul( Input.Position, matWorldViewProj);
 
    Output.LightPos = mul( float4(0,0,0,1), matWorld ); // I can just get the matrix translation directly, but this translate to the same thing
    Output.vPos = ConvertToVPos( Output.Position );
    Output.vEyeRay = CreateEyeRay( Output.Position );
    
    return Output;
}

float4 ps_main(VS_OUTPUT Input) : COLOR
{
    float3 Depth = tex2Dproj(RT_Depths, Input.vPos);
           Input.vEyeRay.xyz /= Input.vEyeRay.z;
    float3 PixelPos   = Input.vEyeRay.xyz * Depth + EyePos;

    float3 LightDir         = Input.LightPos - PixelPos;
    float SqrLightDirLen    = dot(LightDir, LightDir);

    return SqrLightDirLen / 100.0;   // show the scaled distance to the light pos
}

technique Example
{
    pass p0 
    {    
        VertexShader = compile vs_1_1 vs_main();        
        PixelShader  = compile ps_2_0 ps_main();        
    }
}
*/