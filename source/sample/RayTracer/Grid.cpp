#include "Grid.h"


using namespace Ga;

Grid::Grid()
{

}

Grid::~Grid()
{

}

void Grid::Init(const vector<Mesh*>& meshes)
{
	m_Primitives.clear();

	for (vector<Mesh*>::const_iterator it = meshes.begin(); it != meshes.end(); ++it)
	{
		for (int i = 0; i < (*it)->GetNumTriangle(); i++)
		{
			m_Primitives.push_back((*it)->GetTriangle(i));
		}
	}

	// init aabb
	m_PrimAABB.resize(m_Primitives.size());
	m_SceneAABB.Reset();
	for (int i = 0; i < m_PrimAABB.size(); i++)
	{
		m_PrimAABB[i].Reset();
		m_PrimAABB[i].Merge(m_Primitives[i]->p0->pos);
		m_PrimAABB[i].Merge(m_Primitives[i]->p1->pos);
		m_PrimAABB[i].Merge(m_Primitives[i]->p2->pos);
		m_SceneAABB.Merge(m_PrimAABB[i]);
	}

	// init mailbox
	m_Mailboxes.resize(m_Primitives.size());
	for (int i = 0; i < m_Primitives.size(); i++)
	{
		m_Mailboxes[i].rayId = 0;
		m_Mailboxes[i].pTri = m_Primitives[i];
	}

	BuildVoxel();
}



bool TestAABB(const AABB& a, const AABB& b)
{
	for (int i = 0; i < 3; i++)
	{
		if (a.vMin[i] > b.vMax[i] || a.vMax[i] < b.vMin[i])
		{
			return false;
		}
	}

	return true;
}



void Grid::BuildVoxel()
{
	//find opt voxel size
	Vector3f sceneExt = m_SceneAABB.vMax - m_SceneAABB.vMin;
	int maxAxis = 0;
	if (sceneExt.x > sceneExt.y && sceneExt.x > sceneExt.z)
	{
		maxAxis = 0;
	}
	else
	{
		maxAxis  = (sceneExt.y > sceneExt.z) ? 1 : 2;
	}
	float maxExtent = sceneExt[maxAxis];
	float voxelSize = 3.0f * Mathf::Pow(m_Primitives.size(), 1.f / 3.f);

	// split voxels
	for (int i = 0; i < 3; i++)
	{
		m_nNumVoxels[i] = (int)(sceneExt[i] / voxelSize);
		if (m_nNumVoxels[i] == 0)
		{
			m_nNumVoxels[i] = 1;
		}
		m_VoxelSize[i] = sceneExt[i] / (float)m_nNumVoxels[i];

		m_InvVoxelSize[i] = (m_VoxelSize[i]==.0f) ? 0.f : 1.f / m_VoxelSize[i];
	}

	m_nTotalVoxels = m_nNumVoxels[0] * m_nNumVoxels[1] * m_nNumVoxels[2];
	m_Voxels.resize(m_nTotalVoxels);


	// setup prim to voxel
	int idx = 0;
	for (int z = 0; z < m_nNumVoxels[2]; z++)
	{
		for (int y = 0; y < m_nNumVoxels[1]; y++)
		{
			for (int x = 0; x < m_nNumVoxels[0]; x++)
			{
				AABB aabb = GetVoxelAABB(x, y, z);

				vector<PrimMailbox*> prims;
				for (int i = 0; i < m_Primitives.size(); i++)
				{
					if (TestAABB(aabb, m_PrimAABB[i]))
					{
						prims.push_back(&m_Mailboxes[i]);
					}
				}

				m_Voxels[idx].nNumPrim = prims.size();
				if (prims.empty())
				{
					m_Voxels[idx].pPrim = NULL;
				}
				else if (prims.size() == 1)
				{
					m_Voxels[idx].pPrim = prims[0];
				}
				else
				{
					m_Voxels[idx].ppPrim = new PrimMailbox*[prims.size()];
					memcpy_s(m_Voxels[idx].ppPrim, sizeof(PrimMailbox*) * prims.size(),
								&prims[0], sizeof(PrimMailbox*) * prims.size());

				}

				idx++;
			}
		}
	}


}


const AABB Grid::GetVoxelAABB(int x, int y, int z)
{
	AABB aabb;

	aabb.vMin = Vector3f(x * m_VoxelSize.x, y * m_VoxelSize.y, z * m_VoxelSize.z) + m_SceneAABB.vMin;
	aabb.vMax = aabb.vMin + m_VoxelSize;

	return aabb;
}

Voxel* Grid::GetVoxel(int x, int y, int z)
{
	int index = z * m_nNumVoxels[0] * m_nNumVoxels[1] + y * m_nNumVoxels[0] + x;
	return &m_Voxels[index];
}


int Grid::PosToVoxel(float pos, int axis)
{
	int v = (int)((pos - m_SceneAABB.vMin[axis]) * m_InvVoxelSize[axis]);

	if (v < 0)
	{
		return 0;
	}

	if (v >= m_nNumVoxels[axis])
	{
		return m_nNumVoxels[axis] - 1;
	}

	return v;
}


float Grid::VoxelToPos(int v, int axis)
{
	return m_SceneAABB.vMin[axis] + v * m_VoxelSize[axis];
}

bool Grid::TestRay(const Ray& ray, Intersection& inter)
{
	float tmin, tmax;
	if (!TestAABB(m_SceneAABB, ray, tmin, tmax))
	{
		return false;
	}

	if (m_SceneAABB.IsInside(ray.PosFromT(ray.tmin)))
	{
		tmin = ray.tmin;
	}
	Vector3f insectPos = ray.PosFromT(tmin);


	float nextCrossT[3], deltaT[3];
	int step[3], out[3], pos[3];
	for (int i = 0; i < 3; i++)
	{
		pos[i] = PosToVoxel(insectPos[i], i);
		if (ray.dir[i] >= 0.f)
		{
			deltaT[i] = m_VoxelSize[i] / ray.dir[i];
			nextCrossT[i] = tmin + (VoxelToPos(pos[i] + 1, i) - insectPos[i])  / ray.dir[i];
			step[i] = 1;
			out[i] = m_nNumVoxels[i];
			
		}
		else
		{
			deltaT[i] = -m_VoxelSize[i] / ray.dir[i];
			nextCrossT[i] = tmin + (VoxelToPos(pos[i], i) - insectPos[i])  / ray.dir[i];
			step[i] = -1;
			out[i] = -1;
		}
	}

	bool bIsIntersect = false;
	while (true)
	{
		Voxel* pVoxel = GetVoxel(pos[0], pos[1], pos[2]);
		bIsIntersect |= pVoxel->TestRay(ray, inter);

		int stepAxis = 0;
		float nextT = nextCrossT[stepAxis];
		for (int i = 1; i < 2; i++)
		{
			if (nextCrossT[i] < nextT)
			{
				nextT = nextCrossT[i];
				stepAxis = i;
			}
		}

		if (ray.tmax < nextT)
		{
			break;
		}
		pos[stepAxis] += step[stepAxis];
		if (pos[stepAxis] == out[stepAxis])
		{
			break;
		}
		nextCrossT[stepAxis] += deltaT[stepAxis];
	}
	
	return bIsIntersect;
}




bool Voxel::TestRay(const Ray& ray, Intersection& inter)
{
	if (nNumPrim == 0)
	{
		return false;
	}

	bool bIsIntersect = false;
	if (nNumPrim == 1)
	{
		bIsIntersect |= TestTriangle(inter, pPrim->pTri, ray);
	}
	else
	{
		for (int i = 0; i < nNumPrim; i++)
		{
			bIsIntersect |= TestTriangle(inter, ppPrim[i]->pTri, ray);
		}
	}

	return bIsIntersect;
}


