#ifndef GASCENE_H
#define GASCENE_H

#include "GaGeometry.h"
#include "GaSkinInstance.h"
#include "GaRenderer.h"
#include "GaCamera.h"
#include "GaLight.h"
#include "GaTexture.h"

namespace Ga
{
	using namespace boost;

class Renderable;
class SceneNode;
class Scene;
class Attachable;
class Pipeline;
class Controller;

typedef shared_ptr<SceneNode>		SceneNodePtr;
typedef shared_ptr<Controller>		ControllerPtr;


class SceneNode : public ShaderConstProvider
{
public:
	enum TransformReference
	{
		TR_LOCAL = 0,
		TR_PARENT,
		TR_WORLD,
	};

	enum Type
	{
		NT_GEOMETRY = 0,
		NT_LIGHT,
	};

	enum Flag
	{
		FL_DRAW_FROM_SYSMEM = 0x1,
	};

	SceneNode();
	virtual ~SceneNode() {}

	void					SetTranslation(const Vector3f& vTrans);
	void					SetRotation(const Quaternionf& qRot);
	void					SetScale(const Vector3f& vScale);

	void					Translate(const Vector3f& vTrans, int ref = TR_LOCAL);
	void					Rotate(const Matrix3f& mRot, int ref = TR_LOCAL);
	void					Rotate(const Quaternionf& qRot, int ref = TR_LOCAL);

	const string&			GetPropertity() const							{ return m_strPropertity;	}
	void					SetPropertity(const string& strPropertity)		{ m_strPropertity = strPropertity;	}


	const Vector3f&			GetLocalScale() const							{ return m_vScale;	}

	const Quaternionf&		GetWorldRotation() const						{ return m_qRotWorld; }
	const Vector3f&			GetWorldTranslation() const						{ return m_vPosWorld; }
	const Matrix4f&			GetWorldTransform() const						{ return m_mTransform; }

	Quaternionf&			GetWorldRotation()								{ return m_qRotWorld; }
	Vector3f&				GetWorldTranslation()							{ return m_vPosWorld; }
	Matrix4f&				GetWorldTransform()								{ return m_mTransform; }


	// param : 
	// bForce : force static node to update world transform too
	virtual void			UpdateWorld(float fElapsedTime, bool bForce);
	virtual	void			OnUpdateShaderConstF(int id, float* pOutBuffer, uint nCount);
	
	bool					IsDynamic() const								{	return m_bDynamic;	}

	void					AddPositionController(const ControllerPtr& pController);
	void					AddRotationController(const ControllerPtr& pController);
	void					AddEulerController(const ControllerPtr& pController, uint nAxis);

	void					UpdateController(float dt);

private:

	Vector3f				m_vPosLocal;
	Quaternionf				m_qRotLocal;

	Vector3f				m_vPosWorld;
	Quaternionf				m_qRotWorld;

	Vector3f				m_vScale;

	Matrix4f				m_mTransform;
	bool					m_bDynamic;

	Vector3f				m_vEulerAngle;
	bool					m_bUseEulerAngle;

	string					m_strPropertity;

	vector<ControllerPtr>	m_Controllers;



public:
	virtual void					Archive(TiXmlNode* pXmlNode, bool bRead);

	virtual void					EnumEditableProperty(vector<pair<string, string>>& properties);
	virtual const string			GetEditableProperty(const string& propertyName);
	virtual void					SetEditableProperty(const string& propertyName, const string& value);

	virtual const string&			GetTypeName() const;

	SceneNode*						GetParentNode();

	const std::string&				GetName();
	void							SetName(const string& name);

	uint							GetType() const	{	return m_nType;	}
	void							GetNodes(std::vector<SceneNodePtr>& nodes);

	void							AddChildNode(const SceneNodePtr& pNode);
	SceneNodePtr					GetChildNode(uint index);
	SceneNodePtr					RemoveChildNode(uint index);

	SceneNode*						FindNodeByName(const std::string& name);

	void							SetVisible(bool bVisible);
	bool							GetVisible() const			{ return m_bVisible; }

	uint							GetCount() const;

protected:
	uint								m_nType;

private:
	void								SetParentNode(SceneNode* pParentNode) { m_pParent = pParentNode; }
	SceneNode*							m_pParent;

	std::string							m_strName;
	std::vector<SceneNodePtr>			m_vChildren;
	bool								m_bVisible;


public:
	void							Attach(const shared_ptr<Attachable>& obj);
	uint							GetNumAttachable() const						{	return (uint)m_AttachedObj.size();		}
	Attachable*						GetAttachable(uint idx)							{	return m_AttachedObj[idx].get();	}
	uint							GetAttachableRecursive(vector<Attachable*>& attachables, Attachable::Type type) const;

	void							FlushToPipeline(Pipeline* pPipeline);

	const AABB&						GetAABB() const	{	return m_AABB;	}
	virtual void					UpdateAABB();

	int								GetOcclusionId() const;
	void							SetOcclusionId(int id);

private:

	int									m_nOcclusionId;
	AABB								m_AABB;
	vector<shared_ptr<Attachable>>		m_AttachedObj;

};

inline SceneNode* SceneNode::GetParentNode()
{
	return m_pParent;
}

inline const std::string& SceneNode::GetName()		
{ 
	return m_strName;
}

inline void SceneNode::SetName(const string& name)
{	
	m_strName = name;
}

inline int SceneNode::GetOcclusionId() const
{
	return m_nOcclusionId;
}


//////////////////////////////////////////////////////////////////////////

class Scene
{
public:
	Scene();
	~Scene();

	void					AddSceneNode(SceneNodePtr pNode);
	void					RemoveSceneNode(SceneNodePtr pNode);

	// param : 
	// bForce : force static node to update world transform too
	void					UpdateWorld(float fElapsedTime, bool bForce = false);
	void					FlushToPipeline(Pipeline* pPipeline);
	uint					GetNodeCount() const;

	void					CleanUp();

	vector<SceneNodePtr>&	GetNodes();

private:
	vector<SceneNodePtr>	m_SceneNodes;
	
	SceneNode*				m_pCurNode;
	uint					m_nCurIndex;
};

inline vector<SceneNodePtr>& Scene::GetNodes()
{
	return m_SceneNodes;
}


}// end of namespace

#endif