#pragma once

namespace Aurora
{

class Resource
{
public:
	virtual void*			GetBindID()		{	return NULL;	}
	virtual ~Resource()	{}
	virtual void			OnReset()		{}
	virtual void			OnLost()		{}

	void*			HALHandle;

private:

};


class ResPtrHolder
{
public:
	ResPtrHolder(Resource* pResource);
	~ResPtrHolder();

	void				AddRef()			{	m_nRefCount++;	}
	void				Release()			{	m_nRefCount--;	}
	uint				GetRefCount()		{	return m_nRefCount;	}


	void				ResetResource(Resource* pResource);			// replace the resource to a new one
	Resource*			GetResPtr() const	{	return m_pResource;	}

private:
	// disable copy operator
	ResPtrHolder(const ResPtrHolder& ob);
	const ResPtrHolder& operator = (const ResPtrHolder& ob);


	uint					m_nRefCount;
	Resource*	m_pResource;		// resource's life time managed by holder
};

//////////////////////////////////////////////////////////////////////////


template <class ResType>
class ResourcePtr
{
public:
	ResourcePtr(const ResourcePtr<ResType>& ob);
	explicit ResourcePtr(ResPtrHolder* pResHolder = NULL);
	~ResourcePtr();

	ResourcePtr<ResType>& operator = (const ResourcePtr<ResType>& ob);

	ResType*				operator -> () const;
	ResType&				operator * () const;

	ResType*				Get() const;

							operator bool() const;

private:
	// disable heap alloc
	//void*					operator new (size_t size);
	//void					operator delete(void* ptr);

	ResPtrHolder*			m_pResHolder;
};


template <class ResType>
ResourcePtr<ResType>::ResourcePtr(const ResourcePtr<ResType>& ob)
{
	m_pResHolder = ob.m_pResHolder;
	if (m_pResHolder)
	{
		m_pResHolder->AddRef();
	}
}

template <class ResType>
ResourcePtr<ResType>::ResourcePtr(ResPtrHolder* pResHolder)
{
	m_pResHolder = pResHolder;
	if (m_pResHolder)
	{
		m_pResHolder->AddRef();
	}
}


template <class ResType>
ResourcePtr<ResType>::~ResourcePtr()
{
	if (m_pResHolder)
	{
		m_pResHolder->Release();
	}
}

template <class ResType>
ResourcePtr<ResType>& ResourcePtr<ResType>::operator = (const ResourcePtr<ResType>& ob)
{
	if (m_pResHolder)
	{
		m_pResHolder->Release();
	}
	m_pResHolder = ob.m_pResHolder;
	if (m_pResHolder)
	{
		m_pResHolder->AddRef();
	}
	return *this;
}

template <class ResType>
ResType* ResourcePtr<ResType>::operator -> () const
{
	assert(m_pResHolder);
	return static_cast<ResType*>(m_pResHolder->GetResPtr());
}

template <class ResType>
ResType& ResourcePtr<ResType>::operator * () const
{
	assert(m_pResHolder);
	return *static_cast<ResType*>(m_pResHolder->GetResPtr());
}

template <class ResType>
ResType* ResourcePtr<ResType>::Get() const
{
	assert(m_pResHolder);
	return static_cast<ResType*>(m_pResHolder->GetResPtr());
}

template <class ResType>
ResourcePtr<ResType>::operator bool() const
{
	return (m_pResHolder != NULL);
}

//////////////////////////////////////////////////////////////////////////


class Texture;
class VertexLayout;
class Geometry;
class Material;
class RenderTarget;
class MaterialInstance;
class Renderable;
class MaterialInstLib;

class ResourceManager
{
public:
	ResourceManager(void);
	~ResourceManager(void);

	typedef string IDType;

	static char* DEFAULT_GEOM;
	static char* DEFAULT_TEXTURE;
	static char* VEXDESC_PNTT;				// pos, normal, texture, tangent
	static char* VEXDESC_PT;
	static char* VEXDESC_P;

	ResourcePtr<Texture>			GetTexture(const IDType& id);
	ResourcePtr<Geometry>		GetGeometryData(const IDType& id);

	ResourcePtr<RenderTarget> 		AddRenderTarget(const IDType& id, RenderTarget* pRT);
	ResourcePtr<RenderTarget>		GetRenderTarget(const IDType& id);

	void			AddGeometryData(const IDType& id, Geometry* pGeom);

	int				ClearUnrefResource();

	//ResourcePtr<Material>			GetMaterial(const IDType& id);
	//ResourcePtr<Material>			AddMaterial(const IDType& id, Material* pMaterial);
	//MaterialInstLib*				GetMaterialInstLib();
	//MaterialInstance*				CreateMaterialInst(const IDType& mtlId);
	

	void			Init();
	void			OnResetDevice();
	void			OnLostDevice();
	void			CleanUp();


	//RenderableObject*			CreateRenderableObject(const IDType& geomId, const IDType& descId);

private:
	ResPtrHolder*		Find(const IDType& id);
	ResPtrHolder*		CreateTextureFromFile(const IDType& id);

	ResPtrHolder*		CreateDefaultGeometryData(const IDType& id);

	map<IDType, ResPtrHolder*>			m_Resources;
	
	//shared_ptr<MaterialInstLib>			m_pMaterialInstLib;
};

//inline MaterialInstLib* ResourceManager::GetMaterialInstLib()
//{
//	return m_pMaterialInstLib.get();
//}

}
