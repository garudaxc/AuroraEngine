#pragma once

namespace Aurora
{
	class Geometry;
	class Renderable;
	class MaterialInstance;
	class RenderableVisitor;

	class Model
	{
	public:
		Model(const string& name);
		~Model();

		void		SetMesh(Geometry* pMesh);
		void		SetMaterial(uint idx, MaterialInstance* pMtl);

		const string&	GetName() const		{	return m_Name;	}

		void	Accept(RenderableVisitor& visitor);

	protected:
		string			m_Name;
		Geometry*		m_pMesh;
	};


	class ModelManager
	{
	public:
		ModelManager();
		~ModelManager();

		void Initialize();
		void Finalize();

		Model*		GetModel(const ResourceID& id);

		Model* GetDefaultModel()	{	return m_pDefault;	}
		Model* GetDirLight()		{	return m_pDirLight;	}
		Model* GetPointLight()		{	return m_pPointLight;	}
		Model* GetSpotLight()		{	return m_pSpotLight;	}

	private:
		Model*		Find(const ResourceID& id);
		Model*		Load(const ResourceID& id);

		map<ResourceID, Model*>		m_ResourcePool;

		Model* m_pDefault;
		Model* m_pDirLight;
		Model* m_pPointLight;
		Model* m_pSpotLight;
	};

	extern ModelManager GModelManager;

}