#pragma once

namespace Aurora
{


enum ShaderConstID
{
	SHADER_CONST_MATRIX_WORLD = 0,
	SHADER_CONST_MATRIX_VIEW,
	SHADER_CONST_MATRIX_PROJ,
	SHADER_CONST_MATRIX_VIEWPROJ,
	SHADER_CONST_MATRIX_WORLDINV,
	SHADER_CONST_MATRIX_VIEWINV,

	SHADER_CONST_WORLD_POS,

	SHADER_CONST_CAMERA_POS,
	SHADER_CONST_CAMERA_PARAM,
	SHADER_CONST_CAMERA_VEC,
	SHADER_CONST_CAMERA_LOOKDIR,

	SHADER_CONST_BACKBUFFER_SIZE_PARAM,

	SHADER_CONST_GLOBALAMBIENT,

	SHADER_CONST_LIGHT0_POS,
	SHADER_CONST_LIGHT0_DIR,
	SHADER_CONST_LIGHT0_DIFFUSE,
	SHADER_CONST_LIGHT0_AMBIENT,
	SHADER_CONST_LIGHT0_SPECULAR,
	SHADER_CONST_LIGHT1_POS,
	SHADER_CONST_LIGHT1_DIR,
	SHADER_CONST_LIGHT1_DIFFUSE,
	SHADER_CONST_LIGHT1_AMBIENT,
	SHADER_CONST_LIGHT1_SPECULAR,
	
	SHADER_CONST_MATERIAL_DIFFUSE,
	SHADER_CONST_MATERIAL_AMBIENT,
	SHADER_CONST_MATERIAL_SPECULAR,

	SHADER_CONST_TIME,

	SHADER_CONST_USERDEFINE,
};


struct ShaderConstInfo
{
	const char*		name;
	unsigned int	global;
};

static ShaderConstInfo ShaderConstName[] = {
	{"matWorld",			0},
	{"matWiew",				1},
	{"matProj",				1},
	{"matViewProj",			1},
	{"matWorldInv",			0},
	{"matViewInv",			1},

	{"WorldPos",			0},

	{"CameraPos",			1},
	{"CameraParam",			1},	// near far plane,  xy screen scale
	{"CameraVec",			1},	// camera look, right, up direction
	{"CameraLookDir",		1},

	// x : 1 / back buffer width
	// y : 1 / back buffer height
	// z : back buffer width
	// w : back buffer height
	{"BackBufferSizeParam",	1},

	{"GlobalAmbient",		1},

	{"LIGHT0_POS",			0},
	{"LIGHT0_DIR",			0},
	{"LIGHT0_DIFFUSE",		0},
	{"LIGHT0_AMBIENT",		0},
	{"LIGHT0_SPECULAR",		0},
	{"LIGHT1_POS",			0},
	{"LIGHT1_DIR",			0},
	{"LIGHT1_DIFFUSE",		0},
	{"LIGHT1_AMBIENT",		0},
	{"LIGHT1_SPECULAR",		0},

	{"MATERIAL_DIFFUSE",	0},
	{"MATERIAL_AMBIENT",	0},
	{"MATERIAL_SPECULAR",	0},

	{"Time",				1},

	{"USERDEFINE",			0},
};




unsigned int MapSamplerStateType(const std::string& typeName);
unsigned int MapRenderStateType(const std::string& typeName);
unsigned int MapRenderStateValue(const std::string& typeName);



class Texture;
class ShaderConstProvider
{
public:
	virtual	int				OnMapShaderConstId(const std::string& name) {	return -1;	}

	virtual	void			OnUpdateShaderConstF(int id, float* pOutBuffer, uint nCount) {}
	virtual	void			OnUpdateShaderConstI(int id, int* pOutBuffer, uint nCount) {}
	virtual	void			OnUpdateShaderConstB(int id, int* pOutBuffer, uint nCount) {}

	virtual Texture*		OnUpdateTexture(int id) {	return nullptr;	}
};


}