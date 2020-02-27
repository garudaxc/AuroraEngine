#include "stdHeader.h"
#include "GaMaterial.h"
#include "GaRenderer.h"
#include "GaMaterialInstance.h"
#include "GaShader.h"
#include "GaRendererDX10.h"
#include "GaLog.h"
#include "lua.hpp"
#include "GaConfig.h"
#include "GaCamera.h"

namespace Ga
{

	void PreDefineValue(lua_State* L)
	{
		struct StringIntPair 
		{
			const char* name;
			int			value;
		};

		static StringIntPair sizeType[] = {
			{"DS_FRAMEBUFFER_WIDTH",		RenderTarget::DS_FRAMEBUFFER_WIDTH},
			{"DS_FRAMEBUFFER_HEIGHT",		RenderTarget::DS_FRAMEBUFFER_HEIGHT},
			{"DS_FRAMEBUFFER_WIDTH_2_1",	RenderTarget::DS_FRAMEBUFFER_WIDTH_2_1},
			{"DS_FRAMEBUFFER_HEIGHT_2_1",	RenderTarget::DS_FRAMEBUFFER_HEIGHT_2_1},
			{"DS_FRAMEBUFFER_WIDTH_4_1",	RenderTarget::DS_FRAMEBUFFER_WIDTH_4_1},
			{"DS_FRAMEBUFFER_HEIGHT_4_1",	RenderTarget::DS_FRAMEBUFFER_HEIGHT_4_1},
			{"DS_FRAMEBUFFER_WIDTH_8_1",	RenderTarget::DS_FRAMEBUFFER_WIDTH_8_1},
			{"DS_FRAMEBUFFER_HEIGHT_8_1",	RenderTarget::DS_FRAMEBUFFER_HEIGHT_8_1},
			{"DS_FRAMEBUFFER_WIDTH_16_1",	RenderTarget::DS_FRAMEBUFFER_WIDTH_16_1},
			{"DS_FRAMEBUFFER_HEIGHT_16_1",	RenderTarget::DS_FRAMEBUFFER_HEIGHT_16_1}
		};
		int count = sizeof(sizeType) / sizeof(sizeType[0]);
		for (int i = 0; i < count; i++)
		{
			lua_pushinteger(L, sizeType[i].value);
			lua_setglobal(L, sizeType[i].name);
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


		static StringIntPair renderStateValue[] = {
			{"TRUE",					1},
			{"FALSE",					0},

			{"FILL_POINT",				1},
			{"FILL_WIREFRAME",			2},
			{"FILL_SOLID",				3},

			{"BLEND_ZERO",				1},
			{"BLEND_ONE",				2},
			{"BLEND_SRCCOLOR",			3},
			{"BLEND_INVSRCCOLOR",		4},
			{"BLEND_SRCALPHA",			5},
			{"BLEND_INVSRCALPHA",		6},
			{"BLEND_DESTALPHA",			7},
			{"BLEND_INVDESTALPHA",		8},
			{"BLEND_DESTCOLOR",			9},
			{"BLEND_INVDESTCOLOR",		10},
			{"BLEND_SRCALPHASAT",		11},
			{"BLEND_BOTHSRCALPHA",		12},
			{"BLEND_BOTHINVSRCALPHA",	13},
			{"BLEND_BLENDFACTOR",		14},
			{"BLEND_INVBLENDFACTOR",    15},
			{"BLEND_SRCCOLOR2",			16},
			{"BLEND_INVSRCCOLOR2",		17},

			{"BLENDOP_ADD",				1},
			{"BLENDOP_SUBTRACT",		2},
			{"BLENDOP_REVSUBTRACT",		3},
			{"BLENDOP_MIN",				4},
			{"BLENDOP_MAX",				5},

			{"CULL_NONE",				1},
			{"CULL_CW",					2},
			{"CULL_CCW",				3},

			{"CMP_NEVER",				1},
			{"CMP_LESS",				2},
			{"CMP_EQUAL",				3},
			{"CMP_LESSEQUAL",			4},
			{"CMP_GREATER",				5},
			{"CMP_NOTEQUAL",			6},
			{"CMP_GREATEREQUAL",		7},
			{"CMP_ALWAYS",				8},
		};
		count = sizeof(renderStateValue) / sizeof(renderStateValue[0]);
		for (int i = 0; i < count; i++)
		{
			lua_pushinteger(L, renderStateValue[i].value);
			lua_setglobal(L, renderStateValue[i].name);
		}


		static StringIntPair materialClass[] = {
			{"MTL_ORDINARY",			(int)Material::CLASS_ORDINARY},
			{"MTL_TRANSPARENT",			(int)Material::CLASS_TRANSPARENT},
		};
		count = sizeof(materialClass) / sizeof(materialClass[0]);
		for (int i = 0; i < count; i++)
		{
			lua_pushinteger(L, materialClass[i].value);
			lua_setglobal(L, materialClass[i].name);
		}

	}






	uint MapSamplerStateType(const string& typeName)
	{
		struct StringIntPair 
		{
			const char* name;
			int			value;
		};


		static StringIntPair samplerStateType[] = {
			{"ADDRESSU",		1},
			{"ADDRESSV",		2},
			{"ADDRESSW",		3},
			{"BORDERCOLOR",		4},
			{"MAGFILTER",		5},
			{"MINFILTER",		6},
			{"MIPFILTER",		7},
			{"MIPMAPLODBIAS",	8},
			{"MAXMIPLEVEL",		9},
			{"MAXANISOTROPY",	10},
			{"SRGBTEXTURE",		11},
			{"ELEMENTINDEX",	12},
			{"DMAPOFFSET",		13},
			{"AddressU",		1},
			{"AddressV",		2},
			{"AddressW",		3},
			{"BorderColor",		4},
			{"MagFilter",		5},
			{"MinFilter",		6},
			{"MipFilter",		7},
			{"MipmapLodBias",	8},
			{"MaxMipLevel",		9},
			{"MaxAnisotropy",	10},
			{"srgbTexture",		11},
			{"ElementIndex",	12},
			{"DmapOffset",		13},
		};
		int count = sizeof(samplerStateType) / sizeof(samplerStateType[0]);
		for (int i = 0; i < count; i++)
		{
			if (typeName == samplerStateType[i].name)
			{
				return samplerStateType[i].value;
			}
		}

		assert(0);
		return 0;
	}



	uint MapRenderStateType(const string& typeName)
	{
		struct StringIntPair 
		{
			const char* name;
			int			value;
		};

		static StringIntPair renderStateType[] = {
			{"ZENABLE",						7},
			{"FILLMODE",					8},
			{"ZWRITEENABLE",				14},
			{"ALPHATESTENABLE",				15},
			{"LASTPIXEL",					16},
			{"SRCBLEND",					19},
			{"DESTBLEND",					20},
			{"CULLMODE",					22},
			{"ZFUNC",						23},
			{"ALPHAREF",					24},
			{"ALPHAFUNC",					25},
			{"DITHERENABLE",				26},
			{"ALPHABLENDENABLE",			27},
			{"STENCILENABLE",               52},   /* BOOL enable/disable stenciling */
			{"STENCILFAIL",                 53},   /* D3DSTENCILOP to do if stencil test fails */
			{"STENCILZFAIL",                54},   /* D3DSTENCILOP to do if stencil test passes and Z test fails */
			{"STENCILPASS",                 55},   /* D3DSTENCILOP to do if both stencil and Z tests pass */
			{"STENCILFUNC",                 56},   /* D3DCMPFUNC fn.  Stencil Test passes if ((ref & mask) stencilfn (stencil & mask)) is true */
			{"STENCILREF",                  57},   /* Reference value used in stencil test */
			{"STENCILMASK",                 58},   /* Mask value used in stencil test */
			{"STENCILWRITEMASK",            59},   /* Write mask applied to values written to stencil buffer */
			{"TEXTUREFACTOR",				60},
			{"CLIPPING",					136},
			{"LOCALVIEWER",					142},
			{"NORMALIZENORMALS",			143},
			{"VERTEXBLEND",					151},
			{"CLIPPLANEENABLE",				152},
			{"MULTISAMPLEANTIALIAS",		161},
			{"MULTISAMPLEMASK",				162},
			{"PATCHEDGESTYLE",				163},
			{"POINTSIZE_MAX",				166},
			{"INDEXEDVERTEXBLENDENABLE",	167}, 
			{"COLORWRITEENABLE",			168},
			{"TWEENFACTOR",					170},
			{"BLENDOP",						171},
			{"POSITIONDEGREE",				172},
			{"NORMALDEGREE",				173},
			{"SCISSORTESTENABLE",			174},
			{"SLOPESCALEDEPTHBIAS",			175},
			{"ANTIALIASEDLINEENABLE",		176},
			{"ENABLEADAPTIVETESSELLATION",	184},
			{"TWOSIDEDSTENCILMODE",			185},
			{"BLENDFACTOR",					193},
			{"SRGBWRITEENABLE",				194},
			{"DEPTHBIAS",					195},
			{"SEPARATEALPHABLENDENABLE",	206},
			{"SRCBLENDALPHA",				207},
			{"DESTBLENDALPHA",				208},
			{"BLENDOPALPHA",				209},
		};

		int count = sizeof(renderStateType) / sizeof(renderStateType[0]);
		for (int i = 0; i < count; i++)
		{
			if (typeName == renderStateType[i].name)
			{
				return renderStateType[i].value;
			}
		}

		assert(0);
		return 0;
	}


	uint MapRenderStateValue(const string& typeName)
	{
		struct StringIntPair 
		{
			const char* name;
			int			value;
		};

		static StringIntPair renderStateValue[] = {
			{"TRUE",					1},
			{"FALSE",					0},

			{"FILL_POINT",				1},
			{"FILL_WIREFRAME",			2},
			{"FILL_SOLID",				3},

			{"BLEND_ZERO",				1},
			{"BLEND_ONE",				2},
			{"BLEND_SRCCOLOR",			3},
			{"BLEND_INVSRCCOLOR",		4},
			{"BLEND_SRCALPHA",			5},
			{"BLEND_INVSRCALPHA",		6},
			{"BLEND_DESTALPHA",			7},
			{"BLEND_INVDESTALPHA",		8},
			{"BLEND_DESTCOLOR",			9},
			{"BLEND_INVDESTCOLOR",		10},
			{"BLEND_SRCALPHASAT",		11},
			{"BLEND_BOTHSRCALPHA",		12},
			{"BLEND_BOTHINVSRCALPHA",	13},
			{"BLEND_BLENDFACTOR",		14},
			{"BLEND_INVBLENDFACTOR",    15},
			{"BLEND_SRCCOLOR2",			16},
			{"BLEND_INVSRCCOLOR2",		17},

			{"BLENDOP_ADD",				1},
			{"BLENDOP_SUBTRACT",		2},
			{"BLENDOP_REVSUBTRACT",		3},
			{"BLENDOP_MIN",				4},
			{"BLENDOP_MAX",				5},

			{"CULL_NONE",				1},
			{"CULL_CW",					2},
			{"CULL_CCW",				3},

			{"CMP_NEVER",				1},
			{"CMP_LESS",				2},
			{"CMP_EQUAL",				3},
			{"CMP_LESSEQUAL",			4},
			{"CMP_GREATER",				5},
			{"CMP_NOTEQUAL",			6},
			{"CMP_GREATEREQUAL",		7},
			{"CMP_ALWAYS",				8},
		};

		int count = sizeof(renderStateValue) / sizeof(renderStateValue[0]);
		for (int i = 0; i < count; i++)
		{
			if (typeName == renderStateValue[i].name)
			{
				return renderStateValue[i].value;
			}
		}

		assert(0);
		return 0;
	}



	void LoadSampler(lua_State *L, MaterialPassPtr& pMatPass, const string& sampler)
	{
		lua_pushnil(L);
		while (lua_next(L, -2) != 0)
		{	
			assert(lua_isnumber(L, -1));
			string typaName = lua_tostring(L, -2);
			uint type = MapSamplerStateType(typaName);
			uint value = (uint)lua_tointeger(L, -1);
			pMatPass->AddSamplerState(sampler, type, value);
			lua_pop(L, 1);
		}
	}



	MaterialPassPtr LoadMaterialPass(lua_State* L, const string& passName)
	{
		vector<pair<string, string>> defines;

		// load shader define
		lua_getfield(L, -1, "ShaderDefine");
		if (!lua_isnil(L, -1))
		{
			lua_pushnil(L);
			while (lua_next(L, -2) != 0)
			{
				string def = lua_tostring(L, -2);
				string val;
				if (!lua_isnil(L, -1))
				{
					val = lua_tostring(L, -1);
				}
				defines.push_back(make_pair(def, val));
				lua_pop(L, 1);
			}
		}
		lua_pop(L, 1);


		lua_getfield(L, -1, "VertexShader");
		assert(!lua_isnil(L, -1));
		string vsName = lua_tostring(L, -1);
		lua_pop(L, 1);


		lua_getfield(L, -1, "PixelShader");
		assert(!lua_isnil(L, -1));
		string psName = lua_tostring(L, -1);
		lua_pop(L, 1);


		Shader* pVertexShader = GRenderDevice->CreateVertexShader(vsName, defines);

		Shader* pPixelShader = GrenderDevice->CreatePixelShader(psName, defines);


		assert(pVertexShader);
		assert(pPixelShader);

		MaterialPassPtr pPass(new MaterialPass(pVertexShader, pPixelShader, pVertexShader->GetShaderConstTable(), pPixelShader->GetShaderConstTable()));

		// load sampler state
		lua_getfield(L, -1, "Sampler");
		if (!lua_isnil(L, -1))
		{
			lua_pushnil(L);
			while (lua_next(L, -2) != 0)
			{
				string samplerName = lua_tostring(L, -2);
				LoadSampler(L, pPass, samplerName);
				lua_pop(L, 1);
			}
		}
		lua_pop(L, 1);

		// load sampler state
		lua_getfield(L, -1, "RenderState");
		if (!lua_isnil(L, -1))
		{
			lua_pushnil(L);
			while (lua_next(L, -2) != 0)
			{
				string stateName = lua_tostring(L, -2);
				uint type = MapRenderStateType(stateName);
				uint value = (uint)lua_tointeger(L, -1);
				pPass->AddRenderState(type, value);
				lua_pop(L, 1);
			}
		}
		lua_pop(L, 1);

		return pPass;
	}



	Material* LoadMatreial(const string& name)
	//void ResourceFactory::LoadOneMaterial(lua_State *L, const string& name)
	{

		lua_State *L = lua_open();

		PreDefineValue(L);
		//PreDefineSurface(L);
		//PreDefineLight(L);

		string filename = string("../res/scene1/") + name + ".mtl";

		if (luaL_dofile(L, filename.c_str()) != 0)
		{
			lua_close(L);
			assert(0);
			return NULL;
		}

		Material* pMaterial = NULL;


		lua_getfield(L, LUA_GLOBALSINDEX, name.c_str());
		if (!lua_isnil(L, -1))
		{
			//string materialName = lua_tostring(L, -2);
			//assert(lua_istable(L, -1));


			Material::MaterialClass mtlClass = Material::CLASS_ORDINARY;
			lua_getfield(L, -1, "Class");
			if (!lua_isnil(L, -1))
			{
				mtlClass = (Material::MaterialClass)lua_tointeger(L, -1);
			}
			lua_pop(L, 1);

			pMaterial = new Material(mtlClass);

			lua_pushnil(L);
			while (lua_next(L, -2) != 0)
			{
				string elemName = lua_tostring(L, -2);
				if (elemName == "Class")
				{
				}
				else
				{
					MaterialPassPtr pPass = LoadMaterialPass(L, elemName);
					pMaterial->AddMaterialPass(pPass);
				}

				lua_pop(L, 1);
			}

			lua_pop(L, 1);
		}
		lua_pop(L, 1);


		//Global::GetResourceManager()->AddMaterial(name, pMaterial);

		lua_close(L);

		return pMaterial;
	}









}