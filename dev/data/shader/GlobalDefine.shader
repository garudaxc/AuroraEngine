

cbuffer GlobalParameter: register(b0)
{
	float4x4 matView;
	float4x4 matInvView;
	float4x4 matProj;
	float4x4 matViewProj;
	float3	DirectionLight0;
	float4	LightColor0;
};