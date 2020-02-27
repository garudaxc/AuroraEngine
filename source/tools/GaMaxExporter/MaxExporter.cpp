//**************************************************************************/
// Copyright (c) 1998-2007 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
// DESCRIPTION: Appwizard generated plugin
//				3DXI file exporter project template
//				For a more in-depth exemple of a 3DXI exporter,
//				please refer to maxsdk\samples\igame\export.
// AUTHOR:		Jean-Francois Yelle - created Mar.20.2007
//***************************************************************************/

#include "MaxExporter.h"

#include "3dsmaxsdk_preinclude.h"
#include <string>
#include <vector>
#include <map>
#include <set>
#include "tinyxml.h"

using namespace std;


#define GaMaxExporter_CLASS_ID	Class_ID(0x937a083d, 0x6d985d16)

class GaMaxExporter : public SceneExport 
{
	public:
		
		virtual int				ExtCount();					// Number of extensions supported
		virtual const TCHAR *	Ext(int n);					// Extension #n (i.e. "3DS")
		virtual const TCHAR *	LongDesc();					// Long ASCII description (i.e. "Autodesk 3D Studio File")
		virtual const TCHAR *	ShortDesc();				// Short ASCII description (i.e. "3D Studio")
		virtual const TCHAR *	AuthorName();				// ASCII Author name
		virtual const TCHAR *	CopyrightMessage();			// ASCII Copyright message
		virtual const TCHAR *	OtherMessage1();			// Other message #1
		virtual const TCHAR *	OtherMessage2();			// Other message #2
		virtual unsigned int	Version();					// Version number * 100 (i.e. v3.01 = 301)
		virtual void			ShowAbout(HWND hWnd);		// Show DLL's "About..." box

		virtual BOOL SupportsOptions(int ext, DWORD options);
		virtual int	DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);

		//Constructor/Destructor
		GaMaxExporter();
		virtual ~GaMaxExporter();

		int						GetStartFrame() const		{	return m_nStartFrame;	}
		int						GetEndFrame() const			{	return m_nEndFrame;	}
		float					GetTicksPerFrame() const	{	return m_fTickPerFrame;	}

	protected:
		int GetSceneNodes(INodeTab& i_nodeTab, INode* i_currentNode =NULL);

private:
		void					UpdatePropressBar(const char* pNodeName);

		void					ExportNodes(IGameNode* pNode, TiXmlElement* pXmlNodes);

		void					ExportGlobalInfo(IGameScene * pIgame, Interface * ip, TiXmlElement* pXmlScene);

		TiXmlElement*			m_pXmlMeshes;
		TiXmlElement*			m_pXmlMaterials;
		TiXmlElement*			m_pXmlNodes;

		int						m_nNumTotalNode;
		int						m_nNumExportedNode;

		int						m_nStartFrame;
		int						m_nEndFrame;
		float					m_fTickPerFrame;
};


GaMaxExporter* g_Exporter = NULL;

string FilePath;
string SceneName;

map<string, IGameNode*>	m_meshMap;

vector<AnimInfo>	Animations;



__declspec( dllexport ) int ExportProxy(const TCHAR *name, ExpInterface *ei, Interface *i,BOOL suppressPrompts, DWORD options)
{
	//if (g_Exporter) {
	//	return g_Exporter->DoExport(name, ei, i, suppressPrompts, options);
	//}

	g_Exporter = new GaMaxExporter();
	int nRet = g_Exporter->DoExport(name, ei, i, suppressPrompts, options);

	return nRet;
}


class AuroraMaxExporterClassDesc : public ClassDesc2 
{
public:
	virtual int IsPublic() 							{ return TRUE; }
	virtual void* Create(BOOL /*loading = FALSE*/) 		{ return new GaMaxExporter(); }
	virtual const TCHAR *	ClassName() 			{ return GetString(IDS_CLASS_NAME); }
	virtual SClass_ID SuperClassID() 				{ return SCENE_EXPORT_CLASS_ID; }
	virtual Class_ID ClassID() 						{ return GaMaxExporter_CLASS_ID; }
	virtual const TCHAR* Category() 				{ return GetString(IDS_CATEGORY); }

	virtual const TCHAR* InternalName() 			{ return _T("AuroraMaxExporter"); }	// returns fixed parsable name (scripter-visible name)
	virtual HINSTANCE HInstance() 					{ return hInstance; }					// returns owning module handle
	

};


ClassDesc2* GetGaMaxExporterDesc() { 
	static AuroraMaxExporterClassDesc AuroraMaxExporterDesc;
	return &AuroraMaxExporterDesc; 
}

INT_PTR CALLBACK GaMaxExporterOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
int GetOption(int id);


//--- GaMaxExporter -------------------------------------------------------
GaMaxExporter::GaMaxExporter():m_nStartFrame(0),m_nEndFrame(0),m_fTickPerFrame(0.0f),
m_nNumExportedNode(0),m_nNumTotalNode(0)
{
	g_Exporter = this;
}

GaMaxExporter::~GaMaxExporter() 
{

}

int GaMaxExporter::ExtCount()
{
	// Returns the number of file name extensions supported by the plug-in.
	return 1;
}

const TCHAR *GaMaxExporter::Ext(int n)
{	
	return _T("aur");
}

const TCHAR *GaMaxExporter::LongDesc()
{
	return _T("Aurora Engine Exporter");
}
	
const TCHAR *GaMaxExporter::ShortDesc() 
{
	return _T("Aurora Engine Exporter");
}

const TCHAR *GaMaxExporter::AuthorName()
{
	return _T("Garuda");
}

const TCHAR *GaMaxExporter::CopyrightMessage() 
{	
	return _T("");
}

const TCHAR *GaMaxExporter::OtherMessage1() 
{		
	//TODO: Return Other message #1 if any
	return _T("");
}

const TCHAR *GaMaxExporter::OtherMessage2() 
{		
	//TODO: Return other message #2 in any
	return _T("");
}

unsigned int GaMaxExporter::Version()
{				
	//#pragma message	(TODO("Return Version number * 100 (i.e. v3.01 = 301)"))
	return 10;
}

void GaMaxExporter::ShowAbout(HWND hWnd)
{	
}

BOOL GaMaxExporter::SupportsOptions(int ext, DWORD options)
{
	// TODO Decide which options to support.  Simply return
	// true for each option supported by each Extension 
	// the exporter supports.

	return TRUE;
}

int GaMaxExporter::GetSceneNodes(INodeTab& i_nodeTab, INode* i_currentNode /*=NULL*/)
{
	int i;
	if (i_currentNode == NULL){
		i_currentNode = GetCOREInterface()->GetRootNode();
	}else {
		// IGame will crash 3ds Max if it is initialised with the root node.
	    i_nodeTab.AppendNode(i_currentNode);
	}
	for (i = 0; i < i_currentNode->NumberOfChildren(); i++){
		GetSceneNodes(i_nodeTab, i_currentNode->GetChildNode(i));
	}
	return i_nodeTab.Count();
}

//////////////////////////////////////////////////////////////////////////

void GaMaxExporter::UpdatePropressBar(const char* pNodeName)
{
	int progress = (int)((m_nNumExportedNode * 100.0f) / (float)m_nNumTotalNode);

	MCHAR buf[128];
	wsprintf(buf, _T("Processing: %s"), pNodeName);

	GetCOREInterface()->ProgressUpdate(progress, FALSE, buf); 
}


//////////////////////////////////////////////////////////////////////////



bool ExportKeyFrameController(IGameControl* pGameControl, TiXmlElement* pXmlController, IGameControlType type);

void ExportController(IGameControl* pControl, TiXmlElement* pXmlControllers);

void ExportMaterial(IGameMaterial* pMat, TiXmlElement* pXmlParent);

void ExportMesh(IGameMesh* pMesh, TiXmlElement* pXmlMesh, const Matrix3& objTM);

void ExportSkeleton(IGameNode* pBone, TiXmlElement* pXmlSkeleton, vector<AnimInfo>& animInfo);




const char* UnicodeToMultiByte(const MCHAR* pUnicode)
{
	return pUnicode;
	/*int len = WideCharToMultiByte(CP_ACP, 0, pUnicode, -1, NULL, 0, NULL, NULL);

	string str;
	str.resize(len + 1);
	memset(&str[0], 0, len + 1);

	WideCharToMultiByte (CP_ACP, 0,pUnicode, -1, &str[0], len, NULL,NULL);

	return &str[0];*/
}



// transform max's time to our need (to real time, to frame number, etc.)
float TransformTime(float time)
{
	// 转换到以当前时间轴为起始帧的帧数
	return time / g_Exporter->GetTicksPerFrame() - (float)g_Exporter->GetStartFrame();
}

bool TestKeyFrameExport(int time)
{
	int frame = time / (int)g_Exporter->GetTicksPerFrame();
	return frame >= g_Exporter->GetStartFrame() && frame <= g_Exporter->GetEndFrame();
}





TiXmlElement* LinkNewXmlElement(TiXmlElement* pParent, const char* name)
{
	return (TiXmlElement*)pParent->LinkEndChild(new TiXmlElement(name));
}


TiXmlText* LinkNewXmlText(TiXmlElement* pParent, const char* text)
{
	return (TiXmlText*)pParent->LinkEndChild(new TiXmlText(text));
}


const char* ValidateName(const MCHAR* pName)
{
	static char buffer[256];

	strcpy_s(buffer, sizeof(buffer), UnicodeToMultiByte(pName));
	
	char* cPtr = buffer;
	bool validate = false;
	while(*cPtr){
		if (*cPtr & 0x80){
			sprintf_s(buffer, sizeof(buffer), "名称必须为英文字母 (%s)", pName);
			MessageBoxA(NULL, buffer, "", MB_OK);

			const char* defaultName = "DefaultName";
			return defaultName;
		}

		if (*cPtr >= 'A')
			validate = true;

		if (*cPtr < '0' || (!validate && *cPtr < 'A'))
			*cPtr =  '_';

		cPtr++;
	}

	return buffer;
}


void ExportProperty(IGameProperty * prop, TiXmlElement* pParentElem, bool bExportAnim)
{
	if(!prop)	//fix me NH...
		return;

	char buf[128];
	memset(buf, 0, sizeof(buf));
	if(prop->GetType() == IGAME_FLOAT_PROP)	{
		float f = 0.0f;
		prop->GetPropertyValue(f);
		sprintf_s(buf, sizeof(buf), "%.2f", f);
	}else if(prop->GetType() == IGAME_POINT3_PROP){
		Point3 p; 
		prop->GetPropertyValue(p);
		sprintf_s(buf, sizeof(buf), "%.3f %.3f %.3f", p.x, p.y ,p.z);
	}else if( prop->GetType() == IGAME_POINT4_PROP)	{
		Point4 p;
		prop->GetPropertyValue(p);
		sprintf_s(buf, sizeof(buf), "%.3f %.3f %.3f %.3f", p.x, p.y ,p.z, p.w);
	}else if(prop->GetType()==IGAME_STRING_PROP){
		const TCHAR * b;
		prop->GetPropertyValue(b);
		sprintf_s(buf, sizeof(buf), "%s", b);
	}else if (prop->GetType() == IGAME_INT_PROP){
		int i;
		prop->GetPropertyValue(i);
		sprintf_s(buf, sizeof(buf), "%d", i);
	}

	LinkNewXmlText(pParentElem, buf);



}



const MCHAR* FixFileName(const MCHAR* pName)
{
	static basic_string<MCHAR> file;
	file = pName;
	size_t pos = file.find_last_of(_T('\\'));

	file = file.substr(pos + 1);
	return file.c_str();
}



//////////////////////////////////////////////////////////////////////////

const char* GetLightTypeName(IGameLight::LightType type)
{
	switch (type)
	{
	case IGameLight::IGAME_OMNI:						/*!< Omnidirectional Light*/
		return "Point";
	case IGameLight::IGAME_TSPOT:				/*!< Targeted Spot Light*/
		return "Spot";
	case IGameLight::IGAME_DIR:					/*!< Directional Light*/
		return "Directional";
	case IGameLight::IGAME_FSPOT:				/*!< Free spot Light*/
		return "Spot";
	case IGameLight::IGAME_TDIR:					/*!< Targeted Directional Light*/
		return "Directional";
	case IGameLight::IGAME_UNKNOWN:				/*!< An unknown light type*/
		return NULL;
	}

	return NULL;
}



void ExportLight(IGameLight* pGameLight, TiXmlElement* pXmlLight)
{
	IGameLight::LightType type = pGameLight->GetLightType();
	const char* typeName = GetLightTypeName(type);
	if (typeName == NULL){
		MessageBoxA(NULL, "未知的光源类型", "", MB_OK);
	}

	pXmlLight->SetAttribute("Type", typeName);

	//pXmlLight->SetAttribute("IsTarget", pGameLight->GetLightTarget() ? 1 : 0);

	GenLight* pMaxLight = (GenLight*)pGameLight->GetMaxObject();
	pXmlLight->SetAttribute("CastShadow", pMaxLight->GetShadow() ? 1 : 0);

	Texmap* pProjTex = pMaxLight->GetProjMap();
	if (pProjTex && pProjTex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0x00)){
		string fileName = UnicodeToMultiByte(FixFileName(((BitmapTex *)pProjTex)->GetMapName()));
		pXmlLight->SetAttribute("ProjTex", fileName.c_str());
	}

	ExportProperty(pGameLight->GetLightColor(), LinkNewXmlElement(pXmlLight, "Color"), true);
	ExportProperty(pGameLight->GetLightMultiplier(), LinkNewXmlElement(pXmlLight, "Multiplier"), true);

	if (type == IGameLight::IGAME_OMNI){
		ExportProperty(pGameLight->GetLightAttenStart(), LinkNewXmlElement(pXmlLight, "AttenStart"), false);
		ExportProperty(pGameLight->GetLightAttenEnd(), LinkNewXmlElement(pXmlLight, "AttenEnd"), false);
	}else if (type == IGameLight::IGAME_TSPOT || type == IGameLight::IGAME_FSPOT){
		ExportProperty(pGameLight->GetLightHotSpot(), LinkNewXmlElement(pXmlLight, "HotSpot"), false);
		ExportProperty(pGameLight->GetLightFallOff(), LinkNewXmlElement(pXmlLight, "FallOff"), false);
	}


	TiXmlElement* pXmlControllers = new TiXmlElement("Controller");

	IGameControl* pControl = pGameLight->GetLightColor()->GetIGameControl();
	if (pControl && pControl->IsAnimated(IGAME_POINT3))	{
		TiXmlElement* pXmlColorControl = new TiXmlElement("Color");
		if (ExportKeyFrameController(pControl, pXmlColorControl, IGAME_POINT3))	{
			pXmlControllers->LinkEndChild(pXmlColorControl);
		}else{
			delete pXmlColorControl;
		}
	}

	pControl = pGameLight->GetLightMultiplier()->GetIGameControl();
	if (pControl && pControl->IsAnimated(IGAME_FLOAT)){
		TiXmlElement* pXmlColorControl = new TiXmlElement("Multiplier");
		if (ExportKeyFrameController(pControl, pXmlColorControl, IGAME_FLOAT))	{
			pXmlControllers->LinkEndChild(pXmlColorControl);
		}else{
			delete pXmlColorControl;
		}
	}

	if (pXmlControllers->FirstChild()){
		pXmlLight->LinkEndChild(pXmlControllers);
	}else{
		delete pXmlControllers;
	}
}




void ExportCamera(IGameCamera* pCamera, TiXmlElement* pXmlCamera)
{
	bool IsTarget = false;
	IGameNode* pTarget = pCamera->GetCameraTarget();

	pXmlCamera->SetAttribute("IsTarget", pTarget != NULL ? 1 : 0);
	if (pTarget != NULL){
		pXmlCamera->SetAttribute("TargetNode", ValidateName(pTarget->GetName()));
	}

	ExportProperty(pCamera->GetCameraFOV(), LinkNewXmlElement(pXmlCamera, "FOV"), false);
	ExportProperty(pCamera->GetCameraNearClip(), LinkNewXmlElement(pXmlCamera, "NearPlane"), false);
	ExportProperty(pCamera->GetCameraFarClip(), LinkNewXmlElement(pXmlCamera, "FarPlane"), false);
	ExportProperty(pCamera->GetCameraTargetDist(), LinkNewXmlElement(pXmlCamera, "TargetDist"), false);
}



//////////////////////////////////////////////////////////////////////////



string ParsePoint3(const Point3& point)
{
	char buff[128];
	sprintf_s(buff, sizeof(buff), "%.3f %.3f %.3f", point.x, point.y, point.z);
	return string(buff);
}


void ExportSpline3D(IGameSpline3D* pSpline, TiXmlElement* pXmlSpline)
{
	int nNumKnot = pSpline->GetIGameKnotCount();
	pXmlSpline->SetAttribute("NumKnot", nNumKnot);

	for (int i = 0; i < nNumKnot; i++){
		IGameKnot* pKnot = pSpline->GetIGameKnot(i);
		IGameKnot::KnotType type = pKnot->GetKnotType();
		const char* typeName = NULL;
		switch (type)	{
		case IGameKnot::KNOT_AUTO:
			typeName = "Auto";
			break;
		case IGameKnot::KNOT_CORNER:
			typeName = "Corner";
			break;
		case IGameKnot::KNOT_BEZIER:
			typeName = "Bezier";
			break;
		case IGameKnot::KNOT_BEZIER_CORNER:
			typeName = "BezierCorner";
			break;
		}

		TiXmlElement* pXmlKnot = LinkNewXmlElement(pXmlSpline, "Knot");
		pXmlKnot->SetAttribute("type", typeName);
		pXmlKnot->SetAttribute("Point", ParsePoint3(pKnot->GetKnotPoint()).c_str());
		pXmlKnot->SetAttribute("InVec", ParsePoint3(pKnot->GetInVec()).c_str());
		pXmlKnot->SetAttribute("OutVec", ParsePoint3(pKnot->GetOutVec()).c_str());
	}
}


void ExportShape(IGameSpline* pShape, TiXmlElement* pXmlShape)
{
	int nNumSpline = pShape->GetNumberOfSplines();
	pXmlShape->SetAttribute("NumSplines", nNumSpline);

	for (int i = 0; i < nNumSpline; i++){
		IGameSpline3D* pSpline3D = pShape->GetIGameSpline3D(i);
		ExportSpline3D(pSpline3D, LinkNewXmlElement(pXmlShape, "Spline"));
	}
}

void ExportTransform(IGameNode* pNode, TiXmlElement* pXmlNode)
{
	GMatrix matLocal = pNode->GetLocalTM();

	// max api is left handed, convert to right to export
	Quat qRot = matLocal.Rotation().Inverse();
	Point3 vPos = matLocal.Translation();
	Point3 vScale = matLocal.Scaling();

	TiXmlElement* pXmlTransform = LinkNewXmlElement(pXmlNode, "Transform");

	char buff[128];
	sprintf_s(buff, sizeof(buff), "%.3f,%.3f,%.3f", vPos.x, vPos.y, vPos.z);
	pXmlTransform->SetAttribute("Position", buff);

	sprintf_s(buff, sizeof(buff), "%.3f,%.3f,%.3f,%.3f", qRot.x, qRot.y, qRot.z, qRot.w);
	pXmlTransform->SetAttribute("Rotation", buff);

	sprintf_s(buff, sizeof(buff), "%.3f,%.3f,%.3f", vScale.x, vScale.y, vScale.z);
	pXmlTransform->SetAttribute("Scale", buff);
}


void GaMaxExporter::ExportNodes(IGameNode* pNode, TiXmlElement* pXmlNodes)
{
	if (pNode->IsNodeHidden()){
		return;
	}

	// 是否只输出选中物体
	if (GetOption(IDC_RADIO_SELECT) && !pNode->GetMaxNode()->Selected()){
		return;
	}

	IGameObject* pObj = pNode->GetIGameObject();


	const char* pNodeName = ValidateName(pNode->GetName());
	TiXmlElement* pXmlNode = LinkNewXmlElement(pXmlNodes, "Entity");
	pXmlNode->SetAttribute("Name", pNodeName);

	ExportTransform(pNode, pXmlNode);

	INode* pMaxNode = pNode->GetMaxNode();
	MSTR mstr;
	pMaxNode->GetUserPropBuffer(mstr);

	LinkNewXmlText(LinkNewXmlElement(pXmlNode, "Properties"), UnicodeToMultiByte(mstr.data()));

	IGameObject::ObjectTypes objectType = pObj->GetIGameType();


	// 输出一个标记标明这是一个组
	if (pNode->IsGroupOwner()){
		LinkNewXmlElement(pXmlNode, "Group");
	}else if (pObj->GetIGameType() == IGameObject::IGAME_MESH)
	{
		IGameMesh* pMesh = (IGameMesh*)pObj;

		TiXmlElement* pXmlRendering = LinkNewXmlElement(pXmlNode, "Rendering");
		pXmlRendering->SetAttribute("Type", "Model");
		pXmlRendering->SetAttribute("CastShadows", (int)pObj->CastShadows());
		pXmlRendering->SetAttribute("MatID", pNode->GetMaterialIndex());
		//if (pNode->GetMaterialIndex() == -1){
		//	char buffer[256];
		//	sprintf_s(buffer, sizeof(buffer), "模型 %s 必须要有材质", pNodeName);
		//	MessageBoxA(NULL, buffer, "", MB_OK);
		//}

		TiXmlElement* pXmlMeshInst = LinkNewXmlElement(pXmlRendering, "Model");
		pXmlMeshInst->SetAttribute("Ref", pNodeName);
		m_meshMap.insert(make_pair(pNodeName, pNode));

	}else if (objectType == IGameObject::IGAME_LIGHT){
		IGameLight* pLight = (IGameLight*)pObj;
		ExportLight(pLight, LinkNewXmlElement(pXmlNode, "Light"));

	}else if (objectType == IGameObject::IGAME_CAMERA){
		IGameCamera* pCamera = (IGameCamera*)pObj;
		ExportCamera(pCamera, LinkNewXmlElement(pXmlNode, "Camera"));

	}else if (objectType == IGameObject::IGAME_BONE){
		ExportSkeleton(pNode, LinkNewXmlElement(pXmlNode, "Skeleton"), Animations);

	}else if (objectType == IGameObject::IGAME_SPLINE){
		IGameSpline* pIGameShape= (IGameSpline*)pObj;
		pIGameShape->InitializeData();
		ExportShape(pIGameShape, LinkNewXmlElement(pXmlNode, "Shape"));

	}else if (objectType == IGameObject::IGAME_HELPER){
		LinkNewXmlElement(pXmlNode, "Helper");
	}
	pNode->ReleaseIGameObject();

	// export controller
	IGameControl* pControl = pNode->GetIGameControl();
	if (pControl && objectType != IGameObject::IGAME_BONE) {
		TiXmlElement* pXmlControllers = new TiXmlElement("Controller");
		ExportController(pControl, pXmlControllers);
		if (pXmlControllers->FirstChild()){
			pXmlNode->LinkEndChild(pXmlControllers);
		}else{
			delete pXmlControllers;
		}
	}


	m_nNumExportedNode++;
	UpdatePropressBar(UnicodeToMultiByte(pNode->GetName()));

	int nChildNodes = pNode->GetChildCount();
	if (nChildNodes == 0 || objectType == IGameObject::IGAME_BONE){
		return;
	}

	TiXmlElement* pXmlChild = LinkNewXmlElement(pXmlNode, "Child");
	for (int i = 0; i < nChildNodes; i++){
		IGameNode* pChild = pNode->GetNodeChild(i);
		ExportNodes(pChild, pXmlChild);		
	}
}


// Dummy function for progress bar
DWORD WINAPI fn(LPVOID arg)
{
	return(0);
}


void GaMaxExporter::ExportGlobalInfo(IGameScene * pIgame, Interface * ip, TiXmlElement* pXmlScene)
{
	TiXmlElement* pXmlGlobal = LinkNewXmlElement(pXmlScene, "Global");
	LinkNewXmlText(LinkNewXmlElement(pXmlGlobal, "SceneFile"), UnicodeToMultiByte(pIgame->GetSceneFileName()));

	char buf[128];
	Point3 color = ip->GetBackGround(TIME_NegInfinity, FOREVER);
	sprintf_s(buf, sizeof(buf), "%.3f %.3f %.3f", color.x, color.y ,color.z);
	LinkNewXmlText(LinkNewXmlElement(pXmlGlobal, "BackGroundColor"), buf);


	color = ip->GetAmbient(TIME_NegInfinity, FOREVER);
	sprintf_s(buf, sizeof(buf), "%.3f %.3f %.3f", color.x, color.y ,color.z);
	LinkNewXmlText(LinkNewXmlElement(pXmlGlobal, "AmbientColor"), buf);

	TiXmlElement* pXmlTimeBar = LinkNewXmlElement(pXmlGlobal, "TimeBar");

	int ticks = pIgame->GetSceneTicks();
	m_nStartFrame = pIgame->GetSceneStartTime() / ticks;
	m_nEndFrame = pIgame->GetSceneEndTime() / ticks;
	m_fTickPerFrame = (float)ticks;

	pXmlTimeBar->SetAttribute("StartFrame", m_nStartFrame);
	pXmlTimeBar->SetAttribute("EndFrame", m_nEndFrame);
	pXmlTimeBar->SetAttribute("TicksPerFrame", ticks);

}


void ExportMesh(const string& name, IGameMesh* pMesh, const Matrix3& objTM)
{
	TiXmlDocument doc;
	TiXmlElement* pXmlMesh = (TiXmlElement*)doc.LinkEndChild(new TiXmlElement("Mesh"));

	map<int, int> dummy;
	ExportMesh(pMesh, pXmlMesh, objTM);

	doc.SaveFile(name.c_str());
}

void ExportAnimation(const vector<AnimInfo>& animations);

int	GaMaxExporter::DoExport(const TCHAR* name,ExpInterface* ei,Interface* i, BOOL suppressPrompts, DWORD options)
{
	// This is where the file export operation occur.

	if(!suppressPrompts){
		INT_PTR res = DialogBoxParam(hInstance, 
					MAKEINTRESOURCE(IDD_PANEL), 
					GetActiveWindow(), 
					GaMaxExporterOptionsDlgProc, (LPARAM)this);

		if (res == IDCANCEL){
			return 1;
		}
	}

	string pathName = name;
	size_t pos = pathName.find_last_of('\\');
	FilePath = pathName.substr(0, pos + 1);

	pos++;
	size_t posDot = pathName.find_last_of('.');
	SceneName = pathName.substr(pos, posDot - pos);


	// Initialise 3DXI (formerly IGame) object
	// For more information, please see 3ds Max SDK topic PG: Programming with 3DXI.
	IGameScene * pIgame = GetIGameInterface();
	pIgame->InitialiseIGame(false);

	Interface * ip = GetCOREInterface();
	ip->ProgressStart(_T("Exporting ..."), TRUE, fn, NULL);
	m_nNumTotalNode = pIgame->GetTotalNodeCount();
	m_nNumExportedNode = 0;


	
	TiXmlDocument doc;
	TiXmlElement* pXmlScene = (TiXmlElement*)doc.LinkEndChild(new TiXmlElement("Scene"));
	ExportGlobalInfo(pIgame, ip, pXmlScene);

	//m_pXmlMeshes		= (TiXmlElement*)doc.LinkEndChild(new TiXmlElement("Meshes"));
	m_pXmlMaterials		= LinkNewXmlElement(pXmlScene, "Materials");
	m_pXmlNodes			= LinkNewXmlElement(pXmlScene, "Entities");


	bool bExportMaterial = false;
	bool bExportMesh = true;
	bool bExportAnimation = true;
	bool bExportSkeleton = true;

	if (bExportMaterial) {
		//export materials
		int matCount = pIgame->GetRootMaterialCount();
		for(int i = 0;i < matCount; i++) {
			IGameMaterial* pMat = pIgame->GetRootMaterial(i);
			ExportMaterial(pMat, LinkNewXmlElement(m_pXmlMaterials, ValidateName(pMat->GetMaterialName())));
		}
	}


	int numTopLevelNode = pIgame->GetTopLevelNodeCount();
	for (int i = 0; i <numTopLevelNode; i++){
		IGameNode* pNode = pIgame->GetTopLevelNode(i);
		ExportNodes(pNode, m_pXmlNodes);
	}

	//for (map<string, IGameNode*>::iterator it = m_meshMap.begin();
	//	it != m_meshMap.end(); ++it){
	//	INode* pMaxNode = it->second->GetMaxNode();
	//	Matrix3 objTM(TRUE);
	//	Point3 offPos = pMaxNode->GetObjOffsetPos();
	//	Quat rot = pMaxNode->GetObjOffsetRot();
	//	//ScaleValue scl = pNode->GetMaxNode()->GetObjOffsetScale();
	//	objTM.SetTrans(offPos);
	//	PreRotateMatrix(objTM, rot);

	//	IGameMesh* pMesh = (IGameMesh*)it->second->GetIGameObject();

	//	string fileName = FilePath + it->first + ".mesh";
	//	ExportMesh(fileName, pMesh, objTM);
	//}


	doc.SaveFile(UnicodeToMultiByte(name));

	ExportAnimation(Animations);
	
	ip->ProgressEnd();
	return TRUE;
}


 
