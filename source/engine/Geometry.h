#ifndef GAGEOMETRYDATA_H
#define GAGEOMETRYDATA_H
#include "Types.h"
#include "AABB.h"
#include "RendererObject.h"
#include "ResourceManager.h"
#include "Mesh.h"
#include "tinyxml.h"


namespace Aurora
{

struct VertexPNTT
{
	Vector3f		pos;
	Vector3f		normal;
	Vector2f		uv;
	Vector4f		tangent;
};


struct VertexP
{
	Vector3f		pos;
};


struct VertexPT
{
	Vector3f		pos;
	Vector2f		uv;
};


//--------------------------------------------
// free function
//-------------------------------------

// compute normal of triangle
Vector3f ComputeNormal(const Vector3f& v1, const Vector3f& v2, const Vector3f& v3);
// compute weighted normal of triangle
Vector3f ComputeWeightedNormal(const Vector3f& v1, const Vector3f& v2, const Vector3f& v3);

float TriangleArea(const Vector3f& a, const Vector3f& b, const Vector3f& c);
//-------------------------------------------------------------------------------------------------
class RendererObject;
class IRenderDevice;
class Geometry;
class VertexLayout;
class File;


struct MeshElement
{
	int32	IndexStart = 0;
	int32	IndexCount = 0;
	int32	MaterialSlot = 0;
};


struct Vertex
{
	enum ElemType : int8
	{
		TYPE_FLOAT = 0,
		TYPE_FLOAT2,
		TYPE_FLOAT3,
		TYPE_FLOAT4,
		TYPE_UBYTE4_UINT,
		TYPE_UBYTE4_UNORM,
		TYPE_USHORT_UINT,
		TYPE_UINT,
	};

	enum ElemUsage : int8
	{
		USAGE_POSITION = 0,
		USAGE_NORMAL,
		USAGE_COLOR,
		USAGE_TEXCOORD,
		USAGE_TANGENT,
		USAGE_BINORMAL,
		USAGE_BLENDINDEX,
		USAGE_BLENDWEIGHT,
		USAGE_INDEX,
	};
};


class Geometry : public Resource
{
public:

	static uint			GetSizeOfType(Vertex::ElemType type);

	struct Stream_t
	{
		uint8*		data;
		uint		nSizeInByte;
		Vertex::ElemType	type;
		uint8		usage;
		uint8		nUsageIndex;
		uint8		nStride;

		uint8*			Ptr()		{	return data;	}
		const uint8*	Ptr() const	{	return data;	}
	};

	Geometry(uint nVert, uint nTri);
	Geometry();
	~Geometry() {}


	static int32	CalcVertexStride(const vector<VertexLayoutItem>& layout);
		
	uint		GetNumVertex() const { return m_nNumVert; }
	uint		GetNumTri() const { return m_nNumIndex / 3; }

	uint		GetNumStream() const { return (uint)m_Streams.size(); }

	uint		GetNumStreamOfUsage(Vertex::ElemUsage usage) const;

	uint8*		AddStream(Vertex::ElemUsage usage, Vertex::ElemType type);

	uint		GetNumElement() const { return Elements_.size(); }

	void		ComputeTangentSpace();

	int			FindStream(Vertex::ElemUsage usage, uint8 usageIndex) const;
	uint8*				GetStreamPointer(Vertex::ElemUsage usage, uint8 usageIndex);

	virtual void		OnReset();
	virtual void		OnLost();

	bool		Load(const TiXmlNode* pXmlMesh);
	void		Save(TiXmlNode* pXmlMesh) const;
	void		Load(File* file);

	
	bool		HasSkin() const;
	void		GetMaterialInfo(set<string>& info) const;
	void		PrepareVertexData(vector<int8>& data, const vector<VertexLayoutItem>& layout);
	

	//void						ComputeTangent();
	//void						ComputeAABB();
	//const AABB&					GetAABB() const	{	return m_AABB;	}

	vector<MeshElement>		Elements_;

	Handle		VertexBufferHandle_ = -1;
	Handle		IndexBufferHandle_ = -1;

private:
	bool		AssembleVertexElement(void* pBuffer, uint bufferOffset, uint bufferStride,
									Vertex::ElemType usedType, Vertex::ElemUsage usage, uint8 usageIndex);

	void	CreateIndexBuffer();

	uint		m_nNumVert;
	uint		m_nNumIndex;

	vector<Stream_t>		m_Streams;

	AABB		m_AABB;
	

//
//
//public:
//	float						GetSurfaceArea();
//	uint							GetLightmapSize() const	{	return m_nLightmapSize;	}
//	uint							nLightmapID;
//	void							SetLightmapSize(uint lightmapSize)	{	m_nLightmapSize = lightmapSize;	}
//	GeometryDataPtr					AssembleSubmesh(const std::set<ushort>& vertSet, uint triOffset, uint nNumTri);
//	std::vector<GeometryDataPtr>	SplitMesh(float areaThreshold);
//
//private:
//	uint						m_nLightmapSize;

};

//----------------------------------------------------------------------------


class StandardMesh
{
public:
	static Geometry*			CreatePlane(int nRow, int nCol, const Matrix4f& matTrans);
	static Geometry*			CreateBox(bool bInside, const Matrix4f& matTrans);
	static Geometry*			CreateSphere(uint slice, uint stack, bool bInside, const Matrix4f& matTrans);
};



class MeshManager
{
public:
	MeshManager();
	~MeshManager();

	void Initialize();
	void Finalize();

	Geometry*		GetMesh(const ResourceID& id);

private:
	Geometry*		Find(const ResourceID& id);
	Geometry*		Load(const ResourceID& id);

	map<ResourceID, Geometry*>		m_ResourcePool;
};

extern MeshManager GMeshManager;


}


#endif