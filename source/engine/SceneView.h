#pragma once
#include "Matrix.h"
#include "Light.h"

namespace Aurora
{

	class Camera;
	class Entity;
	class RenderingContext;



	class DrawingEntity
	{
	public:
		Entity* pEntity;

		Matrix4f mWorld;

		DrawingEntity* pNext;		
	};

	class DrawingLight
	{
	public:

		ELightType Type;

		Matrix4f mWorld;

		Color cColor;
		Color cShadowColor;

		float fRange;
		float fInnerCone;
		float fOutterCone;

		uint bCastShadow : 1;

		DrawingLight* pNext;	
	};



	class SceneView
	{
	public:
		Matrix4f matView;
		Matrix4f matProj;
		Matrix4f matViewProj;
		float fNearPlane;	
		float fFarPlane;
		int nWidth, nHeight;

		Vector3f vEyePos;

		DrawingEntity* pDrawingEntity;
		DrawingLight* pDrawingLight;
	};


	class FrameMemoryBuffer
	{
	public:
		FrameMemoryBuffer();
		~FrameMemoryBuffer();

		void* Alloc(size_t size);
		SceneView* AllocSceneView();
		DrawingEntity* AllocDrawingEntity();
		DrawingLight* AllocDrawingLight();

		// clear once a frame
		void Clear();

		void ClearAndSwap();

	private:
		uint m_nSize;
		char* m_pBuffer;
		char* m_pPos;

		char* m_pBufferA;
		char* m_pBufferB;
	};

	extern FrameMemoryBuffer* GFrameMemoryBuffer;


}