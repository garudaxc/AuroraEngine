#pragma once
#include "stdHeader.h"
#include "Primitive.h"


class KDTreeNode
{
public:
	enum NodeType
	{
		SPLIT_X = 0,
		SPLIT_Y,
		SPLIT_Z,
		LEAF,
	};

	NodeType		type;

	union
	{
		float	splitPos;
		int		nNumPrim;
	};
	
	union
	{
		int				nRightChildIndex;
		PrimMailbox*	pPrim;
		PrimMailbox**	ppPrim;
	};
};


class KDTree
{
public:
	struct AABBEdge
	{
		enum EdgeType
		{
			EDGE_FRONT = 0,
			EDGE_BACK,
		};

		EdgeType	type;
		int			nPrimIndex;
		float		pos;
	};

	KDTree();
	~KDTree();

	void		Init(const vector<Mesh*>& meshes);
	void		Clear();

	int			BuildTree(const Ga::AABB& aabb, int depth, const vector<int>& primIndex);
						
	void		NewInteriorNode(KDTreeNode::NodeType type, float splitPos);
	void		NewLeafNode(const vector<int>& primIndex);

	bool		TestRay(const Ray& ray, Intersection& inter);
	
	static int	AABBEdgeCompare(const void* a, const void* b);

private:
	bool		TestRay(int rayId, const Ray& ray, Intersection& inter);

	vector<const Triangle*>		m_Primitives;
	vector<PrimMailbox>			m_Mailboxes;
	vector<AABBEdge>			m_Edges;

	vector<Ga::AABB>			m_PrimAABB;

	Ga::AABB					m_SceneAABB;

	vector<KDTreeNode>			m_TreeNodes;

};