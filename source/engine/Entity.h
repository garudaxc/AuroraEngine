#pragma once
#include "tinyxml.h"
#include "Transform.h"

namespace Aurora
{

	class Model;
	class Renderable;
	class Camera;
	class RenderingContext;
	class RenderableVisitor;
	
	class RenderStrategy
	{
	public:
		RenderStrategy();
		virtual ~RenderStrategy();

		void			SetWorldTransform(const Matrix4f& matWorld)		{	m_matWorld = matWorld;	}
		const Matrix4f&	GetWorldTransform() const		{	return m_matWorld;	}

		virtual uint		GetNumRenderable()			{	return 0;	}
		virtual Renderable*	GetRenderable(uint idx)		{	return nullptr;	}

		virtual void		PushToPipe(RenderingContext* pContext)	{}

	protected:

		Matrix4f	m_matWorld;
	};


	class Entity
	{
	public:
		Entity(const string& name);
		~Entity();

		const string& GetName() const		{	return m_Name;	}
		
		virtual void Update();
		
		void SetModel(Model* pModel)	{	m_pModel = pModel;	}

		void SetTransform(const Transform& transform);

		Transform& GetTransfrom()		{	return m_Transform;	}
		const Transform& GetTransfrom() const	{	return m_Transform;	}
		
		void		Accept(RenderableVisitor& visitor);

	private:
		string	m_Name;

		Transform m_Transform;
		
		Model* m_pModel;
	};


	class CEntityFactory
	{
	public:
		void		Initialize();
		void		Finalize();
		
		Entity*		CreateEntity(TiXmlElement* pXmlEntity);
		Entity*		CreateEntity(Model* pModel);

		Entity*		CreateCameraEntity(const string& name);

		Camera*		GetCameraObject(Entity* pCameraEntity);

	private:

	};

	extern CEntityFactory EntityFactory;




}