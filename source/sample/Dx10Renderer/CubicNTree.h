#pragma once

#include "stdheader.h"
#include "GaVector.h"
#include "AABB.h"
#include "GaPrimitive.h"

using namespace Ga;


class MeshBuildData;
struct TriangleMesh;
struct VolumeTextureInfo;
struct VolumeTextureCell;


inline uint FormatTexelValue(uint r, uint g, uint b, uint a)
{
	return a << 24 | b << 16 | g << 8 | r;
}

inline void UINTToRGBA(uint v, ubyte& r, ubyte& g, ubyte& b, ubyte& a)
{
	r = v & 0xff;
	g = (v & 0xff00) >> 8;
	b = (v & 0xff0000) >> 16;
	a = (v & 0xff000000) >> 24;
}

struct VolumeTextureInfo
{
	uint* pData;

	uint nWidth;	// in cell, a cell is 2*2*2 in pixel
	uint nHeight;
	uint nDepth;

	uint nPitch;	// in uint
	uint nDepthPitch;

	uint* LocateVoxel(uint nCell, uint nSubVoxel);

	uint IndexToVoxelValue(uint nIndex) const;

	Vector4f Sample(Vector3f coord, uint& index);

private:
	void IndexToCoord(uint nIndex, uint& u, uint& v, uint& w) const;

	uint* LocateCell(uint u, uint v, uint w);

	uint* LocateVoxel(uint* pCell, uint nSubVoxel);
};

// a brick has number of VoxelSize * VoxelSize * VoxelSize voxels
class BrickPool
{
public:
	typedef Vector4f VoxelData;

	// width in brick
	uint GetWidth() const	{	return m_nWidth;	}

	// height in brick
	uint GetHeight() const	{	return m_nHeight;	}

	// depth in brick
	uint GetDepth() const	{	return m_nDepth;	}

	// get N, means brick is N * N * N
	uint GetBrickSize() const	{	return m_nBrickSize;	}

	void Alloc(uint nWidth, uint nHeight, uint nDepth, uint nBrickSize);

	void SetVoxel(uint nBrickIndex, uint nVoxel, const VoxelData& value);

	void IndexToBrickLocation(uint nBrickIndex, uint& u, uint& v, uint& w) const;

	void FillBrickPoolTexture(float* pData);

private:
	VoxelData* LocateBrick(uint u, uint v, uint w);

	VoxelData* LocateVoxel(VoxelData* pBrick, uint nVoxel);

	vector<VoxelData> m_Data;

	uint m_nBrickSize;

	uint m_nWidth;
	uint m_nHeight;
	uint m_nDepth;

	uint m_nPitch;
	uint m_nDepthPitch;
};

typedef BrickPool::VoxelData VoxelData;



class CubicNTree
{
public:
	enum NodeType
	{
		NODE_EMPTY = -1,
		NODE_LEAF = -2,
	};

	struct Node
	{
		int nParent;
		int nChild[8];
		Vector3f aabbMin;
		float fScale;

		Node() { }
		AABB GetSubNodeAABB(int i) const;

	};

	CubicNTree(void);
	~CubicNTree(void);

	int GetNumNode() const		{	return m_nNumNodes;	}

	const Node* GetNode(int i) const	{	return &m_Tree[i];	}

	const Vector3f& GetScale() const	{	return m_Scale;	}

	const Vector3f& GetTranslate() const	{	return m_Translate;	}



	bool ReadFromFile(const string& pathName);

	bool SaveToFile(const string& pathName);


	void Build(MeshBuildData* pMesh);

private:
	void BuildCubicNTree(TriangleMesh* pMesh);

	void BuildCubicNTreeNode(TriangleMesh* pMesh, const AABB& aabb, float scale, int nodeIndex, int& childIndex);


public:
	void BuildVolumeTexture(VolumeTextureInfo* pTexInfo);

	void BuildNodePool(VolumeTextureInfo* pNodePool, BrickPool* pBrickPool);

	void BuildBrickPool(BrickPool* pBrickPool);

private:
	uint CalculateBrickCount() const;


// test
public:
	bool TraceVoxel(const Ray& ray);

	void Test(VolumeTextureInfo* pTexInfo);

private:
	vector<Node> m_Tree;

	Vector3f m_Scale;

	Vector3f m_Translate;

	int m_nNumNodes;
};


