#pragma once 

namespace Aurora
{


	struct AABB
	{
		Vector3f vMin;
		Vector3f vMax;

		AABB(const Vector3f& minPos, const Vector3f& maxPos):vMin(minPos),vMax(maxPos)
		{
		}

		AABB():vMin(Mathf::MAX_VALUE, Mathf::MAX_VALUE, Mathf::MAX_VALUE),
			vMax(-Mathf::MAX_VALUE, -Mathf::MAX_VALUE, -Mathf::MAX_VALUE)
		{
		}

		void Reset()
		{
			vMin.Set(Mathf::MAX_VALUE, Mathf::MAX_VALUE, Mathf::MAX_VALUE);
			vMax.Set(-Mathf::MAX_VALUE, -Mathf::MAX_VALUE, -Mathf::MAX_VALUE);
		}

		void Merge(const Vector3f& p);
		void Merge(const AABB& a);
		void Merge(const AABB& a, const AABB& b);
		void GetVertexPos(vector<Vector3f>& positions) const;
		bool IsIntersect(const Vector3f& origin, const Vector3f& direction) const;

		bool IsInside(const Vector3f& pos) const;

		bool TestTriangle(const Vector3f& a, const Vector3f& b, const Vector3f& c) const;
	};



}