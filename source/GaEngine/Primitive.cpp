#include "stdheader.h"
#include "Primitive.h"

namespace Aurora
{

	Plane::Plane(const Vector3f& norm, float dist):n(norm),d(dist)
	{
	}

	Plane::Plane(const Vector3f& norm, const Vector3f& point):n(norm)
	{
		d = -Vector3f::Dot(norm, point);
	}

	Plane::Plane(const Vector3f& a, const Vector3f& b, const Vector3f& c)
	{
		Vector3f ab = b - a;
		Vector3f ac = c - a;
		n = Vector3f::Cross(ab, ac);
		n.Normalize();
		d = -Vector3f::Dot(n, a);
	}

	Plane::Plane(const Vector4f& v):n(v.x, v.y, v.z),d(v.w)
	{

	}

	const Plane& Plane::operator = (const Vector4f& v)
	{
		n.Set(v.x, v.y, v.z);
		d = v.w;
		return *this;
	}

	void Plane::Normalize()
	{
		float invLen = 1.0f / n.Length();
		n *= invLen;
		d *= invLen;
	}

	float Distance(const Plane& plane, const Vector3f& point)
	{
		return Vector3f::Dot(plane.n, point) + plane.d;
	}



	Ray::Ray()
	{
	}

	Ray::Ray(const Ray& r):origin(r.origin),dir(r.dir),
		tmin(r.tmin),tmax(r.tmax)
	{
	}

	Ray::Ray(const Vector3f& o, const Vector3f& d):
	origin(o),dir(d),tmin(0.001f),tmax(10000.f)
	{
	}


	Vector3f Ray::PosFromT(float t) const
	{
		return Vector3f(origin.x + t * dir.x, origin.y + t * dir.y, origin.z + t * dir.z);
	}


	bool Sphere::TestIntersect(Intersection& intersection, const Ray& ray) const
	{
		float a, b, c, discrm;
		Vector3f pMinusC = ray.origin - m_vCenter;
		a = Vector3f::Dot(ray.dir, ray.dir);
		b = 2 * Vector3f::Dot(ray.dir, pMinusC);
		c = Vector3f::Dot(pMinusC, pMinusC) - m_fRadius * m_fRadius;
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


	bool TestRayAABB(const AABB& aabb, const Ray& ray, float& tmin, float& tmax)
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

	bool TestRayAABB(const AABB& aabb, const Ray& ray)
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


	bool TestRayTriangle(Intersection& intersection, const Triangle* pTri, const Ray& ray)
	{
		// test ray direction against triangle
		Vector3f e1 = pTri->p1->pos - pTri->p0->pos;
		Vector3f e2 = pTri->p2->pos - pTri->p0->pos;

		// one side triangle, ray must oper with triangle normal
		if (!intersection.towSide)
		{
			Vector3f n = Vector3f::Cross(e1, e2);
			float d = Vector3f::Dot(ray.dir, n);
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

		Vector3f p = Vector3f::Cross(ray.dir, e2);
		float a = Vector3f::Dot(e1, p);
		// if result zero, no intersection or infinite intersections
		// (ray parallel to triangle plane)
		if ( Mathf::IsZero(a) )
			return false;

		// compute denominator
		float f = 1.0f / a;	

		// compute barycentric coordinates
		Vector3f s = ray.origin - pTri->p0->pos;
		float u = f * Vector3f::Dot(s, p);
		if (u < 0.0f || u > 1.0f)
		{
			return false;
		}

		Vector3f q = Vector3f::Cross(s, e1);
		float v = f * Vector3f::Dot(ray.dir, q);
		if (v < 0.0f || u + v > 1.0f)
		{
			return false;
		}

		// compute line parameter
		float t = f * Vector3f::Dot(e2, q);

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

	bool IntersectRayTriangle(Intersection& intersection, const Triangle* pTri, const Ray& ray, float& tRay)
	{
		// test ray direction against triangle
		Vector3f e1 = pTri->p1->pos - pTri->p0->pos;
		Vector3f e2 = pTri->p2->pos - pTri->p0->pos;

		// one side triangle, ray must oper with triangle normal
		if (!intersection.towSide)
		{
			Vector3f n = Vector3f::Cross(e1, e2);
			float d = Vector3f::Dot(ray.dir, n);
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

		Vector3f p = Vector3f::Cross(ray.dir, e2);
		float a = Vector3f::Dot(e1, p);
		// if result zero, no intersection or infinite intersections
		// (ray parallel to triangle plane)
		if ( Mathf::IsZero(a) )
			return false;

		// compute denominator
		float f = 1.0f / a;	

		// compute barycentric coordinates
		Vector3f s = ray.origin - pTri->p0->pos;
		float u = f * Vector3f::Dot(s, p);
		if (u < 0.0f || u > 1.0f)
		{
			return false;
		}

		Vector3f q = Vector3f::Cross(s, e1);
		float v = f * Vector3f::Dot(ray.dir, q);
		if (v < 0.0f || u + v > 1.0f)
		{
			return false;
		}

		// compute line parameter
		float t = f * Vector3f::Dot(e2, q);

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

		Vector3f q = qPrime / Vector3f::Dot(qPrime, qPrime);

		float tStar = (Vector3f::Dot(q, q) - Vector3f::Dot(rayOrigin, q)) / Vector3f::Dot(rayDir, q);
		if (tStar > 0.001f && tStar < tt)
		{
			Vector3f p = rayOrigin + rayDir * tStar;
			float p0 = Vector3f::Dot(p, triangle.ivm0);

			if (p0 < 0.0f)
			{
				return false;
			}

			float p1 = Vector3f::Dot(p, triangle.ivm1);
			if (p1 < 0.0f)
			{
				return false;
			}


			float p2 = Vector3f::Dot(p, triangle.ivm2);
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





}