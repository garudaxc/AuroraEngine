#include "stdHeader.h"
#include "ResourceManager.h"
#include "Renderer.h"
#include "MathFunction.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "Texture.h"

namespace Aurora
{


ResPtrHolder::ResPtrHolder(Resource* pResource):m_pResource(pResource),m_nRefCount(0)
{
	//assert(pResource != NULL);
}

ResPtrHolder::~ResPtrHolder()
{
	assert(m_nRefCount == 0);
}

void ResPtrHolder::ResetResource(Resource* pResource)
{
	m_pResource = pResource;
}
		



//////////////////////////////////////////////////////////////////////////
char* ResourceManager::DEFAULT_GEOM			= "def_geom";
char* ResourceManager::DEFAULT_TEXTURE		= "def_tex";
char* ResourceManager::VEXDESC_PNTT			= "vert_decl_PNTT";
char* ResourceManager::VEXDESC_PT			= "vert_decl_PT";
char* ResourceManager::VEXDESC_P			= "vert_decl_P";


ResourceManager::ResourceManager(void)
{
}

ResourceManager::~ResourceManager(void)
{
	OnLostDevice();
}

int ResourceManager::ClearUnrefResource()
{
	return 0;
}

ResourcePtr<Texture> ResourceManager::GetTexture(const IDType& id)
{
	ResPtrHolder* pHolder = Find(id);
	if (pHolder == NULL)
	{
		pHolder = CreateTextureFromFile(id);
		assert(pHolder);
	}

	return ResourcePtr<Texture>(pHolder);
}


ResourcePtr<VertexDescription>	ResourceManager::GetVertexDesc(const IDType& id)
{
	assert(0);
	ResPtrHolder* pHolder = Find(id);
	if (pHolder == NULL)
	{
		pHolder = Find(VEXDESC_PNTT);
	}

	return ResourcePtr<VertexDescription>(pHolder);
}


ResPtrHolder* ResourceManager::Find(const IDType& id)
{
	map<string, ResPtrHolder*>::iterator it = m_Resources.find(id);
	if (it != m_Resources.end())
	{
		return it->second;
	}
	return NULL;
}

ResPtrHolder* ResourceManager::CreateTextureFromFile(const IDType& id)
{
	//Texture* pTex = ->CreateTexture(id);

	//if (pTex != NULL)
	//{
	//	ResPtrHolder* pHolder = new ResPtrHolder(pTex);
	//	m_Resources.insert(make_pair(id, pHolder));
	//	return pHolder;
	//}

	// return default holder
	return NULL;
}

ResourcePtr<Geometry> ResourceManager::GetGeometryData(const IDType& id)
{
	ResPtrHolder* pHolder = Find(id);
	if (pHolder == NULL)
	{
		pHolder = Find(DEFAULT_GEOM);
	}

	return ResourcePtr<Geometry>(pHolder);
}


void ResourceManager::AddGeometryData(const IDType& id, Geometry* pGeom)
{
	map<string, ResPtrHolder*>::iterator it = m_Resources.find(id);
	if (it != m_Resources.end())
	{
		delete it->second;
		it->second = new ResPtrHolder(pGeom);
	}
	else
	{
		ResPtrHolder* pHolder = new ResPtrHolder(pGeom);
		m_Resources.insert(make_pair(id, pHolder));
	}
}

ResourcePtr<RenderTarget> ResourceManager::AddRenderTarget(const IDType& id, RenderTarget* pRT)
{	
	assert(Find(id) == NULL);

	ResPtrHolder* pHolder = new ResPtrHolder(pRT);
	m_Resources.insert(make_pair(id, pHolder));
	return ResourcePtr<RenderTarget>(pHolder);
}
//
//ResourcePtr<Material> ResourceManager::GetMaterial(const IDType& id)
//{
//	ResPtrHolder* pHolder = Find(id);
//	//if (pHolder == NULL)
//	//{
//	//	//assert(0);
//	//	Material* pMaterial = LoadMatreial(id);
//	//	return this->AddMaterial(id, pMaterial);
//	//}
//
//	return ResourcePtr<Material>(pHolder);
//}
//
//ResourcePtr<Material> ResourceManager::AddMaterial(const IDType& id, Material* pMaterial)
//{	
//	assert(Find(id) == NULL);
//
//	//ResPtrHolder* pHolder = new ResPtrHolder(pMaterial);
//	ResPtrHolder* pHolder = new ResPtrHolder(NULL);
//	m_Resources.insert(make_pair(id, pHolder));
//	return ResourcePtr<Material>(pHolder);
//}
//
//MaterialInstance* ResourceManager::CreateMaterialInst(const IDType& mtlId)
//{
//	MaterialInstance* pMatInst = GetMaterial(mtlId)->CreateMaterialInstance();
//	return pMatInst;
//}


ResourcePtr<RenderTarget> ResourceManager::GetRenderTarget(const IDType& id)
{
	ResPtrHolder* pHolder = Find(id);
	if (pHolder == NULL)
	{
		//assert(0);
	}

	return ResourcePtr<RenderTarget>(pHolder);
}

void ResourceManager::Init()
{
	// create default geom
	Matrix4f mat;
	MatrixScale(mat, Vector3f(5.0f, 5.0f, 5.0f));
	Geometry* pGeom = StandardMesh::CreateSphere(10, 10, false, mat);

	ResPtrHolder* pHolder = new ResPtrHolder(pGeom);
	m_Resources.insert(make_pair(DEFAULT_GEOM, pHolder));

	//m_pMaterialInstLib.reset(new MaterialInstLib());
}

void ResourceManager::OnResetDevice()
{
	map<string, ResPtrHolder*>::iterator it = m_Resources.begin();
	for (; it != m_Resources.end(); ++it)
	{
		it->second->GetResPtr()->OnReset();
	}
}

void ResourceManager::OnLostDevice()
{
	map<string, ResPtrHolder*>::iterator it = m_Resources.begin();
	for (; it != m_Resources.end(); ++it)
	{
		it->second->GetResPtr()->OnLost();
	}
}

void ResourceManager::CleanUp()
{
	//m_pMaterialInstLib.reset();
	map<string, ResPtrHolder*>::iterator it = m_Resources.begin();
	for (; it != m_Resources.end(); ++it)
	{
		delete it->second;
	}

	m_Resources.clear();
}

//RenderableObjectPtr ResourceManager::CreateRenderableObject(const IDType& geomId, const IDType& descId)
//{
//	Geometry* pGeomData = GetGeometryData(geomId).Get();
//	assert(pGeomData);
//
//	ResourcePtr<VertexDescription> pVertexDesc = GetVertexDesc(descId);
//
//	assert(0);
//	RenderableObjectPtr pRenderable;
//	//RenderableObjectPtr pRenderable = RenderableObjectPtr(
//	//	new Renderable(pVertexDesc.Get(), pGeomData));
//
//	return pRenderable;
//}



ResPtrHolder* ResourceManager::CreateDefaultGeometryData(const IDType& id)
{
	Matrix4f mat;
	MatrixScale(mat, Vector3f(5.0f, 5.0f, 5.0f));
	Geometry* pGeom = StandardMesh::CreateSphere(10, 10, false, mat);


	ResPtrHolder* pHolder = new ResPtrHolder(pGeom);
	m_Resources.insert(make_pair(id, pHolder));
	return pHolder;
}



}