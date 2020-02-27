#pragma once
#include "stdHeader.h"
#include "Primitive.h"



struct Voxel
{
	union
	{
		PrimMailbox*		pPrim;
		PrimMailbox**		ppPrim;
	};

	int				nNumPrim;


	bool				TestRay(const Ray& ray, Intersection& inter);

};



class Grid
{
public:
	Grid();
	~Grid();

	void				Init(const vector<Mesh*>& meshes);

	bool				TestRay(const Ray& ray, Intersection& inter);

private:

	void				BuildVoxel();

	const Ga::AABB		GetVoxelAABB(int x, int y, int z);

	int					PosToVoxel(float pos, int axis);
	float				VoxelToPos(int v, int axis);
	Voxel*				GetVoxel(int x, int y, int z);



	vector<const Triangle*>			m_Primitives;
	vector<PrimMailbox>				m_Mailboxes;

	vector<Ga::AABB>				m_PrimAABB;

	Ga::AABB						m_SceneAABB;

	vector<Voxel>					m_Voxels;


	int								m_nNumVoxels[3];
	Ga::Vector3f					m_VoxelSize;
	Ga::Vector3f					m_InvVoxelSize;
	int								m_nTotalVoxels;

};

