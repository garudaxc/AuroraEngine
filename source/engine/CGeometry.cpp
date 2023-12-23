#include "stdHeader.h"
#include "CGeometry.h"
#include <assert.h>
#include "MathFunction.h"
#include "Util.h"
#include "RendererObject.h"
#include "Renderer.h"
#include "FileSystem.h"
namespace Aurora
{

Vector3f ComputeNormal(const Vector3f& v1, const Vector3f& v2, const Vector3f& v3) {
	Vector3f norm = Vector3f::Cross(v2 - v1, v3 - v1);
	norm.Normalize();
	return norm;
}
 
// compute weighted normal of triangle
Vector3f ComputeWeightedNormal(const Vector3f& v1, const Vector3f& v2, const Vector3f& v3) {
	return Vector3f::Cross(v2 - v1, v3 - v1);
}

//---------------------------------------------------------------
struct VertexInfo
{
	VertexInfo() {
		aAdjTri.reserve(8);
		nAdjCount = 0;
	}

	void AddTriangle(int tri) {
		aAdjTri.push_back(tri);
		nAdjCount++;
	}

	// record adjacent triangles' index of the vertex
	std::vector<int>	aAdjTri;

	// number of adjacent triangles
	int		nAdjCount;
};

// get vertex info from buffer, ie. the adjacent triangle of vertex
template<typename T>
void GetVertexInfo(const T* pIndex, VertexInfo* pInfo, uint32 nTri) {
	for (uint32 i = 0, tri = 0; tri < nTri; i += 3, tri++)
	{
		// add this triangle to this vertex
		pInfo[pIndex[i]].AddTriangle(tri);
		pInfo[pIndex[i+1]].AddTriangle(tri);
		pInfo[pIndex[i+2]].AddTriangle(tri);
	}
}

template<typename T>
void ComputeFaceNormal(const T* pIndex, Vector3f* pFaceNormal, const Vector3f* pPos, uint32 nTri)
{
	for (uint32 iFace = 0, iv = 0; iFace < nTri; iFace++, iv += 3) {
		pFaceNormal[iFace] = ComputeWeightedNormal(pPos[pIndex[iv]], 
												pPos[pIndex[iv+1]], 
												pPos[pIndex[iv+2]]);
	}
}



constexpr uint32 CGeometry::GetSizeOfType(Vertex::ElemType type) {
	switch (type)
	{
	case Aurora::Vertex::TYPE_FLOAT:
		return 4;
	case Aurora::Vertex::TYPE_FLOAT2:
		return 8;
	case Aurora::Vertex::TYPE_FLOAT3:
		return 12;
	case Aurora::Vertex::TYPE_FLOAT4:
		return 16;
	case Aurora::Vertex::TYPE_UBYTE4_UINT:
		return 4;
	case Aurora::Vertex::TYPE_UBYTE4_UNORM:
		return 4;
	case Aurora::Vertex::TYPE_USHORT_UINT:
		return 2;
	case Aurora::Vertex::TYPE_UINT:
		return 4;
	default:
		assert(0);
		return 0;
	}

}

void FlipTriangleOrder(uint8* pIndex, uint32 nTri, Vertex::ElemType indexType)
{
	int stride = CGeometry::GetSizeOfType(indexType);
	// swep two points in triangle
	if (stride == 2) {
		uint16* pSrc = (uint16*)pIndex;
		uint16 temp;
		for (uint32 i = 0; i < nTri; i++)	{
			temp = *(pSrc + 1);
			*(pSrc + 1) = *(pSrc + 2);
			*(pSrc + 2) = temp;
			pSrc += 3;
		}
	}
	else if (stride == 4) {
		uint32* pSrc = (uint32*)pIndex;
		uint32 temp;
		for (uint32 i = 0; i < nTri; i++)	{
			temp = *(pSrc + 1);
			*(pSrc + 1) = *(pSrc + 2);
			*(pSrc + 2) = temp;
			pSrc += 3;
		}
	}

}



void ComputeNormal(Vector3f* pNormal, uint8* pIndex, uint32 nTri, Vertex::ElemType indexType,
				   Vector3f* pPos, uint32 nVert)
{
	int stride = CGeometry::GetSizeOfType(indexType);

	std::vector<VertexInfo>	vertInfo(nVert);
	std::vector<Vector3f>	faceNormal(nTri);
	if (stride == 2)
	{
		GetVertexInfo((uint16*)pIndex, &vertInfo[0], nTri);
		ComputeFaceNormal((uint16*)pIndex, &faceNormal[0], pPos, nTri);
	}
	else
	{
		GetVertexInfo(pIndex, &vertInfo[0], nTri);
		ComputeFaceNormal(pIndex, &faceNormal[0], pPos, nTri);
	}

	Vector3f normal;
	for (uint32 i = 0; i < nVert; i++)
	{
		normal = Vector3f::ZERO;
		for (int j = 0; j < vertInfo[i].nAdjCount; j++)
		{
			normal += faceNormal[vertInfo[i].aAdjTri[j]];
		}
		normal.Normalize();
		pNormal[i] = normal;
	}
}

	
vector<VertexLayoutItem>	CGeometry::VertexLayoutPosNormTangentTex(
	{
	   {Vertex::TYPE_FLOAT3, Vertex::USAGE_POSITION, 0},
	   {Vertex::TYPE_FLOAT3, Vertex::USAGE_NORMAL, 0},
	   {Vertex::TYPE_FLOAT4, Vertex::USAGE_TANGENT, 0},
	   {Vertex::TYPE_FLOAT2, Vertex::USAGE_TEXCOORD, 0},
	}
);
	


CGeometry::CGeometry(uint32 InNumVertex, uint32 InNumTriangle):mNumVertex(InNumVertex),
mNumIndex(InNumTriangle*3) {
}

CGeometry::CGeometry():mNumVertex(0),mNumIndex(0) {
}

int32 CGeometry::CalcVertexStride(const vector<VertexLayoutItem>& layout)
{
	int32 stride = 0;
	for (auto it = layout.begin(); it != layout.end(); ++it) {
		stride += CGeometry::GetSizeOfType((Vertex::ElemType)it->type);
	}

	return stride;
}

uint32 CGeometry::GetNumStreamOfUsage(Vertex::ElemUsage usage) const {
	uint32 num = 0;
	for (uint32 i = 0; i < GetNumStream(); i++)
	{
		if (usage == mStreams[i].usage)
		{
			num++;
		}
	}
	return num;
}


uint8* CGeometry::AddStream(Vertex::ElemUsage usage, Vertex::ElemType type) {
	uint32 usageIndex		= GetNumStreamOfUsage(usage);

	uint32 numElem = 0;
	if (usage == Vertex::USAGE_INDEX)	{
		numElem = mNumIndex;
	} else {
		numElem = mNumVertex;
	}

	Stream_t stream;
	stream.type			= type;
	stream.usage		= usage;
	stream.nUsageIndex	= usageIndex;
	stream.nStride		= GetSizeOfType(type);
	stream.nSizeInByte	= numElem * stream.nStride;

	stream.data = new uint8[stream.nSizeInByte];

	mStreams.push_back(stream);

	return stream.data;
}


int CGeometry::FindStream(Vertex::ElemUsage usage, uint8 usageIndex) const {
	for (int i = 0; i < (int)mStreams.size(); i++) {
		if (usage == mStreams[i].usage && usageIndex == mStreams[i].nUsageIndex)	{
			return i;
		}
	}
	return -1;
}


uint8* CGeometry::GetStreamPointer(Vertex::ElemUsage usage, uint8 usageIndex) {
	int nStream = FindStream(usage, usageIndex);
	if (nStream == -1)	{
		return nullptr;
	}

	return mStreams[nStream].Ptr();
}


bool CGeometry::AssembleVertexElement(void* pBuffer, uint32 bufferOffset, uint32 bufferStride, Vertex::ElemType usedType, 
										 Vertex::ElemUsage usage, uint8 usageIndex) const
{
	int streamIndex = FindStream(usage, usageIndex);

	// compute tangent if needed
	// if (streamIndex == -1 && usage == Vertex::USAGE_TANGENT){
	// 	ComputeTangentSpace();
	// 	streamIndex = FindStream(usage, usageIndex);
	// }

	if (streamIndex == -1) {
		return false;
	}

	const Stream_t& stream = mStreams[streamIndex];
	Util::MemMap(stream.Ptr(), stream.nStride,  (uint8*)pBuffer + bufferOffset, 
					bufferStride, GetSizeOfType(usedType), mNumVertex);

	return true;
}


bool CGeometry::Load(const TiXmlNode* pXmlMesh) {
	const TiXmlElement* pMesh = pXmlMesh->ToElement();

	int nNumVerts = 0, nNumTri = 0;
	pMesh->Attribute("numVerts", &nNumVerts);
	pMesh->Attribute("numTri", &nNumTri);

	mNumVertex = nNumVerts;
	mNumIndex = 3 * nNumTri;

	stringstream ss;

	uint32* pIndex = (uint32*)AddStream(Vertex::USAGE_INDEX, Vertex::TYPE_UINT);
	string text = pMesh->FirstChild("Index")->FirstChild()->ToText()->Value();
	ss.write(text.c_str(), text.size());
	uint32 n;
	for (int i = 0; i < nNumTri; i++) {
		char t;
		ss >> n >> pIndex[0] >> pIndex[1] >> pIndex[2] >> t;
		pIndex += 3;
	}
	ss.clear();

	text = pMesh->FirstChild("Position")->FirstChild()->ToText()->Value();
	Vector3f* pPos = (Vector3f*)AddStream(Vertex::USAGE_POSITION, Vertex::TYPE_FLOAT3);
	ss.write(text.c_str(), text.size());
	for (int i = 0; i < nNumVerts; i++) {
		char t;
		ss >> n >> pPos->x >> pPos->y >> pPos->z >> t;
		pPos++;
	}
	ss.clear();

	text = pMesh->FirstChild("Normal")->FirstChild()->ToText()->Value();
	Vector3f* pNormal = (Vector3f*)AddStream(Vertex::USAGE_NORMAL, Vertex::TYPE_FLOAT3);
	ss.write(text.c_str(), text.size());
	for (int i = 0; i < nNumVerts; i++)	{
		char t;
		ss >> n >> pNormal->x >> pNormal->y >> pNormal->z >> t;
		pNormal++;
	}
	ss.clear();

	text = pMesh->FirstChild("Texcoord")->FirstChild()->ToText()->Value();
	Vector2f* pUV = (Vector2f*)AddStream(Vertex::USAGE_TEXCOORD, Vertex::TYPE_FLOAT2);
	ss.write(text.c_str(), text.size());
	for (int i = 0; i < nNumVerts; i++)	{
		char t;
		ss >> n >> pUV->x >> pUV->y >> t;
		pUV++;
	}
	ss.clear();

	if (pMesh->FirstChild("BlendIndex")) {
		text = pMesh->FirstChild("BlendIndex")->FirstChild()->ToText()->Value();
		uint8* pBlendIndex = (uint8*)AddStream(Vertex::USAGE_BLENDINDEX, Vertex::TYPE_UBYTE4_UINT);
		ss.write(text.c_str(), text.size());
		for (int i = 0; i < nNumVerts; i++)	{
			char t;
			ss >> n >> pBlendIndex[0] >> pBlendIndex[1] >> pBlendIndex[2] >> pBlendIndex[3] >> t;
			pBlendIndex += 4;
		}
		ss.clear();
	}

	if (pMesh->FirstChild("BlendWeight")) {
		text = pMesh->FirstChild("BlendWeight")->FirstChild()->ToText()->Value();
		Vector3f* pBlendWeight = (Vector3f*)AddStream(Vertex::USAGE_BLENDWEIGHT, Vertex::TYPE_FLOAT3);
		ss.write(text.c_str(), text.size());
		for (int i = 0; i < nNumVerts; i++)	{
			char t;
			float w;
			ss >> n >> pBlendWeight->x >> pBlendWeight->y >> pBlendWeight->z >> w >> t;
			pBlendWeight++;
		}
		ss.clear();
	}

	mGeometryBuffer = GRenderDevice->CreateGeometryBuffer(this);

	//if (pMesh->FirstChild("BlendIndex")) {
	//	m_pVertexBuffer = CreateVertexBuffer(VertexLayout_PNTTBB);
	//} else {
	//	m_pVertexBuffer = CreateVertexBuffer(VertexLayout_PNTT);
	//}

	return true;
}


void CGeometry::Load(File* file)
{
	unsigned int magic = 0;
	file->Read(magic);
	if (magic != MAGIC_STR("MESH")) {
		GLog->Error("file %s is not a mesh file!", file->Pathname().c_str());
		return;
	}

	int numVertex, numIndex;
	file->Read(numVertex);
	file->Read(numIndex);

	mNumVertex = numVertex;
	mNumIndex = numIndex;

	uint32* pIndex = (uint32*)AddStream(Vertex::USAGE_INDEX, Vertex::TYPE_UINT);
	Vector3f* pPos = (Vector3f*)AddStream(Vertex::USAGE_POSITION, Vertex::TYPE_FLOAT3);
	Vector3f* pNormal = (Vector3f*)AddStream(Vertex::USAGE_NORMAL, Vertex::TYPE_FLOAT3);
	Vector2f* pUV = (Vector2f*)AddStream(Vertex::USAGE_TEXCOORD, Vertex::TYPE_FLOAT2);


	file->Read(magic);
	assert(magic == MAGIC_STR("POSI"));
	file->ReadArray(pPos, numVertex);

	file->Read(magic);
	assert(magic == MAGIC_STR("NORM"));
	file->ReadArray(pNormal, numVertex);

	file->Read(magic);
	assert(magic == MAGIC_STR("TEX0"));
	file->ReadArray(pUV, numVertex);

	file->Read(magic);
	assert(magic == MAGIC_STR("INDX"));
	file->ReadArray(pIndex, numIndex);

	if (file->Read(magic)) {
		assert(magic == MAGIC_STR("ELEM"));
		int numMeshGroup = 0;
		file->Read(numMeshGroup);
		vector<int> meshGroup(numMeshGroup);
		file->ReadArray(&meshGroup[0], numMeshGroup);

		int indexOffset = 0;
		for (size_t i = 0; i < meshGroup.size(); i++) {
			MeshElement elem;
			elem.IndexCount = meshGroup[i] * 3;
			elem.IndexStart = indexOffset;
			//elem.vertexoffset = 0;
			mElements.push_back(elem);

			indexOffset += meshGroup[i] * 3;
		}
	}

	ComputeTangentSpace();
		
	mGeometryBuffer = GRenderDevice->CreateGeometryBuffer(this);
}


bool CGeometry::HasSkin() const {
	bool hasSkin = false;
	for (vector<Stream_t>::const_iterator it = mStreams.begin(); it != mStreams.end(); ++it) {
		if (it->usage == Vertex::USAGE_BLENDINDEX) {
			hasSkin = true;
		}
	}

	return hasSkin;
}

void CGeometry::GetMaterialInfo(set<string>& info) const {
	if (HasSkin()) {
		info.insert("_SKIN_");
	}
}


void CGeometry::PrepareVertexData(vector<int8>& data, const vector<VertexLayoutItem>& layout) const
{
	int32 stride = 0;
	for (auto it = layout.begin(); it != layout.end(); ++it) {
		stride += GetSizeOfType((Vertex::ElemType)it->type);
	}

	data.resize(stride * mNumVertex);
	int32 elementOffset = 0;
	for (auto it = layout.begin(); it != layout.end(); ++it) {
		bool res = AssembleVertexElement(&data[0], elementOffset, stride,
			(Vertex::ElemType)it->type, (Vertex::ElemUsage)it->usage, it->usageIndex);

		if (!res) {
			GLog->Warning("Can not find VertexElement type %d usage %d index %d", 
				(int)it->type, (int)it->usage, it->usageIndex);
		}
		
		elementOffset += GetSizeOfType((Vertex::ElemType)it->type);
	}
}
void CGeometry::PrepareIndexData(vector<int8>& InData) const
{ 
	int streamIndex = FindStream(Vertex::USAGE_INDEX, 0);
	if (streamIndex == -1) {
		GLog->Error("CreateIndexBuffer: can not find index buffer stream !");
		return;
	}

	assert(mStreams[streamIndex].nStride == 4);

	auto stream = mStreams[streamIndex];
	InData.resize(stream.nSizeInByte);

	Util::MemCopy(stream.Ptr(), InData.data(), stream.nSizeInByte);
}


void CGeometry::OnReset() {
}

void CGeometry::OnLost() {
}



struct Triangle
{
	uint32  index[3];
};


void CalculateTangentArray(long vertexCount, const Vector3f *vertex, const Vector3f *normal,
						   const Vector2f *texcoord, long triangleCount, const Triangle *triangle, Vector4f *tangent)
{
	Vector3f *tan1 = new Vector3f[vertexCount * 2];
	Vector3f *tan2 = tan1 + vertexCount;
	ZeroMemory(tan1, vertexCount * sizeof(Vector3f) * 2);

	for (long a = 0; a < triangleCount; a++)	{
		long i1 = triangle->index[0];
		long i2 = triangle->index[1];
		long i3 = triangle->index[2];

		const Vector3f& v1 = vertex[i1];
		const Vector3f& v2 = vertex[i2];
		const Vector3f& v3 = vertex[i3];

		const Vector2f& w1 = texcoord[i1];
		const Vector2f& w2 = texcoord[i2];
		const Vector2f& w3 = texcoord[i3];

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

		tan1[i1] += sdir;
		tan1[i2] += sdir;
		tan1[i3] += sdir;

		tan2[i1] += tdir;
		tan2[i2] += tdir;
		tan2[i3] += tdir;

		triangle++;
	}

	for (long a = 0; a < vertexCount; a++)	{
		const Vector3f& n = normal[a];
		const Vector3f& t = tan1[a];

		// Gram-Schmidt orthogonalize
		Vector3f tmp = t - n * Vector3f::Dot(n, t);
		tmp.Normalize();
		tangent[a].x = tmp.x;
		tangent[a].y = tmp.y;
		tangent[a].z = tmp.z;

		// Calculate handedness
		tangent[a].w = (Vector3f::Dot(Vector3f::Cross(n, t), tan2[a]) < 0.0f) ? -1.0f : 1.0f;
	}

	delete[] tan1;
}

void CGeometry::ComputeTangentSpace()
{
	assert(GetNumStreamOfUsage(Vertex::USAGE_TANGENT) == 0);

	uint8* pData = AddStream(Vertex::USAGE_TANGENT, Vertex::TYPE_FLOAT4);

	int posStream = FindStream(Vertex::USAGE_POSITION, 0);
	int normStream = FindStream(Vertex::USAGE_NORMAL, 0);
	int uvStream = FindStream(Vertex::USAGE_TEXCOORD, 0);
	int indexStream = FindStream(Vertex::USAGE_INDEX, 0);

	assert(posStream != -1 && normStream != -1 && uvStream != -1);
	assert(mStreams[uvStream].type == Vertex::TYPE_FLOAT2);
	assert(mStreams[indexStream].type == Vertex::TYPE_UINT);

	CalculateTangentArray(GetNumVertex(), 
		(const Vector3f *)mStreams[posStream].Ptr(), 
		(const Vector3f *)mStreams[normStream].Ptr(),
		(const Vector2f *)mStreams[uvStream].Ptr(), 
		GetNumTri(), 
		(const Triangle *)mStreams[indexStream].Ptr(),
		(Vector4f *)pData);

}



//
//
///*
//===================
//ComputeNormal
//===================
//*/
//void CGeometry::ComputeNormal()
//{
//	int iIndexStream	= GetStreamIndex(USAGE_INDEX);
//	int iPosStream		= GetStreamIndex(USAGE_POSITION);
//	if (iIndexStream == -1 || iPosStream == -1)
//		return;
//
//	int stride = m_Streams[iIndexStream].nBytePerElem;
//	int triCount = m_nNumIndex / 3;
//
//	std::vector<VertexInfo>	vertInfo(m_nNumVert);
//	std::vector<Vector3f>	faceNormal(triCount);
//	if (stride == 2)
//	{
//		GetVertexInfo((uint16*)GetStreamBuffer(iIndexStream), &vertInfo[0], triCount);
//		ComputeFaceNormal((uint16*)GetStreamBuffer(iIndexStream), &faceNormal[0],
//			(Vector3f*)GetStreamBuffer(iPosStream), triCount);
//	}
//	else
//	{
//		GetVertexInfo((uint32*)GetStreamBuffer(iIndexStream), &vertInfo[0], triCount);
//		ComputeFaceNormal((uint32*)GetStreamBuffer(iIndexStream), &faceNormal[0],
//			(Vector3f*)GetStreamBuffer(iPosStream), triCount);
//	}
//
//	Vector3f* pNormal = (Vector3f*)AddStream(USAGE_NORMAL, sizeof(Vector3f));
//	Vector3f normal;
//	for (uint32 i = 0; i < m_nNumVert; i++)
//	{
//		normal = Vector3f::ZERO;
//		for (int j = 0; j < vertInfo[i].nAdjCount; j++)
//		{
//			normal += faceNormal[vertInfo[i].aAdjTri[j]];
//		}
//		normal.Normalize();
//		pNormal[i] = normal;
//	}
//
//}
//
///*
//===================
//ComputeTangent
//===================
//*/
//void CGeometry::ComputeTangent()
//{
//}
//
//
//
//void CGeometry::ComputeAABB()
//{
//	Vector3f* pPos = (Vector3f*)GetStreamBufferByUsage(USAGE_POSITION);
//	if (pPos == nullptr)
//	{
//		return;
//	}
//	m_AABB.Reset();
//
//	for (uint32 i = 0; i < GetNumVertex(); i++)
//	{
//		m_AABB.AddPoint(pPos[i]);
//	}
//}

//
//
float TriangleArea(const Vector3f& a, const Vector3f& b, const Vector3f& c)
{
	Vector3f ab = b - a;
	Vector3f ac = c - a;

	return 0.5f * Vector3f::Cross(ab, ac).Length();
}
//
//
//float CGeometry::GetSurfaceArea()
//{
//
//	int nTri = GetNumTri();
//	Vector3f* pPos = (Vector3f*)GetStreamBufferByUsage(USAGE_POSITION);
//	uint16* pIndex = (uint16*)GetStreamBufferByUsage(USAGE_INDEX);
//
//	float surfaceArea = 0.0f;
//	for (int i = 0; i < nTri; i++)
//	{
//		surfaceArea += TriangleArea(pPos[pIndex[i * 3 + 0]],
//			pPos[pIndex[i * 3 + 1]],
//			pPos[pIndex[i * 3 + 2]]);
//	}
//	return surfaceArea;
//}
//
//
//
//GeometryDataPtr CGeometry::AssembleSubmesh(const std::set<uint16>& vertSet, uint32 triOffset, uint32 nNumTri)
//{
//	std::vector<uint16> vertArray;
//	std::map<uint16, uint16> indexMap;
//
//	for (std::set<uint16>::const_iterator it = vertSet.begin();
//		it != vertSet.end(); ++it)
//	{
//		vertArray.push_back(*it);
//	}
//
//	uint32 nNumVert = (uint32)vertArray.size();
//	for (uint32 i = 0; i < nNumVert; i++)
//	{
//		indexMap.insert(std::make_pair(vertArray[i], i));
//	}
//
//	GeometryDataPtr pSubmesh(new CGeometry(nNumVert, nNumTri));
//
//
//	Vector3f* pSrcPos = (Vector3f*)GetStreamBufferByUsage(USAGE_POSITION);
//	Vector3f* pSrcNormal = (Vector3f*)GetStreamBufferByUsage(USAGE_NORMAL);
//	Vector2f* pSrcUV = (Vector2f*)GetStreamBufferByUsage(USAGE_TEXCOORD);
//	uint16* pSrcIndex = (uint16*)GetStreamBufferByUsage(USAGE_INDEX);
//
//
//
//	Vector3f* pDesPos = (Vector3f*)pSubmesh->AddStream(CGeometry::USAGE_POSITION, sizeof(Vector3f));
//	Vector3f* pDesNormal = (Vector3f*)pSubmesh->AddStream(CGeometry::USAGE_NORMAL, sizeof(Vector3f));
//	Vector2f* pDesUV = (Vector2f*)pSubmesh->AddStream(CGeometry::USAGE_TEXCOORD, sizeof(Vector2f));
//	uint16* pDesIndex = (uint16*)pSubmesh->AddStream(CGeometry::USAGE_INDEX, 2);
//
//	for (uint32 i = 0; i < nNumVert; i++)
//	{
//		*pDesPos++ = *(pSrcPos + vertArray[i]);
//		*pDesNormal++ = *(pSrcNormal + vertArray[i]);
//		*pDesUV++ = *(pSrcUV + vertArray[i]);
//	}
//
//	pSrcIndex += 3 * triOffset;
//	for (uint32 i = 0; i < nNumTri * 3; i++)
//	{
//		*pDesIndex++ = indexMap[*pSrcIndex++];
//	}
//
//	return pSubmesh;
//}
//
//
//std::vector<GeometryDataPtr> CGeometry::SplitMesh(float areaThreshold)
//{
//	std::vector<GeometryDataPtr> subMeshes;
//	if (GetSurfaceArea() <= areaThreshold)
//	{
//		return subMeshes;
//	}
//
//	int nTri = GetNumTri();
//	Vector3f* pPos = (Vector3f*)GetStreamBufferByUsage(USAGE_POSITION);
//	uint16* pIndex = (uint16*)GetStreamBufferByUsage(USAGE_INDEX);
//
//	std::set<uint16> vertSet;
//
//	for (int i = 0; i < nTri;)
//	{
//		float surfaceArea = 0.0f;
//		int Offset = i;
//		int nNumTri = 0;
//		vertSet.clear();
//		while (i < nTri) 
//		{
//			float faceArea = TriangleArea(pPos[pIndex[i * 3 + 0]],
//				pPos[pIndex[i * 3 + 1]],
//				pPos[pIndex[i * 3 + 2]]);
//
//			if (surfaceArea + faceArea > areaThreshold)
//			{
//				break;
//			}
//
//			surfaceArea +=faceArea;
//			vertSet.insert(pIndex[i * 3 + 0]);
//			vertSet.insert(pIndex[i * 3 + 1]);
//			vertSet.insert(pIndex[i * 3 + 2]);
//
//			nNumTri++;
//			i++;
//		}
//
//		GeometryDataPtr pNewSubmesh = AssembleSubmesh(vertSet, Offset, nNumTri);
//		subMeshes.push_back(pNewSubmesh);
//
//	}
//	return subMeshes;
//	
//}
//
//
//


//-------------------------------------------------------------------------------
CGeometry* StandardMesh::CreatePlane(int nRow, int nCol, const Matrix4f &matTrans)
{
	int		nVert	= (nCol + 1) * (nRow + 1);
	int		nTri	= nCol * nRow * 2;

	CGeometry* pMesh = new CGeometry(nVert, nTri);
	assert(pMesh != nullptr);

	Vector3f* pPos		= (Vector3f*)pMesh->AddStream(Vertex::USAGE_POSITION, Vertex::TYPE_FLOAT3);
	Vector3f* pNormal	= (Vector3f*)pMesh->AddStream(Vertex::USAGE_NORMAL, Vertex::TYPE_FLOAT3);
	Vector2f* pUV		= (Vector2f*)pMesh->AddStream(Vertex::USAGE_TEXCOORD, Vertex::TYPE_FLOAT2);

	// setup vertex
	float yStep = 1.0f / (float)nRow;
	float xStep = 1.0f / (float)nCol;
	float uStep = 1.0f / (float)nCol;
	float vStep = 1.0f / (float)nRow;
	float y =  0.5f;

	Vector3f normal(0.0f, 0.0f, 1.0f);
	Vector3Rotate(normal, normal, matTrans);
	// in case there is scale in transform
	normal.Normalize();	

	for (int i = 0; i <= nRow; i++, y -= yStep)
	{
		float x = -0.5f;
		for (int j = 0; j <= nCol; j++, x += xStep)
		{
			(pPos++)->Set(x, y, 0.0f);
			*(pNormal++) = normal;
			(pUV++)->Set((float)j*uStep, (float)i*vStep);
		}
	}
	// move to the begin of the buffer
	pPos -= nVert;
	Vector3TransformArray(pPos, sizeof(Vector3f), 
		pPos, sizeof(Vector3f), nVert, matTrans);

	uint16* pIndex = (uint16*)pMesh->AddStream(Vertex::USAGE_INDEX, Vertex::TYPE_USHORT_UINT);
	uint16 v0, v1, v2, v3;
	//---------------------
	//		v1 - v3
	//		|  \ |
	//		v0 - v2
	//-------------------
	for (int i = 0; i < nRow; i++)
	{
		for (int j = 0; j < nCol; j++)
		{
			v1 = i * (nCol+1) + j;
			v3 = v1 + 1;
			v0 = v1 + nCol + 1;
			v2 = v3 + nCol + 1;
			(*pIndex++) = v0;
			(*pIndex++) = v2;
			(*pIndex++) = v1;
			(*pIndex++) = v1;
			(*pIndex++) = v2;
			(*pIndex++) = v3;
		}
	}

	return pMesh;
}



CGeometry* StandardMesh::CreateBox(bool bInside, const Matrix4f& matTrans)
{
	CGeometry* pMesh = new CGeometry(8, 12);
	assert(pMesh != nullptr);

	Vector3f* pPos		= (Vector3f*)pMesh->AddStream(Vertex::USAGE_POSITION, Vertex::TYPE_FLOAT3);
	Vector3f* pNormal	= (Vector3f*)pMesh->AddStream(Vertex::USAGE_NORMAL, Vertex::TYPE_FLOAT3);

	pPos[0].Set(-0.5f,  0.5f,  0.5f);
	pPos[1].Set(-0.5f,  0.5f, -0.5f);
	pPos[2].Set( 0.5f,  0.5f, -0.5f);
	pPos[3].Set( 0.5f,  0.5f,  0.5f);

	pPos[4].Set(-0.5f, -0.5f,  0.5f);
	pPos[5].Set(-0.5f, -0.5f, -0.5f);
	pPos[6].Set( 0.5f, -0.5f, -0.5f);
	pPos[7].Set( 0.5f, -0.5f,  0.5f);

	Vector3TransformArray(pPos, sizeof(Vector3f), pPos, sizeof(Vector3f), 8, matTrans);

	uint16* pIndex = (uint16*)pMesh->AddStream(Vertex::USAGE_INDEX, Vertex::TYPE_USHORT_UINT);
	//top
	pIndex[0] = 1;	pIndex[1] = 0;	pIndex[2] = 3;
	pIndex[3] = 1;	pIndex[4] = 3;	pIndex[5] = 2;
	//front
	pIndex[6] = 0;	pIndex[7] = 4;	pIndex[8] = 7;
	pIndex[9] = 0;	pIndex[10] = 7;	pIndex[11] = 3;
	//left
	pIndex[12] = 1;	pIndex[13] = 5;	pIndex[14] = 4;
	pIndex[15] = 1;	pIndex[16] = 4;	pIndex[17] = 0;
	//right
	pIndex[18] = 2;	pIndex[19] = 3;	pIndex[20] = 7;
	pIndex[21] = 2;	pIndex[22] = 7;	pIndex[23] = 6;
	//back
	pIndex[24] = 1;	pIndex[25] = 2;	pIndex[26] = 6;
	pIndex[27] = 1;	pIndex[28] = 6;	pIndex[29] = 5;
	//bottom
	pIndex[30] = 5;	pIndex[31] = 6;	pIndex[32] = 7;
	pIndex[33] = 5;	pIndex[34] = 7;	pIndex[35] = 4;

	

	if (bInside)
	{
		FlipTriangleOrder((uint8*)pIndex, 12, Vertex::TYPE_USHORT_UINT);
	}

	ComputeNormal(pNormal, (uint8*)pIndex, 12, Vertex::TYPE_USHORT_UINT, pPos, 8);

	return pMesh;
}

CGeometry* StandardMesh::CreateSphere(uint32 slice, uint32 stack, bool bInside, const Matrix4f& matTrans)
{
	int nVert = (stack - 1) * (slice + 1) + 2;	// the first and last point in a stack take the same position
	int nTri  = 2 * slice * (stack - 1);

	CGeometry* pMesh = new CGeometry(nVert, nTri);
	assert(pMesh != nullptr);

	Vector3f* pPos		= (Vector3f*)pMesh->AddStream(Vertex::USAGE_POSITION, Vertex::TYPE_FLOAT3);
	Vector3f* pNormal	= (Vector3f*)pMesh->AddStream(Vertex::USAGE_NORMAL, Vertex::TYPE_FLOAT3);
	Vector2f* pUV		= (Vector2f*)pMesh->AddStream(Vertex::USAGE_TEXCOORD, Vertex::TYPE_FLOAT2);


	pPos[0].Set(0.0f, 1.0f, 0.0f);
	Vector3Rotate(pNormal[0], pPos[0], matTrans);
	pNormal[0].Normalize();
	pUV[0].Set(0.5f, 0.0f);

	pPos[nVert-1].Set(0.0f, -1.0f, 0.0f);
	Vector3Rotate(pNormal[nVert-1], pPos[nVert-1], matTrans);
	pNormal[nVert-1].Normalize();
	pUV[nVert-1].Set(0.5f, 1.0f);

	// save sin and cos value in one circle
	std::vector<float> vSin(slice + 1);
	std::vector<float> vCos(slice + 1);
	float fAngle = Mathf::PI * 2.0f / (float)(slice);
	// draw in right hand order(count clock)
	for (uint32 i = 0; i <= slice; i++)
	{
		vSin[slice-i] = Mathf::Sin(i * fAngle);
		vCos[slice-i] = Mathf::Cos(i * fAngle);
	}

	int	iVert = 1;
	fAngle = Mathf::PI / (float)(stack);
	for (uint32 i = 1; i < stack; i++)
	{
		// radius of circle in one stack;
		float radius = Mathf::Sin(i * fAngle);
		float y = Mathf::Cos(i * fAngle);
		float v = i / (float)(stack);
		for (uint32 j = 0; j <= slice; j++)
		{
			pPos[iVert].Set(radius * vCos[j], y, radius * vSin[j]);
			Vector3Rotate(pNormal[iVert], pPos[iVert], matTrans);
			pNormal[iVert].Normalize();
			pUV[iVert].Set(j / (float)(slice), v);
			iVert++;
		}
	}

	Vector3TransformArray(pPos, sizeof(Vector3f), pPos, sizeof(Vector3f), nVert, matTrans);

	// setup index
	uint16* pIndex = (uint16*)pMesh->AddStream(Vertex::USAGE_INDEX, Vertex::TYPE_USHORT_UINT);
	int iIndex = 0;
	for (uint32 i = 0; i < slice; i++)
	{
		pIndex[iIndex++] = 0;
		pIndex[iIndex++] = i + 1;
		pIndex[iIndex++] = i + 2;
	}

	int v0, v1, v2, v3;
	//---------------------
	//		v0 - v3
	//		|  \ |
	//		v1 - v2
	//-------------------
	for (uint32 i = 0; i < stack - 2; i++)
	{
		v0 = i * (slice + 1) + 1;
		v3 = v0 + 1;
		v1 = v0 + slice + 1;
		v2 = v1 + 1;
		for (uint32 j = 0; j < slice; j++)
		{
			pIndex[iIndex++] = v0 + j;
			pIndex[iIndex++] = v1 + j;
			pIndex[iIndex++] = v2 + j;
			pIndex[iIndex++] = v0 + j;
			pIndex[iIndex++] = v2 + j;
			pIndex[iIndex++] = v3 + j;
		}
	}

	v0 = (stack - 2) * (slice + 1) + 1;
	for (uint32 i = 0; i < slice; i++)
	{
		pIndex[iIndex++] = v0 + i;
		pIndex[iIndex++] = nVert - 1;
		pIndex[iIndex++] = v0 + i + 1;
	}

	if (bInside)
	{
		FlipTriangleOrder((uint8*)pIndex, nTri, Vertex::TYPE_USHORT_UINT);
	}

	return pMesh;	
}
//
//void IRenderDevice::BuildScreenQuad()
//{
//	Vector4f pos[4];
//	Vector2f uv[4];
//
//	pos[0].Set(-1.0f,  1.0f, 0.0f, 0.0f);
//	pos[1].Set( 1.0f,  1.0f, 0.0f, 1.0f);
//	pos[2].Set( 1.0f, -1.0f, 0.0f, 2.0f);
//	pos[3].Set(-1.0f, -1.0f, 0.0f, 3.0f);
//
//	float offsetX = 0.5f / (float)m_nWidth;
//	float offsetY = 0.5f / (float)m_nHeight;
//	uv[0].Set(0.0f + offsetX, 0.0f + offsetY);
//	uv[1].Set(1.0f + offsetX, 0.0f + offsetY);
//	uv[2].Set(1.0f + offsetX, 1.0f + offsetY);
//	uv[3].Set(0.0f + offsetX, 1.0f + offsetY); 
//
//	float* pointer = m_pScreenQuadVB.get();
//	for (int i = 0; i < 4; i++)
//	{
//		memcpy(pointer, pos[i].Ptr(), sizeof(Vector4f));
//		pointer += 4;
//		memcpy(pointer, uv[i].Ptr(), sizeof(Vector2f));
//		pointer += 2;
//	}
//
//	uint16 index[] = {0, 2, 1, 0, 3, 2};
//	memcpy(m_pScreenQuadIB.get(), index, sizeof(index));
//}




MeshManager GMeshManager;

void MeshManager::Initialize()
{
}

void MeshManager::Finalize()
{
}


MeshManager::MeshManager()
{

}

MeshManager::~MeshManager()
{

}

CGeometry* MeshManager::GetMesh(const ResourceID& id)
{
	CGeometry* pMesh = Find(id);
	if (pMesh == nullptr)
	{
		return Load(id);
	}
	return pMesh;
}

CGeometry* MeshManager::Find(const ResourceID& id)
{
	map<ResourceID, CGeometry*>::iterator it = m_ResourcePool.find(id);
	if (it != m_ResourcePool.end())
	{
		return it->second;
	}
	return nullptr;
}

CGeometry* MeshManager::Load(const ResourceID& id)
{
	string pathName = Util::ProcessPathName(id);

	CGeometry* pMesh = new CGeometry();

	string ext = Util::GetFileNameExt(pathName);
	if (ext == "xms") {
		TiXmlDocument doc;
		bool loadOK = doc.LoadFile(pathName.c_str());
		assert(loadOK);

		pMesh->Load(doc.FirstChild()->ToElement());
	}

	if (ext == "mesh") {
		FilePtr file(GFileSys->OpenFile(pathName));
		if (!file) {
			// todo default mesh
			return pMesh;
		}
		pMesh->Load(file.get());

	}


	return pMesh;
}

}// end of namespace