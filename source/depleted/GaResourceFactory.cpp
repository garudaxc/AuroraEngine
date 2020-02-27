#include "stdHeader.h"
#include "GaResourceFactory.h"
#include "GaGeometry.h"
#include "GaScene.h"
#include "GaMaterial.h"
#include "GaMaterialInstance.h"
#include "GaResourceManager.h"
#include "GaPipeline.h"
#include "GaLight.h"
#include "lua.hpp"
#include "tinyxml.h"
#include "GaController.h"
#include "GaEngine.h"
#include "GaLightNode.h"
#include "GaPrimitiveNode.h"
#include "GaUtil.h"
#include <sstream>
namespace Ga
{
#if 0


const string& GetResourcePath()
{
	static string path = "../res/";
	return path;
}
//
//// convert from max's path to local resource path
//string ConvertFileName(const string& filename)
//{
//	size_t pos = filename.find_last_of('\\');
//
//	return GetResourcePath() + filename.substr(pos + 1);
//}
//


shared_ptr<FloatsController> ParseBezierFloat1Controller(const string& text, uint nNumFrame)
{
	stringstream ss;
	ss.write(text.c_str(), text.size());

	vector<BezierKey> keys(nNumFrame);

	int n = 0;
	BezierKey key;
	char t;
	for (uint i = 0; i < nNumFrame; i++)
	{
		ss >> n >> key.t[0] >> key.v[0] >> key.t[1] >> key.v[1] >> key.t[2] >> key.v[2] >> t;
		keys[i] = key;
	}
	ss.clear();

	return SampleBezierCurve(&keys[0], nNumFrame, 100.0f);
}

shared_ptr<FloatsController> ParseLinearFloat1Controller(const string& text, uint nNumFrame)
{
	stringstream ss;
	ss.write(text.c_str(), text.size());

	shared_ptr<FloatsController> pController(new FloatsController());
	pController->Alloc(nNumFrame, 1);

	int n = 0;
	char t = 0x0;
	float time = 0.0f;
	float value = 0.0f;
	for (uint i = 0; i < nNumFrame; i++)
	{
		ss >> n >> time >> value >> t;
		pController->SetKey(n, time, &value);
	}
	ss.clear();

	return pController;
}

shared_ptr<FloatsController> ParseLinearFloat3Controller(const string& text, uint nNumFrame)
{
	shared_ptr<FloatsController> pController(new FloatsController());

	pController->Alloc(nNumFrame, 3);

	stringstream ss;
	ss.write(text.c_str(), text.size());

	int n = 0;
	char t = 0x0;
	float time = 0.0f;
	Vector3f pos = Vector3f::ZERO;
	for (uint i = 0; i < nNumFrame; i++)
	{
		ss >> n >> time >> pos.x >> pos.y >> pos.z >> t;
		pController->SetKey(n, time, pos.Ptr());
	}
	ss.clear();

	return pController;
}

shared_ptr<FloatsController> ParseLinearFloat4Controller(const string& text, uint nNumFrame)
{
	shared_ptr<FloatsController> pController(new FloatsController());

	pController->Alloc(nNumFrame, 4);

	stringstream ss;
	ss.write(text.c_str(), text.size());

	int n = 0;
	char t = 0x0;
	float time = 0.0f;
	Quaternionf value = Quaternionf::IDENTITY;
	for (uint i = 0; i < nNumFrame; i++)
	{
		ss >> n >> time >> value.x >> value.y >> value.z >> value.w >> t;
		pController->SetKey(n, time, value.Ptr());
	}
	ss.clear();

	return pController;
}


void LoadXmlLightController(TiXmlElement* pXmlController, LightObject* pLight)
{

	TiXmlNode* pElem = pXmlController->FirstChild();
	while (pElem)
	{

		const char* pPlayback = pElem->ToElement()->Attribute("PlayBack");
		Controller::PlayBackType playBackType = Controller::PLAYBACK_LOOP;
		if (strcmp(pPlayback, "ONCE") == 0)
		{
			playBackType = Controller::PLAYBACK_ONCE;
		}

		bool bBezier = false;
		const char* pType = pElem->ToElement()->Attribute("Type");
		if (strcmp(pType, "Bezier") == 0)
		{
			bBezier = true;
		}

		uint nNumFrame = (uint)atoi(pElem->ToElement()->Attribute("NumFrames"));

		if (strcmp(pElem->Value(), "Color") == 0)
		{
			if (bBezier)
			{
				continue;
			}

			shared_ptr<FloatsController> pController(new FloatsController());

			pController->SetPlaybackType(playBackType);
			pController->Alloc(nNumFrame, 4);
			
			string text = pElem->FirstChild()->ToText()->Value();
			stringstream ss;
			ss.write(text.c_str(), text.size());

			int n = 0;
			char t = 0x0;
			float time = 0.0f;
			Color color = Color::BLACK;
			for (uint i = 0; i < nNumFrame; i++)
			{
				ss >> n >> time >> color.r >> color.g >> color.b >> t;
				pController->SetKey(n, time, color.Ptr());
			}
			ss.clear();

			pLight->AddBaseColorController(pController);
		}
		else if (strcmp(pElem->Value(), "Multiplier") == 0)
		{
			string text = pElem->FirstChild()->ToText()->Value();
			shared_ptr<FloatsController> pController;

			if (bBezier)
			{
				pController = ParseBezierFloat1Controller(text, nNumFrame);
			}
			else
			{
				pController = ParseLinearFloat1Controller(text, nNumFrame);
			}

			pController->SetPlaybackType(playBackType);
			pLight->AddMultiplierController(pController);
		}
		
		pElem = pElem->NextSibling();
	}
}




void LoadXmlNodeController(TiXmlElement* pXmlController, SceneNode* pNode)
{

	TiXmlNode* pElem = pXmlController->FirstChild();
	while (pElem)
	{
		const char* pPlayback = pElem->ToElement()->Attribute("PlayBack");
		Controller::PlayBackType playBackType = Controller::PLAYBACK_LOOP;
		if (strcmp(pPlayback, "ONCE") == 0)
		{
			playBackType = Controller::PLAYBACK_ONCE;
		}

		bool bBezier = false;
		const char* pType = pElem->ToElement()->Attribute("Type");
		if (strcmp(pType, "Bezier") == 0)
		{
			bBezier = true;
		}


		uint nNumFrame = (uint)atoi(pElem->ToElement()->Attribute("NumFrames"));

		if (strcmp(pElem->Value(), "Position") == 0)
		{
			string text = pElem->FirstChild()->ToText()->Value();
			shared_ptr<FloatsController> pController = ParseLinearFloat3Controller(text, nNumFrame);
			
			pController->SetPlaybackType(playBackType);
			pNode->AddPositionController(pController);
		}
		else if (strcmp(pElem->Value(), "Rotation") == 0)
		{
			string text = pElem->FirstChild()->ToText()->Value();
			shared_ptr<FloatsController> pController = ParseLinearFloat4Controller(text, nNumFrame);

			pController->SetInterpolaterType(FloatsController::IPT_LINEAR_QUATERNION);
			pController->SetPlaybackType(playBackType);
			pNode->AddRotationController(pController);
		}
		else if (strcmp(pElem->Value(), "EulerX") == 0 ||
				strcmp(pElem->Value(), "EulerY") == 0 ||
				strcmp(pElem->Value(), "EulerZ") == 0)
		{
			string text = pElem->FirstChild()->ToText()->Value();

			shared_ptr<FloatsController> pController;
			if (bBezier)
			{
				pController = ParseBezierFloat1Controller(text, nNumFrame);
			}
			else
			{
				pController = ParseLinearFloat1Controller(text, nNumFrame);
			}

			pController->SetPlaybackType(playBackType);

			if (strcmp(pElem->Value(), "EulerX") == 0)
			{
				pNode->AddEulerController(pController, 0);
			}
			else if (strcmp(pElem->Value(), "EulerY") == 0)
			{
				pNode->AddEulerController(pController, 1);
			}
			else  if (strcmp(pElem->Value(), "EulerZ") == 0)
			{
				pNode->AddEulerController(pController, 2);
			}
		}

		pElem = pElem->NextSibling();
	}
}



SceneNodePtr LoadXmlNode(TiXmlNode* pXmlNode)
{
	if (strcmp(pXmlNode->Value(), "DirectionalLight") == 0)
	{
		DirectionalLightNodePtr pLightNode = DirectionalLightNodePtr(new DirectionalLightNode());
		pLightNode->Archive(pXmlNode, true);

		return pLightNode;
	}
	else if (strcmp(pXmlNode->Value(), "PointLight") == 0)
	{
		PointLightNodePtr pLightNode = PointLightNodePtr(new PointLightNode());
		pLightNode->Archive(pXmlNode, true);

		return pLightNode;
	}
	else if (strcmp(pXmlNode->Value(), "SpotLight") == 0)
	{
		SpotLightNodePtr pLightNode = SpotLightNodePtr(new SpotLightNode());
		pLightNode->Archive(pXmlNode, true);

		return pLightNode;
	}
	else if (strcmp(pXmlNode->Value(), "StaticMesh") == 0)
	{
		PrimitiveNodePtr pNode = PrimitiveNodePtr(new PrimitiveNode());
		pNode->Archive(pXmlNode, true);
		return pNode;
	}

	//SceneNodePtr pNode(new SceneNode());
	//pNode->Archive(pXmlNode, true);

	//TiXmlNode* pElem = pXmlNode->FirstChild();
	//while (pElem)
	//{
	//	else if (strcmp(pElem->Value(), "Child") == 0)
	//	{
	//		TiXmlNode* pXmlChild = pElem->FirstChild();
	//		while (pXmlChild)
	//		{
	//			LoadXmlNode(pXmlChild, pNode.get());

	//			pXmlChild = pXmlChild->NextSibling();
	//		}
	//	}
	//	else 
	//	if (strcmp(pElem->Value(), "Camera") == 0)
	//	{
	//		LoadXmlCamera(pElem, pNode.get());
	//	}
	//	else if (strcmp(pElem->Value(), "Controller") == 0)
	//	{
	//		LoadXmlNodeController(pElem->ToElement(), pNode.get());
	//	}

	//	pElem = pElem->NextSibling();
	//}

	return SceneNodePtr();
}


void LoadAsset(const string& pathname)
{
	TiXmlDocument doc;
	if (!doc.LoadFile(pathname.c_str()))
	{
		assert(0);
		return;
	}

	TiXmlNode* pXmlMeshes = doc.FirstChild("Meshes");

	TiXmlNode* pNode = pXmlMeshes->FirstChild();
	while (pNode)
	{
		TiXmlElement* pMesh = pNode->ToElement();
		Geometry* pGeomData = new Geometry();
		pGeomData->Load(pMesh);

		Global::GetResourceManager()->AddGeometryData(pMesh->Value(), pGeomData);

		pNode = pNode->NextSibling();
	}
}


void LoadXmlScene(const string& pathname)
{
	TiXmlDocument doc;
	if (!doc.LoadFile(pathname.c_str()))
	{
		assert(0);
		return;
	}

	TiXmlNode* pXmlScene = doc.FirstChild("Scene");

	Global::GetPipeline()->GetRenderingContext()->SetGlobalShaderConstValueF("GlobalAmbient", Color::BLACK.Ptr(), 3);

	//TiXmlNode* pXmlGlobal = pXmlScene->FirstChild("Global")->FirstChild();
	//while (pXmlGlobal)
	//{

	//	if (strcmp(pXmlGlobal->Value(), "BackGroundColor") == 0)
	//	{
	//		string text = pXmlGlobal->FirstChild()->ToText()->Value();
	//	}
	//	else if (strcmp(pXmlGlobal->Value(), "AmbientColor") == 0)
	//	{
	//		string text = pXmlGlobal->FirstChild()->ToText()->Value();
	//		Color color = ParseColor(text);
	//		Global::GetPipeline()->GetRenderingContext()->SetGlobalShaderConstValueF("GlobalAmbient", color.Ptr(), 3);
	//	}

	//	pXmlGlobal = pXmlGlobal->NextSibling();
	//}

	TiXmlNode* pXmlNode = pXmlScene->FirstChild("Nodes")->FirstChild();
	while (pXmlNode)
	{
		SceneNodePtr pNode = LoadXmlNode(pXmlNode);
		if (pNode)
		{
			GetEngine()->GetScene()->AddSceneNode(pNode);
		}

		pXmlNode = pXmlNode->NextSibling();
	}

	// 加载完成后整个场景要更新一下
	GetEngine()->GetScene()->UpdateWorld(0.0f, true);
}


ResourceFactory::ResourceFactory(void)
{
}

ResourceFactory::~ResourceFactory(void)
{
}

void ResourceFactory::LoadScene(const string& filename)
{
	//LoadAsset(GetResourcePath() + "asset/asset.xml");

	//Global::GetResourceManager()->GetMaterialInstLib()->LoadMaterialInstances(GetResourcePath() + "scene1/DefaultMtlInst.MtlInst");

	//LoadXmlScene(GetResourcePath() + filename);
}

void ResourceFactory::SaveScene(const string& filename)
{
	TiXmlDocument doc;
	TiXmlElement* pXmlScene = (TiXmlElement*)doc.LinkEndChild(new TiXmlElement("Scene"));

	TiXmlElement* pXmlNodes = LinkNewXmlElement(pXmlScene, "Nodes");

	vector<SceneNodePtr>& nodes = GetEngine()->GetScene()->GetNodes();

	for (vector<SceneNodePtr>::iterator it = nodes.begin(); it != nodes.end(); ++it)
	{
		TiXmlElement* pXmlNode = LinkNewXmlElement(pXmlNodes, (*it)->GetTypeName().c_str());
		(*it)->Archive(pXmlNode, false);
	}

	doc.SaveFile(filename.c_str());
}


//
//
//void ResourceFactory::LoadOneMaterialInstance(lua_State *L, const string& name)
//{
//	lua_getfield(L, -1, "Template");
//	assert(!lua_isnil(L, -1));
//	const void* pTable = lua_topointer(L, -1);
//	map<const void*, string>::const_iterator it = m_MaterialTable.find(pTable);
//	assert(it != m_MaterialTable.end());
//
//	string materialName = it->second;
//	lua_pop(L, 1);
//
//	lua_getfield(L, -1, "Node");
//	assert(!lua_isnil(L, -1));
//	string nodeName = lua_tostring(L, -1);
//	lua_pop(L, 1);
//	SceneNode* pNode = GetEngine()->GetScene()->GetRootNode()->FindNodeByName(nodeName);
//	if (!pNode)
//	{
//		return;
//	}
//
//	MaterialInstancePtr pMatInst = Global::GetResourceManager()->GetMaterial(materialName)->CreateMaterialInstance();
//	Renderable* pObj = (Renderable*)(pNode->GetAttachable(0));
//	pObj->SetMaterialInst(pMatInst);
//
//	lua_getfield(L, -1, "Param");
//	assert(!lua_isnil(L, -1));
//	lua_pushnil(L);
//	while (lua_next(L, -2) != 0)
//	{
//		string paramName = lua_tostring(L, -2);
//		string value = lua_tostring(L, -1);
//		pMatInst->SetTexture(paramName, Global::GetResourceManager()->GetTexture(value));
//
//		lua_pop(L, 1);
//	}
//	lua_pop(L, 1);
//}
//


//void ResourceFactory::LoadScene(lua_State* L)
//{
//	lua_getglobal(L, "Scene");
//	assert(!lua_isnil(L, -1));
//
//	lua_pushnil(L);
//	while (lua_next(L, -2) != 0)
//	{
//		assert(lua_isstring(L, -1));
//		string sceneFile = lua_tostring(L, -1);
//
//		LoadXmlScene(GetResourcePath() + sceneFile);
//
//		lua_pop(L, 1);
//	}
//
//	lua_pop(L, 1);
//}

//
//void ResourceFactory::LoadOneMaterial(lua_State *L, const string& name)
//{
//
//	//Material* pMaterial = NULL;
//
//	//lua_pushnil(L);
//	//while (lua_next(L, -2) != 0)
//	//{
//	//	string materialName = lua_tostring(L, -2);
//	//	assert(lua_istable(L, -1));
//
//
//	//	Material::MaterialClass mtlClass = Material::CLASS_ORDINARY;
//	//	lua_getfield(L, -1, "Class");
//	//	if (!lua_isnil(L, -1))
//	//	{
//	//		mtlClass = (Material::MaterialClass)lua_tointeger(L, -1);
//	//	}
//	//	lua_pop(L, 1);
//
//	//	pMaterial = new Material(mtlClass);
//
//	//	lua_pushnil(L);
//	//	while (lua_next(L, -2) != 0)
//	//	{
//	//		string elemName = lua_tostring(L, -2);
//	//		if (elemName == "Class")
//	//		{
//	//		}
//	//		else
//	//		{
//	//			MaterialPassPtr pPass = LoadMaterialPass(L, elemName);
//	//			pMaterial->AddMaterialPass(pPass);
//	//		}
//
//	//		lua_pop(L, 1);
//	//	}
//
//	//	lua_pop(L, 1);
//	//}
//
//	////Global::GetResourceManager()->AddMaterial(name, pMaterial);
//
//	//lua_close(L);
//
//	//return pMaterial;
//}
//
//
//void ResourceFactory::LoadMaterials(lua_State* L)
//{
//
//	m_MaterialTable.clear();
//	lua_getglobal(L, "Material");
//
//	assert(!lua_isnil(L, -1));
//	lua_pushnil(L);
//	while (lua_next(L, -2) != 0)
//	{
//		string materialName = lua_tostring(L, -2);
//		assert(lua_istable(L, -1));
//
//		m_MaterialTable.insert(make_pair(lua_topointer(L, -1), materialName));
//		LoadOneMaterial(L, materialName);
//		lua_pop(L, 1);
//	}
//
//	lua_pop(L, 1);
//
//}
//
//void ResourceFactory::LoadMaterialInstances(lua_State* L)
//{
//	lua_getglobal(L, "MaterialInstance");
//
//	assert(!lua_isnil(L, -1));
//	lua_pushnil(L);
//	while (lua_next(L, -2) != 0)
//	{
//		string instName = lua_tostring(L, -2);
//		assert(lua_istable(L, -1));
//
//		LoadOneMaterialInstance(L, instName);
//
//		lua_pop(L, 1);
//	}
//
//	lua_pop(L, 1);
//}
//
//
//void LoadRenderTarget(lua_State* L, const string& name)
//{
//	uint width = 0, height = 0;
//	RenderTarget::Desc desc;
//	Global::GetRenderer()->GetFrameBufferSize(width, height);
//
//	lua_getfield(L, -1, "Width");
//	if (!lua_isnil(L, -1))
//	{
//		desc.nWidth = (uint)lua_tointeger(L, -1);
//	}
//	else
//	{
//		desc.nWidth = width;
//	}
//	lua_pop(L, 1);
//
//	lua_getfield(L, -1, "Height");
//	if (!lua_isnil(L, -1))
//	{
//		desc.nHeight = (uint)lua_tointeger(L, -1);
//	}
//	else
//	{
//		desc.nHeight = height;
//	}
//	lua_pop(L, 1);
//
//	lua_getfield(L, -1, "Format");
//	assert(lua_isnumber(L, -1));
//	desc.nFormat = (Surface::Format)lua_tointeger(L, -1);
//	lua_pop(L, 1);
//
//	lua_getfield(L, -1, "Usage");
//
//	assert(lua_isnumber(L, -1));
//	desc.nUsage = (Surface::Usage)lua_tointeger(L, -1);
//	lua_pop(L, 1);
//
//
//	RenderTarget* pRT = Global::GetRenderer()->CreateRenderTarget(desc);
//	Global::GetResourceManager()->AddRenderTarget(name, pRT);
//}

//
//void PreDefineSurface(lua_State* L)
//{
//	int count = sizeof(SurfaceFormatName) / sizeof(SurfaceFormatName[0]);
//	for (int i = 0; i < count; i++)
//	{
//		lua_pushinteger(L, i);
//		lua_setglobal(L, SurfaceFormatName[i]);
//	}
//
//	count = sizeof(SurfaceUsageName) / sizeof(SurfaceUsageName[0]);
//	for (int i = 0; i < count; i++)
//	{
//		lua_pushinteger(L, i);
//		lua_setglobal(L, SurfaceUsageName[i]);
//	}
//}
//
//
//void PreDefineLight(lua_State* L)
//{
//	int count = sizeof(LightTypeName) / sizeof(LightTypeName[0]);
//	for (int i = 0; i < count; i++)
//	{
//		lua_pushinteger(L, i);
//		lua_setglobal(L, LightTypeName[i]);
//	}
//}

//
//void ResourceFactory::LoadConfig(lua_State* L)
//{
//	lua_getglobal(L, "Config");
//	assert(!lua_isnil(L, -1));
//
//	lua_getfield(L, -1, "RenderTarget");
//	if (!lua_isnil(L, -1))
//	{
//
//		lua_pushnil(L);
//		while (lua_next(L, -2) != 0)
//		{
//			string rtName = lua_tostring(L, -2);
//			LoadRenderTarget(L, rtName);
//			lua_pop(L, 1);
//		}
//		
//	}
//	lua_pop(L, 1);
//
//	lua_pop(L, 1);
//}

//
//void ResourceFactory::LoadResource(const string& filename)
//{
//	lua_State *L = lua_open();
//
//	//PreDefineValue(L);
//	//PreDefineSurface(L);
//	//PreDefineLight(L);
//
//	if (luaL_dofile(L, filename.c_str()) != 0)
//	{
//		lua_close(L);
//		assert(0);
//		return;
//	}
//
//	//LoadConfig(L);
//
//	// must load material and assets before scene
//	//LoadMaterials(L);
//
//	LoadAsset(GetResourcePath() + "asset/asset.xml");
//
//	LoadScene(L);
//
//	//LoadMaterialInstances(L);
//
//	lua_close(L);
//}
//


#endif

}