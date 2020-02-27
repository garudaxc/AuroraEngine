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
class Effect;
class IndexBuffer;
class Geometry;
class VertexDeclaration;
class VertexDescription;
class VertexBuffer;

typedef map<VertexLayout, VertexBuffer*> VertexBufferPool;


class Geometry : public Resource
{
public:

	enum VertexElemType
	{
		TYPE_FLOAT = 0,
		TYPE_FLOAT2,
		TYPE_FLOAT3,
		TYPE_FLOAT4,
		TYPE_UBYTE,
		TYPE_UBYTE4,
		TYPE_USHORT,
		TYPE_USHORT2,
		TYPE_UINT,
		NUM_OF_TYPE
	};

	enum ElemUsage
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
		NUM_OF_USAGE
	};


	static uint						GetSizeOfType(VertexElemType type);

	struct Stream_t
	{
		uint8*			data;
		uint						nSizeInByte;
		VertexElemType				type;
		uint8						usage;
		uint8						nUsageIndex;
		uint8						nStride;

		uint8*						Ptr()		{	return data;	}
		const uint8*				Ptr() const	{	return data;	}
	};

	Geometry(uint nVert, uint nTri);
	Geometry();
	~Geometry() {}
		
	uint				GetNumVertex() const { return m_nNumVert; }
	uint				GetNumTri() const { return m_nNumIndex / 3; }

	uint				GetNumStream() const { return (uint)m_Streams.size(); }

	uint				GetNumStreamOfUsage(ElemUsage usage) const;

	uint8*				AddStream(ElemUsage usage, VertexElemType type);

	uint				GetNumElement() const		{	return 1;	}

	void				ComputeTangentSpace();

	int					FindStream(ElemUsage usage, uint8 usageIndex) const;
	uint8*				GetStreamPointer(ElemUsage usage, uint8 usageIndex);
	virtual void*		GetBindID() const		{	return NULL;	}
	virtual void		OnReset();
	virtual void		OnLost();

	bool				Load(const TiXmlNode* pXmlMesh);
	void				Save(TiXmlNode* pXmlMesh) const;
	bool				CreateFromBuildData(MeshBuildData* pData);


	VertexBuffer*		GetVertexBuffer();
	IndexBuffer*		GetIndexBuffer();
	VertexLayout		GetVertexLayout();

	bool				HasSkin() const;

	void				GetMaterialInfo(set<string>& info) const;


	//void						ComputeTangent();
	//void						ComputeAABB();
	//const AABB&					GetAABB() const	{	return m_AABB;	}

	bool					AssembleVertexBuffer(void* pBuffer, const VertexDescription::Stream& stream);
private:
	bool					AssembleVertexElement(void* pBuffer, uint bufferOffset, uint bufferStride,
													VertexElemType usedType, ElemUsage usage, uint8 usageIndex);


	VertexBuffer*			CreateVertexBuffer(VertexLayout layout);
	IndexBuffer*			CreateIndexBuffer();

	uint					m_nNumVert;
	uint					m_nNumIndex;

	vector<Stream_t>		m_Streams;

	AABB					m_AABB;

	IndexBuffer*		m_pIndexBuffer;

	VertexBufferPool		m_VertexBufferPool;

	VertexBuffer*			m_pVertexBuffer;


//
//public:
//	bool						CreateRendererObject();
//	RendererObject*				GetRendererObject() { return &m_RendererObject; }
//
//	void						SetVertexDecl(const VertexDeclarationPtr& pVertDecl) { m_pVertexDecl = pVertDecl; }
//	VertexDeclaration*			GetVertexDecl() { return m_pVertexDecl.get(); }
//private:
//	RendererObject				m_RendererObject;
//	VertexDeclarationPtr		m_pVertexDecl;
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