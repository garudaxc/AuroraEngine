#ifndef GARENDEREROBJECT_H
#define GARENDEREROBJECT_H
#include <d3d9.h>
#include "Types.h"
#include "ResourceManager.h"


namespace Aurora
{

class Geometry;
class IRenderDevice;
class VertexBuffer;
class IndexBuffer;
class RenderOperator;
class VertexDescription;
class VertexBuffer;
class MaterialInstance;
class Renderable;
class LightObject;


class Renderable
{
public:
	Renderable(Geometry* pGeometry, uint nElemIndex);
	Renderable();

	virtual ~Renderable();

	/*virtual void						OnPreRender()	{}
	virtual void						OnPostRender()	{}*/

	void							SetMaterialInst(MaterialInstance* pMatInst);
	MaterialInstance*				GetMaterialInst();
	const MaterialInstance*			GetMaterialInst() const;

	void								BuildRenderOperator(Geometry* pGeom);
	virtual	const RenderOperator&		GetRenderOperator() const;
	virtual void						UpdateFromParentNode();
	float								GetCullDist() const;

private:
	MaterialInstance*	m_pMatInst;
	VertexBuffer*		m_pVertexBuffer;
	IndexBuffer*		m_pIndexBuffer;
	Geometry*		m_pGeometry;
	uint				m_nNumVert;
	uint				m_nNumTri;

	uint				m_nElemIndex;

	RenderOperator*		m_pOP;

	float							m_fCullDist;
};



inline void Renderable::SetMaterialInst(MaterialInstance* pMatInst)
{	
	m_pMatInst = pMatInst;
}

inline MaterialInstance* Renderable::GetMaterialInst()	
{	
	return m_pMatInst;	
}

inline const MaterialInstance* Renderable::GetMaterialInst() const
{	
	return m_pMatInst;	
}




//////////////////////////////////////////////////////////////////////////

class VertexBuffer : public Resource
{
public:

	class Locker
	{
	public:
		friend class VertexBuffer;

	~Locker();
	Locker(Locker& ob);		// can only create from copy constructor

	void* Ptr() const	{	return m_Pointer;	}

	private:
		Locker(VertexBuffer* pVertexBuffer);
		const Locker& operator = (const Locker& ob);

		VertexBuffer* m_pVertexBuffer;

		void* m_Pointer;
	};

	virtual ~VertexBuffer();


	uint GetStride() const		{	return m_nStride;	}

	uint GetNumVertex() const	{	return m_nNumVertex;	}

	uint GetSizeInByte() const;

	Locker GetLocker();


	void* HALHandle;

protected:
	VertexBuffer(uint nStride, uint nNumVertex);

private:
	virtual void* Lock()		{	return NULL;	}

	virtual void Unlock()	{}

	uint				m_nStride;
	uint				m_nNumVertex;
};

//////////////////////////////////////////////////////////////////////////


class IndexBuffer : public Resource
{
public:
	enum Format
	{
		FMT_INDEX16,
		FMT_INDEX32,
	};

	class Locker
	{
	public:
		friend class IndexBuffer;

		~Locker();
		Locker(Locker& ob);		// can only create from copy constructor

		void* Ptr() const	{	return m_Pointer;	}

	private:
		Locker(IndexBuffer* pIndexBuffer);
		const Locker& operator = (const Locker& ob);

		IndexBuffer* m_pIndexBuffer;

		void* m_Pointer;
	};

	virtual ~IndexBuffer();

	Locker GetLocker();

	IndexBuffer::Format GetFormat() const		{	return m_nFormat;	}

	uint GetNumIndex() const	{	return m_nNumIndex;	}


protected:
	IndexBuffer(Format format, uint numIndex);

private:

	virtual void* Lock() = 0;

	virtual void Unlock() = 0;


	Format m_nFormat;

	uint m_nNumIndex;
};

//////////////////////////////////////////////////////////////////////////

class VertexDescription : public Resource
{
public:

	struct Item
	{
		uint8	type;
		uint8	usage;
		uint8	usageIndex;
	};

	struct Stream
	{
		vector<Item>	items;
		uint			nVertexStride;
	};

	Stream			stream;

	virtual bool	BuildDeviceObject()		{	return false;	}

	void			OnCreate();
	void			OnRelease();

};


enum VertexLayout
{
	VertexLayout_PNTT = 0,
	VertexLayout_PT,
	VertexLayout_P,
	VertexLayout_PNTTBB,
	VertexLayout_NUM,
};




}

#endif