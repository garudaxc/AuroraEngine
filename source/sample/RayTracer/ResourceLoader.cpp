#include "stdheader.h"
#include "ResourceLoader.h"
#include "gageometrydata.h"
#include "GaScene.h"
#include "GaMaterial.h"
#include "GaMaterialInstance.h"
#include "GaMaterial.h"
#include "GaResourceManager.h"
#include "GaPipeline.h"
#include "GaLight.h"
#include "lua/include/lua.hpp"
#include "tinyxml/include/tinyxml.h"
#include "GaController.h"
#include "Primitive.h"
#include <sstream>
#include <string>

using namespace Ga;
using namespace std;


Vector3f ParseVec3(const string& str)
{
	float x, y, z;
	sscanf_s(str.c_str(), "%f,%f,%f", &x, &y, &z);
	return Vector3f(x, y, z);
}

Quaternionf ParseRotation(const string& str)
{
	float x, y, z, w;
	sscanf_s(str.c_str(), "%f,%f,%f,%f", &x, &y, &z, &w);
	return Quaternionf(w, x, y, z);	
}


Color ParseColor(const string& str)
{
	float r, g, b;
	sscanf_s(str.c_str(), "%f %f %f", &r, &g, &b);
	return Color(r, g, b);
}

const string& GetResourcePath()
{
	static string path = "../res/rayTracer/";
	return path;
}


// convert from max's path to local resource path
string ConvertFileName(const string& filename)
{
	size_t pos = filename.find_last_of('\\');

	return GetResourcePath() + filename.substr(pos + 1);
}


vector<TiXmlNode*> gMtlList;



void LoadXmlLight(TiXmlNode* pXmlLight, SceneNode* pSceneNode)
{
	LightObjectPtr pObject;

	float fMultiplayer = 1.0f;
	Color color = Color::WHITE;

	TiXmlNode* pElem = pXmlLight->FirstChild();
	while (pElem)
	{
		if (strcmp(pElem->Value(), "Color") == 0)
		{
			string text = pElem->FirstChild()->ToText()->Value();
			color = ParseColor(text);
		}
		else if (strcmp(pElem->Value(), "Multiplier") == 0)
		{
			string text = pElem->FirstChild()->ToText()->Value();
			fMultiplayer = (float)atof(text.c_str());
		}

		pElem = pElem->NextSibling();
	}

	const char* type = pXmlLight->ToElement()->Attribute("Type");
	if (strcmp(type, "Directional") == 0)
	{
		pObject.reset(new DirectionalLight(color, fMultiplayer));
	}
	else if (strcmp(type, "Point") == 0)
	{
		float attenStart = 20.0f, attenEnd = 80.0f;

		pElem = pXmlLight->FirstChild();
		while (pElem)
		{
			if (strcmp(pElem->Value(), "AttenStart") == 0)
			{
				string text = pElem->FirstChild()->ToText()->Value();
				attenStart = (float)atof(text.c_str());
			}
			else if (strcmp(pElem->Value(), "AttenEnd") == 0)
			{
				string text = pElem->FirstChild()->ToText()->Value();
				attenEnd = (float)atof(text.c_str());
			}

			pElem = pElem->NextSibling();
		}

		pObject.reset(new PointLight(color, fMultiplayer, attenStart, attenEnd));
	}
	else if (strcmp(type, "Spot") == 0)
	{
		float hotSpot = 20.0f, fallOff = 40.0f;

		TiXmlNode* pElem = pXmlLight->FirstChild();
		while (pElem)
		{
			if (strcmp(pElem->Value(), "HotSpot") == 0)
			{
				string text = pElem->FirstChild()->ToText()->Value();
				hotSpot = (float)atof(text.c_str());
			}
			else if (strcmp(pElem->Value(), "FallOff") == 0)
			{
				string text = pElem->FirstChild()->ToText()->Value();
				fallOff = (float)atof(text.c_str());
			}

			pElem = pElem->NextSibling();
		}

		pObject.reset(new SpotLight(color, fMultiplayer, 
			hotSpot * Mathf::DEG_TO_RAD, fallOff * Mathf::DEG_TO_RAD));

	}

	const char* pAttr = pXmlLight->ToElement()->Attribute("CastShadow");
	if (pAttr && atoi(pAttr) == 1)
	{
		pObject->SetCastShadow(true);
	}

	pAttr = pXmlLight->ToElement()->Attribute("ProjTex");
	if (pAttr)
	{
		pObject->SetProjTexture(ConvertFileName(pAttr));
	}

	pSceneNode->Attach(pObject);
}





//
//
//void ApplyXmlTextures(TiXmlNode* pXmlTextures, MaterialInstance* pMatInst)
//{
//
//	string diffuseTex = GetResourcePath() + "default_albedo.dds";
//	string bumpTex = GetResourcePath() + "default_normal.dds";
//
//	float bumpniess = 1.0f;
//
//
//	TiXmlNode* pXmlTex = pXmlTextures->FirstChild();
//	while (pXmlTex)
//	{
//		if (strcmp(pXmlTex->Value(), "Diffuse") == 0)
//		{
//			string text = pXmlTex->FirstChild()->ToText()->Value();
//			diffuseTex = ConvertFileName(text);
//		}
//		else if (strcmp(pXmlTex->Value(), "Bump") == 0)
//		{
//			bumpniess = (float)atof(pXmlTex->ToElement()->Attribute("Amount"));
//
//			string text = pXmlTex->FirstChild()->ToText()->Value();
//			bumpTex = ConvertFileName(text);
//		}
//
//		pXmlTex = pXmlTex->NextSibling();
//	}
//
//
//	pMatInst->SetTexture("texAlbedo", Global::GetResourceManager()->GetTexture(diffuseTex));
//	pMatInst->SetTexture("texNormal", Global::GetResourceManager()->GetTexture(bumpTex));
//
//	pMatInst->SetValueF("Bumpniess", &bumpniess, 1);
//}


//
//void ApplyXmlMaterial(Renderable* pRenderable, int matID)
//{
//	MaterialInstancePtr pMatInst;
//
//#if 1
//	TiXmlNode* pElem =  gMtlList[matID]->FirstChild("Opacity");
//	if (pElem)
//	{
//
//		string text = pElem->FirstChild()->ToText()->Value();
//		float value = (float)atof(text.c_str());
//		if (value < 1.0f)
//		{
//			pMatInst = Global::GetResourceManager()->CreateMaterialInst("MatSemitransparent");
//			static int pattern = 0;
//			float fp = (float)pattern;
//			pMatInst->SetValueF("TransPattern", &fp, 1);
//			pattern = ++pattern % 4;
//		}
//	}
//#else
//	TiXmlNode* pElem = NULL;
//#endif
//
//	if (!pMatInst)
//	{
//		pMatInst = Global::GetResourceManager()->CreateMaterialInst("MatPreNormal");
//	}
//
//	pElem =  gMtlList[matID]->FirstChild();
//	while (pElem)
//	{
//		if (strcmp(pElem->Value(), "Diffuse") == 0)
//		{
//			string text = pElem->FirstChild()->ToText()->Value();
//			Color c = ParseColor(text);
//			pMatInst->SetValueF("DiffuseColor", c.Ptr(), 3);
//		}
//		else if (strcmp(pElem->Value(), "Ambient") == 0)
//		{
//		}
//		else if (strcmp(pElem->Value(), "Specular") == 0)
//		{
//		}
//		else if (strcmp(pElem->Value(), "Emissive") == 0)
//		{
//			string text = pElem->FirstChild()->ToText()->Value();
//			Color c = ParseColor(text);
//			pMatInst->SetValueF("EmissiveColor", c.Ptr(), 3);
//		}
//		else if (strcmp(pElem->Value(), "Glossiness") == 0)
//		{
//			string text = pElem->FirstChild()->ToText()->Value();
//			float value = (float)atof(text.c_str());
//			pMatInst->SetValueF("Glossiness", &value, 1);
//		}
//		else if (strcmp(pElem->Value(), "SpecularLevel") == 0)
//		{
//			string text = pElem->FirstChild()->ToText()->Value();
//			float value = (float)atof(text.c_str());
//			pMatInst->SetValueF("SpecularLevel", &value, 1);
//		}
//		else if (strcmp(pElem->Value(), "Textures") == 0)
//		{
//			ApplyXmlTextures(pElem, pMatInst.get());
//		}
//
//		pElem = pElem->NextSibling();
//	}
//
//	pRenderable->SetMaterialInst(pMatInst);
//}
//




void LoadXmlCamera(TiXmlNode* pXmlCamera, SceneNode* pSceneNode)
{
	CameraPtr pCamera(new CameraPerspectiveFov());

	float nearPlane = 1.0f, farPlane = 1000.0f, fov = Mathf::HALF_PI / 2.0f;
	
	TiXmlNode* pElem = pXmlCamera->FirstChild();
	while (pElem)
	{
		if (strcmp(pElem->Value(), "FOV") == 0)
		{
			string text = pElem->FirstChild()->ToText()->Value();
			fov = (float)atof(text.c_str());
		}
		else if (strcmp(pElem->Value(), "NearPlane") == 0)
		{
			string text = pElem->FirstChild()->ToText()->Value();
			nearPlane = (float)atof(text.c_str());
		}
		else if (strcmp(pElem->Value(), "FarPlane") == 0)
		{
			string text = pElem->FirstChild()->ToText()->Value();
			farPlane = (float)atof(text.c_str());
		}

		pElem = pElem->NextSibling();
	}

	pCamera->SetNearFarPlane(nearPlane, farPlane);

	pCamera->SetFov(fov);

	pSceneNode->Attach(pCamera);

	pCamera->UpdateFromParentNode();
}

BRDF ParseBrdf(const string& text)
{
	BRDF brdf;
	brdf.diffuseColor = Color::WHITE;
	brdf.type = BRDF::BRDF_Diffuse;

	size_t pos = string::npos;
	pos = text.find("diffuse");

	if (pos != string::npos)
	{
		size_t colorPos = text.find("color", pos);
		assert(colorPos != string::npos);

		int r, g, b;
		sscanf_s(text.c_str() + colorPos, "color %d %d %d", &r, &g, &b);

		brdf.diffuseColor.Set(r / 255.f, g / 255.f, b / 255.f, 1.f);
	}

	pos = text.find("type");

	if (pos != string::npos)
	{
		if (text.find("light") != string::npos || text.find("Light") != string::npos)
		{
			brdf.type = BRDF::BRDF_Light;
		}
		else if (text.find("reflect") != string::npos || text.find("Reflect") != string::npos)
		{
			brdf.type = BRDF::BRDF_Reflect;
		}
	}

	return brdf;
}


void LoadXmlNode(TiXmlNode* pXmlNode, SceneNode* pParentNode)
{
	SceneNodePtr pNode = GetEngine()->GetScene()->CreateSceneNode();
	pNode->SetName(pXmlNode->Value());

	string nodeProperty;

	TiXmlNode* pElem = pXmlNode->FirstChild();
	while (pElem)
	{
		if (strcmp(pElem->Value(), "Transform") == 0)
		{
			string text;
			text = pElem->ToElement()->Attribute("Position");
			pNode->SetTranslation(ParseVec3(text));

			text = pElem->ToElement()->Attribute("Rotation");
			pNode->SetRotation(ParseRotation(text));

			text = pElem->ToElement()->Attribute("Scale");
			pNode->SetScale(ParseVec3(text));
		}
		else if (strcmp(pElem->Value(), "Properties") == 0)
		{
			if (pElem->FirstChild())
			{
				nodeProperty = pElem->FirstChild()->ToText()->Value();
			}
		}
		else if (strcmp(pElem->Value(), "Mesh") == 0)
		{
			TiXmlElement* p = pElem->ToElement();
			const char* pRefMesh = p->Attribute("Ref");

			string fileName = GetResourcePath() + pRefMesh + string(".mesh");
			TiXmlDocument doc;
			bool loadOK = doc.LoadFile(fileName.c_str());
			assert(loadOK);

			Mesh* pMesh = new Mesh;
			pMesh->Load(doc.FirstChild()->ToElement());

			pNode->UpdateWorld(0.0f, true);
			pMesh->Transform(pNode->GetWorldTransform());
			GetLoadedMeshes().push_back(pMesh);

			pMesh->m_brdf = ParseBrdf(nodeProperty);

		}
		else if (strcmp(pElem->Value(), "Child") == 0)
		{
			TiXmlNode* pXmlChild = pElem->FirstChild();
			while (pXmlChild)
			{
				LoadXmlNode(pXmlChild, pNode.get());

				pXmlChild = pXmlChild->NextSibling();
			}
		}
		else if (strcmp(pElem->Value(), "Camera") == 0)
		{
			LoadXmlCamera(pElem, pNode.get());
		}
		else if (strcmp(pElem->Value(), "Light") == 0)
		{
			LoadXmlLight(pElem, pNode.get());
		}


		pElem = pElem->NextSibling();
	}

	pParentNode->AddChildNode(pNode);
}



vector<Mesh*>& GetLoadedMeshes()
{
	static vector<Mesh*> meshes;
	return meshes;

}





void LoadXmlScene(const string& pathname)
{
	TiXmlDocument doc;
	if (!doc.LoadFile((GetResourcePath() + pathname).c_str()))
	{
		assert(0);
		return;
	}

	TiXmlNode* pXmlRoot = doc.FirstChild();

	TiXmlNode* pXmlGlobal = pXmlRoot->FirstChild("Global")->FirstChild();
	while (pXmlGlobal)
	{

		if (strcmp(pXmlGlobal->Value(), "BackGroundColor") == 0)
		{
			string text = pXmlGlobal->FirstChild()->ToText()->Value();
		}
		else if (strcmp(pXmlGlobal->Value(), "AmbientColor") == 0)
		{
			string text = pXmlGlobal->FirstChild()->ToText()->Value();
			Color color = ParseColor(text);
			//Global::GetPipeline()->GetRenderingContext()->SetGlobalShaderConstValueF("GlobalAmbient", color.Ptr(), 3);
		}

		pXmlGlobal = pXmlGlobal->NextSibling();
	}

	TiXmlNode* pXmlNode = pXmlRoot->FirstChild("Nodes")->FirstChild();
	while (pXmlNode)
	{
		LoadXmlNode(pXmlNode, GetEngine()->GetScene()->GetRootNode());

		pXmlNode = pXmlNode->NextSibling();
	}

}





Vector3f ParseVector3(const string& str)
{
	float x, y, z;
	sscanf_s(str.c_str(), "(%f, %f, %f)", &x, &y, &z);
	return Vector3f(x, y, z);
}

