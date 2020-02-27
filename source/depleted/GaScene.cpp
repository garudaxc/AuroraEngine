#include "stdHeader.h"
#include "GaScene.h"
#include "GaMathFunction.h"
#include "GaConsole.h"
#include "GaUtil.h"
#include "GaCamera.h"
#include "GaTimer.h"
#include "GaAttachable.h"
#include "GaPipeline.h"
#include "GaController.h"
#include "tinyxml.h"

namespace Ga
{


SceneNode::SceneNode():m_vPosLocal(Vector3f::ZERO),m_qRotLocal(Quaternionf::IDENTITY),
	m_vPosWorld(Vector3f::ZERO),m_qRotWorld(Quaternionf::IDENTITY),m_vScale(1.0f, 1.0f, 1.0f),
	m_mTransform(Matrix4f::IDENTITY),m_bDynamic(false),m_bUseEulerAngle(false),
	m_vEulerAngle(Vector3f::ZERO),m_pParent(NULL),m_bVisible(true),m_nOcclusionId(-1)
{
}

void SceneNode::SetTranslation(const Vector3f& vTrans)
{
	m_vPosLocal = vTrans;
}


void SceneNode::SetScale(const Vector3f& vScale)
{
	m_vScale = vScale;
}

void SceneNode::SetRotation(const Quaternionf& qRot)
{
	m_qRotLocal = qRot;
	//QuaternionRotationMatrix(m_mRotLocal, qRot);
}


void SceneNode::Translate(const Vector3f& vTrans, int ref)
{
	assert(0);
	/*if (ref == TR_LOCAL)
	{
		// move toward its own heading
		Vector3f vT;
		Vector3Transform(vT, vTrans, m_mRotLocal);
		m_vPosLocal += vT;
	}
	else if (ref == TR_PARENT)
	{
		// parent heading
		m_vPosLocal += vTrans;
	}
	else	// TR_WORLD
	{
		// move in the world
		if (GetParentNode())
		{
			Matrix3f inv;
			MatrixInverse(inv, GetParentNode()->GetWorldRotation());
			Vector3f vT;
			Vector3Transform(vT, vTrans, inv);
			m_vPosLocal += vT;
		}
	}*/
}


void SceneNode::Rotate(const Matrix3f& mRot, int ref)
{
	assert(0);
	//if (ref == TR_LOCAL)
	//{
	//	MatrixMultiply(m_mRotLocal, mRot, m_mRotLocal);
	//}
	//else if (ref == TR_PARENT)
	//{
	//	MatrixMultiply(m_mRotLocal, m_mRotLocal, mRot);
	//}
	//else	// TR_WORLD
	//{
	//}
}

void SceneNode::Rotate(const Quaternionf& qRot, int ref)
{
	assert(0);
	//Matrix3f mRot;
	//QuaternionRotationMatrix(mRot, qRot);
	//this->Rotate(mRot, ref);
}

void SceneNode::UpdateWorld(float fElapsedTime, bool bForce)
{
	if (m_bUseEulerAngle)
	{
		Matrix4f matRot;
		MatrixEulerXYZ(matRot, m_vEulerAngle.x, m_vEulerAngle.y, m_vEulerAngle.z);
		QuaternionFromRotationMatrix(m_qRotLocal, matRot);
	}

	if (bForce || m_bDynamic)
	{
		if (GetParentNode())
		{
			// Y = (X * R1 + T1) * R2 + T2
			// Y = X * R1 * R2 + T1 * R2 + T2
			// R = R1 * R2, T = T1 * R2 + T2
			QuaternionMultiply(m_qRotWorld, GetParentNode()->GetWorldRotation(), m_qRotLocal);
			Vector3Rotate(m_vPosWorld, m_vPosLocal, GetParentNode()->GetWorldRotation());
			m_vPosWorld += GetParentNode()->GetWorldTranslation();
		}
		else
		{
			m_vPosWorld = m_vPosLocal;
			m_qRotWorld = m_qRotLocal;
		}

		MatrixTransform(m_mTransform, m_qRotWorld, m_vPosWorld);

		Matrix4f matScale;
		MatrixScale(matScale, m_vScale);
		MatrixMultiply(m_mTransform, matScale, m_mTransform);
	}

	/*if (bForce)
	{
		for (vector<shared_ptr<Attachable>>::iterator it = m_AttachedObj.begin();
			it != m_AttachedObj.end(); ++it)
		{
			(*it)->UpdateFromParentNode();
		}
	}*/



	std::vector<SceneNodePtr>::iterator it = m_vChildren.begin();
	for (; it != m_vChildren.end(); ++it)
	{
		(*it)->UpdateWorld(fElapsedTime, bForce);
	}
}

void SceneNode::AddPositionController(const ControllerPtr& pController)
{
	m_Controllers.push_back(pController);
	pController->BindFloats(m_vPosLocal.Ptr(), 3);
	m_bDynamic = true;
}


void SceneNode::AddRotationController(const ControllerPtr& pController)
{
	m_Controllers.push_back(pController);
	pController->BindFloats(m_qRotLocal.Ptr(), 4);
	m_bDynamic = true;
}

void SceneNode::AddEulerController(const ControllerPtr& pController, uint nAxis)
{
	m_Controllers.push_back(pController);
	pController->BindFloats(m_vEulerAngle.Ptr() + nAxis, 1);
	m_bUseEulerAngle = true;
	m_bDynamic = true;
}

const string& SceneNode::GetTypeName() const
{
	static string typeName = "Dummy";
	return typeName;
}

void SceneNode::EnumEditableProperty(vector<pair<string, string>>& properties)
{
	properties.push_back(make_pair("Name", "string"));
}

const string SceneNode::GetEditableProperty(const string& propertyName)
{
	if (propertyName == "Name")
	{
		return m_strName;
	}
	else
	{
		return string();
	}
}

void SceneNode::SetEditableProperty(const string& propertyName, const string& value)
{
	if (propertyName == "Name")
	{
		m_strName = value;
	}
}

void SceneNode::Archive(TiXmlNode* pXmlNode, bool bRead)
{
	if (bRead)
	{
		SetName(pXmlNode->ToElement()->Attribute("Name"));

		TiXmlNode* pElem = pXmlNode->FirstChild();
		while (pElem)
		{
			if (strcmp(pElem->Value(), "Transform") == 0)
			{
				string text;
				text = pElem->ToElement()->Attribute("Position");
				SetTranslation(ParseVec3(text));

				text = pElem->ToElement()->Attribute("Rotation");
				SetRotation(ParseRotation(text));

				text = pElem->ToElement()->Attribute("Scale");
				SetScale(ParseVec3(text));
			}
			else if (strcmp(pElem->Value(), "Properties") == 0)
			{
				if (pElem->FirstChild())
				{
					string text = pElem->FirstChild()->ToText()->Value();
					SetPropertity(text);
				}
			}

			pElem = pElem->NextSibling();
		}
	}
	else
	{
		Quaternionf qRot = GetWorldRotation();
		Vector3f vPos = GetWorldTranslation();
		Vector3f vScale = m_vScale;

		TiXmlElement* pElemNode = (TiXmlElement*)pXmlNode;
		pElemNode->SetAttribute("Name", GetName().c_str());

		TiXmlElement* pXmlTransform = LinkNewXmlElement(pElemNode, "Transform");

		char buff[128];
		sprintf_s(buff, sizeof(buff), "%.3f,%.3f,%.3f", vPos.x, vPos.y, vPos.z);
		pXmlTransform->SetAttribute("Position", buff);
		sprintf_s(buff, sizeof(buff), "%.3f,%.3f,%.3f,%.3f", qRot.x, qRot.y, qRot.z, qRot.w);
		pXmlTransform->SetAttribute("Rotation", buff);
		sprintf_s(buff, sizeof(buff), "%.3f,%.3f,%.3f", vScale.x, vScale.y, vScale.z);
		pXmlTransform->SetAttribute("Scale", buff);

		TiXmlElement* pXmlProperties = LinkNewXmlElement(pElemNode, "Properties");
		LinkNewXmlText(pXmlProperties, GetPropertity().c_str());
	}
}


void SceneNode::UpdateController(float dt)
{
	for (vector<ControllerPtr>::iterator it = m_Controllers.begin();
		it != m_Controllers.end(); ++it)
	{
		(*it)->AdvanceTime(dt);
	}


	for (vector<shared_ptr<Attachable>>::const_iterator it = m_AttachedObj.begin();
		it != m_AttachedObj.end(); ++it)
	{
		(*it)->UpdateController(dt);
	}
	
	for (std::vector<SceneNodePtr>::iterator it = m_vChildren.begin();
		it != m_vChildren.end(); ++it)
	{
		(*it)->UpdateController(dt);
	}
}

void SceneNode::AddChildNode(const SceneNodePtr& pNode)
{
	pNode->SetParentNode(this);
	m_vChildren.push_back(pNode);
	
}

SceneNodePtr SceneNode::GetChildNode(uint index)
{
	return m_vChildren[index];
}

SceneNodePtr RemoveChildNode(uint index)
{
	return SceneNodePtr();
}

SceneNode* SceneNode::FindNodeByName(const std::string& name)
{	
	if (m_strName == name)
	{
		return this;
	}

	std::vector<SceneNodePtr>::iterator it = m_vChildren.begin();
	for (; it != m_vChildren.end(); ++it)
	{
		SceneNode* pNode = (*it)->FindNodeByName(name);
		if (pNode)
		{
			return pNode;
		}
	}

	return NULL;
}

uint SceneNode::GetCount() const
{
	uint count = 0;
	std::vector<SceneNodePtr>::const_iterator it = m_vChildren.begin();
	for (; it != m_vChildren.end(); ++it)
	{
		count += (*it)->GetCount();
	}
	return count + 1;
}

void SceneNode::GetNodes(std::vector<SceneNodePtr>& nodes)
{
	std::vector<SceneNodePtr>::iterator it = m_vChildren.begin();
	for (; it != m_vChildren.end(); ++it)
	{
		nodes.push_back(*it);
		(*it)->GetNodes(nodes);
	}
}


void SceneNode::SetVisible(bool bVisible)
{
	m_bVisible = bVisible;
	std::vector<SceneNodePtr>::iterator it = m_vChildren.begin();
	for (; it != m_vChildren.end(); ++it)
	{
		(*it)->SetVisible(bVisible);
	}
}


void SceneNode::UpdateAABB()
{
	m_AABB.Reset();
	std::vector<SceneNodePtr>::iterator it = m_vChildren.begin();
	for (; it != m_vChildren.end(); ++it)
	{

		(*it)->UpdateAABB();
		m_AABB.Merge(m_AABB, (*it)->GetAABB());
	}
}


void SceneNode::OnUpdateShaderConstF(int id, float* pOutBuffer, uint nCount)
{
	switch (id)
	{
	case SHADER_CONST_MATRIX_WORLD:
		{
			MapMatrixToShaderConst(GetWorldTransform(), pOutBuffer, nCount);
		}
		break;
	case SHADER_CONST_WORLD_POS:
		{
			Matrix4f& matWorld = GetWorldTransform();
			pOutBuffer[0] = matWorld._41;
			pOutBuffer[1] = matWorld._42;
			pOutBuffer[2] = matWorld._43;
		}
		break;
	}

}

void SceneNode::Attach(const shared_ptr<Attachable>& obj)
{
	obj->SetParentNode(this);
	m_AttachedObj.push_back(obj);
}


uint SceneNode::GetAttachableRecursive(vector<Attachable*>& attachables, Attachable::Type type) const
{
	for (vector<AttachablePtr>::const_iterator it = m_AttachedObj.begin();
		it != m_AttachedObj.end(); ++it)
	{
		if ((*it)->GetType() == type)
		{
			attachables.push_back((*it).get());
		}
	}

	std::vector<SceneNodePtr>::const_iterator it = m_vChildren.begin();
	for (; it != m_vChildren.end(); ++it)
	{
		(*it)->GetAttachableRecursive(attachables, type);
	}
	return (uint)attachables.size();
}


void SceneNode::FlushToPipeline(Pipeline* pPipeline)
{
	//for (vector<AttachablePtr>::iterator it = m_AttachedObj.begin();
	//	it != m_AttachedObj.end(); ++it)
	//{
	//	pPipeline->PushObject((*it).get());
	//}
}

void SceneNode::SetOcclusionId(int id)
{
	m_nOcclusionId = id;
	for (vector<AttachablePtr>::iterator it = m_AttachedObj.begin();
		it != m_AttachedObj.end(); ++it)
	{
		(*it)->SetOcclusionId(id);
	}
}

//////////////////////////////////////////////////////////////////////////


Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::AddSceneNode(SceneNodePtr pNode)
{
	m_SceneNodes.push_back(pNode);
}

void Scene::RemoveSceneNode(SceneNodePtr pNode)
{
	for (vector<SceneNodePtr>::iterator it = m_SceneNodes.begin();
		it != m_SceneNodes.end(); ++it)
	{
		if ((*it) == pNode)
		{
			m_SceneNodes.erase(it);
			return;
		}
	}
}

void Scene::UpdateWorld(float fElapsedTime, bool bForce)
{
	for (vector<SceneNodePtr>::iterator it = m_SceneNodes.begin();
		it != m_SceneNodes.end(); ++it)
	{
		(*it)->UpdateController(fElapsedTime);
		(*it)->UpdateWorld(fElapsedTime, bForce);
		(*it)->UpdateAABB();
	}
}

uint Scene::GetNodeCount() const
{
	return m_SceneNodes.size();
}

void Scene::FlushToPipeline(Pipeline* pPipeline)
{
	for (vector<SceneNodePtr>::iterator it = m_SceneNodes.begin();
		it != m_SceneNodes.end(); ++it)
	{
		(*it)->FlushToPipeline(pPipeline);
	}
}

void Scene::CleanUp()
{
	m_SceneNodes.clear();
}

}