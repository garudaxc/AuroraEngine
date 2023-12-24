

float4x4 matWorld;

float4 CameraParam;
float3 CameraVec[3];

// x : 1 / back buffer width
// y : 1 / back buffer height
// z : back buffer width
// w : back buffer height
float4 BackBufferSizeParam;


struct GeomVertexInput
{
    float4 pos		: SV_Position;
    float3 normal	: NORMAL;
	float4 tangentEx: TANGENT;
	float2 tex		: TEXCOORD0;

#if USE_TANGENT
#endif
};



float4 GetHomogeneousCoord(GeomVertexInput input)
{
	float4 worldPos = mul(float4(input.pos.xyz, 1.0), mWorldMatrix);
	return mul(worldPos, mViewProjectionMatrix);
}


void GetHomogeneousCoordAndDepth(GeomVertexInput input, out float4 pos, out float depth)
{
	float4 worldPos = mul(float4(input.pos.xyz, 1.0), mWorldMatrix);
	pos = mul(worldPos, mViewProjectionMatrix);
	depth = pos.z / CameraParam.y;
}


float4 GetLightHomogeneousCoord(float3 pos)
{
	float4 worldPos = mul(float4(pos, 1.0), mWorldMatrix);
	return mul(worldPos, mViewProjectionMatrix);
}

/*
void GetNTB(GeomVertexInput input, out float3 normal, out float3 tangent, out float3 binormal)
{
	normal = normalize(mul(input.normal, (float3x3)mWorldMatrix));
	tangent = normalize(mul(input.tangentEx.xyz, (float3x3)mWorldMatrix));
	binormal = cross(normal, tangent) * input.tangentEx.w;
}
*/

float3 GetNormal(GeomVertexInput input)
{
	return normalize(mul(input.normal, (float3x3)mWorldMatrix));
}

float3 GetEyeDir(float4 hPos)
{
	float2 uv = hPos.xy * CameraParam.zw;
	return CameraVec[0] * hPos.w + CameraVec[1] * uv.x + CameraVec[2] * uv.y;
}
