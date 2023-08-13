#include "stdheader.h"
#include "HelperRenderer.h"
#include "Camera.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "Renderer.h"
#include "OcclusionQuery.h"

namespace Aurora
{


	//// ������ƽ��Ľ��������ĳһ�������Ƿ�Ϊ�߽�
	//bool TestEdge(const Plane& p0, const Plane& p1, const Vector3f& dir)
	//{
	//	float t0 = Vector3f::Dot(p0.n, dir);
	//	float t1 = Vector3f::Dot(p1.n, dir);
	//	if (t0 * t1 < 0.0f)
	//	{
	//		return true;
	//	}

	//	return false;
	//}



	class LineRenderer
	{
	public:
		LineRenderer();
		~LineRenderer();

		bool Init();

		void DrawLine(const Vector3f& a, const Vector3f& b, const Color& color, int pickupId, OcclusionQuery* pQuery);

		void DrawAABB(const Vector3f& vMin, const Vector3f& vMax, const Color& color, int pickupId, OcclusionQuery* pQuery);
	private:
		void RenderOp(RenderOperator& op, int pickupId, OcclusionQuery* pQuery);

		MaterialInstance* m_pMatInst;

		//VertexBuffer* m_pVertexBuffer;

		RenderOperator m_op;
	};


	const int VERTEX_BUFFER_SIZE = 1024;

	LineRenderer::LineRenderer():m_pMatInst(nullptr)
	{

	}

	LineRenderer::~LineRenderer()
	{
		delete m_pMatInst;
		//delete m_pVertexBuffer;
	}

	bool LineRenderer::Init()
	{
		m_pMatInst = GMaterialInstanceManager.GetMaterialInstance("asset:DrawLine.mtlInst", set<string>());
		//m_pVertexBuffer = GRenderDevice->CreateDynamicVertexBuffer(sizeof(Vector3f), VERTEX_BUFFER_SIZE);

		memset(&m_op, 0, sizeof(m_op));

		//m_op.pVertexBuffer = m_pVertexBuffer;
		//m_op.pMtlInst = m_pMatInst;
		//m_op.VertLayout = VertexLayout_P;
		//m_op.nPrimType = RenderOperator::PT_LINELIST;
		//m_op.nBaseVertexIndex = 0;
		//m_op.nStartIndex = 0;
		//m_op.nVertexStride = sizeof(Vector3f);

		return true;
	}
//
//	void HelperRenderer::DrawFrustum(Camera* pCamera, const Color& color, const Vector3f& dir)
//	{
//		float halfFov = pCamera->GetFov() / 2.0f;
//		float nearHalfHeight = pCamera->GetNearPlane() * Mathf::Tan(halfFov);
//		float farHalfHeight = pCamera->GetFarPlane() * Mathf::Tan(halfFov);
//		float nearHalfWidth = pCamera->GetAspect() * nearHalfHeight;
//		float farHalfWidth = pCamera->GetAspect() * farHalfHeight;
//
//		Vector3f lookDir = pCamera->GetLookDir();
//		Vector3f upDir = pCamera->GetUpDir();
//		Vector3f rightDir = pCamera->GetRightDir();
//
//		Vector3f nearPos = pCamera->GetEyePos() + lookDir * pCamera->GetNearPlane();
//		Vector3f farPos = pCamera->GetEyePos() + lookDir * pCamera->GetFarPlane();
//
//		Vector3f corner[8];
//		corner[0] = nearPos - rightDir * nearHalfWidth + upDir * nearHalfHeight;
//		corner[1] = nearPos + rightDir * nearHalfWidth + upDir * nearHalfHeight;
//		corner[2] = nearPos + rightDir * nearHalfWidth - upDir * nearHalfHeight;
//		corner[3] = nearPos - rightDir * nearHalfWidth - upDir * nearHalfHeight;
//
//		corner[4] = farPos - rightDir * farHalfWidth + upDir * farHalfHeight;
//		corner[5] = farPos + rightDir * farHalfWidth + upDir * farHalfHeight;
//		corner[6] = farPos + rightDir * farHalfWidth - upDir * farHalfHeight;
//		corner[7] = farPos - rightDir * farHalfWidth - upDir * farHalfHeight;
//
//		uint16 index[] = 
//		{
//			0, 1, 1, 2, 2, 3, 3, 0,
//			4, 5, 5, 6, 6, 7, 7, 4, 
//			0, 4, 1, 5, 2, 6, 3, 7,
//		};
//
//		m_op.pMtlInst->SetValueF("Color", color.Ptr(), 4);
//
//
//		m_op.pSystemVertBuff = (float*)corner[0].Ptr();
//		m_op.pSystemIndexBuff = index;
//		m_op.nPrimType = RenderOperator::PT_LINELIST;
//		m_op.nNumVertices = 8;
//		m_op.nStartIndex = 0;
//		m_op.nPrimitiveCount = 12;
//		m_op.pMaterial->BeginPass(0);
//		HALExecuteOperator(m_op);
//
//
//
//
//		Frustum frustum;
//		ExtractFrustum(pCamera, frustum);
//
//		/*corner[0] = IntersectPlane(frustum.planes[Frustum::PID_FRONT], frustum.planes[Frustum::PID_LEFT], frustum.planes[Frustum::PID_TOP]);
//		corner[1] = IntersectPlane(frustum.planes[Frustum::PID_FRONT], frustum.planes[Frustum::PID_TOP], frustum.planes[Frustum::PID_RIGHT]);
//		corner[2] = IntersectPlane(frustum.planes[Frustum::PID_FRONT], frustum.planes[Frustum::PID_RIGHT], frustum.planes[Frustum::PID_BOTTOM]);
//		corner[3] = IntersectPlane(frustum.planes[Frustum::PID_FRONT], frustum.planes[Frustum::PID_BOTTOM], frustum.planes[Frustum::PID_LEFT]);
//
//		corner[4] = IntersectPlane(frustum.planes[Frustum::PID_BACK], frustum.planes[Frustum::PID_LEFT], frustum.planes[Frustum::PID_TOP]);
//		corner[5] = IntersectPlane(frustum.planes[Frustum::PID_BACK], frustum.planes[Frustum::PID_TOP], frustum.planes[Frustum::PID_RIGHT]);
//		corner[6] = IntersectPlane(frustum.planes[Frustum::PID_BACK], frustum.planes[Frustum::PID_RIGHT], frustum.planes[Frustum::PID_BOTTOM]);
//		corner[7] = IntersectPlane(frustum.planes[Frustum::PID_BACK], frustum.planes[Frustum::PID_BOTTOM], frustum.planes[Frustum::PID_LEFT]);
//
//		m_op.pMtlInst->SetValueF("Color", Color::RED.Ptr(), 4);
//
//		*/
//
//		Vector3f insidePoint = (nearPos + farPos) / 2.0f;
//
//		// ǰһ����˳�űߣ���һ�������ű�
//		vector<pair<Frustum::PlaneID, Frustum::PlaneID>> planePair;
//		planePair.push_back(make_pair(Frustum::PID_FRONT, Frustum::PID_TOP));
//		planePair.push_back(make_pair(Frustum::PID_FRONT, Frustum::PID_RIGHT));
//		planePair.push_back(make_pair(Frustum::PID_FRONT, Frustum::PID_BOTTOM));
//		planePair.push_back(make_pair(Frustum::PID_FRONT, Frustum::PID_LEFT));
//
//		planePair.push_back(make_pair(Frustum::PID_TOP, Frustum::PID_BACK));
//		planePair.push_back(make_pair(Frustum::PID_RIGHT, Frustum::PID_BACK));
//		planePair.push_back(make_pair(Frustum::PID_BOTTOM, Frustum::PID_BACK));
//		planePair.push_back(make_pair(Frustum::PID_LEFT, Frustum::PID_BACK));
//
//		planePair.push_back(make_pair(Frustum::PID_TOP, Frustum::PID_LEFT));
//		planePair.push_back(make_pair(Frustum::PID_RIGHT, Frustum::PID_TOP));
//		planePair.push_back(make_pair(Frustum::PID_BOTTOM, Frustum::PID_RIGHT));
//		planePair.push_back(make_pair(Frustum::PID_LEFT, Frustum::PID_BOTTOM));
//
//
//		//PID_TOP = 0,
//		//	PID_BOTTOM,
//		//	PID_LEFT,
//		//	PID_RIGHT,
//		//	PID_FRONT,
//		//	PID_BACK,
//
//
//
//		vector<Vector3f> points;
//		vector<Vector3f> normals;
//		uint16 edgeIndex[24];
//		int numEdge = 0;
//		for (uint i = 0; i < planePair.size(); i++)
//		{	
//			pair<Frustum::PlaneID, Frustum::PlaneID> twoPlane = planePair[i];
//			if (TestEdge(frustum.planes[twoPlane.first],
//				frustum.planes[twoPlane.second], dir))
//			{
//				edgeIndex[numEdge * 2 + 0] = index[i * 2 + 0];
//				edgeIndex[numEdge * 2 + 1] = index[i * 2 + 1];
//				numEdge++;
//
//				// ����ǵ�һ��ƽ�棬����˳�ŵģ�����ǵڶ���ƽ�棬�������ŵ�
//				Vector3f edgeDir;
//				if (Vector3f::Dot(frustum.planes[twoPlane.first].n, dir) > 0.0f)
//				{
//					edgeDir = corner[index[i * 2 + 1]] - corner[index[i * 2 + 0]];
//				}
//				else
//				{
//					edgeDir = corner[index[i * 2 + 0]] - corner[index[i * 2 + 1]];
//				}
//
//				edgeDir.Normalize();
//
//				Vector3f normal = CrossProduct(dir, edgeDir);
//
//				Vector3f pos = corner[index[i * 2 + 0]];
//				points.push_back(pos);
//				points.push_back(pos + dir * 200.0f);
//
//				pos = corner[index[i * 2 + 1]];
//				points.push_back(pos);
//				points.push_back(pos + dir * 200.0f);
//
//				//  normal
//				pos = corner[index[i * 2 + 0]];
//				normals.push_back(pos);
//				normals.push_back(pos + normal * 200.0f);
//
//				pos = corner[index[i * 2 + 1]];
//				normals.push_back(pos);
//				normals.push_back(pos + normal * 200.0f);
//
//
//			}
//		}
//
//		m_op.pMtlInst->SetValueF("Color", Color::RED.Ptr(), 4);
//		m_op.nPrimitiveCount = numEdge;
//		m_op.pSystemIndexBuff = edgeIndex;
//		HALExecuteOperator(m_op);
//
//
//
//		points.push_back(pCamera->GetEyePos());
//		points.push_back( pCamera->GetEyePos() + dir * 200.0f);
//
//
//		m_op.nPrimitiveCount = (uint)points.size() / 2;
//		m_op.pSystemVertBuff = points[0].Ptr();
//		m_op.pSystemIndexBuff = nullptr;
//		HALExecuteOperator(m_op);
//
//		//show normal
//		m_op.pMtlInst->SetValueF("Color", Color::GREEN.Ptr(), 4);
//		m_op.nPrimitiveCount = (uint)normals.size() / 2;
//		m_op.pSystemVertBuff = normals[0].Ptr();
//		m_op.pSystemIndexBuff = nullptr;
//		m_op.nPrimType = RenderOperator::PT_LINELIST;
//		HALExecuteOperator(m_op);
//
//	}
//
	void LineRenderer::RenderOp(RenderOperator& op, int pickupId, OcclusionQuery* pQuery)
	{

		m_op.pMtlInst->Apply(0);
		GRenderDevice->ExecuteOperator(m_op);

		//if (pQuery)
		//{
		//	pQuery->BeginObject(pickupId);
		//	HALExecuteOperator(m_op);
		//	pQuery->EndObject();
		//}
		//else
		//{
		//	HALExecuteOperator(m_op);
		//}
	}


	void LineRenderer::DrawLine(const Vector3f& a, const Vector3f& b, const Color& color, int pickupId, OcclusionQuery* pQuery)
	{
		{
			Vector3f corner[2];
			corner[0] = a;
			corner[1] = b;

			//VertexBuffer::Locker locker = m_pVertexBuffer->GetLocker();
			//memcpy(locker.Ptr(), corner, sizeof(corner));
		}

		//m_op.pMtlInst->SetVariableValue("vDiffuse", color.Ptr());
		//m_op.nNumVertices = 2;
		//m_op.nStartIndex = 0;
		//m_op.nPrimitiveCount = 1;
		//m_op.nPrimType = RenderOperator::PT_LINELIST;

		RenderOp(m_op, pickupId, pQuery);
	}
//
//	void HelperRenderer::DrawLineList(const Vector3f* pPos, uint nNumPoint, const Color& color, int pickupId, OcclusionQuery* pQuery)
//	{
//		m_op.pMtlInst->SetValueF("Color", color.Ptr(), 4);
//
//		m_op.pSystemVertBuff = (float*)pPos;
//		m_op.pSystemIndexBuff = nullptr;
//		m_op.nNumVertices = nNumPoint;
//		m_op.nStartIndex = 0;
//		m_op.nPrimitiveCount = nNumPoint / 2;
//		m_op.nPrimType = RenderOperator::PT_LINELIST;
//		m_op.pMaterial->BeginPass(0);
//		HALExecuteOperator(m_op);
//		RenderOp(m_op, pickupId, pQuery);
//	}
//
//	void HelperRenderer::DrawLineStrip(const Vector3f* pPos, uint nNumPoint, const Color& color, int pickupId, OcclusionQuery* pQuery)
//	{
//		m_op.pMtlInst->SetValueF("Color", color.Ptr(), 4);
//
//		m_op.pSystemVertBuff = (float*)pPos;
//		m_op.pSystemIndexBuff = nullptr;
//		m_op.nNumVertices = nNumPoint;
//		m_op.nStartIndex = 0;
//		m_op.nPrimitiveCount = nNumPoint - 1;
//		m_op.nPrimType = RenderOperator::PT_LINESTRIP;
//		m_op.pMaterial->BeginPass(0);
//		HALExecuteOperator(m_op);
//		RenderOp(m_op, pickupId, pQuery);
//	}
//
//	void HelperRenderer::DrawCicle(const Vector3f& center, float radius, const Vector3f& dir, 
//									const Color& color, int pickupId, OcclusionQuery* pQuery)
//	{
//		uint split = 20;
//		vector<Vector3f> pos;
//		pos.resize(split + 1);
//		float step = Mathf::PI * 2.0f / (float)split;
//		for (uint i = 0; i < split; i++)
//		{
//			float angle = step * i;
//			pos[i] = Vector3f(Mathf::Cos(angle) * radius, Mathf::Sin(angle) * radius, 0.0f);
//		}
//		pos[split] = pos[0];
//
//		Quaternionf qRot;
//		QuaternionVectorToVector(qRot, Vector3f::UNIT_Z, dir);
//		
//		Matrix4f mat;
//		MatrixTransform(mat, qRot, center);
//
//		Vector3TransformArray(&pos[0], sizeof(Vector3f), &pos[0], sizeof(Vector3f), split + 1, mat);
//
//		DrawLineStrip(&pos[0], split + 1, color, pickupId, pQuery);
//	}
//
	void LineRenderer::DrawAABB(const Vector3f& vMin, const Vector3f& vMax, const Color& color, 
								int pickupId, OcclusionQuery* pQuery)
	{
		Vector3f corner[8];
		corner[0].Set(vMin.x, vMin.y, vMin.z);
		corner[1].Set(vMin.x, vMax.y, vMin.z);
		corner[2].Set(vMax.x, vMax.y, vMin.z);
		corner[3].Set(vMax.x, vMin.y, vMin.z);
		corner[4].Set(vMin.x, vMin.y, vMax.z);
		corner[5].Set(vMin.x, vMax.y, vMax.z);
		corner[6].Set(vMax.x, vMax.y, vMax.z);
		corner[7].Set(vMax.x, vMin.y, vMax.z);

		{
			uint16 index[] = 
			{
				0, 1, 1, 2, 2, 3, 3, 0,
				4, 5, 5, 6, 6, 7, 7, 4, 
				0, 4, 1, 5, 2, 6, 3, 7,
			};

			//VertexBuffer::Locker locker = m_pVertexBuffer->GetLocker();
			//Vector3f* pPos = (Vector3f*)locker.Ptr();
			//for (int i = 0; i < 24; i++)
			//{
			//	pPos[i] = corner[index[i]];
			//}
		}

		//m_op.pMtlInst->SetVariableValue("vDiffuse", color.Ptr());

		//m_op.nPrimType = RenderOperator::PT_LINELIST;
		//m_op.nNumVertices = 24;
		//m_op.nStartIndex = 0;
		//m_op.nPrimitiveCount = 12;

		//RenderOp(m_op, pickupId, pQuery);
	}


//
//	TextRenderer::TextRenderer(const string& strTexture, uint nRow, uint nCol):
//	m_strTexture(strTexture),m_nRow(nRow),m_nCol(nCol)
//	{
//	}
//
//	TextRenderer::~TextRenderer()
//	{
//	}
//
//	bool TextRenderer::Init()
//	{
//		float fWidth = 1.0f / (float)MAX_TEXT;
//
//		m_pMatInst = Global::GetResourceManager()->CreateMaterialInst("MtlUI");
//		m_pMatInst->SetTexture("texUI", Global::GetResourceManager()->GetTexture(m_strTexture));
//
//		m_VertBuff.resize((MAX_TEXT + 1) * 4);
//		m_Index.resize(MAX_TEXT * 3 * 2);
//
//		for (uint i = 0; i <= MAX_TEXT; i++)
//		{
//			m_VertBuff[i * 4 + 0].pos.Set(i * fWidth, 0.0f, 0.0f);
//			m_VertBuff[i * 4 + 1].pos.Set(i * fWidth, 1.0f, 0.0f);
//
//			m_VertBuff[i * 4 + 2].pos.Set((i + 1) * fWidth, 0.0f, 0.0f);
//			m_VertBuff[i * 4 + 3].pos.Set((i + 1) * fWidth, 1.0f, 0.0f);
//		}
//
//		for (uint i = 0; i < MAX_TEXT; i++)
//		{
//			m_Index[i * 6 + 0] = i * 4 + 0;
//			m_Index[i * 6 + 1] = i * 4 + 1;
//			m_Index[i * 6 + 2] = i * 4 + 2;
//
//			m_Index[i * 6 + 3] = i * 4 + 2;
//			m_Index[i * 6 + 4] = i * 4 + 1;
//			m_Index[i * 6 + 5] = i * 4 + 3;
//		}
//
//		m_op.pSystemVertBuff = (float*)&m_VertBuff[0];
//		m_op.pSystemIndexBuff = (uint16*)&m_Index[0];
//		m_op.pMaterial = m_pMatInst->GetMaterialTemplate();
//		m_op.pMtlInst = m_pMatInst.get();
//		m_op.pVertDesc = Global::GetResourceManager()->GetVertexDesc(ResourceManager::VEXDESC_PT).Get();
//		m_op.nPrimType = RenderOperator::PT_TRIANGLELIST;
//		m_op.nBaseVertexIndex = 0;
//		m_op.nMinVertexIndex = 0;
//		m_op.nNumVertices = (uint)m_VertBuff.size();
//		m_op.nStartIndex = 0;
//		m_op.nVertexStride = sizeof(VertexPT);
//		return true;
//	}
//
//	void TextRenderer::DrawText(const char* pText, uint xPos, uint yPos, uint cw, uint ch, const Color& color)
//	{
//		float fWidth = 1.0f / (float)MAX_TEXT;
//		uint nNumChar = (uint)strlen(pText);
//
//		Vector2f uvStride(1.0f / (float)m_nCol, 1.0f / (float)m_nRow);
//		for (uint i = 0; i < nNumChar; i++)
//		{
//			int xidx = pText[i] - '0';
//			int yidx = 0;
//
//			Vector2f uvBase(uvStride.x * xidx, uvStride.y * yidx);
//			m_VertBuff[i * 4 + 0].uv.Set(uvBase.x, uvBase.y);
//			m_VertBuff[i * 4 + 1].uv.Set(uvBase.x, uvBase.y + uvStride.y);
//			m_VertBuff[i * 4 + 2].uv.Set(uvBase.x + uvStride.x, uvBase.y);
//			m_VertBuff[i * 4 + 3].uv.Set(uvBase.x + uvStride.x, uvBase.y + uvStride.y);
//		}
//
//
//		uint nWidth, nHeight;
//		HALGetFrameBufferSize(nWidth, nHeight);
//
//		uint dw = nNumChar * cw;
//		uint dh = 1 * ch;
//
//		float param[4];
//		param[0] = (float)dw / (nWidth * nNumChar * fWidth * 0.5f);
//		param[1] = -(float)dh / (nHeight * 0.5f);
//		param[2] = 2.0f * xPos / (float)nWidth - 1.0f;
//		param[3] = -2.0f * yPos / (float)nHeight + 1.0f;
//		m_op.pMtlInst->SetValueF("SpriteParam", param, 4);
//		m_op.pMtlInst->SetValueF("Color", color.Ptr(), 4);
//
//		m_op.pMaterial->BeginPass(0);
//		m_op.nPrimitiveCount = 2 * nNumChar;
//		HALExecuteOperator(m_op);
//	}
//
//


	class ScreenQuadRenderer
	{
	public:
		ScreenQuadRenderer();
		~ScreenQuadRenderer();

		bool Init();

		void DrawScreenQuad(MaterialInstance* pMtlInst);

	private:
		void InitVertexBuffer();

		MaterialInstance* m_pMatInst;
		
		RenderOperator m_op;
	};


	ScreenQuadRenderer::ScreenQuadRenderer()
	{
	}

	ScreenQuadRenderer::~ScreenQuadRenderer()
	{
	}


	bool ScreenQuadRenderer::Init()
	{
		m_pMatInst = GMaterialInstanceManager.GetMaterialInstance("asset:DrawLine.mtlInst", set<string>());

		//m_pVertexBuffer = GRenderDevice->CreateDynamicVertexBuffer(sizeof(VertexPT), 6);

		//memset(&m_op, 0, sizeof(m_op));
		//m_op.pVertexBuffer = m_pVertexBuffer;
		//m_op.pMtlInst = nullptr;
		//m_op.VertLayout = VertexLayout_PT;
		//m_op.nPrimType = RenderOperator::PT_TRIANGLELIST;
		//m_op.nVertexStride = sizeof(VertexPT);
		//m_op.nBaseVertexIndex = 0;
		//m_op.nStartIndex = 0;
		//m_op.nNumVertices = 6;
		//m_op.nPrimitiveCount = 2;

		InitVertexBuffer();

		return true;
	}


	void ScreenQuadRenderer::InitVertexBuffer()
	{
		VertexPT verties[6];
		verties[0].pos.Set(-1.f,  1.f, 0.f);
		verties[1].pos.Set(-1.f, -1.f, 0.f);
		verties[2].pos.Set( 1.f, -1.f, 0.f);
		verties[3].pos.Set(-1.f,  1.f, 0.f);
		verties[4].pos.Set( 1.f, -1.f, 0.f);
		verties[5].pos.Set( 1.f,  1.f, 0.f);

		verties[0].uv.Set(0.f, 0.f);
		verties[1].uv.Set(0.f, 1.f);
		verties[2].uv.Set(1.f, 1.f);
		verties[3].uv.Set(0.f, 0.f);
		verties[4].uv.Set(1.f, 1.f);
		verties[5].uv.Set(1.f, 0.f);

		//{
		//	VertexBuffer::Locker locker = m_pVertexBuffer->GetLocker();
		//	memcpy(locker.Ptr(), verties, sizeof(verties));
		//}
	}

	void ScreenQuadRenderer::DrawScreenQuad(MaterialInstance* pMtlInst)
	{
		m_op.pMtlInst = pMtlInst;
		m_op.pMtlInst->Apply(0);
		GRenderDevice->ExecuteOperator(m_op);
	}


//////////////////////////////////////////////////////////////////////////

	HelperRenderer::HelperRenderer():m_pLineRenderer(nullptr),m_pQuadRenderer(nullptr)
	{
	}

	HelperRenderer::~HelperRenderer()
	{
		if (m_pLineRenderer)
		{
			delete m_pLineRenderer;
			m_pLineRenderer = nullptr;
		}
	}

	bool HelperRenderer::Init()
	{
		m_pLineRenderer = new LineRenderer();
		if (!m_pLineRenderer->Init())
		{
			return false;
		}

		m_pQuadRenderer = new ScreenQuadRenderer();
		if (!m_pQuadRenderer->Init())
		{
			return false;
		}

		return true;
	}

	void HelperRenderer::AddLine(const Vector3f& a, const Vector3f& b, const Color& color)
	{
		m_pLineRenderer->DrawLine(a, b, color, 0, nullptr);
	}

	void HelperRenderer::AddAABB(const Vector3f& vMin, const Vector3f& vMax, const Color& color)
	{
		m_pLineRenderer->DrawAABB(vMin, vMax, color, 0, nullptr);
	}

	HelperRenderer GHelperRenderer;


	//void HelperRenderer::DrawScreenQuad(MaterialInstance* pMtlInst)
	//{
	//	m_pQuadRenderer->DrawScreenQuad(pMtlInst);
	//}


}