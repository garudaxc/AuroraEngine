#pragma once
#include "tinyxml.h"

namespace Aurora
{

#if 0

class Material;
class MaterialInstance;
class ShaderConstBuffer;
class ShaderConstTable;
class Texture;
class ShaderConstParam;
template <class ResType> class ResourcePtr;


class ShaderConstParam
{
public:
	ShaderConstParam(uint numReg);
	~ShaderConstParam()			{}

	void					AddBufferPointer(float* pointer);
	void					AddBufferPointer(const vector<float*>& pointers);

	const Vector3f&			operator = (const Vector3f& vec);
	const Color&			operator = (const Color& color);
	const Matrix4f&			operator = (const Matrix4f& matrix);

private:
	vector<float*>	m_BufPointers;
	uint			m_nNumReg;
	
	// material instance ptr
};


class MaterialInstance
{
public:
	friend class Material;

	MaterialInstance(Material* pMaterial);
	~MaterialInstance();

	void					SetName(const string& name);
	const string&			GetName() const;

	Material*				GetMaterialTemplate();
	const Material*			GetMaterialTemplate() const;
	
	uint					GetNumPass() const;

	void					UpdateShaderConst(ShaderConstProvider* pProvider);

	void					SetValueF(const string& name, const float* pValue, uint numFloats);

	void					SetTexture(const string& name, const ResourcePtr<Texture>& pTex);

	ShaderConstParamPtr		CreateShaderConstParam(const string& name);

private:
	void					AddMaterialPass(const ShaderConstTable*& pVertexSCT, const ShaderConstTable*& pPixelSCT);
	// flush shader const to device
	void					FlushShaderConst(uint nPass);

	string							m_Name;

	Material*						m_pMaterial;
	vector<ShaderConstTable*>		m_pVertexSCT;
	vector<ShaderConstTable*>		m_pPixelSCT;
	vector<ShaderConstBuffer*>	m_pVertexSCB;
	vector<ShaderConstBuffer*>	m_pPixelSCB;
};

inline void MaterialInstance::SetName(const string& name)
{
	m_Name = name;
}

inline const string& MaterialInstance::GetName() const
{
	return m_Name;
}

inline Material* MaterialInstance::GetMaterialTemplate()
{	
	return m_pMaterial;	
}

inline const Material* MaterialInstance::GetMaterialTemplate() const
{	
	return m_pMaterial;
}


#endif



class MaterialInstance;
class Texture;

class MaterialInstLib
{
public:
	MaterialInstLib();
	~MaterialInstLib();

	void LoadMaterialInstances(const string& pathName);

	MaterialInstance* GetMaterialInst(const string& id);

private:
	TiXmlDocument* m_pOpenedLib;
};



class MaterialTemplate;
class ShaderVariable;


class ShaderVariableValue
{
public:
	ShaderVariableValue(ShaderVariable* pVariable);
	virtual ~ShaderVariableValue();

	const string& GetName() const;
	uint GetNumElement() const;
	int GetType() const;

	virtual void Commit() = 0;
	virtual void SetValue(const void* pValue) = 0;

protected:

	ShaderVariable*		m_pVariable;
};



class MaterialInstance
{
public:
	MaterialInstance(MaterialTemplate* pMtlTemplate, const vector<ShaderVariableValue*>& definedVariables);
	~MaterialInstance();

	const string& GetTemplateName() const;

	bool SetVariableValue(const string& name, const void* pValue);

	ShaderVariableValue* CreateVariable(const string& name);

	void Apply(uint nTechnique);

	void Commit();

	uint bReciveLight : 1;
	uint bTransparent : 1;
	uint bAlphaTest : 1;
	uint bReciveShadow : 1;
	uint bCastShadow : 1;

private:
	void AddShaderVariableValue(ShaderVariableValue* pValue);

	ShaderVariableValue* FindVariableValue(const string& name);

	string m_Name;

	Color m_cDiffuse;

	Texture* m_pTexDiffuse;
	Texture* m_pTexBump;

	MaterialTemplate* m_pMtlTemplate;

	vector<ShaderVariableValue*> m_VariableValues;
};

//////////////////////////////////////////////////////////////////////////


class ShaderVariableStrategy
{
public:
	ShaderVariableStrategy();
	~ShaderVariableStrategy();

	void SetWorldMatrix(const Matrix4f& matWorld);
	const Matrix4f& GetWorldMatrix() const;

private:
	Matrix4f		m_matWorld;

};

extern ShaderVariableStrategy shaderManager;


class MaterialTemplate;
class MaterialInstanceManager
{
public:
	MaterialInstanceManager();
	~MaterialInstanceManager();

	void Initialize();
	void Finalize();

	MaterialInstance* GetMaterialInstance(const ResourceID& id, const set<string>& MaterialDef);

	MaterialInstance* CreateInstanceFromTemplate(MaterialTemplate* pTemplate);


	ShaderVariableValue* CreateValueFromVarible(ShaderVariable* pVariable);

	ShaderVariableValue* CreateDefiendVariable(ShaderVariable* pVariable);

private:
	MaterialInstance* Find(const set<string>& id);
	MaterialInstance* Load(const ResourceID& id, const set<string>& MaterialDef);


	map<set<string>, MaterialInstance*>	m_ResourcePool;

};

extern MaterialInstanceManager GMaterialInstanceManager;


}