#pragma once

namespace Aurora
{
	class CGeometry;
	class Renderable;
	class MaterialInstance;
	class RenderableVisitor;

	class Model
	{
	public:
		Model(const string& name);
		~Model();

		void		SetMesh(CGeometry* pMesh);
		void		SetMaterial(uint idx, MaterialInstance* pMtl);

		const string&	GetName() const		{	return m_Name;	}

		void	Accept(RenderableVisitor& visitor);


	protected:
		string			m_Name;
		CGeometry*		m_pMesh;
	};


	class CModelManager
	{
	public:

		void Initialize();
		void Finalize();

		Model*		GetModel(const ResourceID& id);

		Model* GetDefaultModel()	{	return m_pDefault;	}

	private:
		Model*		Find(const ResourceID& id);
		Model*		Load(const ResourceID& id);

		map<ResourceID, Model*>		m_ResourcePool;

		Model* m_pDefault = nullptr;
	};

	extern CModelManager ModelManager;

}