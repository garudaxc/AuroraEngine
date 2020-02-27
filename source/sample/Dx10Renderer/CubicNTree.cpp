#include "CubicNTree.h"
#include "GaMesh.h"
#include "GaMath.h"
#include "GaByteStream.h"
#include <fstream>

struct TriangleMesh
{
	int nTri;
	int nVert;
	const Vector3f* Vertex;
	const int* Index;
};


CubicNTree::CubicNTree(void):m_nNumNodes(0)
{
}

CubicNTree::~CubicNTree(void)
{
}

static AABB CalcAABB(TriangleMesh* pMesh)
{
	AABB aabb;
	for (int i = 0; i < pMesh->nVert; i++)
	{
		aabb.Merge(pMesh->Vertex[i]);
	}

	return aabb;
}


static Vector3f NormalizeMesh(Vector3f* pPos, int nVert, const AABB& aabb)
{
	Vector3f scale = aabb.vMax - aabb.vMin;
	Vector3f invScale = 1.f / scale;
	for (int i = 0; i < nVert; i++)
	{
		pPos[i] -= aabb.vMin;

		pPos[i].x *= invScale.x;
		pPos[i].y *= invScale.y;
		pPos[i].z *= invScale.z;
	}

	return scale;
}


AABB GetSubNodeAABB(const Vector3f& parentNodeMinPos, float scale, int index)
{
	Vector3f minPos = parentNodeMinPos;

	float offset[][3] = {
		{0.f,	0.f,	0.f},
		{scale, 0.f,	0.f},
		{0.f,	scale,	0.f},
		{scale, scale,	0.f},
		{0.f,	0.f,	scale},
		{scale, 0.f,	scale},
		{0.f,	scale,	scale},
		{scale, scale,	scale},
	};

	for (int i = 0; i < 3; i++)
	{
		minPos[i] += offset[index][i];
	}

	Vector3f maxPos(minPos.x + scale,
		minPos.y + scale,
		minPos.z + scale);

	return AABB(minPos, maxPos);
}


bool TestAABBTriangle(TriangleMesh* pMesh, const AABB& aabb)
{
	for (int i = 0; i < pMesh->nTri; i++)
	{
		const int* pIndex = pMesh->Index + i * 3;
		const Vector3f* pPos = pMesh->Vertex;
		if (aabb.TestTriangle(
			pPos[pIndex[0]],
			pPos[pIndex[1]],
			pPos[pIndex[2]]))
		{
			return true;
		}
	}

	return false;
}

bool IsSmallEnough(float scale)
{
	return scale < 0.1f;
}

void EnsureTreeCapcaity(vector<CubicNTree::Node>& tree, int nodeIndex)
{
	if (tree.size() <= (size_t)nodeIndex)
	{
		tree.resize(tree.size() * 2);
	}
}


AABB CubicNTree::Node::GetSubNodeAABB(int i) const
{
	return ::GetSubNodeAABB(aabbMin, fScale / 2.f, i);
}

void CubicNTree::BuildCubicNTreeNode(TriangleMesh* pMesh, const AABB& aabb, float scale, int nodeIndex, int& childIndex)
{
	float childScale = scale / 2.f;

	Node& node = m_Tree[nodeIndex];
	for (int i = 0; i < 8; i++)
	{
		AABB childAABB = GetSubNodeAABB(aabb.vMin, childScale, i);
		if (TestAABBTriangle(pMesh, childAABB))
		{
			if (IsSmallEnough(childScale))
			{
				// leaf node
				node.nChild[i] = NODE_LEAF;
			}
			else
			{
				int newNodeIndex = childIndex++;
				EnsureTreeCapcaity(m_Tree, newNodeIndex);

				node.nChild[i] = newNodeIndex;
				m_Tree[newNodeIndex].nParent = nodeIndex;
				m_Tree[newNodeIndex].aabbMin = childAABB.vMin;
				m_Tree[newNodeIndex].fScale = childScale;

				BuildCubicNTreeNode(pMesh, childAABB, childScale, newNodeIndex, childIndex);
			}
		}
		else
		{
			node.nChild[i] = NODE_EMPTY;
		}
	}
}

void CubicNTree::BuildCubicNTree(TriangleMesh* pMesh)
{
	m_Tree.resize(8192);
	AABB aabb(Vector3f::ZERO, Vector3f(1.f, 1.f, 1.f));
	m_Tree[0].nParent = -1;

	int childIndex = 1;
	BuildCubicNTreeNode(pMesh, aabb, 1.f, 0, childIndex);
	m_nNumNodes = childIndex;
}


void CubicNTree::Build(MeshBuildData* pMesh)
{
	TriangleMesh triMesh;
	triMesh.nTri = pMesh->GetNumFace();
	triMesh.nVert = pMesh->GetNumVertex();
	triMesh.Index = pMesh->GetIndexData();
	triMesh.Vertex = (const Vector3f*)pMesh->GetPositionData();

	AABB aabb = CalcAABB(&triMesh);
	m_Translate = aabb.vMin;

	vector<Vector3f> verties(triMesh.nVert);
	memcpy(&verties[0], pMesh->GetPositionData(), sizeof(Vector3f) * triMesh.nVert);
	triMesh.Vertex = &verties[0];

	m_Scale = NormalizeMesh(&verties[0], triMesh.nVert, aabb);
	aabb = CalcAABB(&triMesh);

	BuildCubicNTree(&triMesh);
}

bool CubicNTree::TraceVoxel(const Ray& ray)
{
	for (int i = 0; i < m_nNumNodes; i++)
	{
		const Node& node = m_Tree[i];

		for (int j = 0; j < 8; j++)
		{
			if (node.nChild[j] == NODE_LEAF)
			{
				AABB aabb = node.GetSubNodeAABB(j);

				if (TestRayAABB(aabb, ray))
				{
					return true;
				}
			}
		}
	}

	return false;
}


bool CubicNTree::ReadFromFile(const string& pathName)
{
	std::fstream fs;
	fs.open(pathName.c_str(), std::ios::in | std::ios::binary);

	if (!fs)
	{
		return false;
	}

	fs.seekg(0, ios_base::end);
	size_t size = fs.tellg();
	fs.seekg(0, ios_base::beg);

	CDynamicStreamBuf buff(size);
	fs.read(buff.GetBytes(), size);
	buff.ExpandTo(size);

	CIStream stream(&buff);
	stream >> m_Scale >> m_Translate;
	stream >> m_nNumNodes;
	m_Tree.resize(m_nNumNodes);
	for (int i = 0; i < m_nNumNodes; i++)
	{
		stream.Read(&m_Tree[i], sizeof(Node));
	}

	return true;
}

bool CubicNTree::SaveToFile(const string& pathName)
{
	CDynamicStreamBuf buff;
	COStream stream(&buff);

	stream << m_Scale << m_Translate;
	stream << m_nNumNodes;
	for (int i = 0; i < m_nNumNodes; i++)
	{
		stream.Write(&m_Tree[i], sizeof(Node));
	}

	std::fstream fs;
	fs.open(pathName.c_str(), std::ios::out | std::ios::binary);
	if (!fs)
	{
		return false;
	}

	fs.write(buff.GetBytes(), buff.Size());
	fs.close();

	return true;
}


//////////////////////////////////////////////////////////////////////////


Vector4f VolumeTextureInfo::Sample(Vector3f coord, uint& index)
{
	uint u = (uint)(coord.x * nWidth);
	uint v = (uint)(coord.y * nHeight);
	uint w = (uint)(coord.z * nDepth);

	//index = w * nWidth * nHeight + v * nWidth + w;
	
	uint x = (uint)(coord.x * nWidth * 2.f);
	uint y = (uint)(coord.y * nHeight * 2.f);
	uint z = (uint)(coord.z * nDepth * 2.f);

	uint* p = pData + z * nDepthPitch + y * nPitch + x;

	ubyte r, g, b, a;
	UINTToRGBA(*p, r, g, b, a);

	index = b * nWidth * nHeight + g * nWidth + r;

	float s = 1.f / 255.f;
	return Vector4f(r * s, g * s, b * s, a * s);
}

uint* VolumeTextureInfo::LocateVoxel(uint nCell, uint nSubVoxel)
{
	uint u, v, w;
	IndexToCoord(nCell, u, v, w);

	uint* pCell = LocateCell(u, v, w);

	return LocateVoxel(pCell, nSubVoxel);
}

void VolumeTextureInfo::IndexToCoord(uint nIndex, uint& u, uint& v, uint& w) const
{
	uint nSliceSize = nWidth * nHeight;
	w = nIndex / nSliceSize;

	uint nSlice = nIndex % nSliceSize;

	v = nSlice / nWidth;
	u = nSlice % nWidth;
}

uint* VolumeTextureInfo::LocateCell(uint u, uint v, uint w)
{
	uint x = 2 * u;
	uint y = 2 * v;
	uint z = 2 * w;

	uint* p = pData + z * nDepthPitch + y * nPitch + x;

	return p;
}

uint* VolumeTextureInfo::LocateVoxel(uint* pCell, uint nSubVoxel)
{
	uint offset[] = {
		0, 1, nPitch, nPitch + 1, 
		nDepthPitch, nDepthPitch + 1, 
		nDepthPitch + nPitch, nDepthPitch + nPitch + 1
	};

	return pCell + offset[nSubVoxel];
}

Ga::uint VolumeTextureInfo::IndexToVoxelValue(uint nIndex) const
{
	uint u, v, w;
	IndexToCoord(nIndex, u, v, w);

	return FormatTexelValue(u, v, w, 0x7f);
}

void CubicNTree::BuildVolumeTexture(VolumeTextureInfo* pTexInfo)
{
	for (int i = 0; i < m_nNumNodes; i++)
	{
		const Node& node = m_Tree[i];
		
		for (int j = 0; j < 8; j++)
		{
			uint* pVoxel = pTexInfo->LocateVoxel(i, j);

			// empty node
			if (node.nChild[j] == NODE_EMPTY)
			{
				*pVoxel = FormatTexelValue(0, 0, 0, 0x0);
			}
			// leaf node
			else if (node.nChild[j] == NODE_LEAF)
			{
				AABB aabb = node.GetSubNodeAABB(j);
				Vector3f pos = aabb.vMin;
				pos *= 255.f;
				*pVoxel = FormatTexelValue((uint)pos.x, (uint)pos.y, (uint)pos.z, 0xff);
			}
			else
			{
				*pVoxel = pTexInfo->IndexToVoxelValue(node.nChild[j]);
			}	
		}
	}

	//Test(pTexInfo);
}

void CubicNTree::BuildNodePool(VolumeTextureInfo* pNodePool, BrickPool* pBrickPool)
{
	uint nBrickIndex = m_nNumNodes;
	for (int i = 0; i < m_nNumNodes; i++)
	{
		const Node& node = m_Tree[i];
		
		for (int j = 0; j < 8; j++)
		{
			uint* pVoxel = pNodePool->LocateVoxel(i, j);

			// empty node
			if (node.nChild[j] == NODE_EMPTY)
			{
				*pVoxel = FormatTexelValue(0, 0, 0, 0x0);
			}
			// leaf node
			else if (node.nChild[j] == NODE_LEAF)
			{
				uint u, v, w;
				pBrickPool->IndexToBrickLocation(nBrickIndex, u, v, w);
				*pVoxel = FormatTexelValue(u, v, w, 0xff);
			}
			else
			{
				*pVoxel = pNodePool->IndexToVoxelValue(node.nChild[j]);
			}	
		}
	}
}

uint CubicNTree::CalculateBrickCount() const
{
	uint count = 0;
	for (int i = 0; i < m_nNumNodes; i++)
	{
		const Node& node = m_Tree[i];

		for (int j = 0; j < 8; j++)
		{
			if (node.nChild[j] == NODE_LEAF)
			{
				count++;
			}
		}
	}
	
	count += m_nNumNodes;

	return count;
}

void CubicNTree::BuildBrickPool(BrickPool* pBrickPool)
{
	uint nNumBricks = CalculateBrickCount();
	float s = Mathf::Pow((float)nNumBricks, 0.3333f);
	int size = (int)(s + 1.0f);

	pBrickPool->Alloc(size, size, size, 3);

	int index = m_nNumNodes;
	for (int i = 0; i < m_nNumNodes; i++)
	{
		const Node& node = m_Tree[i];

		for (int j = 0; j < 8; j++)
		{
			if (node.nChild[j] == NODE_LEAF)
			{
				AABB aabb = node.GetSubNodeAABB(j);
				Vector4f voxelData(aabb.vMin.x, aabb.vMin.y, aabb.vMin.z, 1.f);

				for (int k = 0; k < 27; k++)
				{
					pBrickPool->SetVoxel(index, k, voxelData);
				}

				index++;
			}
		}
	}
}


Vector4f Lookup(VolumeTextureInfo* pTexInfo, const Vector3f& invS, float N, const Vector3f& M);

void CubicNTree::Test(VolumeTextureInfo* pTexInfo)
{
	Vector3f invS(1.f / (float)pTexInfo->nWidth,
		1.f / (float)pTexInfo->nHeight,
		1.f / (float)pTexInfo->nDepth);

	Vector3f M(0.f, 0.99f, 0.f);


	int index = 0;
	int sub = 0;
	vector<uint> stack;

	for (int i = 0; i < m_nNumNodes; i++)
	{
		const CubicNTree::Node* pNode = this->GetNode(i);

		for (int j = 0; j < 8; j++)
		{
			if (pNode->nChild[j] == CubicNTree::NODE_LEAF)
			{
				AABB aabb = pNode->GetSubNodeAABB(j);

				if (aabb.IsInside(M))
				{
					index = i;
					sub = j;
					stack.push_back(i);
					while (pNode->nParent != -1)
					{
						stack.push_back(pNode->nParent);
						pNode = this->GetNode(pNode->nParent);
					}

					break;
				}
			}
		}
	}

	Vector4f v = Lookup(pTexInfo, invS, 2.f, M);

}

Vector4f Lookup(VolumeTextureInfo* pTexInfo, const Vector3f& invS, float N, const Vector3f& M)
{
	Vector4f I = Vector4f::ZERO;
	Vector3f MND = M;

	//stack = [5](502,499,486,429,0)

	vector<uint> stack;

	for (float i = 0; i < 30; i++)
	{
		Vector3f P(
			(Mathf::Floor(I.x * 255.f) + (MND.x - Mathf::Floor(MND.x))) * invS.x,
			(Mathf::Floor(I.y * 255.f) + (MND.y - Mathf::Floor(MND.y))) * invS.y,
			(Mathf::Floor(I.z * 255.f) + (MND.z - Mathf::Floor(MND.z))) * invS.z);

		if (I.w < 0.9)
		{
			uint index;
			I = pTexInfo->Sample(P, index);
			stack.push_back(index);
		}

		if (I.w > 0.9)
			break;

		if (I.w < 0.1)
			return Vector4f(-1.f, -1.f, -1.f, -1.f);

		MND = MND * N;		
	}

	return I;
}

//////////////////////////////////////////////////////////////////////////

void BrickPool::Alloc(uint nWidth, uint nHeight, uint nDepth, uint nBrickSize)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_nDepth = nDepth;

	m_nBrickSize = nBrickSize;

	m_nPitch = m_nWidth * m_nBrickSize;
	m_nDepthPitch = m_nPitch * m_nHeight * m_nBrickSize;

	uint voxelInBrick = m_nBrickSize * m_nBrickSize * m_nBrickSize;
	uint nNumVoxel = m_nWidth * m_nHeight * m_nDepth * voxelInBrick;
	m_Data.resize(nNumVoxel);

	memset(&m_Data[0], 0, sizeof(VoxelData) * nNumVoxel);
}

void BrickPool::IndexToBrickLocation(uint nBrickIndex, uint& u, uint& v, uint& w) const
{
	uint nSliceSize = m_nWidth * m_nHeight;
	w = nBrickIndex / nSliceSize;

	uint nSlice = nBrickIndex % nSliceSize;

	v = nSlice / m_nWidth;
	u = nSlice % m_nWidth;
}


void BrickPool::SetVoxel(uint nBrickIndex, uint nVoxel, const VoxelData& value)
{
	uint u, v, w;
	IndexToBrickLocation(nBrickIndex, u, v, w);

	VoxelData* pBrick = LocateBrick(u, v, w);

	VoxelData* pVoxelData = LocateVoxel(pBrick, nVoxel);

	*pVoxelData = value;
}


VoxelData* BrickPool::LocateBrick(uint u, uint v, uint w)
{
	uint x = m_nBrickSize * u;
	uint y = m_nBrickSize * v;
	uint z = m_nBrickSize * w;

	VoxelData* p = &m_Data[0] + z * m_nDepthPitch + y * m_nPitch + x;

	return p;
}

VoxelData* BrickPool::LocateVoxel(VoxelData* pBrick, uint nVoxel)
{
	uint nSliceSize = m_nBrickSize * m_nBrickSize;
	uint w = nVoxel / nSliceSize;

	uint nSlice = nVoxel % nSliceSize;

	uint v = nSlice / m_nBrickSize;
	uint u = nSlice % m_nBrickSize;

	return pBrick +
		w * m_nDepthPitch + 
		v * m_nPitch +
		u;
}

void BrickPool::FillBrickPoolTexture(float* pData)
{
	uint voxelInBrick = m_nBrickSize * m_nBrickSize * m_nBrickSize;
	uint nNumVoxel = m_nWidth * m_nHeight * m_nDepth * voxelInBrick;

	memcpy(pData, &m_Data[0], sizeof(VoxelData) * nNumVoxel);
}


