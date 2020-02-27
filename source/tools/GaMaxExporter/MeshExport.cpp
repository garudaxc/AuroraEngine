#include "3dsmaxsdk_preinclude.h"
#include "MaxExporter.h"
#include <string>
#include <vector>
#include <set>
#include <map>
using namespace std;



struct VertInfo
{
	vector<uint>	indeics;
	uint			vertIdx;
};


struct VertInfoComp : public binary_function<VertInfo, VertInfo, bool>
{	// functor for operator<
	bool operator()(const VertInfo& _Left, const VertInfo& _Right) const
	{	// apply operator< to operands
		return (_Left.indeics < _Right.indeics);
	}
};


void ExportMesh(IGameMesh* pMesh, TiXmlElement* pXmlMesh, const Matrix3& objTM)
{

	pMesh->SetCreateOptimizedNormalList();
	pMesh->InitializeData();

	int nNumFace = pMesh->GetNumberOfFaces();
	vector<uint> indeics(nNumFace * 3);

	int nNumVert = pMesh->GetNumberOfVerts();
	vector<Point3> verts(nNumVert);
	for (int i = 0; i < nNumVert; i++)
	{
		Point3 pos;
		pMesh->GetVertex(i, pos, true);
		verts[i] = objTM.PointTransform(pos);
	}

	int nNumNormal = pMesh->GetNumberOfNormals();
	vector<Point3> normals(nNumNormal);
	for (int i = 0; i < nNumNormal; i++)
	{
		Point3 normal;
		pMesh->GetNormal(i, normal, true);
		normals[i] = objTM.VectorTransform(normal);
	}

	int nNumTexVerts = pMesh->GetNumberOfTexVerts();
	vector<Point2> texVerts(nNumTexVerts);
	for (int i = 0; i < nNumTexVerts; i++)
	{
		pMesh->GetTexVertex(i, texVerts[i]);
	}


	vector<set<VertInfo, VertInfoComp>> vertexMap(nNumVert);

	int index = 0;
	uint expVertIdx = 0;
	for (int i = 0; i < nNumFace; i++)
	{
		FaceEx* f = pMesh->GetFace(i);

		for (int j = 0; j < 3; j++)
		{
			uint vertIdx = f->vert[j];
			uint normalIdx = f->norm[j];
			uint texVert = f->texCoord[j];

			VertInfo info;
			info.indeics.push_back(normalIdx);
			info.indeics.push_back(texVert);

			pair<set<VertInfo, VertInfoComp>::iterator, bool> res = vertexMap[vertIdx].insert(info);

			if (res.second)
			{
				res.first->vertIdx = expVertIdx++;
			}

			indeics[index++] = res.first->vertIdx;
		}
	}
	uint nNumExpVert = expVertIdx;

	vector<uint> expPos(nNumExpVert);
	vector<uint> expNorms(nNumExpVert);
	vector<uint> expTexs(nNumExpVert);

	for (int i = 0; i < nNumVert; i++)
	{
		for (set<VertInfo, VertInfoComp>::iterator it = vertexMap[i].begin();
			it != vertexMap[i].end(); ++it)
		{
			uint idx = it->vertIdx;
			expPos[idx]		= i;
			expNorms[idx]	= it->indeics[0];
			expTexs[idx]	= it->indeics[1];
		}
	}


	pXmlMesh->SetAttribute("numVerts", nNumExpVert);
	pXmlMesh->SetAttribute("numTri", nNumFace);


	string val;
	char buff[128];
	for (uint i = 0; i < indeics.size(); i += 3)
	{
		sprintf_s(buff, sizeof(buff), "%d %d %d %d|", i / 3, indeics[i], indeics[i+1], indeics[i+2]);
		val += buff;
	}
	pXmlMesh->LinkEndChild(new TiXmlElement("Index"))->LinkEndChild(new TiXmlText(val.c_str()));



	val = "";
	for (uint i = 0; i < nNumExpVert; i++)
	{
		Point3 pos = verts[expPos[i]];
		sprintf_s(buff, sizeof(buff), "%d %.3f %.3f %.3f|", i, pos.x, pos.y, pos.z);
		val += buff;
	}
	pXmlMesh->LinkEndChild(new TiXmlElement("Position"))->LinkEndChild(new TiXmlText(val.c_str()));


	val = "";
	for (uint i = 0; i < nNumExpVert; i++)
	{
		Point3 n = normals[expNorms[i]];
		sprintf_s(buff, sizeof(buff), "%d %.3f %.3f %.3f|", i, n.x, n.y, n.z);
		val += buff;
	}
	pXmlMesh->LinkEndChild(new TiXmlElement("Normal"))->LinkEndChild(new TiXmlText(val.c_str()));


	val = "";
	for (uint i = 0; i < nNumExpVert; i++)
	{
		Point2 tex = texVerts[expTexs[i]];
		// opengl texcoord to d3d texcoord (flip y coord)
		sprintf_s(buff, sizeof(buff), "%d %.3f %.3f|", i, tex.x, 1.0f - tex.y);		
		val += buff;
	}
	pXmlMesh->LinkEndChild(new TiXmlElement("Texcoord"))->LinkEndChild(new TiXmlText(val.c_str()));

	

	// output skin
	IGameSkin* pSkin = pMesh->GetIGameSkin();
	if (pSkin == NULL) {
		return;
	}

	bool bOnlyExportUsedJoint = true;

	assert(pSkin->GetNumOfSkinnedVerts() == nNumVert);

	val = "";
	for (uint i = 0; i < pSkin->GetNumOfSkinnedVerts(); i++)
	{
		int blendIndex[4];
		memset(blendIndex, 0, sizeof(blendIndex));

		assert(pSkin->GetNumberOfBones(i) <= 4);
		for (int j = 0; j < pSkin->GetNumberOfBones(i); j++) {
			IGameNode* pBone = pSkin->GetIGameBone(i, j);
			blendIndex[j] = pSkin->GetBoneIndex(pBone, bOnlyExportUsedJoint);
		}

		sprintf_s(buff, sizeof(buff), "%d %d %d %d %d|", i, blendIndex[0], blendIndex[1], blendIndex[2], blendIndex[3]);
		val += buff;
	}
	pXmlMesh->LinkEndChild(new TiXmlElement("BlendIndex"))->LinkEndChild(new TiXmlText(val.c_str()));

	val = "";
	for (uint i = 0; i < pSkin->GetNumOfSkinnedVerts(); i++)
	{
		float blendWeight[4];
		memset(blendWeight, 0, sizeof(blendWeight));

		assert(pSkin->GetNumberOfBones(i) <= 4);
		for (int j = 0; j < pSkin->GetNumberOfBones(i); j++) {
			blendWeight[j] = pSkin->GetWeight(i, j);
		}

		sprintf_s(buff, sizeof(buff), "%d %.3f %.3f %.3f %.3f|", i, blendWeight[0], blendWeight[1], blendWeight[2], blendWeight[3]);
		val += buff;
	}
	pXmlMesh->LinkEndChild(new TiXmlElement("BlendWeight"))->LinkEndChild(new TiXmlText(val.c_str()));


	TiXmlElement* pXmlJoints = pXmlMesh->LinkEndChild(new TiXmlElement("Joints"))->ToElement();
	pXmlJoints->SetAttribute("TotalJointCount", pSkin->GetTotalBoneCount());
	pXmlJoints->SetAttribute("TotalSkinJointCount", pSkin->GetTotalSkinBoneCount());


	int numExportBone = 0;
	if (bOnlyExportUsedJoint) {
		numExportBone = pSkin->GetTotalBoneCount();
	} 
	else {
		numExportBone = pSkin->GetTotalSkinBoneCount();
	}

	for (int i = 0; i < numExportBone; i++) {
		IGameNode* pBone = pSkin->GetIGameBone(i, bOnlyExportUsedJoint);
		sprintf_s(buff, sizeof(buff), "%d %s", i, pBone->GetName());

		TiXmlElement* pXml = pXmlJoints->LinkEndChild(new TiXmlElement("Joint"))->ToElement();
		pXml->SetAttribute("id", pBone->GetNodeID());
		pXml->LinkEndChild(new TiXmlText(buff));
	}

}





//void LoadNode(vector<JointInfo>& jointList, FCDSceneNode* pNode, int& index, int parent)
//{
//	JointInfo& joint = jointList[index];
//	joint.name = pNode->GetName();
//	joint.parent = parent;
//	memcpy(joint.transform, pNode->ToMatrix(), sizeof(float) * 16);
//
//	int thisIndex = index;
//	for (int i = 0; i < (int)pNode->GetChildrenCount(); i++) {
//		index++;
//		LoadNode(jointList, pNode->GetChild(i), index, thisIndex);
//	}
//}


int GetNumAllBone(IGameNode* pBone)
{
	int n = 1;
	for (int i = 0; i < pBone->GetChildCount(); i++) {
		n += GetNumAllBone(pBone->GetNodeChild(i));
	}

	return n;
}

void ExportBone(IGameNode* pBone, TiXmlElement* pXmlSkeleton, int& index, int parent, vector<AnimInfo>& animInfo)
{
	// init controller
	pBone->GetIGameObject();
	pBone->ReleaseIGameObject();

	const char* pNodeName = ValidateName(pBone->GetName());
	TiXmlElement* pXmlBone = LinkNewXmlElement(pXmlSkeleton, pNodeName);

	int thisIndex = index;
	pXmlBone->SetAttribute("id", thisIndex);
	pXmlBone->SetAttribute("parent", parent);

	GMatrix matLocal = pBone->GetLocalTM();
	// max api is left handed, convert to right to export
	Quat qRot = matLocal.Rotation().Inverse();
	Point3 vPos = matLocal.Translation();
	Point3 vScale = matLocal.Scaling();

	char buff[128];
	sprintf_s(buff, sizeof(buff), "%.3f %.3f %.3f", vPos.x, vPos.y, vPos.z);
	LinkNewXmlText(LinkNewXmlElement(pXmlBone, "Position"), buff);

	sprintf_s(buff, sizeof(buff), "%.3f %.3f %.3f %.3f", qRot.x, qRot.y, qRot.z, qRot.w);
	LinkNewXmlText(LinkNewXmlElement(pXmlBone, "Rotation"), buff);

	sprintf_s(buff, sizeof(buff), "%.3f %.3f %.3f", vScale.x, vScale.y, vScale.z);
	LinkNewXmlText(LinkNewXmlElement(pXmlBone, "Scale"), buff);

	// export skeleton animation
	IGameControl* pControl = pBone->GetIGameControl();
	if (pControl) {
		AnimInfo info;
		info.nodeName = pNodeName;
		info.control = pControl;
		animInfo.push_back(info);
	}

	for (int i = 0; i < pBone->GetChildCount(); i++)
	{
		index++;
		IGameNode* pChildNode = pBone->GetNodeChild(i);
		ExportBone(pChildNode, pXmlSkeleton, index, thisIndex, animInfo);
	}
}


void ExportSkeleton(IGameNode* pBone, TiXmlElement* pXmlSkeleton, vector<AnimInfo>& animInfo)
{
	int index = 0;
	ExportBone(pBone, pXmlSkeleton, index, -1, animInfo);
}