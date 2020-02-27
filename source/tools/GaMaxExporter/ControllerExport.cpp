#include "3dsmaxsdk_preinclude.h"
#include "MaxExporter.h"
#include <string>
#include <vector>
using namespace std;




bool ExportBezierKeys(IGameKeyTab &keyTabs, IGameControlType type, TiXmlElement* pXmlController)
{
	pXmlController->SetAttribute("Type", "Bezier");
	int nExportKeys = 0;

	if (type == IGAME_POS || type == IGAME_POINT3 || type == IGAME_SCALE){
		string val;
		char buff[512];
		for (int i = 0; i < keyTabs.Count(); i++){
			if (!TestKeyFrameExport(keyTabs[i].t)){
				continue;
			}
			Point3 t[3];
			Point3 v[3];
			t[1] = Point3(keyTabs[i].t, keyTabs[i].t, keyTabs[i].t);
			v[1] = keyTabs[i].bezierKey.pval;
			if (i != 0)	{
				float timeSpan = keyTabs[i].t - keyTabs[i - 1].t;
				t[0] = t[1] - keyTabs[i].bezierKey.pinLength * timeSpan;
				v[0] = v[1] + keyTabs[i].bezierKey.pinLength * keyTabs[i].bezierKey.pintan * timeSpan;
			}else{
				t[0] = Point3(0.0f, 0.0f, 0.0f);
				v[0] = Point3(0.0f, 0.0f, 0.0f);
			}

			if (i != keyTabs.Count() - 1){
				float timeSpan = keyTabs[i + 1].t - keyTabs[i].t;
				t[2] = t[1] + timeSpan * keyTabs[i].bezierKey.poutLength;
				v[2] = v[1] + timeSpan * keyTabs[i].bezierKey.poutLength * keyTabs[i].bezierKey.pouttan;
			}else{
				t[2] = Point3(0.0f, 0.0f, 0.0f);
				v[2] = Point3(0.0f, 0.0f, 0.0f);
			}

			sprintf_s(buff, sizeof(buff), "%d (%.3f %.3f %.3f) (%.3f %.3f %.3f) (%.3f %.3f %.3f) (%.3f %.3f %.3f) (%.3f %.3f %.3f) (%.3f %.3f %.3f)|",
				i, TransformTime((float)t[0].x), TransformTime((float)t[0].y), TransformTime((float)t[0].z), v[0].x, v[0].y, v[0].z,
				TransformTime((float)t[1].x), TransformTime((float)t[1].y), TransformTime((float)t[1].z), v[1].x, v[1].y, v[1].z, 
				TransformTime((float)t[2].x), TransformTime((float)t[2].y), TransformTime((float)t[2].z), v[2].x, v[2].y, v[2].z);
			val += buff;
			nExportKeys++;
		}

		LinkNewXmlText(pXmlController, val.c_str());

	}else if (type == IGAME_ROT)	{
		string val;
		char buff[128];
		for (int i = 0; i < keyTabs.Count(); i++){
			if (!TestKeyFrameExport(keyTabs[i].t)){
				continue;
			}
			Quat rot = keyTabs[i].bezierKey.qval;
			int time = keyTabs[i].t;
			sprintf_s(buff, sizeof(buff), "%d %.3f %.3f %.3f %.3f %.3f|", i, TransformTime((float)time), rot.x, rot.y, rot.z, rot.w);
			val += buff;

			nExportKeys++;
		}

		LinkNewXmlText(pXmlController, val.c_str());
	}
	else if (type == IGAME_FLOAT || 
		type == IGAME_EULER_X || 
		type == IGAME_EULER_Y || 
		type == IGAME_EULER_Z || 
		type == IGAME_POS_X || 
		type == IGAME_POS_Y || 
		type == IGAME_POS_Z)
	{
		string val;
		char buff[128];
		for (int i = 0; i < keyTabs.Count(); i++){
			if (!TestKeyFrameExport(keyTabs[i].t)){
				continue;
			}
			float t[3], v[3];
			t[1] = keyTabs[i].t;
			v[1] = keyTabs[i].bezierKey.fval;
			if (i != 0 && keyTabs[i].bezierKey.finLength != -1.0f){
				float timeSpan = t[1] - keyTabs[i - 1].t;
				t[0] = t[1] - timeSpan * keyTabs[i].bezierKey.finLength;
				v[0] = v[1] + timeSpan * keyTabs[i].bezierKey.finLength * keyTabs[i].bezierKey.fintan;
			}else{
				t[0] = 0.0f;
				v[0] = 0.0f;
			}

			if (i != keyTabs.Count() - 1 && keyTabs[i].bezierKey.foutLength != -1.0f){
				float timeSpan = keyTabs[i + 1].t - t[1];
				t[2] = t[1] + timeSpan * keyTabs[i].bezierKey.foutLength;
				v[2] = v[1] + timeSpan * keyTabs[i].bezierKey.foutLength * keyTabs[i].bezierKey.fouttan;
			}else{
				t[2] = 0.0f;
				v[2] = 0.0f;
			}

			sprintf_s(buff, sizeof(buff), "%d %.3f %.3f %.3f %.3f %.3f %.3f|", i, TransformTime((float)t[0]), v[0], TransformTime((float)t[1]), v[1], TransformTime((float)t[2]), v[2]);
			val += buff;

			nExportKeys++;
		}

		LinkNewXmlText(pXmlController, val.c_str());
	}

	pXmlController->SetAttribute("NumFrames", nExportKeys);
	return true;
}


bool ExportLinearKeys(IGameKeyTab &keyTabs, IGameControlType type, TiXmlElement* pXmlController)
{
	pXmlController->SetAttribute("Type", "Linear");
	int nExportKeys = 0;

	if (type == IGAME_POS || type == IGAME_POINT3){
		string val;
		char buff[128];
		for (int i = 0; i < keyTabs.Count(); i++){
			if (!TestKeyFrameExport(keyTabs[i].t)){
				continue;
			}
			Point3 pos = keyTabs[i].linearKey.pval;
			int time = keyTabs[i].t;
			sprintf_s(buff, sizeof(buff), "%d %.3f %.3f %.3f %.3f|", i, TransformTime((float)time), pos.x, pos.y, pos.z);
			val += buff;
			nExportKeys++;
		}

		LinkNewXmlText(pXmlController, val.c_str());
	}else if (type == IGAME_ROT){
		string val;
		char buff[128];
		for (int i = 0; i < keyTabs.Count(); i++){
			if (!TestKeyFrameExport(keyTabs[i].t))	{
				continue;
			}
			Quat rot = keyTabs[i].linearKey.qval;
			int time = keyTabs[i].t;
			sprintf_s(buff, sizeof(buff), "%d %.3f %.3f %.3f %.3f %.3f|", i, TransformTime((float)time), rot.x, rot.y, rot.z, rot.w);
			val += buff;
			nExportKeys++;
		}

		LinkNewXmlText(pXmlController, val.c_str());
	}
	else if (type == IGAME_FLOAT || type == IGAME_EULER_X || type == IGAME_EULER_Y || type == IGAME_EULER_Z || 
		IGAME_POS_X || IGAME_POS_Y || IGAME_POS_Z)
	{
		string val;
		char buff[128];
		for (int i = 0; i < keyTabs.Count(); i++){
			if (!TestKeyFrameExport(keyTabs[i].t)){
				continue;
			}
			float f =  keyTabs[i].linearKey.fval;
			int time = keyTabs[i].t;
			sprintf_s(buff, sizeof(buff), "%d %.3f %.3f|", i, TransformTime((float)time), f);
			val += buff;
			nExportKeys++;
		}

		LinkNewXmlText(pXmlController, val.c_str());
	}

	pXmlController->SetAttribute("NumFrames", nExportKeys);
	return true;
}





bool ExportKeyFrameController(IGameControl* pGameControl, TiXmlElement* pXmlController, IGameControlType type)
{
	/*struct ControlType
	{
	IGameControlType		type;
	bool					animed;
	};

	ControlType controlTyps[] = {
	{IGAME_POS,		false},
	{IGAME_ROT,		false},
	{IGAME_SCALE,	false},
	{IGAME_FLOAT,	false},
	{IGAME_POINT3,	false},
	};
	*/


	if (!pGameControl->IsAnimated(type)){
		return false;
	}

	Control* pMaxControl = pGameControl->GetMaxControl(type);
	int ortBefore = pMaxControl->GetORT(ORT_BEFORE);
	int ortAfter = pMaxControl->GetORT(ORT_AFTER);
	if (ortBefore != ortAfter){
		MessageBoxA(NULL, "不支持前后不一致的out of range type", "", MB_OK);
	}

	string ortType = "ONCE";
	if (ortBefore == ORT_CONSTANT)
	{
	}
	else if (ortBefore == ORT_CYCLE || ortBefore == ORT_LOOP){
		ortType = "LOOP";
	}

	pXmlController->SetAttribute("PlayBack", ortType);

	IGameKeyTab keyTabs;
	if (pGameControl->GetLinearKeys(keyTabs, type))	{	
		return ExportLinearKeys(keyTabs, type, pXmlController);

	}else if (pGameControl->GetBezierKeys(keyTabs, type)){
		return ExportBezierKeys(keyTabs, type, pXmlController);

	}else if (pGameControl->GetTCBKeys(keyTabs, type)){			
		MessageBoxA(NULL, "不支持TCB关键帧动画", "", MB_OK);
		return false;
	}

	return false;
}



void ExportConstraint(IGameConstraint* pConstraint, TiXmlElement* pXmlConstraint)
{
	IGameConstraint::ConstraintType type = pConstraint->GetConstraintType();
	const char* typeName = NULL;
	switch (type)
	{
	case IGameConstraint::IGAME_PATH:
		typeName = "Path";
		break;
	case IGameConstraint::IGAME_ORIENTATION:
		typeName = "Orientation";
		break;
	case IGameConstraint::IGAME_LOOKAT:
		typeName = "LookAt";
		break;
	case IGameConstraint::IGAME_POSITION:
		typeName = "Position";
		break;
	case IGameConstraint::IGAME_LINK:
		typeName = "Link";
		break;
	}

	pXmlConstraint->SetAttribute("Type", typeName);

	int nNumNodes = pConstraint->NumberOfConstraintNodes();
	pXmlConstraint->SetAttribute("NumNodes", nNumNodes);
	for (int i = 0; i < nNumNodes; i++){
		TiXmlElement* pXmlNode = LinkNewXmlElement(pXmlConstraint, "ConstraintNode");

		IGameNode* pNode = pConstraint->GetConstraintNodes(i);
		pXmlNode->SetAttribute("Name", ValidateName(pNode->GetName()));

		float weight = pConstraint->GetConstraintWeight(i);
		char buff[16];
		sprintf_s(buff, sizeof(buff), "%.3f", weight);
		pXmlNode->SetAttribute("Weight", buff);
	}

	IPropertyContainer* pPropertyContainer = pConstraint->GetIPropertyContainer();
	int nNumProperties = pPropertyContainer->GetNumberOfProperties();
	if (nNumProperties == 0){
		return;
	}


	for (int i = 0; i < nNumProperties; i++){
		// 只支持percent曲线
		IGameProperty* pProperty = pPropertyContainer->GetProperty(i);
		const MCHAR* pName = _T("");
		if (pProperty != NULL){
			pName = pProperty->GetName();
		}

		if (strcmp(pName, _T("percent")) != 0)	{
			continue;
		}

		if (pProperty->IsPropAnimated()){
			IGameControl* pControl = pProperty->GetIGameControl();

			if (pControl && pControl->IsAnimated(IGAME_FLOAT)){
				TiXmlElement* pXmlColorControl = new TiXmlElement("Percent");
				if (ExportKeyFrameController(pControl, pXmlColorControl, IGAME_FLOAT)){
					pXmlConstraint->LinkEndChild(pXmlColorControl);
				}else{
					delete pXmlColorControl;
				}
			}
		}

		break;

	}
}


void ExportController(IGameControl* pControl, TiXmlElement* pXmlControllers)
{
	struct ControlType
	{
		IGameControlType		type;
		const char*				name;
	};

	static ControlType controlTypes[] = {
		{IGAME_POS,		"Position"},
		{IGAME_ROT,		"Rotation"},
		{IGAME_SCALE,	"Scale"},
		{IGAME_EULER_X,	"EulerX"},
		{IGAME_EULER_Y,	"EulerY"},
		{IGAME_EULER_Z,	"EulerZ"},
		{IGAME_POS_X,	"PositionX"},
		{IGAME_POS_Y,	"PositionY"},
		{IGAME_POS_Z,	"PositionZ"},
	};

	uint controlTypeCount = sizeof(controlTypes) / sizeof(controlTypes[0]);
	for (uint i = 0; i < controlTypeCount; i++)	{
		if (pControl->IsAnimated(controlTypes[i].type))		{
			TiXmlElement* pXmlColorControl = new TiXmlElement(controlTypes[i].name);

			if (ExportKeyFrameController(pControl, pXmlColorControl, controlTypes[i].type))	{
				pXmlControllers->LinkEndChild(pXmlColorControl);
			}else{
				delete pXmlColorControl;
			}
		}

		IGameConstraint* pConstraint = pControl->GetConstraint(controlTypes[i].type);
		if (pConstraint){
			ExportConstraint(pConstraint, LinkNewXmlElement(pXmlControllers, "Constraint"));
		}
	}
}
