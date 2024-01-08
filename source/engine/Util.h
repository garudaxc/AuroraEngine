#ifndef GAUTIL_H
#define GAUTIL_H
#include "stdheader.h"
#include "Types.h"

namespace Aurora
{

	enum EEncoding : int8
	{
		Encoding_ANSI,
		Encoding_Unicode,
		Encoding_BigEndianUnicode,
		Encoding_UTF_8,
	};

	class Util
	{
	public:

		static void			MemMap(const void* pSrc, uint srcStride, void* pDes, 
			uint desStride, uint byteToMap, uint count);

		static  void		MemCopy(const void *Source, void* Destination, int Size);

		static uint			MakeFOURCC(const char* pStr);

		static std::string	GetFileNameExt(const std::string& fileName);
		static string		GetPath(const string& pathName);

		static string		ProcessPathName(const string& pathName);

		static String& StringToLower(String& InString);
		static String& StringToUpper(String& InString);

		static int SplitString(const String& InString, char InSplitChar, Array<String>& OutResult);

		static EEncoding	GetEncoding(const int8* str);
	};




void
ComputeBoundingSphere(void* pVertex, int numVertices, int stride, Vector3f& center, float& radius);
//---------------------------------------------------------------------------
// compute normal of triangle
//Vector3f ComputeNormal(const Vector3f& v1, const Vector3f& v2, const Vector3f& v3);
//---------------------------------------------------------------------------
// compute weighted normal of triangle
//Vector3f ComputeWeightedNormal(const Vector3f& v1, const Vector3f& v2, const Vector3f& v3);
//----------------------------------------------------------------------------
// compute plane from triangle
void ComputePlane(const Vector3f& v1, const Vector3f& v2, const Vector3f& v3,
				  Vector3f& normal, float& fD);
//----------------------------------------------------------------------------
// compute area of triangle
float ComputeArea(const Vector3f& v1, const Vector3f& v2, const Vector3f& v3);
//----------------------------------------------------------------------------
// test intersection between ray and triangle
bool IntersectTriangle(const Vector3f& origin, const Vector3f& dir, 
				  const Vector3f& v0, const Vector3f& v1, const Vector3f& v2,
				  float& fu, float& fv, float& fDist);
//----------------------------------------------------------------------------
Vector3f BarycentricToPos(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2,
						float fu, float fv);
//-------------------------------------------------------------------------------------
void MemMap(const void* pSrc, uint srcStride, void* pDes, uint desStride, uint byteToMap, uint count);

void MapMatrixToShaderConst(const Matrix4f& mat, float* pOutBuffer, uint nNumReg);


/*

//-----------------------------------------------------------------------------------------
class SurfaceData_32Bit
{
public:
	static uint GetData(void* pBits, int nPitch, int x, int y)
	{
		return (*(uint*)(((ubyte*)pBits) + x * sizeof(uint) + y * nPitch));
	}

	static void SetData(void* pBits, int nPitch, int x, int y, uint nData)
	{
		*(uint*)(((ubyte*)pBits) + x * sizeof(uint) + y * nPitch) = nData;
	}
};

class SurfaceData_16Bit
{
public:
	static uint GetData(void* pBits, int nPitch, int x, int y)
	{
		return (uint)(*(ushort*)(((ubyte*)pBits) + x * sizeof(ushort) + y * nPitch));
	}

	static void SetData(void* pBits, int nPitch, int x, int y, uint nData)
	{
		*(ushort*)(((ubyte*)pBits) + x * sizeof(ushort) + y * nPitch) = (ushort)nData;
	}
};

class SurfaceData_8Bit
{
public:
	static uint GetData(void* pBits, int nPitch, int x, int y)
	{
		return (uint)(*((ubyte*)pBits + x + y * nPitch));
	}

	static void SetData(void* pBits, int nPitch, int x, int y, uint nData)
	{
		*((ubyte*)pBits + x + y * nPitch) = (ubyte)nData;
	}
};

class SurfaceData_X8R8G8B8 : public SurfaceData_32Bit
{
public:
};

class SurfaceData_A8R8G8B8 : public SurfaceData_32Bit
{
public:
	static void GetColor(void* pBits, int nPitch, int x, int y, float& r, float& g, float& b, float& a)
	{
		uint value = GetData(pBits, nPitch, x, y);
		b = (float)(value & 0xff) / 255.0f;
		g = (float)((value >> 8) & 0xff) / 255.0f;
		r = (float)((value >> 16) & 0xff) / 255.0f;
		a = (float)((value >> 24) & 0xff) / 255.0f;
	}

	static void SetColor(void* pBits, int nPitch, int x, int y, float r, float g, float b, float a)
	{
		ubyte alpha	= (ubyte)(a * 255.0f);
		ubyte red	= (ubyte)(r * 255.0f);
		ubyte green	= (ubyte)(g * 255.0f);
		ubyte blue	= (ubyte)(b * 255.0f);
		uint  value = (uint)((uint)alpha << 24 | (uint)red << 16 | (uint)green << 8 | blue);
		SetData(pBits, nPitch, x, y, value);
	}

	static void GetVector(void* pBits, int nPitch, int x, int y, Vector3f& vec)
	{
		float a;
		GetColor(pBits, nPitch, x, y, vec.x, vec.y, vec.z, a);
		vec *= 2.0f;
		vec.x -= 1.0f;
		vec.y -= 1.0f;
		vec.z -= 1.0f;
	}

	static void SetVector(void* pBits, int nPitch, int x, int y, const Vector3f& vec)
	{
		Vector3f v = vec + Vector3f(1.0f, 1.0f, 1.0f);
		v *= 127.5f;
		ubyte r = (ubyte)v.x;
		ubyte g = (ubyte)v.y;
		ubyte b = (ubyte)v.z;
		uint  value = (uint)(0xff000000 | (uint)r << 16 | (uint)g << 8 | b);
		SetData(pBits, nPitch, x, y, value);

	}
};

class SurfaceData_L8 : public SurfaceData_8Bit
{
public:
	static float GetLuminance(void* pBits, int nPitch, int x, int y)
	{
		return (float)GetData(pBits, nPitch, x, y) / 255.0f;
	}
};


inline 	void WrapTextureAddress(int& xWarp, int& yWarp, int x, int y, int nWidth, int nHeight)
{
	if (x >= 0)
	{
		xWarp = (x % nWidth);
	}
	else
	{
		xWarp = (x + nWidth) % nWidth;
	}

	if (y >= 0)
	{
		yWarp = (y % nHeight);
	}
	else
	{
		yWarp = (y + nHeight) % nHeight;
	}

}


template<typename HeightMap, typename NormalMap>
void ComputeNormalMap(void* pHeightBits, int nHeightPitch, void* pNormalMap, int nNormalPitch,
					  int nWidth, int nHeight, float fAmplitude)
{
	float fLeft, fRight, fTop, fBottom;
	int   xSample, ySample;
	for (int y = 0; y < nHeight; y++)
	{
		for (int x = 0; x < nWidth; x++)
		{
			WrapTextureAddress(xSample, ySample, x-1, y, nWidth, nHeight);
			fLeft	= HeightMap::GetLuminance(pHeightBits, nHeightPitch, xSample, ySample);

			WrapTextureAddress(xSample, ySample, x+1, y, nWidth, nHeight);
			fRight	= HeightMap::GetLuminance(pHeightBits, nHeightPitch, xSample, ySample);

			WrapTextureAddress(xSample, ySample, x, y-1, nWidth, nHeight);
			fTop	= HeightMap::GetLuminance(pHeightBits, nHeightPitch, xSample, ySample);

			WrapTextureAddress(xSample, ySample, x, y+1, nWidth, nHeight);
			fBottom	= HeightMap::GetLuminance(pHeightBits, nHeightPitch, xSample, ySample);

			// the u and v vector align to texture's uv coordinate
			Vector3f u(2.0f, 0.0f, (fRight - fLeft) * fAmplitude);
			Vector3f v(0.0f, 2.0f, (fBottom - fTop) * fAmplitude);
			Vector3f n = CrossProduct(u, v);
			n.Normalize();

			NormalMap::SetVector(pNormalMap, nNormalPitch, x, y, n);
		}
	}

}
//---------------------------------------------------------------------------------



//-----------------------------------------------------------------
class Camera;
class Billboard
{
public:
	static	uint	GetFVF()				{	return VertexPT::FVF;			}
	uint			GetNumVertex()	const	{	return (uint)m_aVertex.size();	}
	uint			GetNumTri()		const	{	return (uint)m_aIndex.size()/3;	}

	virtual void	UpdateOrientation(const Camera* pCamera) = 0;



//protected:
	Billboard()	{}
	std::vector<VertexPT>		m_aVertex;
	std::vector<ushort>			m_aIndex;
};


class Quad : public Billboard
{
public:
	Quad(float width = 1.0f, float height = 1.0f, const Vector3f& pos = Vector3f::ZERO, bool bUseRotation = false);

	void			SetPosition(const Vector3f& pos)	{	m_vPos = pos;				}
	void			SetSize(float width, float height)	{	m_fHalfWidth = width/2.0f; m_fHalfHeight = height/2.0f;	}
	void			SetRotation(float fRotation)		{	m_fRotation = fRotation;	}

	virtual void	UpdateOrientation(const Camera* pCamera);

//protected:
	Vector3f		m_vPos;
	float			m_fHalfWidth, m_fHalfHeight;
	float			m_fRotation;
	bool			m_bUseRotation;

};

class Beam : public Billboard
{
public:
	Beam(float fWidth = 1.0f);

	void SetWidth(float fWidth)		{	m_fHalfWidth = fWidth / 2.0f;	}
	void SetPosition(const Vector3f& p0, const Vector3f& p1)
	{	m_p0 = p0;	m_p1 = p1;	}

	virtual void	UpdateOrientation(const Camera* pCamera);




protected:
	Vector3f		m_p0, m_p1;
	float			m_fHalfWidth;
};
*/


}// namespace

#endif