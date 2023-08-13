#include "stdHeader.h"
#include "AABB.h"

namespace Aurora
{

	/********************************************************/

	/* AABB-triangle overlap test code                      */

	/* by Tomas Akenine-M鰈ler                              */

	/* Function: int triBoxOverlap(float boxcenter[3],      */

	/*          float boxhalfsize[3],float triverts[3][3]); */

	/* History:                                             */

	/*   2001-03-05: released the code in its first version */

	/*   2001-06-18: changed the order of the tests, faster */

	/*                                                      */

	/* Acknowledgement: Many thanks to Pierre Terdiman for  */

	/* suggestions and discussions on how to optimize code. */

	/* Thanks to David Hunt for finding a ">="-bug!         */

	/********************************************************/

#include <math.h>
#include <stdio.h>


#define X 0
#define Y 1
#define Z 2

#define CROSS(dest,v1,v2) \
	dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
	dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
	dest[2]=v1[0]*v2[1]-v1[1]*v2[0]; 


#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])


#define SUB(dest,v1,v2) \
	dest[0]=v1[0]-v2[0]; \
	dest[1]=v1[1]-v2[1]; \
	dest[2]=v1[2]-v2[2]; 



#define FINDMINMAX(x0,x1,x2,min,max) \
	min = max = x0;   \
	if(x1<min) min=x1;\
	if(x1>max) max=x1;\
	if(x2<min) min=x2;\
	if(x2>max) max=x2;



		int planeBoxOverlap(float normal[3], float vert[3], float maxbox[3])	// -NJMP-
		{
			int q;
			float vmin[3],vmax[3],v;

			for(q=X;q<=Z;q++)
			{
				v=vert[q];				
				if(normal[q]>0.0f)
				{
					vmin[q]=-maxbox[q] - v;
					vmax[q]= maxbox[q] - v;
				}
				else
				{
					vmin[q]= maxbox[q] - v;
					vmax[q]=-maxbox[q] - v;
				}
			}

			if(DOT(normal,vmin)>0.0f) return 0;
			if(DOT(normal,vmax)>=0.0f) return 1;

			return 0;
		}


	/*======================== X-tests ========================*/

#define AXISTEST_X01(a, b, fa, fb)			   \
	p0 = a*v0[Y] - b*v0[Z];			       	   \
	p2 = a*v2[Y] - b*v2[Z];			       	   \
	if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;} \
	rad = fa * boxhalfsize[Y] + fb * boxhalfsize[Z];   \
	if(min>rad || max<-rad) return 0;

#define AXISTEST_X2(a, b, fa, fb)			   \
	p0 = a*v0[Y] - b*v0[Z];			           \
	p1 = a*v1[Y] - b*v1[Z];			       	   \
	if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
	rad = fa * boxhalfsize[Y] + fb * boxhalfsize[Z];   \
	if(min>rad || max<-rad) return 0;

	/*======================== Y-tests ========================*/

#define AXISTEST_Y02(a, b, fa, fb)			   \
	p0 = -a*v0[X] + b*v0[Z];		      	   \
	p2 = -a*v2[X] + b*v2[Z];	       	       	   \
	if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;} \
	rad = fa * boxhalfsize[X] + fb * boxhalfsize[Z];   \
	if(min>rad || max<-rad) return 0;


#define AXISTEST_Y1(a, b, fa, fb)			   \
	p0 = -a*v0[X] + b*v0[Z];		      	   \
	p1 = -a*v1[X] + b*v1[Z];	     	       	   \
	if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
	rad = fa * boxhalfsize[X] + fb * boxhalfsize[Z];   \
	if(min>rad || max<-rad) return 0;


	/*======================== Z-tests ========================*/

#define AXISTEST_Z12(a, b, fa, fb)			   \
	p1 = a*v1[X] - b*v1[Y];			           \
	p2 = a*v2[X] - b*v2[Y];			       	   \
	if(p2<p1) {min=p2; max=p1;} else {min=p1; max=p2;} \
	rad = fa * boxhalfsize[X] + fb * boxhalfsize[Y];   \
	if(min>rad || max<-rad) return 0;



#define AXISTEST_Z0(a, b, fa, fb)			   \
	p0 = a*v0[X] - b*v0[Y];				   \
	p1 = a*v1[X] - b*v1[Y];			           \
	if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
	rad = fa * boxhalfsize[X] + fb * boxhalfsize[Y];   \
	if(min>rad || max<-rad) return 0;



	int triBoxOverlap(float boxcenter[3],float boxhalfsize[3],float triverts[3][3])
	{
		/*    use separating axis theorem to test overlap between triangle and box */
		/*    need to test for overlap in these directions: */
		/*    1) the {x,y,z}-directions (actually, since we use the AABB of the triangle */
		/*       we do not even need to test these) */
		/*    2) normal of the triangle */
		/*    3) crossproduct(edge from tri, {x,y,z}-directin) */
		/*       this gives 3x3=9 more tests */

		float v0[3],v1[3],v2[3];
		//   float axis[3];
		float min,max,p0,p1,p2,rad,fex,fey,fez;		// -NJMP- "d" local variable removed
		float normal[3],e0[3],e1[3],e2[3];

		/* This is the fastest branch on Sun */
		/* move everything so that the boxcenter is in (0,0,0) */
		SUB(v0,triverts[0],boxcenter);
		SUB(v1,triverts[1],boxcenter);
		SUB(v2,triverts[2],boxcenter);

		/* compute triangle edges */
		SUB(e0,v1,v0);      /* tri edge 0 */
		SUB(e1,v2,v1);      /* tri edge 1 */
		SUB(e2,v0,v2);      /* tri edge 2 */

		/* Bullet 3:  */
		/*  test the 9 tests first (this was faster) */
		fex = fabsf(e0[X]);
		fey = fabsf(e0[Y]);
		fez = fabsf(e0[Z]);

		AXISTEST_X01(e0[Z], e0[Y], fez, fey);
		AXISTEST_Y02(e0[Z], e0[X], fez, fex);
		AXISTEST_Z12(e0[Y], e0[X], fey, fex);

		fex = fabsf(e1[X]);
		fey = fabsf(e1[Y]);
		fez = fabsf(e1[Z]);

		AXISTEST_X01(e1[Z], e1[Y], fez, fey);
		AXISTEST_Y02(e1[Z], e1[X], fez, fex);
		AXISTEST_Z0(e1[Y], e1[X], fey, fex);

		fex = fabsf(e2[X]);
		fey = fabsf(e2[Y]);
		fez = fabsf(e2[Z]);

		AXISTEST_X2(e2[Z], e2[Y], fez, fey);
		AXISTEST_Y1(e2[Z], e2[X], fez, fex);
		AXISTEST_Z12(e2[Y], e2[X], fey, fex);

		/* Bullet 1: */
		/*  first test overlap in the {x,y,z}-directions */
		/*  find min, max of the triangle each direction, and test for overlap in */
		/*  that direction -- this is equivalent to testing a minimal AABB around */
		/*  the triangle against the AABB */

		/* test in X-direction */
		FINDMINMAX(v0[X],v1[X],v2[X],min,max);
		if(min>boxhalfsize[X] || max<-boxhalfsize[X]) return 0;

		/* test in Y-direction */
		FINDMINMAX(v0[Y],v1[Y],v2[Y],min,max);
		if(min>boxhalfsize[Y] || max<-boxhalfsize[Y]) return 0;

		/* test in Z-direction */
		FINDMINMAX(v0[Z],v1[Z],v2[Z],min,max);
		if(min>boxhalfsize[Z] || max<-boxhalfsize[Z]) return 0;

		/* Bullet 2: */
		/*  test if the box intersects the plane of the triangle */
		/*  compute plane equation of triangle: normal*x+d=0 */
		CROSS(normal,e0,e1);

		if(!planeBoxOverlap(normal,v0,boxhalfsize)) return 0;	

		return 1;   /* box and triangle overlaps */
	}






	void AABB::Merge(const Vector3f& p)
	{
#define SET_IF(A, B, REL)	\
	if (A REL B)	\
		{				\
		B = A;		\
	}				

		SET_IF( p.x, vMin.x, < )
			SET_IF( p.y, vMin.y, < )
			SET_IF( p.z, vMin.z, < )
			SET_IF( p.x, vMax.x, > )
			SET_IF( p.y, vMax.y, > )
			SET_IF( p.z, vMax.z, > )

	}


	void AABB::Merge(const AABB& a)
	{
		vMin.x = Mathf::Min(a.vMin.x, vMin.x);
		vMin.y = Mathf::Min(a.vMin.y, vMin.y);
		vMin.z = Mathf::Min(a.vMin.z, vMin.z);

		vMax.x = Mathf::Max(a.vMax.x, vMax.x);
		vMax.y = Mathf::Max(a.vMax.y, vMax.y);
		vMax.z = Mathf::Max(a.vMax.z, vMax.z);
	}

	void AABB::Merge(const AABB& a, const AABB& b)
	{
		vMin.x = Mathf::Min(a.vMin.x, b.vMin.x);
		vMin.y = Mathf::Min(a.vMin.y, b.vMin.y);
		vMin.z = Mathf::Min(a.vMin.z, b.vMin.z);

		vMax.x = Mathf::Max(a.vMax.x, b.vMax.x);
		vMax.y = Mathf::Max(a.vMax.y, b.vMax.y);
		vMax.z = Mathf::Max(a.vMax.z, b.vMax.z);
	}


	void AABB::GetVertexPos(vector<Vector3f>& positions) const
	{
		positions.resize(8);
		positions[0].Set(vMin.x, vMax.y, vMin.z);
		positions[1].Set(vMax.x, vMax.y, vMin.z);
		positions[2].Set(vMax.x, vMax.y, vMax.z);
		positions[3].Set(vMin.x, vMax.y, vMax.z);
		positions[4].Set(vMin.x, vMin.y, vMin.z);
		positions[5].Set(vMax.x, vMin.y, vMin.z);
		positions[6].Set(vMax.x, vMin.y, vMax.z);
		positions[7].Set(vMin.x, vMin.y, vMax.z);
	}

	bool AABB::IsIntersect(const Vector3f& origin, const Vector3f& direction) const
	{
		float maxS = -Mathf::MAX_VALUE;
		float minT = Mathf::MAX_VALUE;

		// ray is parallel to plane
		for (int i = 0; i < 3; i++)
		{
			if (Mathf::IsZero(direction[i]))
			{
				// ray passes by box
				if (origin[i] < vMin[i] || origin[i] > vMax[i])
					return false;
			}
			else
			{
				// compute intersection parameters and sort
				float s = (vMin[i] - origin[i]) / direction[i];
				float t = (vMax[i] - origin[i]) / direction[i];
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

	bool AABB::IsInside(const Vector3f& pos) const
	{
		if (vMin.x <= pos.x && vMax.x >= pos.x &&
			vMin.y <= pos.y && vMax.y >= pos.y &&
			vMin.z <= pos.z && vMax.z >= pos.z)
		{
			return true;
		}

		return false;
	}


	bool AABB::TestTriangle(const Vector3f& a, const Vector3f& b, const Vector3f& c) const
	{
		float center[] = {
			(vMax.x + vMin.x) * 0.5f,
			(vMax.y + vMin.y) * 0.5f,
			(vMax.z + vMin.z) * 0.5f,
		};

		float halfLen[] = {
			(vMax.x - vMin.x) * 0.5f,
			(vMax.y - vMin.y) * 0.5f,
			(vMax.z - vMin.z) * 0.5f,
		};

		float tri[3][3] = {
			{a.x, a.y, a.z},
			{b.x, b.y, b.z},
			{c.x, c.y, c.z},
		};

		int overlap = triBoxOverlap(center, halfLen, tri);

		return overlap == 1;
	}

}