#pragma once
#include "ResourceManager.h"
#include <d3d10.h>
namespace Aurora
{

#if 0

	class Shader;
	class ShaderConstTable;
	class MaterialInstance;
	class MaterialPass;
	class Material;


	Material* LoadMatreial(const string& name);



class StateValuePair
{
public:
	void	AddStateValue(uint type, uint value)		{	states.push_back(make_pair(type, value));	}

	vector<pair<uint, uint>>		states;
};

class MaterialPass
{
public:
	MaterialPass(const Shader*& pVertexShader, const Shader*& pPixelShader, 
		const ShaderConstTable*& pVertexSCT, const ShaderConstTable*& pPixelSCT);
	~MaterialPass();

	void									BindToRenderer();
	const ShaderConstTable*&				GetVertexSCT() const	{	return m_pVertexSCT;	}
	const ShaderConstTable*&				GetPixelSCT() const		{	return m_pPixelSCT;		}
	void									AddSamplerState(const string& sampler, uint type, uint value);
	void									AddRenderState(uint type, uint value);

private:
	Shader*				m_pVertexShader;
	Shader*				m_pPixelShader;

	ShaderConstTable*		m_pVertexSCT;
	ShaderConstTable*		m_pPixelSCT;

	map<uint, StateValuePair>	m_SamplerStates;
	StateValuePair				m_RenderState;

};



class Material : public Resource
{
public:
	enum MaterialClass
	{
		CLASS_ORDINARY,
		CLASS_TRANSPARENT,
	};
	

	Material(MaterialClass mtlClass = CLASS_ORDINARY);
	~Material();

	void									AddMaterialPass(const MaterialPassPtr& pMaterialPass);

	void									BeginPass(uint nPass);
	void									EndPass();
	void									ApplyInstance(MaterialInstance* pInst);

	MaterialClass							GetClass() const			{	return m_nMtlClass;	}

	void									SetPriority(int priority)	{	m_nPriority = priority;	}
	int										GetPriority() const			{	return m_nPriority;	}

	uint									GetNumPass() const			{	return (uint)m_MaterialPass.size();	}

	MaterialInstance*						CreateMaterialInstance();
	virtual void*							GetBindID() const				{	return 0;	}

	/*bool									IsTransparent() const;
	bool									IsUseAlphaTest() const;*/


private:
	vector<MaterialPassPtr>					m_MaterialPass;
	uint									m_nCurPass;

	MaterialClass							m_nMtlClass;
	int										m_nPriority;

};


#endif


//////////////////////////////////////////////////////////////////////////

enum EShaderVarType
{
	SVT_SCALAR,
	SVT_VECTOR,
	SVT_MATRIX,
	SVT_TEXTURE,
	SVT_SAMPLER,
};

class ShaderVariable
{
public:


	ShaderVariable(const string& name);
	virtual ~ShaderVariable();

	const string&		GetName() const					{	return m_Name;	}
	uint				GetNumElement() const			{	return m_nNumElem;	}
	virtual EShaderVarType	GetType() const = 0;
	virtual void		SetValue(void* pData) = 0;

protected:
	string		m_Name;
	uint		m_nNumElem;
};

class ScalarShaderVariable : public ShaderVariable
{
public:
	ScalarShaderVariable(const string& name, ID3D10EffectVariable* pEffectVariable);
	~ScalarShaderVariable();
	virtual EShaderVarType	GetType() const;
	virtual void		SetValue(void* pData);

private:
	ID3D10EffectScalarVariable*		m_pD3DEffectVariable;

};

class VectorShaderVariable : public ShaderVariable
{
public:
	VectorShaderVariable(const string& name, uint numFloats, ID3D10EffectVariable* pEffectVariable);
	virtual ~VectorShaderVariable();

	virtual EShaderVarType	GetType() const;
	virtual void		SetValue(void* pData);

private:
	ID3D10EffectVectorVariable*		m_pD3DEffectVariable;
};

class MatrixShaderVariable : public ShaderVariable
{
public:
	MatrixShaderVariable(const string& name, uint nNumRows, uint nNumColumns, 
							ID3D10EffectVariable* pEffectVariable);
	virtual ~MatrixShaderVariable();

	virtual EShaderVarType	GetType() const;
	virtual void		SetValue(void* pData);

private:
	ID3D10EffectMatrixVariable*		m_pD3DEffectVariable;
	uint		m_nNumRows;
	uint		m_nNumColumns;

};

class TextureShaderVariable : public ShaderVariable
{
public:
	TextureShaderVariable(const string& name, ID3D10EffectVariable* pEffectVariable);
	virtual ~TextureShaderVariable();

	virtual EShaderVarType	GetType() const;
	virtual void		SetValue(void* pData);

private:
	ID3D10EffectShaderResourceVariable*		m_pD3DEffectVariable;
};


struct ConstBuffer
{
	string name;
	int size;
	int numMember;

	ID3D10Buffer* handle;
};



class MaterialTemplate
{
public:
	MaterialTemplate();
	~MaterialTemplate();

	bool	Load(const string& pathName, const set<string>& MaterialDef);

	ShaderVariable*	FindShaderVariable(const string& name);

	uint	GetNumShaderVariables() const		{	return m_Variables.size();	}
	ShaderVariable*	GetShaderVariable(uint idx)	{	return m_Variables[idx];	}

	void	Apply(uint nTechnique);

protected:
	void	InitShaderVariables();

	uint		m_nNumTechnique;
	uint		m_nNumVariable;

	int m_nConstBuffer;


	ID3D10Effect*	m_pD3DEffect;

	vector<ShaderVariable*>		m_Variables;

	vector<ConstBuffer> m_ConstBuffers;

};


//////////////////////////////////////////////////////////////////////////
enum EShaderVarible
{
	SV_MATVIEW = 0,
	SV_MATPROJ,
	SV_MATVIEWPROJ,

	SV_BACKBUFFERSIZE,
	SV_CAMERAVECX,
	SV_CAMERAVECY,
	SV_CAMERAVECZ,
	SV_CAMERAPARAM,
	SV_CAMERAPOS,
	SV_CAMERALOOKDIR,

	SV_GBUFFER0,
	SV_GBUFFER1,
	SV_LIGHTINGBUFFER,
	
	SV_NUMBER,
};


class ShaderManager : public MaterialTemplate
{
public:
	ShaderManager();
	~ShaderManager();

	void		Initialize();
	void		Finalize();

	void*		GetPoolHandle()			{	return m_PoolHandle;	}

	void SetVector(EShaderVarible sv, const float* v);

	void SetTexture(EShaderVarible sv, Texture* pTex);

	void SetMatrix(EShaderVarible sv, const Matrix4f& mat);

	void BindGPassShader();
	void BindDirLightShader();
	void BindShadingPassShader();
	void BindDebugShader();

	//void		SetViewProj(const Matrix4f& matViewProj);
private:

	
	VectorShaderVariable*		m_pVecCameraParam;

	ShaderVariable* m_ShaderVars[SV_NUMBER];

	void*		m_PoolHandle;
};

extern ShaderManager GShaderManager;


//////////////////////////////////////////////////////////////////////////


class MaterialTemplateManager
{
public:
	MaterialTemplateManager();
	~MaterialTemplateManager();

	void Initialize();
	void Finalize();

	MaterialTemplate*	GetMaterialTemplate(const ResourceID& id, const set<string>& MaterialDef);
	MaterialTemplate*	GetMaterialTemplate(const ResourceID& id);

private:
	MaterialTemplate*	Find(const set<string>& id);
	MaterialTemplate*	Load(const ResourceID& id, const set<string>& MaterialDef);

	map<set<string>, MaterialTemplate*>		m_ResourcePool;
};


extern MaterialTemplateManager GMaterialTemplateManager;


}