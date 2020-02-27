#include "GaMaterialLibrary.h"
#include "GaMaterialScript.h"
#include "../Kernel/GaLog.h"
#include "../Kernel/GaConsole.h"
#include "../Render/GaDX9Render.h"
#include "GaMaterial.h"
#include "GaMaterialInstance.h"
#include <lua.hpp>
#include <sstream>

#include <d3dx9.h>

namespace Ga
{




class MaterialScriptLoader
{
public:
	MaterialScriptLoader();
	~MaterialScriptLoader();

	shared_ptr<MaterialScript>	LoadMaterial(const string& pathName);

private:
	void						InitPredefinedConst(lua_State *L);
	MaterialScript::Variable	LoadVirable(lua_State *L, const string& name);
	SamperState					LoadSamperState(lua_State *L, const string& name);
};


MaterialScriptLoader::MaterialScriptLoader()
{
}

MaterialScriptLoader::~MaterialScriptLoader()
{
}


struct StringIntPair 
{
	const char* name;
	int			value;
};


void MaterialScriptLoader::InitPredefinedConst(lua_State *L)
{
	int count = sizeof(ShaderConstName) / sizeof(ShaderConstName[0]);
	for (int i = 0; i < count; i++)
	{
		lua_pushinteger(L, i);
		lua_setglobal(L, ShaderConstName[i]);
	}

	static StringIntPair addressType[] = {
		{"WRAP",		1},
		{"MIRROR",		2},
		{"CLAMP",		3},
		{"BORDER",		4},
		{"MIRRORONCE",	5},
		{"Wrap",		1},
		{"Mirror",		2},
		{"Clamp",		3},
		{"Border",		4},
		{"MirrorOnce",	5}
	};
	count = sizeof(addressType) / sizeof(addressType[0]);
	for (int i = 0; i < count; i++)
	{
		lua_pushinteger(L, addressType[i].value);
		lua_setglobal(L, addressType[i].name);
	}


	static StringIntPair filterType[] = {
		{"NONE",			0},
		{"POINT",			1},
		{"LINEAR",			2},
		{"ANISOTROPIC",		3},
		{"PYRAMIDALQUAD",	6},
		{"GAUSSIANQUAD",	7},
		{"CONVOLUTIONMONO",	8},
		{"None",			0},
		{"Point",			1},
		{"Linear",			2},
		{"Anisotropic",		3},
		{"PyramidalQuad",	6},
		{"GaussianQuad",	7},
		{"ConvolutionMono",	8}
	};
	count = sizeof(filterType) / sizeof(filterType[0]);
	for (int i = 0; i < count; i++)
	{
		lua_pushinteger(L, filterType[i].value);
		lua_setglobal(L, filterType[i].name);
	}


}


SamperState MaterialScriptLoader::LoadSamperState(lua_State *L, const string& name)
{
	static const char* strStateName[] = {
		"RESERVED",			// 0
		"AddressU",
		"AddressV",
		"AddressW",
		"BorderColor",
		"MagFilter",
		"MinFilter",
		"MipFilter",
		"MipmapLodBias",
		"MaxMipLevel",
		"MaxAnisotropy",
		"SRGBTexture",
		"ElementIndex",
		"DmapOffset"		//	= 13,
	};
	static int nNumStateType = sizeof(strStateName) / sizeof(strStateName[0]);

	SamperState state;
	state.strName = name;
	state.nNumState = 0;
	state.nRegister = 0xff;
	state.id		= -1;

	lua_getglobal(L, name.c_str());
	assert(!lua_isnil(L, -1));

	int index = 0;
	lua_pushnil(L);
	while (lua_next(L, -2) != 0)
	{
		index++;

		if (!lua_isnumber(L, -1))
		{
			stringstream ss;
			ss << "sampler state " << index << "in samper " << name << " is not a RreDefined value";
			Console::AppendText(ss.str().c_str());

			lua_pop(L, 1);
			continue;
		}

		int value = (int)lua_tointeger(L, -1);
		lua_pop(L, 1);

		if (!lua_isstring(L, -1))
		{
			stringstream ss;
			ss << "sampler state " << index << "in sampler " << name << " must have a type";
			Console::AppendText(ss.str().c_str());

			continue;
		}

		string name = lua_tostring(L, -1);
		if (name.compare("id") == 0)
		{
			state.id = value;
			continue;
		}

		for (int i = 0; i < nNumStateType; i++)
		{
			if (name.compare(strStateName[i]) == 0)
			{
				state.nStateType[state.nNumState] = i;
				state.nValue[state.nNumState] = (uint)value;
				state.nNumState++;
				break;
			}
		}

		
		stringstream ss;
		ss << "sampler state type " << index << "in sampler " << name << " is not valid";
		Console::AppendText(ss.str().c_str());
	}

	lua_pop(L, 1);

	return state;
}



MaterialScript::Variable MaterialScriptLoader::LoadVirable(lua_State *L, const string& name)
{
	MaterialScript::Variable var;

	lua_getglobal(L, name.c_str());
	assert(!lua_isnil(L, -1));
	int index = lua_gettop(L);

	lua_getfield(L, index, "type");
	// must have a "type"
	assert(!lua_isnil(L, -1));
	var.type = lua_tostring(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, index, "value");
	if (!lua_isnil(L, -1))
	{
		var.value = lua_tostring(L, -1);
	}
	lua_pop(L, 1);

	lua_getfield(L, index, "global");
	if (!lua_isnil(L, -1))
	{
		var.global = (lua_toboolean(L, -1) == 1);
	}
	lua_pop(L, 1);

	lua_getfield(L, index, "id");
	if (!lua_isnil(L, -1))
	{
		var.id = (int)lua_tointeger(L, -1);
	}
	lua_pop(L, 1);
	
	lua_pop(L, 1);

	return var;
}

shared_ptr<MaterialScript>	MaterialScriptLoader::LoadMaterial(const string& pathName)
{
	lua_State *L = lua_open();
	InitPredefinedConst(L);

	if (luaL_dofile(L, pathName.c_str()) != 0)
	{
		lua_close(L);
		return shared_ptr<MaterialScript>();
	}

	map<const void*, string> varTable;
	lua_pushnil(L); 
	while (lua_next(L, LUA_GLOBALSINDEX) != 0)
	{
		if (lua_istable(L, -1))
		{
			varTable.insert(make_pair(
				lua_topointer(L, -1), 
				lua_tostring(L, -2)));
		}
		lua_pop(L, 1);
	}

	string name, vsin, vsout, vsCode, psCode, vsProfile, psProfile;

	lua_getglobal(L, "MaterialName");
	assert(!lua_isnil(L, -1));
	name = lua_tostring(L, -1);
	lua_pop(L, 1);

	lua_getglobal(L, "vs");
	assert(!lua_isnil(L, -1));
	int index = lua_gettop(L);

	lua_getfield(L, index, "profile");
	if (!lua_isnil(L, -1))
	{
		vsProfile = lua_tostring(L, -1);
	}
	else
	{
		vsProfile = "vs_3_0";
	}
	lua_pop(L, 1);

	lua_getfield(L, index, "code");
	assert(!lua_isnil(L, -1));
	vsCode = lua_tostring(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, index, "vsin");
	assert(!lua_isnil(L, -1));
	vsin = lua_tostring(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, index, "vsout");
	assert(!lua_isnil(L, -1));
	vsout = lua_tostring(L, -1);
	lua_pop(L, 1);


	map<string, MaterialScript::Variable> vsVariable;
	lua_getfield(L, index, "variable");
	assert(!lua_isnil(L, -1));
	lua_pushnil(L);
	while (lua_next(L, -2) != 0)
	{
		// the element in variable need to be table
		assert(lua_istable(L, -1));

		const void* pTable = lua_topointer(L, -1);
		map<const void*, string>::iterator it = varTable.find(pTable);

		assert(it != varTable.end());

		MaterialScript::Variable var = LoadVirable(L, it->second);
		vsVariable.insert(make_pair(
			it->second, var));

		lua_pop(L, 1);
	}


	//load ps
	lua_getglobal(L, "ps");
	assert(!lua_isnil(L, -1));
	index = lua_gettop(L);

	lua_getfield(L, index, "profile");
	if (!lua_isnil(L, -1))
	{
		psProfile = lua_tostring(L, -1);
	}
	else
	{
		psProfile = "ps_3_0";
	}
	lua_pop(L, 1);

	lua_getfield(L, index, "code");
	assert(!lua_isnil(L, -1));
	psCode = lua_tostring(L, -1);
	lua_pop(L, 1);


	map<string, MaterialScript::Variable> psVariable;
	assert(!lua_isnil(L, -1));
	lua_getfield(L, index, "variable");
	assert(!lua_isnil(L, -1));
	lua_pushnil(L);
	while (lua_next(L, -2) != 0)
	{
		// the element in variable need to be table
		assert(lua_istable(L, -1));

		const void* pTable = lua_topointer(L, -1);
		map<const void*, string>::iterator it = varTable.find(pTable);

		assert(it != varTable.end());

		MaterialScript::Variable var = LoadVirable(L, it->second);
		psVariable.insert(make_pair(
			it->second, var));

		lua_pop(L, 1);
	}

	vector<SamperState> samperState;
	lua_getfield(L, index, "sampler");
	assert(!lua_isnil(L, -1));
	lua_pushnil(L);
	while (lua_next(L, -2) != 0)
	{
		// the element in variable need to be table
		assert(lua_istable(L, -1));

		const void* pTable = lua_topointer(L, -1);
		map<const void*, string>::iterator it = varTable.find(pTable);

		assert(it != varTable.end());

		samperState.push_back(LoadSamperState(L, it->second));
		lua_pop(L, 1);
	}

	shared_array<SamperState> pSampers;
	if (!samperState.empty())
	{
		pSampers.reset(new SamperState[samperState.size()]);
	}
	for (uint i = 0; i < samperState.size(); i++)
	{
		pSampers[i] = samperState[i];
	}

	lua_close(L);

	return shared_ptr<MaterialScript>(
		new MaterialScript(name, vsin, vsout, vsCode, psCode, 
		vsProfile, psProfile, vsVariable, psVariable,
		pSampers, (uint)samperState.size()));

}





MaterialLibrary::MaterialLibrary(void)
{
}

MaterialLibrary::~MaterialLibrary(void)
{
}


bool MaterialLibrary::LoadMaterialScript(const string& pathName)
{
	MaterialScriptLoader loader;
	shared_ptr<MaterialScript> pScript = loader.LoadMaterial(pathName);

	if (pScript)
	{
		AddMaterialScript(pScript);
		return true;
	}

	return false;
}


void MaterialLibrary::AddMaterialScript(shared_ptr<MaterialScript>& pScript)
{
	m_MaterialScripts.push_back(pScript);
}
//
//bool CreateVSShader(shared_ptr<MaterialScript> pScript,
//					IDirect3DVertexShader9*& pShader, 
//					uint& nNumVar, shared_array<ShaderVarDesc>& pVarDesc)
//{
//	string vsCode = pScript->MakeVSCode();
//
//	Global::GetLog()->Append(vsCode);
//
//	LPD3DXBUFFER			pCompiledShader			= NULL;
//	LPD3DXBUFFER			pErrorMsgs		= NULL;
//	LPD3DXCONSTANTTABLE		pConstantTable	= NULL;
//
//	HRESULT hr = D3DXCompileShader(vsCode.c_str(), (UINT)vsCode.length(), NULL, NULL, 
//		"VS", pScript->GetVSProfile().c_str(), 0,
//		&pCompiledShader, &pErrorMsgs, &pConstantTable);
//
//	if (FAILED(hr))
//	{
//		std::string msg = "compile shader error. material: ";
//		msg += '\n';
//		Console::AppendText(msg.c_str());
//		if (pErrorMsgs)
//		{
//			std::string strError = (char*)pErrorMsgs->GetBufferPointer();
//			Console::AppendText(strError.c_str());
//			pErrorMsgs->Release();
//		}
//
//		return false;
//	}
//
//	hr = Global::GetRenderer()->GetDevice()->CreateVertexShader(
//		(const DWORD*)pCompiledShader->GetBufferPointer(), &pShader);
//	assert(SUCCEEDED(hr));
//
//	vector<ShaderVarDesc>	shaderVar;
//	const map<string, MaterialScript::Variable>& vsVar = pScript->GetVSVariables();
//	for (map<string, MaterialScript::Variable>::const_iterator it = vsVar.begin();
//		it != vsVar.end(); ++it)
//	{
//		if (it->second.global)
//		{
//			continue;
//		}
//
//		D3DXHANDLE handle = pConstantTable->GetConstantByName(NULL, it->first.c_str());
//
//		// variable may not used in shader
//		if (handle == NULL)
//		{
//			continue;
//		}
//
//		D3DXCONSTANT_DESC desc;
//		UINT nNumDesc = 1;
//		pConstantTable->GetConstantDesc(handle, &desc, &nNumDesc);
//
//		if (desc.Type != D3DXPT_FLOAT)
//		{
//			continue;
//		}
//
//		ShaderVarDesc var;
//		var.id			= it->second.id;
//		var.strName		= it->first;
//		var.nStartReg	= desc.RegisterIndex;
//		var.nNumReg		= desc.RegisterCount;
//		var.nBufferOffset	= desc.RegisterIndex * 4;
//
//		shaderVar.push_back(var);
//	}
//
//	nNumVar = (uint)shaderVar.size();
//	if (nNumVar > 0)
//	{
//		pVarDesc.reset(new ShaderVarDesc[nNumVar]);
//	}
//
//	for (uint i = 0; i < nNumVar; i++)
//	{
//		pVarDesc[i] = shaderVar[i];
//	}
//
//	return true;
//}
//
//
//
//
//bool CreatePSShader(shared_ptr<MaterialScript> pScript,
//					IDirect3DPixelShader9*& pShader, 
//					uint& nNumVar, shared_array<ShaderVarDesc>& pVarDesc,
//					uint& nNumSamperState, shared_array<SamperState>& pSamperState)
//{
//	string psCode = pScript->MakePSCode();
//
//	Global::GetLog()->Append(psCode);
//
//	LPD3DXBUFFER			pCompiledShader			= NULL;
//	LPD3DXBUFFER			pErrorMsgs		= NULL;
//	LPD3DXCONSTANTTABLE		pConstantTable	= NULL;
//
//	HRESULT hr = D3DXCompileShader(psCode.c_str(), (UINT)psCode.length(), NULL, NULL, 
//		"PS", pScript->GetPSProfile().c_str(), 0,
//		&pCompiledShader, &pErrorMsgs, &pConstantTable);
//
//	if (FAILED(hr))
//	{
//		std::string msg = "compile shader error. material: ";
//		msg += '\n';
//		Console::AppendText(msg.c_str());
//		if (pErrorMsgs)
//		{
//			std::string strError = (char*)pErrorMsgs->GetBufferPointer();
//			Console::AppendText(strError.c_str());
//			pErrorMsgs->Release();
//		}
//
//		return false;
//	}
//
//	hr = Global::GetRenderer()->GetDevice()->CreatePixelShader(
//		(const DWORD*)pCompiledShader->GetBufferPointer(), &pShader);
//	assert(SUCCEEDED(hr));
//
//	vector<ShaderVarDesc>	shaderVar;
//	const map<string, MaterialScript::Variable>& vsVar = pScript->GetPSVariables();
//	for (map<string, MaterialScript::Variable>::const_iterator it = vsVar.begin();
//		it != vsVar.end(); ++it)
//	{
//		if (it->second.global)
//		{
//			continue;
//		}
//
//		D3DXHANDLE handle = pConstantTable->GetConstantByName(NULL, it->first.c_str());
//
//		// variable may not used in shader
//		if (handle == NULL)
//		{
//			continue;
//		}
//
//		D3DXCONSTANT_DESC desc;
//		UINT nNumDesc = 1;
//		pConstantTable->GetConstantDesc(handle, &desc, &nNumDesc);
//
//		// temporarily only support float variable
//		if (desc.Type != D3DXPT_FLOAT)
//		{
//			continue;
//		}
//
//		ShaderVarDesc var;
//		var.id			= it->second.id;
//		var.strName		= it->first;
//		var.nStartReg	= desc.RegisterIndex;
//		var.nNumReg		= desc.RegisterCount;
//		var.nBufferOffset	= desc.RegisterIndex * 4;
//
//		shaderVar.push_back(var);
//	}
//
//	nNumVar = (uint)shaderVar.size();
//	if (nNumVar > 0)
//	{
//		pVarDesc.reset(new ShaderVarDesc[nNumVar]);
//	}
//
//	for (uint i = 0; i < nNumVar; i++)
//	{
//		pVarDesc[i] = shaderVar[i];
//	}
//
//
//	nNumSamperState = pScript->GetNumSamperState();
//	pSamperState = pScript->GetSamperStates();
//	for (uint i = 0; i < nNumSamperState; i++)
//	{
//		D3DXHANDLE handle = pConstantTable->GetConstantByName(NULL, pSamperState[i].strName.c_str());
//
//		// variable may not used in shader
//		if (handle == NULL)
//		{
//			continue;
//		}
//
//		D3DXCONSTANT_DESC desc;
//		UINT nNumDesc = 1;
//		pConstantTable->GetConstantDesc(handle, &desc, &nNumDesc);
//
//		assert(desc.Type == D3DXPT_SAMPLER ||
//		desc.Type == D3DXPT_SAMPLER1D ||
//		desc.Type == D3DXPT_SAMPLER2D ||
//		desc.Type == D3DXPT_SAMPLER3D ||
//		desc.Type == D3DXPT_SAMPLERCUBE);
//
//		pSamperState[i].nRegister = desc.RegisterIndex;
//	}
//
//
//	return true;
//}


void MaterialLibrary::BuildFromCurrentScripts()
{
	// 1.collect all global variable
	// 2.count max local variable
	// 3.index global variable
	// 4.generate and compile shader


	for (vector<shared_ptr<MaterialScript>>::iterator it = m_MaterialScripts.begin();
		it != m_MaterialScripts.end(); ++it)
	{
		//IDirect3DVertexShader9*  = NULL;
		//IDirect3DPixelShader9*	pPSShader = NULL;

		ShaderPtr pVSShader;
		ShaderPtr pPSShader;

		uint nNumVSVar = 0, nNumPSVar = 0;
		shared_array<ShaderVarDesc> pVSVarDesc;
		shared_array<ShaderVarDesc> pPSVarDesc;

		uint nNumSamperState = 0;
		shared_array<SamperState> pSamperState;

		if (!Global::GetRenderer()->CreateVSShader(*it, pVSShader, nNumVSVar, pVSVarDesc))
		{
			stringstream ss;
			ss << "create vertex shader " << (*it)->GetName() << " failed!";
			Console::AppendText(ss.str().c_str());
			continue;
		}

		if (!Global::GetRenderer()->CreatePSShader(*it, pPSShader, nNumPSVar, pPSVarDesc, 
			nNumSamperState, pSamperState))
		{
			stringstream ss;
			ss << "create pixel shader " << (*it)->GetName() << " failed!";
			Console::AppendText(ss.str().c_str());
			continue;
		}

		MaterialPtr pMaterial(new Material(pVSShader, pPSShader, 
										pVSVarDesc, nNumVSVar,
										pPSVarDesc, nNumPSVar,
										pSamperState, nNumSamperState));

		m_MaterialLib.insert(make_pair((*it)->GetName(), pMaterial));
	}
}

MaterialInstancePtr MaterialLibrary::CreateMaterialInstance(const string& matName)
{
	MaterialIter it = m_MaterialLib.find(matName);
	if (it != m_MaterialLib.end())
	{
		return it->second->CreateMaterialInstance();
	}

	return MaterialInstancePtr();
}

MaterialPtr MaterialLibrary::GetMaterial(const string& matName)
{
	MaterialIter it = m_MaterialLib.find(matName);
	if (it != m_MaterialLib.end())
	{
		return it->second;
	}

	return MaterialPtr();
}

}