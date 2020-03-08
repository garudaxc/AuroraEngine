#ifndef GAPRIMITIVE_H
#define GAPRIMITIVE_H

#include "AABB.h"
namespace Aurora
{
	namespace Primitive
	{

		class Plane
		{
		public:
			Plane() {}
			Plane(const Vector3f& norm, float dist);
			Plane(const Vector3f& norm, const Vector3f& point);
			Plane(const Vector3f& a, const Vector3f& b, const Vector3f& c);
			Plane(const Vector4f& v);

			const Plane& operator = (const Vector4f& v);

			void Normalize();

			Vector3f n;

			float d;
		};


		float Distance(const Plane& plane, const Vector3f& point);



		struct Vertex
		{
			Aurora::Vector3f			pos;
			Aurora::Vector3f			normal;
			Aurora::Vector3f			tangent;
			Aurora::Vector3f			binormal;
			Aurora::Vector3f			color;
			Aurora::Vector2f			uv;
			float* coeff;
		};


		struct Triangle
		{
			Vertex* p0;
			Vertex* p1;
			Vertex* p2;
			float			area;
#if 0

			Aurora::Vector3f		ivm0;
			Aurora::Vector3f		ivm1;
			Aurora::Vector3f		ivm2;
#endif
		};


		class DifferentialGeometry
		{
		public:
			Aurora::Vector3f		pos;
			Aurora::Vector3f		normal;
			Aurora::Vector3f		tangent;
			Aurora::Vector3f		binormal;
			Aurora::Vector2f		uv;
			const Triangle* pTri;

		};


		class Intersection
		{
		public:
			bool towSide;
			bool backFace;

			Vector3f pos;
			float u;
			float v;
			const Triangle* pTri;

			Intersection()
			{
				backFace = false;
			}
		};


		void CalcDifferentialGeometry(const Intersection& inter, DifferentialGeometry& dg);


		class Ray
		{
		public:
			Ray();
			Ray(const Ray& r);
			Ray(const Vector3f& o, const Vector3f& d);

			Aurora::Vector3f PosFromT(float t) const;

			Aurora::Vector3f origin;

			Aurora::Vector3f dir;

			mutable float tmin, tmax;
		};


		class Sphere
		{
		public:
			virtual bool TestIntersect(Intersection& intersection, const Ray& ray) const;

			Aurora::Vector3f m_vCenter;

			float m_fRadius;
		};


		bool TestRayAABB(const AABB& aabb, const Ray& ray, float& tmin, float& tmax);

		bool TestRayAABB(const AABB& aabb, const Ray& ray);

		bool TestRayTriangle(Intersection& intersection, const Triangle* pTri, const Ray& ray);

		bool IntersectRayTriangle(Intersection& intersection, const Triangle* pTri, const Ray& ray, float& tRay);
	}

}

#endif
