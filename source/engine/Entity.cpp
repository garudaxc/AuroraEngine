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


	void Entity::Update()
	{
	}

	void Entity::Accept(RenderableVisitor& visitor)
	{
		m_pModel->Accept(visitor);
	}


	//////////////////////////////////////////////////////////////////////////

	void CEntityFactory::Initialize()
	{
	}

	void CEntityFactory::Finalize()
	{
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
			pStrategy->SetTranslation(Vector3f::ZERO);
			pStrategy->SetRotation(Quaternionf::IDENTITY);
			pStrategy->SetScale(Vector3f(1.f, 1.f, 1.f));
		} 
		else
		{
			pStr = pXmlTransform->ToElement()->Attribute("Position");
			pStrategy->SetTranslation(ParseVec3(pStr));

			pStr = pXmlTransform->ToElement()->Attribute("Rotation");
			pStrategy->SetRotation(ParseRotation(pStr));

			pStr = pXmlTransform->ToElement()->Attribute("Scale");
			pStrategy->SetScale(ParseVec3(pStr));
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
			Model* pModel = ModelManager.GetModel(pModelName);
			assert(pModel);

			return pModel;
		}

		return NULL;
	}


	Entity* CEntityFactory::CreateEntity(TiXmlElement* pXmlEntity)
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


	Entity* CEntityFactory::CreateEntity(Model* pModel)
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


	CEntityFactory EntityFactory;

}