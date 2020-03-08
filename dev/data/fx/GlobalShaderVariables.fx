
#ifndef GLOBALSHADERVARIABLES_FX
#define GLOBALSHADERVARIABLES_FX

shared cbuffer Transform
{
	float4x4 MatView;
	float4x4 MatProj;
	float4x4 MatViewProj;
	
	// x : 1 / back buffer width
	// y : 1 / back buffer height
	// z : back buffer width
	// w : back buffer height
	float4 BackBufferSize;
	
	// x : near plane
	// y : far plane
	// z : 1 / _11 of proj matrix
	// w : 1 / _22 of proj matrix
	float4 vCameraParam;	
	
	float3 vCameraVecX;
	float3 vCameraVecY;
	float3 vCameraVecZ;
	float3 vCameraPos;
	float3 vCameraLookDir;
	
	float3 vGlobalAmbient = {0.f, 0.f, 0.f};
};

	float GetNearPlane()
	{
		return vCameraParam.x;
	}
	
	float GetFarPlane()
	{
		return vCameraParam.y;
	}
	
shared Texture2D tGbuffer0;
shared Texture2D tGbuffer1;
shared Texture2D tLightingBuffer;

#ifndef _SKIN_
#define _SKIN_ 0
#endif

#endif