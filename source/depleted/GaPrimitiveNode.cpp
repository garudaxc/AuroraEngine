#include "stdHeader.h"
#include "GaPrimitiveNode.h"
#include "GaMaterialInstance.h"
#include "GaUtil.h"

namespace Ga
{


	static ResourcePtr<Geometry> LoadXmlMesh(const string& name)
	{
		string fileName = string("../res/") + name;
		TiXmlDocument doc;
		bool loadOK = doc.LoadFile(fileName.c_str());
		assert(loadOK);

		Geometry* pGeomData = new Geometry();
		pGeomData->Load(doc.FirstChild()->ToElement());

		Global::GetResourceManager()->AddGeometryData(name, pGeomData);

		return Global::GetResourceManager()->GetGeometryData(name); 
	}


	PrimitiveNode::PrimitiveNode():m_bCastShadow(false)
	{

	}

	PrimitiveNode::~PrimitiveNode()
	{

	}

	const string& PrimitiveNode::GetTypeName() const
	{
		static string typeName = "StaticMesh";
		return typeName;
	}


	void PrimitiveNode::EnumEditableProperty(vector<pair<string, string>>& properties)
	{
		super::EnumEditableProperty(properties);
		properties.push_back(make_pair("CastShadow", "bool"));
	}

	const string PrimitiveNode::GetEditableProperty(const string& propertyName)
	{
		if (propertyName == "CastShadow")
		{
			return m_bCastShadow ? "1" : "0";
		}
		else
		{
			return super::GetEditableProperty(propertyName);
		}
	}

	void PrimitiveNode::SetEditableProperty(const string& propertyName, const string& value)
	{
		if (propertyName == "CastShadow")
		{
			if (value == "1")
			{
				m_bCastShadow = true;
			}
			else
			{
				m_bCastShadow = false;
			}
		}
		else
		{
			super::SetEditableProperty(propertyName, value);
		}
	}

	void PrimitiveNode::Archive(TiXmlNode* pXmlPrimitiveNode, bool bRead)
	{
		SceneNode::Archive(pXmlPrimitiveNode, bRead);
		if (bRead)
		{
			assert(pXmlPrimitiveNode->FirstChild("Mesh"));
			TiXmlElement* pElem = pXmlPrimitiveNode->FirstChild("Mesh")->ToElement();
			string meshName = pElem->Attribute("Ref");

			SetMesh(meshName);

			if (strcmp(pElem->Attribute("CastShadow"), "1") == 0)
			{
				m_bCastShadow = true;
			}
			else
			{
				m_bCastShadow = false;
			}
		}
		else
		{
			TiXmlElement* pElemNode = (TiXmlElement*)pXmlPrimitiveNode;

			TiXmlElement* pXmlMesh = LinkNewXmlElement(pElemNode, "Mesh");

			pXmlMesh->SetAttribute("Ref", m_MeshName.c_str());
			pXmlMesh->SetAttribute("CastShadow", m_bCastShadow ? "1" : "0");
		}
	}

	void PrimitiveNode::SetMesh(const string& meshName)
	{
		//m_MeshName = meshName;
		//string pathName = string("scene1/") + m_MeshName;

		//LoadXmlMesh(pathName);

		//RenderableObjectPtr pRenderable = Global::GetResourceManager()->CreateRenderableObject(pathName, ResourceManager::VEXDESC_PNTT);

		//MaterialInstancePtr pMatInst = Global::GetResourceManager()->GetMaterialInstLib()->GetMaterialInst("DefaultMaterialInstances");
		//pRenderable->SetMaterialInst(pMatInst);

		//Attach(pRenderable);
	}

}