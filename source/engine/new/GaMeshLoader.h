#ifndef GAMESHLOADER_H
#define GAMESHLOADER_H
#include "GaTypes.h"
#include "GaVector.h"
#include "GaQuaternion.h"
#include "GaMatrix.h"
namespace Ga
{

// FOURCC  |  Contents
// --------+------------------------
// 'MODL'  | Header info
// 'NAME'  | List of bone name strings
// 'SKEL'  | Array of bone data
// 'VTXL'  | Array of basic vertex data
// 'TRIS'  | Array of triangle indices
// 'VEXT'  | Array of extra vertex data (OPTIONAL SECTION)
// 'INFO'  | List of null terminated information strings (OPTIONAL SECTION)

// Section offsets in the file header point to the start of the data for that section (ie, the first byte AFTER
// the section's identifying FOURCC) Padding characters are added to the end of each section to ensure that 
// the next section is 16-byte aligned. Ommitted sections are indicated by an offset of 0. 
 
// *** All offsets are relative to the start of the file ***

//============================
//		ScmMeshLoader
//============================
class GeometryData;
class Skeleton;
class ScmMeshLoader
{
public:

	struct ScmHeader
	{
		// The FOURCC 'MODL'
		uint			mMagic;
		uint			mVersion;

		
		uint			mBoneOffset;	// Offset to SCM_BoneData[0]

		// Number of elements in SCM_BoneData that actually influence verts (no reference points)
		uint			mWeightedBoneCount;

		// Offset of basic vertex data section (SCM_VertData[0])
		uint			mVertexOffset;

		// Offset of extra vertex data section (SCM_VertExtraData[0]) 
		// Contains additional per-vertex information. *** Currently unused (and omitted) in SupCom 1.0 ***
		uint			mVertexExtraOffset;   

		// Number of elements in the SCM_VertData array
		// (and the SCM_VertExtraData array, if mVertexExtraOffset != 0)
		uint			mVertexCount;        

		// Offset of the triangle index section (SCM_TriangleData[0])
		uint			mIndexOffset;         

		// Number of elements in the SCM_TriangleData array
		uint			mIndexCount;          

		// Offset of information section (SCM_InfoData[0])
		uint			mInfoOffset;

		// Number of elements in the SCM_InfoData list
		uint			mInfoCount;           

		// Number of elements in the SCM_BoneData array (including 'reference point' bones)
		uint			mTotalBoneCount;      
	};


	struct ScmVertData
	{
		// Position of the vertex relative to the local origin of the mesh
		Vector3f				mPosition;

		// 'Tangent Space' normal, tangent & binormal unit vectors
		Vector3f				mNormal;
		Vector3f				mTangent;
		Vector3f				mBinormal;

		// Two sets of UV coordinates 
		Vector2f				mUV0;
		Vector2f				mUV1;

		// Up to 4-bone skinning can be supported using additional 
		// indices (in conjunction with bone weights in the optional VertexExtra array)
		// Skinned meshes are not used in SupCom 1.0 so only mBoneIndex[0] is expected
		// to contain a valid index.
		ubyte					mBoneIndex[4]; 
	};


	struct ScmBoneData
	{
		// Inverse transform of the bone relative to the local origin of the mesh
		// 4x4 Matrix with row major (i.e. D3D default ordering)
		Matrix4f				mRestPoseInverse;
		Vector3f				mPosition;			// Position relative to the parent bone.
		Quaternionf				mRotation;			// Rotation relative to the parent bone.
		uint					mNameOffset;		// Offset of the bone's name string
		uint					mParentBoneIndex;	// Index of the bone's parent in the SCM_BoneData array

		uint					RESERVED_0;      
		uint					RESERVED_1;      
	};

	GeometryData*		LoadMesh(const char* pFileName);
	Skeleton*			LoadSkeleton(const char* pFileName);


private:
	ScmHeader					m_Header;

};


//============================
//		ScaAnimLoader
//============================
class AnimationSet;
class ScaAnimLoader
{
public:
	struct ScaHeader
	{
		uint				mMagic;					// The FOURCC 'ANIM'
		uint				mVersion;
		uint				mNumFrames;

		// The duration (in seconds) of this animation.  
		// The animation plays at (mNumFrames-1)/mDuration frames per second.
		float				mDuration;
		uint				mNumBones;

		// Offset of the bone names (SScaFileBoneNames[0])
		uint				mBoneNamesOffset;

		// Offset of the bone link info (SScaFileBoneLinks[0])
		uint				mBoneLinksOffset;

		// Offset of the actual animation data (SScaFileAnimData[0])
		uint				mFirstFrameOffset;

		// The number of bytes in one animation frame.
		uint				mFrameSize;
	};


	struct ScmBoneData
	{
		// Inverse transform of the bone relative to the local origin of the mesh
		// 4x4 Matrix with row major (i.e. D3D default ordering)
		Matrix4f		mRestPoseInverse;
		Vector3f		mPosition;			// Position relative to the parent bone.
		Quaternionf		mRotation;			// Rotation relative to the parent bone.
	
		uint mNameOffset;					// Offset of the bone's name string

		uint mParentBoneIndex;				// Index of the bone's parent in the SCM_BoneData array

		uint RESERVED_0;      
		uint RESERVED_1;      
	};

	struct ScaAnimKey
	{
		Vector3f		vTranslation;
		Quaternionf		qRotation;
	};

	AnimationSet*		LoadAnimation(const char* pFileName);

private:
	ScaHeader					m_Header;


};
















}// end of namespace

#endif