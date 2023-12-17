#include "stdHeader.h"
#include <memory.h>
#include <assert.h>
#include "Util.h"
#include "Config.h"

namespace Aurora
{
	void Util::MemMap(const void* pSrc, uint srcStride, void* pDes, 
		uint desStride, uint byteToMap, uint count)
	{
		const char* pByteSrc = (const char*)pSrc;
		char* pByteDes = (char*)pDes;
		for (uint i = 0; i < count; i++)
		{
			memcpy(pByteDes, pByteSrc, byteToMap);
			pByteSrc += srcStride;
			pByteDes += desStride;
		}
	}
	
	void Util::MemCopy(const void* Source, void* Destination, int Size)
	{
		memcpy(Destination, Source, Size);
	}

	uint Util::MakeFOURCC(const char* pStr)
	{
		return (pStr[3] << 24) | (pStr[2] << 16) | (pStr[1] << 8) | pStr[0];
	}

	std::string	Util::GetFileNameExt(const std::string& fileName)
	{
		std::string::size_type i = fileName.rfind('.');
		if (i != std::string::npos)
		{
			return fileName.substr(i+1);
		}

		return "";
	}


	string Util::GetPath(const string& pathName)
	{
		string tmp = pathName;
		//transform(tmp.begin(), tmp.end(), tmp.begin(), tolower);
		replace(tmp.begin(), tmp.end(), '\\', '/');

		auto pos = tmp.rfind('/');
		if (pos == -1) {
			return "";
		}
		
		return tmp.substr(0, pos + 1);
	}


	string Util::ProcessPathName(const string& pathName)
	{
		string tmp = pathName;
		transform(tmp.begin(), tmp.end(), tmp.begin(), tolower);
		replace(tmp.begin(), tmp.end(), '\\', '/');

		if (tmp.find("resource:") == 0)
		{
			return Config::Get()->GetResourcePath() + (tmp.c_str() + strlen("resource:"));
		}
		else if (tmp.find("asset:") == 0)
		{
			return Config::Get()->GetAssetPath() + (tmp.c_str() + strlen("asset:"));
		}

		assert(0);
		return tmp;
	}


	EEncoding Util::GetEncoding(const int8* str)
	{
		if (str[0] == 0xEF && str[1] == 0xBB) {
			return Encoding_UTF_8;
		} else if (str[0] == 0xFE && str[1] == 0xFF) {
			return Encoding_BigEndianUnicode;
		} else if (str[0] == 0xFF && str[1] == 0xFE) {
			return Encoding_Unicode;
		}

		return Encoding_ANSI;
	}

	//TiXmlElement* LinkNewXmlElement(TiXmlElement* pParent, const char* name)
	//{
	//	return (TiXmlElement*)pParent->LinkEndChild(new TiXmlElement(name));
	//}

	//TiXmlText* LinkNewXmlText(TiXmlElement* pParent, const char* text)
	//{
	//	return (TiXmlText*)pParent->LinkEndChild(new TiXmlText(text));
	//}



void ComputeBoundingSphere(void* pVertex, int numVertices, int stride, Vector3f& center, float& radius)
{
	char* pCurPos = (char*)pVertex;
	center = Vector3f::ZERO;

	for (int i = 0; i < numVertices; i++)
	{
		center += *((Vector3f*)pCurPos);
		pCurPos += stride;
	}

	// center is average of all vertices
	center /= (float)numVertices;

	pCurPos = (char*)pVertex;
	radius = 0.0f;
	float dis = 0.0f;
	Vector3f r;

	// find radius as max distance between vertex and center 
	for (int i = 0; i < numVertices; i++)
	{
		r = *((Vector3f*)pCurPos) - center;
		dis = r.LengthSQ();
		if (dis > radius)
		{
			radius = dis;
		}
		pCurPos += stride;
	}

	radius = Mathf::Sqrt(radius);
}
/*
//------------------------------------------------------------------------------
Vector3f 
ComputeNormal(const Vector3f& v1, const Vector3f& v2, const Vector3f& v3)
{
	Vector3f norm = Vector3f::Cross(v2 - v1, v3 - v1);
	norm.Normalize();
	return norm;
}
//---------------------------------------------------------------------------
// compute weighted normal of triangle
Vector3f 
ComputeWeightedNormal(const Vector3f& v1, const Vector3f& v2, const Vector3f& v3)
{
	return Vector3f::Cross(v2 - v1, v3 - v1);
}
*/
//------------------------------------------------------------------------------
void ComputePlane(const Vector3f& v1, const Vector3f& v2, const Vector3f& v3,
				  Vector3f& normal, float& fD)
{
	normal = Vector3f::Cross(v2 - v1, v3 - v1);
	normal.Normalize();
	fD = -Vector3f::Dot(v1, normal);
}
//-----------------------------------------------------------------------------
float ComputeArea(const Vector3f& v1, const Vector3f& v2, const Vector3f& v3)
{
	Vector3f e1 = v2 - v1;
	Vector3f e2 = v3 - v1;
	Vector3f norm = Vector3f::Cross(e1, e2);
	return 0.5f * norm.Length();
}

//-----------------------------------------------------------------------------

bool IntersectTriangle(const Vector3f& origin, const Vector3f& dir, 
				  const Vector3f& v0, const Vector3f& v1, const Vector3f& v2,
				  float& fu, float& fv, float& fDist)
{
	Vector3f edge1 = v1 - v0;
	Vector3f edge2 = v2 - v0;
	Vector3f diff;

	Vector3f normal = Vector3f::Cross(edge1, edge2);

	float det = Vector3f::Dot(dir, normal);
	if (det > Mathf::ZERO_TOLERANCE)
	{
		diff  = origin - v0;
	}
	else if (det < -Mathf::ZERO_TOLERANCE)
	{
		diff  = v0 - origin;
		det = -det;
	}
	else
	{
		return false;
	}

	float b1 = Vector3f::Dot(dir, Vector3f::Cross(diff, edge2));
	if (b1 >= 0.0f)
	{
		float b2 = Vector3f::Dot(dir, Vector3f::Cross(edge1, diff));
		if (b2 >= 0.0f)
		{
			if (b1 + b2 <= det)
			{
				float inv = 1.0f / det;
				fu = b1 * inv;
				fv = b2 * inv;
				fDist = -Vector3f::Dot(diff, normal) * inv;
				return true;
			}
			// else b1 + b2 > 1, no intersection
		}
		// else b2 < 0, no intersection
	}
	// else b1 < 0, no intersection

	return false;

}

//-----------------------------------------------------------------------------
Vector3f BarycentricToPos(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2,
						float fu, float fv)
{
	Vector3f edge1 = v1 - v0;
	Vector3f edge2 = v2 - v0;

	edge1 *= fu;
	edge2 *= fv;
	edge1 += edge2;

	return v0 + edge1;
}
//--------------------------------------------------------------------------------
void MemMap(const void* pSrc, uint srcStride, void* pDes, uint desStride, uint byteToMap, uint count)
{
	const char* pByteSrc = (const char*)pSrc;
	char* pByteDes = (char*)pDes;
	for (uint i = 0; i < count; i++)
	{
		memcpy(pByteDes, pByteSrc, byteToMap);
		pByteSrc += srcStride;
		pByteDes += desStride;
	}

}
//------------------------------------------------------------------------------------
void MapMatrixToShaderConst(const Matrix4f& mat, float* pOutBuffer, uint nNumReg)
{
	if (nNumReg == 1)
	{
		pOutBuffer[0] = mat._11; pOutBuffer[1] = mat._21; pOutBuffer[2] = mat._31; pOutBuffer[3] = mat._41; 
	}
	else if (nNumReg == 2)
	{
		pOutBuffer[0] = mat._11; pOutBuffer[1] = mat._21; pOutBuffer[2] = mat._31; pOutBuffer[3] = mat._41; 
		pOutBuffer[4] = mat._12; pOutBuffer[5] = mat._22; pOutBuffer[6] = mat._32; pOutBuffer[7] = mat._42; 
	}
	else if (nNumReg == 3)
	{
		pOutBuffer[0] = mat._11; pOutBuffer[1] = mat._21; pOutBuffer[2] = mat._31; pOutBuffer[3] = mat._41;
		pOutBuffer[4] = mat._12; pOutBuffer[5] = mat._22; pOutBuffer[6] = mat._32; pOutBuffer[7] = mat._42;  
		pOutBuffer[8] = mat._13; pOutBuffer[9] = mat._23; pOutBuffer[10] = mat._33; pOutBuffer[11] = mat._43;  
	}
	else if (nNumReg == 4)
	{
		pOutBuffer[0] = mat._11; pOutBuffer[1] = mat._21; pOutBuffer[2] = mat._31; pOutBuffer[3] = mat._41; 
		pOutBuffer[4] = mat._12; pOutBuffer[5] = mat._22; pOutBuffer[6] = mat._32; pOutBuffer[7] = mat._42;  
		pOutBuffer[8] = mat._13; pOutBuffer[9] = mat._23; pOutBuffer[10] = mat._33; pOutBuffer[11] = mat._43;
		pOutBuffer[12] = mat._14; pOutBuffer[13] = mat._24; pOutBuffer[14] = mat._34; pOutBuffer[15] = mat._44;
	}
}

/*
//-------------------------------------------------------------------
Quad::Quad(float width, float height, const Vector3f& pos, bool bUseRotation):
m_vPos(pos),m_fHalfWidth(width/2.0f),m_fHalfHeight(height/2.0f),
m_fRotation(0.0f),m_bUseRotation(bUseRotation)
{
	ushort index[] = {0, 1, 2, 0, 2, 3};
	m_aIndex.assign(index, index+6);

	VertexPT vert[] = { {Vector3f(-0.5f,  0.5f, 0.0f), Vector2f(0.0f, 0.0f)},
						{Vector3f(-0.5f, -0.5f, 0.0f), Vector2f(0.0f, 1.0f)},
						{Vector3f( 0.5f, -0.5f, 0.0f), Vector2f(1.0f, 1.0f)},
						{Vector3f( 0.5f,  0.5f, 0.0f), Vector2f(1.0f, 0.0f)} };
	m_aVertex.assign(vert, vert+4);
}
//-------------------------------------------------------------------------
void Quad::UpdateOrientation(const Camera* pCamera)
{
	Matrix4f matView = pCamera->GetViewMatrix();
	Vector3f r	= Vector3f(matView._11, matView._21, matView._31);
	Vector3f u	= Vector3f(matView._12, matView._22, matView._32);
	Vector3f vRight, vUp;

	Vector3f z = pCamera->GetEyePos() - m_vPos;
	z.Normalize();
	vRight = Vector3f::Cross(u, z);
	if (vRight.LengthSQ() > 0.05f)
	{
		vRight.Normalize();
		vUp = Vector3f::Cross(z, vRight);
	}
	// the billboard is very close to the top or bottom of the camera
	else
	{
		vUp = Vector3f::Cross(z, u);
		vUp.Normalize();
		vRight = Vector3f::Cross(vUp, z);
	}

	if (m_bUseRotation)
	{
		float c = Mathf::Cos(m_fRotation);
		float s = Mathf::Sin(m_fRotation);

		// avoid c++ implicit construct temp object for performance reason
		// (w*cos)*r + (w*sin)*u
		r.Set(c*vRight.x + s*vUp.x,		c*vRight.y + s*vUp.y,	c*vRight.z + s*vUp.z);
		// (-h*sin)*r + (h*cos)*u
		u.Set(-s*vRight.x + c*vUp.x,	-s*vRight.y + c*vUp.y,	-s*vRight.z + c*vUp.z);
	}
	else
	{
		r = vRight;
		u = vUp;
	}

	r *= m_fHalfWidth;
	u *= m_fHalfHeight;


	m_aVertex[0].pos = m_vPos;
	m_aVertex[0].pos -= r;
	m_aVertex[0].pos += u;

	m_aVertex[1].pos = m_vPos;
	m_aVertex[1].pos -= r;
	m_aVertex[1].pos -= u;

	m_aVertex[2].pos = m_vPos;
	m_aVertex[2].pos += r;
	m_aVertex[2].pos -= u;

	m_aVertex[3].pos = m_vPos;
	m_aVertex[3].pos += r;
	m_aVertex[3].pos += u;
}

//--------------------------------------------------------------------------
Beam::Beam(float fWidth):m_fHalfWidth(fWidth/2.0f)
{
	ushort index[] = {0, 1, 2, 0, 2, 3};
	m_aIndex.assign(index, index+6);

	VertexPT vert[] = { {Vector3f(-0.5f,  0.5f, 0.0f), Vector2f(0.0f, 0.0f)},
						{Vector3f(-0.5f, -0.5f, 0.0f), Vector2f(0.0f, 1.0f)},
						{Vector3f( 0.5f, -0.5f, 0.0f), Vector2f(1.0f, 1.0f)},
						{Vector3f( 0.5f,  0.5f, 0.0f), Vector2f(1.0f, 0.0f)} };
	m_aVertex.assign(vert, vert+4);
}
//---------------------------------------------------------------------------
void Beam::UpdateOrientation(const Camera* pCamera)
{
	Vector3f	dir = m_p1 - m_p0;
	Vector3f	l	= pCamera->GetEyePos() - m_p0;
	Vector3f	u	= Vector3f::Cross(l, dir);
	if (u.LengthSQ() < 0.05f)
	{
		Matrix4f mat = pCamera->GetViewMatrix();
		u.Set(mat._12, mat._22, mat._32);
	}
	else
		u.Normalize();
	u *= m_fHalfWidth;

	m_aVertex[0].pos = m_p0;
	m_aVertex[0].pos += u;

	m_aVertex[1].pos = m_p0;
	m_aVertex[1].pos -= u;

	m_aVertex[2].pos = m_p1;
	m_aVertex[2].pos -= u;

	m_aVertex[3].pos = m_p1;
	m_aVertex[3].pos += u;
}


//------------------------------------------------------------------------------
*/


}