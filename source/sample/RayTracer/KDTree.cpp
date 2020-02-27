#include "KDTree.h"

using namespace Ga;

int KDTree::AABBEdgeCompare(const void* a, const void* b)
{
	const AABBEdge* ea = (const AABBEdge*)a;
	const AABBEdge* eb = (const AABBEdge*)b;
	if (ea->pos == eb->pos)
	{
		if (ea->nPrimIndex == eb->nPrimIndex)
		{
			return (ea->type < eb->type) ? -1 : 1;
		}
		else
		{
			return (ea->nPrimIndex  < eb->nPrimIndex) ? -1 : 1;
		}
	}

	return (ea->pos < eb->pos) ? -1 : 1;
}

KDTree::KDTree()
{

}

KDTree::~KDTree()
{

}

void KDTree::NewInteriorNode(KDTreeNode::NodeType type, float splitPos)
{
	KDTreeNode node;
	node.type = type;
	node.splitPos = splitPos;

	m_TreeNodes.push_back(node);
}


void KDTree::NewLeafNode(const vector<int>& primIndex)
{
	KDTreeNode node;
	node.type = KDTreeNode::LEAF;
	node.nNumPrim = primIndex.size();

	if (node.nNumPrim == 0)
	{
		node.pPrim = NULL;
	}
	else if (node.nNumPrim == 1)
	{
		node.pPrim = &m_Mailboxes[primIndex[0]];
	}
	else
	{
		node.ppPrim = new PrimMailbox*[node.nNumPrim];
		for (int i = 0; i < node.nNumPrim; i++)
		{
			node.ppPrim[i] = &m_Mailboxes[primIndex[i]];
		}
	}

	m_TreeNodes.push_back(node);
}

void KDTree::Init(const vector<Mesh*>& meshes)
{
	m_Primitives.clear();

	for (vector<Mesh*>::const_iterator it = meshes.begin(); it != meshes.end(); ++it)
	{
		for (int i = 0; i < (*it)->GetNumTriangle(); i++)
		{
			m_Primitives.push_back((*it)->GetTriangle(i));
		}
	}

	// init aabb
	m_PrimAABB.resize(m_Primitives.size());
	m_SceneAABB.Reset();
	for (int i = 0; i < m_PrimAABB.size(); i++)
	{
		m_PrimAABB[i].Reset();
		m_PrimAABB[i].Merge(m_Primitives[i]->p0->pos);
		m_PrimAABB[i].Merge(m_Primitives[i]->p1->pos);
		m_PrimAABB[i].Merge(m_Primitives[i]->p2->pos);
		m_SceneAABB.Merge(m_PrimAABB[i]);
	}

	// init edges
	m_Edges.resize(m_Primitives.size() * 2);

	// init mailbox
	m_Mailboxes.resize(m_Primitives.size());
	for (int i = 0; i < m_Primitives.size(); i++)
	{
		m_Mailboxes[i].rayId = 0;
		m_Mailboxes[i].pTri = m_Primitives[i];
	}

	// build tree
	vector<int> prims;
	prims.resize(m_Primitives.size());
	for (int i = 0; i < m_Primitives.size(); i++)
	{
		prims[i] = i;
	}
	BuildTree(m_SceneAABB, 0, prims);
}

void KDTree::Clear()
{

}


float CalcCost(const AABB& nodeAABB, int numPrim0, int numPrim1,
			   int splitAxis, float splitPos, float invNodeArea)
{
	float traversalCost = 1.f;
	float intersectCost = 20.f;

	int otherAxis[3][2] = {{1, 2}, {0, 2}, {0, 1}};
	int axis0 = otherAxis[splitAxis][0], axis1 = otherAxis[splitAxis][1];

	Vector3f d = nodeAABB.vMax - nodeAABB.vMin;
	float d0 = splitPos - nodeAABB.vMin[splitAxis];
	float d1 = nodeAABB.vMax[splitAxis] - splitPos;

	float area0 = 2.f * (d[axis0] * d[axis1]  + d0 * (d[axis0] + d[axis1]));
	float area1 = 2.f * (d[axis0] * d[axis1]  + d1 * (d[axis0] + d[axis1]));

	float p0 = area0 * invNodeArea;
	float p1 = area1 * invNodeArea;

	float cost = traversalCost + intersectCost * (p0 * numPrim0 + p1 * numPrim1);

	return cost;
}



int KDTree::BuildTree(const AABB& aabb, int depth,
					   const vector<int>& primIndex)
{
	if (primIndex.size() == 0)
	{
		int a = 0;
		a++;
	}


	Vector3f nodeSize = aabb.vMax - aabb.vMin;

	// choose split axis
	int splitAxis = 0;
	if (nodeSize.x > nodeSize.y && nodeSize.x > nodeSize.z)
	{
		splitAxis = 0;
	}
	else
	{
		splitAxis  = (nodeSize.y > nodeSize.z) ? 1 : 2;
	}

	// init edges
	int nNumPrim = primIndex.size();
	for (int i = 0; i < nNumPrim; i++)
	{
		AABBEdge edge;
		edge.nPrimIndex = primIndex[i];

		edge.type = AABBEdge::EDGE_FRONT;
		edge.pos = m_PrimAABB[primIndex[i]].vMin[splitAxis];
		m_Edges[i * 2] = edge;

		edge.type = AABBEdge::EDGE_BACK;
		edge.pos = m_PrimAABB[primIndex[i]].vMax[splitAxis];
		m_Edges[i * 2 + 1] = edge;
	}
	qsort(&m_Edges[0], nNumPrim * 2, sizeof(AABBEdge), AABBEdgeCompare);

	// find split pos
	Vector3f d = aabb.vMax - aabb.vMin;
	float invNodeArea = 1.f / (2.f * (d.x * d.y + d.y * d.z + d.x * d.z));
	float bestCost = Mathf::MAX_VALUE;
	int bestSplitEdge = -1;

	int nNumPrim0 = 0, nNumPrim1 = nNumPrim;
	for (int i = 0; i < 2 * nNumPrim; i++)
	{
		if (m_Edges[i].type == AABBEdge::EDGE_BACK)
		{
			nNumPrim1--;
		}

		if (m_Edges[i].pos > aabb.vMin[splitAxis] && m_Edges[i].pos < aabb.vMax[splitAxis])
		{
			float cost = CalcCost(aabb, nNumPrim0, nNumPrim1, splitAxis, m_Edges[i].pos, invNodeArea);
			if (cost < bestCost)
			{
				bestCost = cost;
				bestSplitEdge = i;
			}
		}

		if (m_Edges[i].type == AABBEdge::EDGE_FRONT)
		{
			nNumPrim0++;
		}
	}

	// if not enough primitives or can not find a split,
	// buid a leaf node
	if (nNumPrim <= 2 || bestSplitEdge == -1)
	{
		NewLeafNode(primIndex);
		return 1;
	}

	// build sub-tree
	// *** great algorithm piece
	vector<int> primIndex0, primIndex1;
	for (int i = 0; i < bestSplitEdge; i++)
	{
		if (m_Edges[i].type == AABBEdge::EDGE_FRONT)
		{
			primIndex0.push_back(m_Edges[i].nPrimIndex);
		}
	}

	for (int i = bestSplitEdge + 1; i < nNumPrim * 2; i++)
	{
		if (m_Edges[i].type == AABBEdge::EDGE_BACK)
		{
			primIndex1.push_back(m_Edges[i].nPrimIndex);
		}
	}

	Ga::AABB aabb0 = aabb, aabb1 = aabb;
	aabb0.vMax[splitAxis] = m_Edges[bestSplitEdge].pos;
	aabb1.vMin[splitAxis] = m_Edges[bestSplitEdge].pos;

	// construct this interior node
	int newNodeIndex = m_TreeNodes.size();
	NewInteriorNode((KDTreeNode::NodeType)splitAxis, m_Edges[bestSplitEdge].pos);

	int  numSubTree0 = BuildTree(aabb0, depth + 1, primIndex0);
	int  numSubTree1 = BuildTree(aabb1, depth + 1, primIndex1);

	m_TreeNodes[newNodeIndex].nRightChildIndex = newNodeIndex + numSubTree0 + 1;

	return numSubTree0 + numSubTree1 + 1;
}



bool KDTree::TestRay(const Ray& ray, Intersection& inter)
{
	return TestRay(0, ray, inter);
}


bool KDTree::TestRay(int rayId, const Ray& ray, Intersection& inter)
{
	float tmin, tmax;
	if (!TestAABB(m_SceneAABB, ray, tmin, tmax))
	{
		return false;
	}


	struct TodoNode
	{
		const KDTreeNode*	pNode;
		float				tmax;
		float				tmin;
	};


	TodoNode todoNodes[256];
	int todoPos = 0;

	vector<const KDTreeNode*> nodeStack;


	Vector3f invDir(1.f / ray.dir.x, 1.f / ray.dir.y, 1.f / ray.dir.z);
	bool bIntersect = false;

	const KDTreeNode* pNode = &m_TreeNodes[0];
	while (pNode)
	{
		if (ray.tmax < tmin)
		{
			break;
		}

		nodeStack.push_back(pNode);
		if (pNode->type == KDTreeNode::LEAF)
		{
			// leaf node, test primitive
			if (pNode->nNumPrim == 1)
			{
				if (TestTriangle(inter, pNode->pPrim->pTri, ray))
				{
					bIntersect = true;
				}
			}
			else
			{
				for (int i = 0; i < pNode->nNumPrim; i++)
				{
					if (TestTriangle(inter, pNode->ppPrim[i]->pTri, ray))
					{
						bIntersect = true;
					}
				}
			}

			if (todoPos > 0)
			{
				todoPos--;
				pNode = todoNodes[todoPos].pNode;
				tmin = todoNodes[todoPos].tmin;
				tmax = todoNodes[todoPos].tmax;
			}
			else
			{
				break;
			}
		}
		else
		{
			int splitAxis = (int)pNode->type;
			float tPlane = (pNode->splitPos - ray.origin[splitAxis]) * invDir[splitAxis];
			const KDTreeNode *pChild0, *pChild1;
			if (ray.origin[splitAxis] <= pNode->splitPos)
			{
				pChild0 = pNode + 1;
				pChild1 = &m_TreeNodes[pNode->nRightChildIndex];
			}
			else
			{
				pChild0 = &m_TreeNodes[pNode->nRightChildIndex];
				pChild1 = pNode + 1;
			}

			if (tPlane > tmax || tPlane < 0.f)
			{
				pNode = pChild0;
			}
			else if (tPlane < tmin)
			{
				pNode = pChild1;
			}
			else
			{
				todoNodes[todoPos].pNode = pChild1;
				todoNodes[todoPos].tmin = tPlane;
				todoNodes[todoPos].tmax = tmax;
				todoPos++;

				pNode = pChild0;
				tmax = tPlane;
			}
		}

	}

	return bIntersect;
}