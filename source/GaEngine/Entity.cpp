#include "stdheader.h"
#include "Entity.h"
#include "Model.h"
#include "Camera.h"
#include "LightNode.h"
#include "Transform.h"

namespace Aurora
{

	RenderStrategy::RenderStrategy()
	{

	}

	RenderStrategy::~RenderStrategy()
	{

	}

	//////////////////////////////////////////////////////////////////////////

	Entity::Entity(const string& name):m_Name(name)
	{

	}

	Entity::~Entity()
	{

	}

	void Entity::SetTransform(const Transform& transform)
	{
		m_Transform = transform;
	}

	const Matrix4f&	Entity::GetWorldTransform() const
	{
		const Matrix4f& matWorld = m_Transform.GetWorldTransform();
		return matWorld;
	}

	void Entity::Update()
	{
		m_Transform.Update();
	}

	uint Entity::GetNumRenderable() const
	{
		return m_pModel->GetNumRenderables();
	}

	Renderable* Entity::GetRenderable(uint idx) const
	{
		return m_pModel->GetRenderable(idx);
	}


	//////////////////////////////////////////////////////////////////////////
	static EntityFactory* g_pEntityFactory = NULL;
	void EntityFactory::Initialize()
	{
		assert(g_pEntityFactory == NULL);
		g_pEntityFactory = new EntityFactory();
	}

	void EntityFactory::Finalize()
	{
		assert(g_pEntityFactory != NULL);
		delete g_pEntityFactory;
		g_pEntityFactory = NULL;
	}

	EntityFactory* EntityFactory::Get()
	{
		assert(g_pEntityFactory != NULL);
		return g_pEntityFactory;
	}


	inline Vector3f ParseVec3(const char* pStr)
	{
		float x, y, z;
		sscanf_s(pStr, "%f,%f,%f", &x, &y, &z);
		return Vector3f(x, y, z);
	}

	Transform* CreateTransfromStrategy(TiXmlElement* pXmlTransform)
	{
		Transform* pStrategy = new Transform();

		const char* pStr = NULL;
		if (pXmlTransform == NULL)
		{
			pStrategy->SetLocalTranslation(Vector3f::ZERO);
			pStrategy->SetLocalRotation(Quaternionf::IDENTITY);
			pStrategy->SetLocalScale(Vector3f(1.f, 1.f, 1.f));
		} 
		else
		{
			pStr = pXmlTransform->ToElement()->Attribute("Position");
			pStrategy->SetLocalTranslation(ParseVec3(pStr));

			pStr = pXmlTransform->ToElement()->Attribute("Rotation");
			pStrategy->SetLocalRotation(ParseRotation(pStr));

			pStr = pXmlTransform->ToElement()->Attribute("Scale");
			pStrategy->SetLocalScale(ParseVec3(pStr));
		}

		return pStrategy;
	}

	/*RenderStrategy* CreateRenderStrategy(TiXmlElement* pXmlRenderStrategy)
	{
		RenderStrategy* pRenderStrategy = NULL;
		const char* pType = pXmlRenderStrategy->Attribute("Type");
		if (strcmp(pType ,"Model") == 0)
		{
			TiXmlElement* pXmlModel = pXmlRenderStrategy->FirstChildElement("Model");
			assert(pXmlModel);

			const char* pModelName = pXmlModel->Attribute("Ref");
			Model* pModel = ModelManager::Get()->GetModel(pModelName);
			assert(pModel);

			pRenderStrategy = new RenderStrategyModel(pModel);
		}
		else if (strcmp(pType ,"Light") == 0)
		{
			pRenderStrategy = LightManager::Get()->CreateLight(pXmlRenderStrategy);
		}

		return pRenderStrategy;
	}*/


	Model* CreateModel(TiXmlElement* pXmlRendering)
	{
		RenderStrategy* pRenderStrategy = NULL;
		const char* pType = pXmlRendering->Attribute("Type");
		if (strcmp(pType ,"Model") == 0)
		{
			TiXmlElement* pXmlModel = pXmlRendering->FirstChildElement("Model");
			assert(pXmlModel);

			const char* pModelName = pXmlModel->Attribute("Ref");
			Model* pModel = GModelManager.GetModel(pModelName);
			assert(pModel);

			return pModel;
		}

		return NULL;
	}


	Entity* EntityFactory::CreateEntity(TiXmlElement* pXmlEntity)
	{
		const char* pName = pXmlEntity->Attribute("Name");
		assert(pName);

		Entity* pEntity = new Entity(string(pName));

		{
			TiXmlElement* pXmlElem = pXmlEntity->FirstChildElement("Transform");
			assert(pXmlElem);
			Transform* pStrategy = CreateTransfromStrategy(pXmlElem);
			pEntity->SetTransform(*pStrategy);
		}

		{
			TiXmlElement* pXmlElem = pXmlEntity->FirstChildElement("Rendering");
			assert(pXmlElem);

			Model* pModel = CreateModel(pXmlElem);
			pEntity->SetModel(pModel);
		}

		return pEntity;
	}


	Entity* EntityFactory::CreateEntity(Model* pModel)
	{
		Entity* pEntity = new Entity(string(""));

		{
			Transform* pStrategy = CreateTransfromStrategy(NULL);
			pEntity->SetTransform(*pStrategy);
		}

		{
			//RenderStrategy* pStrategy = new RenderStrategyModel(pModel);
			//pEntity->SetRenderStrategy(pStrategy);

			pEntity->SetModel(pModel);
		}

		return pEntity;
	}


}