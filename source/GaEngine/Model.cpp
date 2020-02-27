#include "stdHeader.h"
#include "Model.h"
#include "Geometry.h"
#include "RendererObject.h"
#include "Config.h"
#include "Log.h"
#include "MaterialInstance.h"
#include "Util.h"
#include "Mesh.h"
namespace Aurora
{


	//////////////////////////////////////////////////////////////////////////

	Model::Model(const string& name):m_Name(name)
	{

	}

	Model::~Model()
	{

	}

	void Model::SetMesh(Geometry* pMesh)
	{
		m_pMesh = pMesh;
		for (uint i = 0; i < pMesh->GetNumElement(); i++)
		{
			Renderable* pRenderable = new Renderable(pMesh, i);
			m_Renderables.push_back(pRenderable);
		}
	}

	void Model::SetMaterial(uint idx, MaterialInstance* pMtl)
	{
		assert(idx < m_Renderables.size());
		m_Renderables[idx]->SetMaterialInst(pMtl);
	}

	uint Model::GetNumRenderables() const
	{
		return m_Renderables.size();
	}

	Renderable* Model::GetRenderable(uint index)
	{
		return m_Renderables[index];
	}

	const Renderable* Model::GetRenderable(uint index) const
	{
		return m_Renderables[index];
	}

	void Model::CreateRenderable()
	{
	}

	//////////////////////////////////////////////////////////////////////////

	ModelManager GModelManager;

	ModelManager::ModelManager()	{}
	ModelManager::~ModelManager()	{}

	void ModelManager::Initialize()
	{
		m_pDefault		= NULL;
		m_pDirLight		= NULL;
		m_pPointLight	= NULL;
		m_pSpotLight	= NULL;

		//m_pDirLight = GetModel("asset:DirectionalLight.Model");
		//m_pPointLight = GetModel("asset:PointLight.Model");
		//m_pSpotLight = GetModel("asset:SpotLight.Model");
	}

	void ModelManager::Finalize()
	{
		m_ResourcePool.clear();
	}


	Model* ModelManager::GetModel(const ResourceID& id)
	{
		Model* pModel = Find(id);
		if (pModel == NULL)
		{
			return Load(id);
		}
		return pModel;
	}

	Model* ModelManager::Find(const ResourceID& id)
	{
		map<ResourceID, Model*>::iterator it = m_ResourcePool.find(id);
		if (it != m_ResourcePool.end())
		{
			return it->second;
		}
		return NULL;
	}

	Model* ModelManager::Load(const ResourceID& id)
	{
		string pathName = Util::ProcessPathName(id);

		TiXmlDocument doc;
		bool bOK = doc.LoadFile(pathName.c_str());
		if (!bOK)
		{
			// log open log file failed
			return NULL;
		}

		TiXmlElement* pXmlModel = doc.FirstChild()->ToElement();
		if (strcmp(pXmlModel->Value(), "Model") != 0)
		{
			GLog->Error("file \"%s\"is not a Model file!", id.c_str());
			return NULL;
		}

		string name = pXmlModel->Attribute("name");
		Model* pModel = new Model(name);		

		Geometry* pGeometry = NULL;
		{
			string meshId = pXmlModel->FirstChild("Mesh")->FirstChild()->ToText()->Value();
			pGeometry = GMeshManager.GetMesh(meshId);
		}
		pModel->SetMesh(pGeometry);

		set<string> mtlInfo;
		pGeometry->GetMaterialInfo(mtlInfo);

		{
			TiXmlElement* pXmlMtl = pXmlModel->FirstChildElement("Material");
			TiXmlElement* pXmlInst = pXmlMtl->FirstChildElement();

			int idx = 0;
			while (pXmlInst != NULL)
			{
				string id = pXmlInst->FirstChild()->ToText()->Value();

				MaterialInstance* pMtlInst = GMaterialInstanceManager.GetMaterialInstance(id, mtlInfo);
//				assert(pMtlInst);
				pModel->SetMaterial(idx, pMtlInst);

				pXmlInst = pXmlInst->NextSiblingElement();
				idx++;
			}
		}

		m_ResourcePool.insert(make_pair(id, pModel));

		return pModel;
	}

}