#include "stdHeader.h"
#include "RendererObject.h"
#include "Geometry.h"
#include "Util.h"
#include "Console.h"
#include "Renderer.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "Camera.h"
#include "Pipeline.h"


namespace Aurora
{

Renderable::Renderable(Geometry* pGeometry, uint nElemIndex):
m_pGeometry(pGeometry),m_pOP(new RenderOperator()),
m_nElemIndex(nElemIndex)
{
	m_pVertexBuffer = pGeometry->GetVertexBuffer();
	m_pIndexBuffer =  pGeometry->GetIndexBuffer();
	BuildRenderOperator(pGeometry);
}

Renderable::~Renderable()
{

}

void Renderable::BuildRenderOperator(Geometry* pGeom)
{
	m_pOP->nBaseVertexIndex		= 0;
	m_pOP->nNumVertices			= m_pGeometry->GetNumVertex();
	m_pOP->nPrimitiveCount		= m_pGeometry->GetNumTri();
	m_pOP->nPrimType			= RenderOperator::PT_TRIANGLELIST;
	m_pOP->nStartIndex			= 0;
	m_pOP->pIndexBuffer			= m_pGeometry->GetIndexBuffer();
	m_pOP->pVertexBuffer		= m_pGeometry->GetVertexBuffer();
	m_pOP->VertLayout			= m_pGeometry->GetVertexLayout();
}

const RenderOperator& Renderable::GetRenderOperator() const
{
	m_pOP->pMtlInst = m_pMatInst;
	//m_pOP->pMaterial = m_pMatInst->GetMaterialTemplate();
	return *m_pOP;
}


void Renderable::UpdateFromParentNode()
{
	//GetMaterialInst()->UpdateShaderConst((ShaderConstProvider*)GetParentNode());
	//Vector3f pos = GetParentNode()->GetWorldTranslation();
	//Vector3f eye = Global::GetPipeline()->GetActiveCamera()->GetEyePos();
	//m_fCullDist = (pos - eye).Length();
}

float Renderable::GetCullDist() const
{
	return m_fCullDist;
}
//////////////////////////////////////////////////////////////////////////



VertexBuffer::Locker::Locker(VertexBuffer* pVertexBuffer):
m_pVertexBuffer(pVertexBuffer)
{
	m_Pointer = pVertexBuffer->Lock();
}

VertexBuffer::Locker::~Locker()
{
	if (m_Pointer != NULL)
	{
		m_pVertexBuffer->Unlock();
	}
}

VertexBuffer::Locker::Locker(Locker& ob)
{
	m_pVertexBuffer = ob.m_pVertexBuffer;
	m_Pointer = ob.m_Pointer;
	ob.m_Pointer = NULL;
}

VertexBuffer::VertexBuffer(uint nStride, uint nNumVertex):
m_nStride(nStride),m_nNumVertex(nNumVertex),HALHandle(NULL)
{
}

VertexBuffer::~VertexBuffer()
{
}

uint VertexBuffer::GetSizeInByte() const
{
	return m_nStride * m_nNumVertex;
}

VertexBuffer::Locker VertexBuffer::GetLocker()
{
	Locker locker(this);
	return locker;
}


//////////////////////////////////////////////////////////////////////////
IndexBuffer::Locker::Locker(IndexBuffer* pIndexBuffer):
m_pIndexBuffer(pIndexBuffer)
{
	m_Pointer = pIndexBuffer->Lock();
}

IndexBuffer::Locker::~Locker()
{
	if (m_Pointer != NULL)
	{
		m_pIndexBuffer->Unlock();
	}
}


IndexBuffer::Locker::Locker(Locker& ob)
{
	m_pIndexBuffer = ob.m_pIndexBuffer;
	m_Pointer		= ob.m_Pointer;

	ob.m_Pointer		= NULL;
}


IndexBuffer::IndexBuffer(Format format, uint numIndex):m_nFormat(format),m_nNumIndex(numIndex)
{
}

IndexBuffer::~IndexBuffer()
{
}


IndexBuffer::Locker IndexBuffer::GetLocker()
{
	Locker locker(this);

	return locker;
}


//////////////////////////////////////////////////////////////////////////
void VertexDescription::OnCreate()
{

}

void VertexDescription::OnRelease()
{

}



}
