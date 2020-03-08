#pragma once

namespace Aurora
{

	class Entity;
	class Light;
	class SceneView;
	class Camera;	
	struct RectSize;

	class IScene
	{
	public:
		virtual void Release() = 0;

		virtual void Load(const string& fileName) = 0;
		virtual void Save(const string& fileName) = 0;

		virtual void AddEntity(Entity* pEntity) = 0;

		virtual void AddLight(Light* pLight) = 0;

		virtual int GetNumEntity() const = 0;

		virtual int GetNumLight() const = 0;

		virtual SceneView* MakeSceneView(const Camera* pCamera, const RectSize& viewSize) = 0;
	};

	IScene* CreateScene();

}