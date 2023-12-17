
#include "stdHeader.h"
#include "Model.h"
#include "CGeometry.h"
#include "RendererObject.h"
#include "Renderer.h"
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

	void Model::SetMesh(CGeometry* pMesh)
	{
		mMesh = pMesh;
	}

	void Model::SetMaterial(uint idx, MaterialInstance* pMtl)
	{
		//assert(idx < m_Renderables.size());
		//m_Renderables[idx]->SetMaterialInst(pMtl);
	}

	void Model::Accept(RenderableVisitor& visitor)
	{
		for (uint i = 0; i < mMesh->GetNumElement(); i++)
		{
			auto elem = mMesh->mElements[i];
			RenderOperator op(-1, mMesh->GetGeometryBuffer(), RenderOperator::PT_TRIANGLELIST,
				0, elem.IndexStart, elem.IndexCount, 0);
			
			visitor.Visit(op);
		}		
	}

	//////////////////////////////////////////////////////////////////////////

	CModelManager ModelManager;


	void CModelManager::Initialize()
	{
	}

	void CModelManager::Finalize()
	{
		m_ResourcePool.clear();
	}


	Model* CModelManager::GetModel(const ResourceID& id)
	{
		Model* pModel = Find(id);
		if (pModel == nullptr)
		{
			return Load(id);
		}
		return pModel;
	}

	Model* CModelManager::Find(const ResourceID& id)
	{
		map<ResourceID, Model*>::iterator it = m_ResourcePool.find(id);
		if (it != m_ResourcePool.end())
		{
			return it->second;
		}
		return nullptr;
	}

	Model* CModelManager::Load(const ResourceID& id)
	{
		string pathName = Util::ProcessPathName(id);

		TiXmlDocument doc;
		bool bOK = doc.LoadFile(pathName.c_str());
		if (!bOK)
		{
			// log open log file failed
			return nullptr;
		}

		TiXmlElement* pXmlModel = doc.FirstChild()->ToElement();
		if (strcmp(pXmlModel->Value(), "Model") != 0)
		{
			GLog->Error("file \"%s\"is not a Model file!", id.c_str());
			return nullptr;
		}

		string name = pXmlModel->Attribute("name");
		Model* pModel = new Model(name);		

		CGeometry* pGeometry = nullptr;
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
			while (pXmlInst != nullptr)
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