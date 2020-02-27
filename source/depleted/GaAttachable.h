#pragma once

namespace Ga
{

class SceneNode;
class Attachable;

typedef shared_ptr<Attachable>		AttachablePtr;

class Attachable
{
public:
	friend class SceneNode;
	enum Type
	{
		TYPE_MESH = 0,
		TYPE_LIGHT,
		TYPE_CAMERA,
	};

	Attachable(Type type);
	virtual ~Attachable(void);

	SceneNode*				GetParentNode() const					{	return m_pParentNode;	}
	Type					GetType() const							{	return m_Type;			}

	virtual void			UpdateController(float dt)				{};
	// retrieve infomation from scene node,
	// call by attached scene node before flushed to rendering pipeline
	virtual void			UpdateFromParentNode() = 0;


	void					SetOcclusionId(int id);
	int						GetOcclusionId() const;
private:
	void					SetParentNode(SceneNode* pNode)			{	m_pParentNode = pNode;	}
	SceneNode*				m_pParentNode;

	Type					m_Type;
	int						m_nOcclusionId;

};


inline void Attachable::SetOcclusionId(int id)
{
	m_nOcclusionId = id;
}

inline int Attachable::GetOcclusionId() const
{
	return m_nOcclusionId;
}



}