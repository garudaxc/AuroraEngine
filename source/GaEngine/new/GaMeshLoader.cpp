#include "stdHeader.h"
#include "GaMeshLoader.h"
#include <fstream>
#include <boost/format.hpp>
#include "GaUtil.h"
#include "GaConsole.h"
#include "GaGeometryData.h"
#include "GaSkeleton.h"
#include "GaAnimation.h"
namespace Ga
{


	

GeometryData* ScmMeshLoader::LoadMesh(const char* pFileName)
{
	
	GeometryData* NullPtr = NULL;
	
	std::ifstream fin(pFileName, std::ios_base::binary | std::ios_base::in);
	if (!fin)
	{
		std::string msg;
		//msg = boost::str(boost::format("load scm mesh file error: %1\n") % pFileName);
		Console::AppendText(msg.c_str());
		return NullPtr;
	}

	
	fin.read((char*)&m_Header, sizeof(ScmHeader));
	if (m_Header.mMagic != Util::MakeFOURCC("MODL"))
	{
		std::string msg;
		//msg = boost::str(boost::format("file %1 is not a scm model file\n") % pFileName);
		Console::AppendText(msg.c_str());
		return NullPtr;
	}
	GeometryData* pMesh = new GeometryData(m_Header.mVertexCount, m_Header.mIndexCount/3);
	

	char* pBuffer = pMesh->AddStream(GeometryData::EU_INDEX, 2);
	fin.seekg(m_Header.mIndexOffset);
	fin.read(pBuffer, 2 * m_Header.mIndexCount);

	std::vector<ScmVertData> vertData;
	vertData.resize(m_Header.mVertexCount);

	fin.seekg(m_Header.mVertexOffset);
	fin.read((char*)&vertData[0], sizeof(ScmVertData) * m_Header.mVertexCount);

	pBuffer = pMesh->AddStream(GeometryData::EU_POSITION, sizeof(Vector3f));
	Util::MemMap(vertData[0].mPosition.Ptr(), sizeof(ScmVertData), pBuffer, sizeof(Vector3f), sizeof(Vector3f), m_Header.mVertexCount);

	pBuffer = pMesh->AddStream(GeometryData::EU_BLENDINDEX, sizeof(ubyte[4]));
	Util::MemMap(vertData[0].mBoneIndex, sizeof(ScmVertData), pBuffer, sizeof(ubyte[4]), sizeof(ubyte[4]), m_Header.mVertexCount);

	pBuffer = pMesh->AddStream(GeometryData::EU_NORMAL, sizeof(Vector3f));
	Util::MemMap(vertData[0].mNormal.Ptr(), sizeof(ScmVertData), pBuffer, sizeof(Vector3f), sizeof(Vector3f), m_Header.mVertexCount);

	pBuffer = pMesh->AddStream(GeometryData::EU_TANGENT, sizeof(Vector3f));
	Util::MemMap(vertData[0].mTangent.Ptr(), sizeof(ScmVertData), pBuffer, sizeof(Vector3f), sizeof(Vector3f), m_Header.mVertexCount);

	pBuffer = pMesh->AddStream(GeometryData::EU_BINORMAL, sizeof(Vector3f));
	Util::MemMap(vertData[0].mBinormal.Ptr(), sizeof(ScmVertData), pBuffer, sizeof(Vector3f), sizeof(Vector3f), m_Header.mVertexCount);

	pBuffer = pMesh->AddStream(GeometryData::EU_TEXCOORD, sizeof(Vector2f));
	Util::MemMap(vertData[0].mUV0.Ptr(), sizeof(ScmVertData), pBuffer, sizeof(Vector2f), sizeof(Vector2f), m_Header.mVertexCount);

	pBuffer = pMesh->AddStream(GeometryData::EU_TEXCOORD, sizeof(Vector2f));
	Util::MemMap(vertData[0].mUV1.Ptr(), sizeof(ScmVertData), pBuffer, sizeof(Vector2f), sizeof(Vector2f), m_Header.mVertexCount);

	return pMesh;
}


Skeleton* ScmMeshLoader::LoadSkeleton(const char* pFileName)
{
		
	Skeleton* NullPtr = NULL;
	
	std::ifstream fin(pFileName, std::ios_base::binary | std::ios_base::in);
	if (!fin)
	{
		std::string msg;
		//msg = boost::str(boost::format("load scm mesh file error: %1\n") % pFileName);
		Console::AppendText(msg.c_str());
		return NullPtr;
	}

	
	fin.read((char*)&m_Header, sizeof(ScmHeader));
	if (m_Header.mMagic != Util::MakeFOURCC("MODL"))
	{
		std::string msg;
		//msg = boost::str(boost::format("file %1 is not a scm model file\n") % pFileName);
		Console::AppendText(msg.c_str());
		return NullPtr;
	}


	fin.seekg(m_Header.mBoneOffset);	
	std::vector<ScmBoneData> bones(m_Header.mTotalBoneCount);
	fin.read((char*)&bones[0], sizeof(ScmBoneData) * m_Header.mTotalBoneCount);

	Skeleton* pSkeleton = new Skeleton();
	std::vector<int> parentIndex(m_Header.mTotalBoneCount);
	for (uint i = 0; i < m_Header.mTotalBoneCount; i++)
	{
		parentIndex[i] = bones[i].mParentBoneIndex;
	}
	
	bool bResult = pSkeleton->SetupHierarchy(m_Header.mTotalBoneCount, &parentIndex[0]);
	if (!bResult)
		return NullPtr;

	std::string boneName;
	for (uint i = 0; i < m_Header.mTotalBoneCount; i++)
	{
		fin.seekg(bones[i].mNameOffset);
		std::getline(fin, boneName, '\0');
		pSkeleton->SetBoneName(i, boneName);

		Skeleton::Bone* pBone = pSkeleton->GetBone(i);
		pBone->m_fScaleLocal = 1.0f;
		pBone->m_vTransLocal = bones[i].mPosition;
		pBone->m_qRotationLocal = bones[i].mRotation;
		pBone->m_matOffset = bones[i].mRestPoseInverse;
	}



	return pSkeleton;
}


AnimationSet* ScaAnimLoader::LoadAnimation(const char* pFileName)
{
	AnimationSet* NullPtr = NULL;

	std::ifstream fin(pFileName, std::ios_base::binary | std::ios_base::in);
	if (!fin)
	{
		std::string msg;
		//msg = boost::str(boost::format("load scm mesh file error: %1\n") % pFileName);
		Console::AppendText(msg.c_str());
		return NullPtr;
	}

	fin.read((char*)&m_Header, sizeof(ScaHeader));
	if (m_Header.mMagic != Util::MakeFOURCC("ANIM"))
	{
		std::string msg;
		//msg = boost::str(boost::format("file %1 is not a scm model file\n") % pFileName);
		Console::AppendText(msg.c_str());
		return NullPtr;
	}

	AnimationSet* pAnim = new AnimationSet("", m_Header.mNumBones, m_Header.mNumFrames);

	std::string boneName;
	fin.seekg(m_Header.mBoneNamesOffset);
	for (uint i = 0; i < m_Header.mNumBones; i++)
	{
		getline(fin, boneName, '\0');
		pAnim->SetKeyName(i, boneName);
	}

	// skip a animation header of total translation and rotation
	fin.seekg(m_Header.mFirstFrameOffset + sizeof(Vector3f) + sizeof(Quaternionf));

	std::vector<ScaAnimKey> aBuffer(m_Header.mNumBones);
	AnimationSet::KeyFrame	key;
	key.fScale = 1.0f;

	float		time;
	uint		reserved;
	int			frameSize = m_Header.mFrameSize - (sizeof(time) + sizeof(reserved));
	for (uint i = 0; i < m_Header.mNumFrames; i++)
	{
		// read time
		fin.read((char*)&time, sizeof(float));
		pAnim->SetTimeSample(i, time);
		// read reserved data
		fin.read((char*)&reserved, sizeof(reserved));
		fin.read((char*)&aBuffer[0], frameSize);
		for (uint j = 0; j < m_Header.mNumBones; j++)
		{
			key.vTranslation	= aBuffer[j].vTranslation;
			key.qRotation		= aBuffer[j].qRotation;
			pAnim->SetKeyFrame(i, j, key);
		}

	}

	return pAnim;
}




}// end of namespace