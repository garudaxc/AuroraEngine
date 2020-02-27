#include "stdheader.h"
#include "Primitive.h"
#include "GaMath.h"
#include "GaMathFunction.h"
#include "GaGeometryData.h"
#include "tinyxml/include/tinyxml.h"
#include <sstream>
#include <xmmintrin.h>

using namespace Ga;



Ray::Ray(const Vector3f& o, const Vector3f& d):origin(o),dir(d),tmin(0.001f),tmax(10000.f)
{
}


Ga::Vector3f Ray::PosFromT(float t) const
{
	return Vector3f(origin.x + t * dir.x, origin.y + t * dir.y, origin.z + t * dir.z);
}



bool Sphere::TestIntersect(Intersection& intersection, const Ray& ray) const
{
	float a, b, c, discrm;
	Vector3f pMinusC = ray.origin - m_vCenter;
	a = DotProduct(ray.dir, ray.dir);
	b = 2 * DotProduct(ray.dir, pMinusC);
	c = DotProduct(pMinusC, pMinusC) - m_fRadius * m_fRadius;
	discrm = b * b - 4 * a * c;
	if (discrm > 0) {
		float t = (-b - sqrt(discrm)) / (2 * a);
		intersection.pos = ray.origin + ray.dir * t;

//		intersection.normal = intersection.pos - m_vCenter;
//		intersection.normal.Normalize();

		return true;

		//t[0] = (-b + sqrt(discrm)) / (2 * a);
		//t[1] = (-b - sqrt(discrm)) / (2 * a);
	}
	else if (discrm == 0)
	{
		// The line is tangent to the sphere
		//t[0] = -b / (2 * a);
		float t =  -b / (2 * a);
		intersection.pos = ray.origin + ray.dir * t;

//		intersection.normal = intersection.pos - m_vCenter;
//		intersection.normal.Normalize();
		return true;

		return 1;
	} 
	else 
	{
		return false;
	}
}


bool TestAABB(const AABB& aabb, const Ray& ray, float& tmin, float& tmax)
{
	float t0 = ray.tmin, t1 = ray.tmax;
	for (int i = 0; i < 3; i++)
	{
		float invDir = 1.f / ray.dir[i];
		float tNear = (aabb.vMin[i] - ray.origin[i]) * invDir;
		float tFar	= (aabb.vMax[i] - ray.origin[i]) * invDir;

		if (tNear > tFar)
		{
			float tmp = tNear;
			tNear = tFar;
			tFar = tmp;
		}

		t0 = (tNear > t0) ? tNear : t0;
		t1 = (tFar < t1) ? tFar : t1;
		if (t0 > t1)
		{
			return false;
		}
	}

	tmin = t0;
	tmax = t1;
	return true;
}



bool RayAABBTest(const AABB& aabb, const Ray& ray)
{
	float maxS = -Mathf::MAX_VALUE;
	float minT = ray.tmax;

	const Vector3f& rayDir = ray.dir;
	const Vector3f& rayOrig = ray.origin;
	const Vector3f& vmin = aabb.vMin;
	const Vector3f& vmax = aabb.vMax;

	// ray is parallel to plane
	for (int i = 0; i < 3; i++)
	{
		if (Mathf::IsZero(rayDir[i]))
		{
			// ray passes by box
			if (rayOrig[i] < vmin[i] || rayOrig[i] > vmax[i])
				return false;
		}
		else
		{
			// compute intersection parameters and sort
			float s = (vmin[i] - rayOrig[i]) / rayDir[i];
			float t = (vmax[i] - rayOrig[i]) / rayDir[i];
			if (s > t)
			{
				float temp = s;
				s = t;
				t = temp;
			}
			// adjust min and max values
			if ( s > maxS )
				maxS = s;
			if ( t < minT )
				minT = t;
			// check for intersection failure
			if (minT < 0.0f || maxS > minT)
				return false;
		}
	}

	// done, have intersection
	return true;
}



#if 0

void CalcIVM(Triangle& triangle)
{
	Matrix3f matIvm;
	matIvm._11 = triangle.p0->pos.x;
	matIvm._21 = triangle.p0->pos.y;
	matIvm._31 = triangle.p0->pos.z;


	matIvm._12 = triangle.p1->pos.x;
	matIvm._22 = triangle.p1->pos.y;
	matIvm._32 = triangle.p1->pos.z;

	matIvm._13 = triangle.p2->pos.x;
	matIvm._23 = triangle.p2->pos.y;
	matIvm._33 = triangle.p2->pos.z;

	Matrix3f matOut;
	MatrixInverse(matOut, matIvm);

	triangle.ivm0.Set(matOut._11, matOut._12, matOut._13);
	triangle.ivm1.Set(matOut._21, matOut._22, matOut._23);
	triangle.ivm2.Set(matOut._31, matOut._32, matOut._33);

}
#endif


bool TestTriangle(Intersection& intersection, const Triangle* pTri, const Ray& ray)
{
	// test ray direction against triangle
	Vector3f e1 = pTri->p1->pos - pTri->p0->pos;
	Vector3f e2 = pTri->p2->pos - pTri->p0->pos;

	// one side triangle, ray must oper with triangle normal
	if (!intersection.towSide)
	{
		Vector3f n = CrossProduct(e1, e2);
		float d = DotProduct(ray.dir, n);
		if (intersection.backFace)
		{
			if (d < 0.0f)
			{
				return false;
			}
		}
		else
		{
			if (d > 0.0f)
			{
				return false;
			}
		}
	}

	Vector3f p = CrossProduct(ray.dir, e2);
	float a = DotProduct(e1, p);
	// if result zero, no intersection or infinite intersections
	// (ray parallel to triangle plane)
	if ( Mathf::IsZero(a) )
		return false;

	// compute denominator
	float f = 1.0f / a;	

	// compute barycentric coordinates
	Vector3f s = ray.origin - pTri->p0->pos;
	float u = f * DotProduct(s, p);
	if (u < 0.0f || u > 1.0f)
	{
		return false;
	}

	Vector3f q = CrossProduct(s, e1);
	float v = f * DotProduct(ray.dir, q);
	if (v < 0.0f || u + v > 1.0f)
	{
		return false;
	}

	// compute line parameter
	float t = f * DotProduct(e2, q);

	if (t > ray.tmax || t < ray.tmin)
	{
		return false;
	}

	ray.tmax = t;

	intersection.pTri = pTri;
	intersection.u = u;
	intersection.v = v;
	intersection.pos = ray.origin + ray.dir * t;

	return true;
}

bool RayTriangleIntersect(Intersection& intersection, const Triangle* pTri, const Ray& ray, float& tRay)
{
	// test ray direction against triangle
	Vector3f e1 = pTri->p1->pos - pTri->p0->pos;
	Vector3f e2 = pTri->p2->pos - pTri->p0->pos;

	// one side triangle, ray must oper with triangle normal
	if (!intersection.towSide)
	{
		Vector3f n = CrossProduct(e1, e2);
		float d = DotProduct(ray.dir, n);
		if (intersection.backFace)
		{
			if (d < 0.0f)
			{
				return false;
			}
		}
		else
		{
			if (d > 0.0f)
			{
				return false;
			}
		}
	}

	Vector3f p = CrossProduct(ray.dir, e2);
	float a = DotProduct(e1, p);
	// if result zero, no intersection or infinite intersections
	// (ray parallel to triangle plane)
	if ( Mathf::IsZero(a) )
		return false;

	// compute denominator
	float f = 1.0f / a;	

	// compute barycentric coordinates
	Vector3f s = ray.origin - pTri->p0->pos;
	float u = f * DotProduct(s, p);
	if (u < 0.0f || u > 1.0f)
	{
		return false;
	}

	Vector3f q = CrossProduct(s, e1);
	float v = f * DotProduct(ray.dir, q);
	if (v < 0.0f || u + v > 1.0f)
	{
		return false;
	}

	// compute line parameter
	float t = f * DotProduct(e2, q);

	if (t > ray.tmax || t < ray.tmin)
	{
		return false;
	}

	tRay = t;

	intersection.pTri = pTri;
	intersection.u = u;
	intersection.v = v;
	intersection.pos = ray.origin + ray.dir * t;

	return true;
}


void CalcDifferentialGeometry(const Intersection& inter, DifferentialGeometry& dg)
{
	dg.pTri = inter.pTri;
	dg.pos = inter.pos;


	float w = 1.0f - inter.u - inter.v;

	const Vertex *p0 = dg.pTri->p0, *p1 = dg.pTri->p1, *p2 = dg.pTri->p2;

	dg.normal = p0->normal * w + p1->normal * inter.u + p2->normal * inter.v;
	dg.normal.Normalize();
	dg.tangent = p0->tangent * w + p1->tangent * inter.u + p2->tangent * inter.v;
	dg.tangent.Normalize();
	dg.binormal = p0->binormal * w + p1->binormal * inter.u + p2->binormal * inter.v;
	dg.binormal.Normalize();

	//intersection.color = pTri->p0->color * (1.0f - u - v) + pTri->p1->color * u + pTri->p2->color * v;

}


#if 0

bool RayTriangleIntersect(Intersection& intersection, float& tt,
						  const Triangle& triangle,
						  const Vector3f& rayOrigin, const Vector3f& rayDir)
{
	Vector3f qPrime = triangle.ivm0;
	qPrime += triangle.ivm1;
	qPrime += triangle.ivm2;

	Vector3f q = qPrime / DotProduct(qPrime, qPrime);

	float tStar = (DotProduct(q, q) - DotProduct(rayOrigin, q)) / DotProduct(rayDir, q);
	if (tStar > 0.001f && tStar < tt)
	{
		Vector3f p = rayOrigin + rayDir * tStar;
		float p0 = DotProduct(p, triangle.ivm0);

		if (p0 < 0.0f)
		{
			return false;
		}

		float p1 = DotProduct(p, triangle.ivm1);
		if (p1 < 0.0f)
		{
			return false;
		}


		float p2 = DotProduct(p, triangle.ivm2);
		if (p2 < 0.0f)
		{
			return false;
		}

		tt = tStar;
		intersection.pos = p;
		intersection.normal = triangle.p0->normal * p0 + 
			triangle.p1->normal * p1 + 
			triangle.p2->normal * p2;

		intersection.normal.Normalize();

		return true;
	}


	return false;
}
float DotProduct3(const __m128& a, const __m128& b)
{
	__m128 d = _mm_mul_ps(a, b);
	return d.m128_f32[3] + d.m128_f32[2] + d.m128_f32[1];
}


float DotProduct4(const __m128& a, const __m128& b)
{
	__m128 d = _mm_mul_ps(a, b);
	return d.m128_f32[3] + d.m128_f32[2] + d.m128_f32[1] + d.m128_f32[0];
}

bool RayTriangleIntersectSSE(Intersection& intersection, float& tt,
							 const Triangle& triangle,
							 const Vector3f& rayOrigin, const Vector3f& rayDir)
{

	__m128 ivm0 = _mm_set_ps(triangle.ivm0.x, triangle.ivm0.y, triangle.ivm0.z, 0.0f);
	__m128 ivm1 = _mm_set_ps(triangle.ivm1.x, triangle.ivm1.y, triangle.ivm1.z, 0.0f);
	__m128 ivm2 = _mm_set_ps(triangle.ivm2.x, triangle.ivm2.y, triangle.ivm2.z, 0.0f);

	__m128 qPrime = _mm_add_ps(ivm0, ivm1);
	qPrime = _mm_add_ps(qPrime, ivm2);

	float d = DotProduct3(qPrime, qPrime);
	__m128 q = _mm_div_ps(qPrime, _mm_set_ps(d, d, d, d));


	__m128 ro = _mm_set_ps(rayOrigin.x, rayOrigin.y, rayOrigin.z, 0.0f);
	__m128 rd = _mm_set_ps(rayDir.x, rayDir.y, rayDir.z, 0.0f);
	float tStar = (DotProduct3(q, q) - DotProduct3(ro, q)) / DotProduct3(rd, q);

	if (tStar > 0.001f && tStar < tt)
	{
		__m128 p = _mm_mul_ps(rd, _mm_set_ps(tStar, tStar, tStar, 0.0f));
		p = _mm_add_ps(p, ro);

		float p0 = DotProduct3(p, ivm0);

		if (p0 < 0.0f)
		{
			return false;
		}

		float p1 = DotProduct3(p, ivm1);
		if (p1 < 0.0f)
		{
			return false;
		}


		float p2 = DotProduct3(p, ivm2);
		if (p2 < 0.0f)
		{
			return false;
		}

		tt = tStar;
		intersection.pos = Vector3f(p.m128_f32[3], p.m128_f32[2], p.m128_f32[1]);
		intersection.normal = triangle.p0->normal * p0 + 
			triangle.p1->normal * p1 + 
			triangle.p2->normal * p2;

		intersection.normal.Normalize();

		return true;
	}


	return false;
}


#endif



//
//
//// for test
//bool TestIntersect(Intersection& intersection, const Ray& ray)
//{
//
//	Vector3f p0(15.337, 0.259, 24.161);
//	Vector3f p1(-14.438,0.133, -0.019);
//	Vector3f p2(15.337, 0.133, -0.019);
//
//	Vector3f normal(0.0f, -1.0f, 0.0f);
//
//
//	Vertex v0, v1, v2;
//	v0.pos = p0;
//	v0.normal = normal;
//	v1.pos = p1;
//	v1.normal = normal;
//	v2.pos = p2;
//	v2.normal = normal;
//
//
//
//	Triangle triangle;
//	triangle.p0 = &v0;
//	triangle.p1 = &v1;
//	triangle.p2 = &v2;
//
//	CalcIVM(triangle);
//
//
//
//	bool bIntersection = false;
//	float t = 10000.0f;
//	Intersection inter;
//	//RayTriangleIntersect(inter, t,
//	//triangle.p0->pos, triangle.p1->pos, triangle.p2->pos,
//	//triangle.p0->normal, triangle.p1->normal, triangle.p2->normal,
//	//ray.origin, ray.dir);
//
//	RayTriangleIntersect(inter, t, triangle, ray.origin, ray.dir);
//
//	if (t < 10000.0f)
//	{
//		intersection = inter;
//		return true;
//	}
//	else
//	{
//		return false;
//	}
//
//	//return bIntersection;
//
//}
//
//




void Mesh::Load(TiXmlElement* pMesh)
{
	int nNumVerts = 0, nNumTri = 0;
	pMesh->Attribute("numVerts", nNumVerts);
	pMesh->Attribute("numTri", nNumTri);


	m_VertiesWorld.resize(nNumVerts);
	m_VertiesWorld.resize(nNumVerts);
	m_Triagnles.resize(nNumTri);


	string text = pMesh->FirstChild("Index")->FirstChild()->ToText()->Value();
	stringstream ss;
	ss.write(text.c_str(), text.size());
	uint n;
	for (int i = 0; i < nNumTri; i++)
	{
		char t;
		int p0, p1, p2;
		ss >> n >> p0 >> p1 >> p2 >> t;
		m_Triagnles[i].p0 = &m_VertiesWorld[p0];
		m_Triagnles[i].p1 = &m_VertiesWorld[p1];
		m_Triagnles[i].p2 = &m_VertiesWorld[p2];
		m_Triagnles[i].brdf = &m_brdf;
	}
	ss.clear();


	Vertex* pVertex = &m_VertiesWorld[0];

	text = pMesh->FirstChild("Position")->FirstChild()->ToText()->Value();
	ss.write(text.c_str(), text.size());
	for (int i = 0; i < nNumVerts; i++)
	{
		char t;
		ss >> n >> pVertex->pos.x >> pVertex->pos.y >> pVertex->pos.z >> t;
		pVertex++;
	}
	ss.clear();

	pVertex = &m_VertiesWorld[0];
	text = pMesh->FirstChild("Normal")->FirstChild()->ToText()->Value();
	ss.write(text.c_str(), text.size());
	for (int i = 0; i < nNumVerts; i++)
	{
		char t;
		ss >> n >> pVertex->normal.x >> pVertex->normal.y >> pVertex->normal.z >> t;
		pVertex++;
	}
	ss.clear();


	pVertex = &m_VertiesWorld[0];
	text = pMesh->FirstChild("Texcoord")->FirstChild()->ToText()->Value();
	ss.write(text.c_str(), text.size());
	for (int i = 0; i < nNumVerts; i++)
	{
		char t;
		ss >> n >> pVertex->uv.x >>pVertex->uv.y >> t;
		pVertex++;
	}
	ss.clear();

	// calc triangle area
	for (int i = 0; i < nNumTri; i++)
	{
		m_Triagnles[i].area = Ga::TriangleArea(m_Triagnles[i].p0->pos, m_Triagnles[i].p1->pos, m_Triagnles[i].p2->pos);
	}
}


void CalculateTangentArray(vector<Vertex>& vertiesObj, Triangle *triangle, int triangleCount)
{
	for (vector<Vertex>::iterator it = vertiesObj.begin(); it != vertiesObj.end(); ++it)
	{
		it->tangent = Vector3f::ZERO;
		it->binormal = Vector3f::ZERO;
	}

	for (long i = 0; i < triangleCount; i++)
	{
		const Vector3f& v1 = triangle[i].p0->pos;
		const Vector3f& v2 = triangle[i].p1->pos;
		const Vector3f& v3 = triangle[i].p2->pos;

		const Vector2f& w1 = triangle[i].p0->uv;
		const Vector2f& w2 = triangle[i].p1->uv;
		const Vector2f& w3 = triangle[i].p2->uv;

		float x1 = v2.x - v1.x;
		float x2 = v3.x - v1.x;
		float y1 = v2.y - v1.y;
		float y2 = v3.y - v1.y;
		float z1 = v2.z - v1.z;
		float z2 = v3.z - v1.z;

		float s1 = w2.x - w1.x;
		float s2 = w3.x - w1.x;
		float t1 = w2.y - w1.y;
		float t2 = w3.y - w1.y;

		float r = 1.0f / (s1 * t2 - s2 * t1);
		Vector3f sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
			(t2 * z1 - t1 * z2) * r);
		Vector3f tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
			(s1 * z2 - s2 * z1) * r);

		triangle[i].p0->tangent += sdir;
		triangle[i].p1->tangent += sdir;
		triangle[i].p2->tangent += sdir;

		triangle[i].p0->binormal += tdir;
		triangle[i].p1->binormal += tdir;
		triangle[i].p2->binormal += tdir;
	}


	for (vector<Vertex>::iterator it = vertiesObj.begin(); it != vertiesObj.end(); ++it)
	{
		const Vector3f& n = it->normal;
		const Vector3f& t = it->tangent;

		// Gram-Schmidt orthogonalize
		Vector3f tmp = t - n * DotProduct(n, t);
		tmp.Normalize();
		it->tangent.x = tmp.x;
		it->tangent.y = tmp.y;
		it->tangent.z = tmp.z;

		Vector3f bi = CrossProduct(n, t);

		it->binormal = bi * ((DotProduct(bi, it->binormal) < 0.0f) ? -1.0f : 1.0f);
	}
}



void Mesh::Transform(const Matrix4f& matWorld)
{
	m_VertiesWorld = m_VertiesWorld;

	CalculateTangentArray(m_VertiesWorld, &m_Triagnles[0], m_Triagnles.size());

	Vector3TransformArray(&m_VertiesWorld[0].pos, sizeof(Vertex), &m_VertiesWorld[0].pos, sizeof(Vertex), m_VertiesWorld.size(), matWorld);
	
	Matrix4f matRot = matWorld;
	matRot._41 = 0.0f;
	matRot._42 = 0.0f;
	matRot._43 = 0.0f;
	Vector3TransformArray(&m_VertiesWorld[0].normal, sizeof(Vertex), &m_VertiesWorld[0].normal, sizeof(Vertex), m_VertiesWorld.size(), matRot);
	Vector3TransformArray(&m_VertiesWorld[0].tangent, sizeof(Vertex), &m_VertiesWorld[0].tangent, sizeof(Vertex), m_VertiesWorld.size(), matRot);
	Vector3TransformArray(&m_VertiesWorld[0].binormal, sizeof(Vertex), &m_VertiesWorld[0].binormal, sizeof(Vertex), m_VertiesWorld.size(), matRot);

#if 0
	for (int i = 0; i < m_Triagnles.size(); i++)
	{
		CalcIVM(m_Triagnles[i]);
	}

#endif

	m_aabb.Reset();
	for (int i = 0; i < m_VertiesWorld.size(); i++)
	{
		m_aabb.Merge(m_VertiesWorld[i].pos);
	}
}

int Mesh::GetNumTriangle()		const
{
	return m_Triagnles.size();
}

const Triangle* Mesh::GetTriangle(int i) const
{
	return &m_Triagnles[i];
}

int Mesh::GetNumVertex() const
{
	return m_VertiesWorld.size();
}

Vertex* Mesh::GetVertex(int i)
{
	return &m_VertiesWorld[i];
}


bool Mesh::TestIntersect(Intersection& intersection, const Ray& ray) const
{
	if (!RayAABBTest(m_aabb, ray))
	{
		return false;
	}

	int nTri = GetNumTriangle();

	bool bIntersect = false;
	for (int i = 0; i < nTri; i++)
	{
		const Triangle* pTri = GetTriangle(i);

		float tRay = 10000.f;
		if (RayTriangleIntersect(intersection, pTri, ray, tRay))
		{
			ray.tmax = tRay;
			bIntersect = true;
		}

		//RayTriangleIntersect(inter, t, *pTri, ray.origin, ray.dir);
	}

	return bIntersect;
}