#pragma once
#include "stdHeader.h"
#include "GaGeometrydata.h"

class TiXmlElement;



struct Triangle;
struct PrimMailbox
{
	int					rayId;
	const Triangle*		pTri;
};





class BRDF
{
public:
	enum Type
	{
		BRDF_Light,
		BRDF_Diffuse,
		BRDF_Reflect,
	};


	Type			type;
	Ga::Color		diffuseColor;
};




struct Vertex
{
	Ga::Vector3f			pos;
	Ga::Vector3f			normal;
	Ga::Vector3f			tangent;
	Ga::Vector3f			binormal;
	Ga::Vector3f			color;
	Ga::Vector2f			uv;
	float*					coeff;
};



struct Triangle
{
	Vertex*			p0;
	Vertex*			p1;
	Vertex*			p2;	
	float			area;
	BRDF*			brdf;

#if 0

	Ga::Vector3f		ivm0;
	Ga::Vector3f		ivm1;
	Ga::Vector3f		ivm2;
#endif
};




class DifferentialGeometry
{
public:
	Ga::Vector3f		pos;
	Ga::Vector3f		normal;
	Ga::Vector3f		tangent;
	Ga::Vector3f		binormal;
	Ga::Vector2f		uv;
	const Triangle*		pTri;

};




class Intersection
{
public:
	bool			towSide;
	bool			backFace;

	Ga::Vector3f		pos;
	float				u;
	float				v;
	const Triangle*		pTri;

	Intersection()
	{
		backFace = false;
	}
};

void CalcDifferentialGeometry(const Intersection& inter, DifferentialGeometry& dg);


class Primitive
{
public:
};



class Ray : public Primitive
{
public:
	Ray(const Ga::Vector3f& o, const Ga::Vector3f& d);

	Ga::Vector3f		PosFromT(float t) const;

	Ga::Vector3f		origin;
	Ga::Vector3f		dir;

	mutable float				tmin, tmax;
};




class Shape
{
public:
	virtual bool				TestIntersect(Intersection& intersection, const Ray& ray) const = 0;

	BRDF					m_brdf;


	Ga::AABB				m_aabb;

};


class Sphere : public Shape
{
public:
	virtual bool				TestIntersect(Intersection& intersection, const Ray& ray) const;


	Ga::Vector3f			m_vCenter;
	float					m_fRadius;

};







class Mesh : public Shape
{
public:
	Mesh()	{}
	~Mesh()	{}

	void					Load(TiXmlElement* pMesh);
	void					Transform(const Ga::Matrix4f& matWorld);

	int						GetNumTriangle()		const;
	const Triangle*			GetTriangle(int i)		const;

	int						GetNumVertex()			const;
	Vertex*					GetVertex(int i);

	virtual bool			TestIntersect(Intersection& intersection, const Ray& ray) const;


private:
	vector<Vertex>			m_VertiesWorld;
	vector<Triangle>		m_Triagnles;
};



bool TestTriangle(Intersection& intersection, const Triangle* pTri, const Ray& ray);
bool RayTriangleIntersect(Intersection& intersection, const Triangle* pTri, const Ray& ray, float& tRay);
bool TestAABB(const Ga::AABB& aabb, const Ray& ray, float& tmin, float& tmax);