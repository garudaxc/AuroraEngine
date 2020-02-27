#include "stdHeader.h"
#include "MaterialInstance.h"
#include "Shader.h"
#include "Material.h"
#include "Texture.h"
#include "Shader.h"
#include "Util.h"
#include "Config.h"
#include "Log.h"
#include "Util.h"
namespace Aurora
{

#if 0


ShaderConstParam::ShaderConstParam(uint numReg):m_nNumReg(numReg)
{

}

void ShaderConstParam::AddBufferPointer(float* pointer)
{
	m_BufPointers.push_back(pointer);
}

void ShaderConstParam::AddBufferPointer(const vector<float*>& pointers)
{
	m_BufPointers.insert(m_BufPointers.end(), pointers.begin(), pointers.end());
}

const Vector3f& ShaderConstParam::operator = (const Vector3f& vec)
{
	if (m_nNumReg > 0)
	{
		for (vector<float*>::iterator it = m_BufPointers.begin(); 
			it != m_BufPointers.end(); ++it)
		{
			memcpy_s((*it), sizeof(float) * 4 * m_nNumReg, vec.Ptr(), sizeof(vec));
		}
	}
	return vec;
}

const Color& ShaderConstParam::operator = (const Color& color)
{
	if (m_nNumReg > 0)
	{
		for (vector<float*>::iterator it = m_BufPointers.begin(); 
			it != m_BufPointers.end(); ++it)
		{
			memcpy_s((*it), sizeof(float) * 4 * m_nNumReg, color.Ptr(), sizeof(color));
		}
	}
	return color;
}


const Matrix4f& ShaderConstParam::operator = (const Matrix4f& matrix)
{
	if (m_nNumReg > 0)
	{
		for (vector<float*>::iterator it = m_BufPointers.begin(); 
			it != m_BufPointers.end(); ++it)
		{
			MapMatrixToShaderConst(matrix, (*it), m_nNumReg);
		}
	}
	return matrix;
}







//////////////////////////////////////////////////////////////////////////

MaterialInstance::MaterialInstance(Material* pMaterial):
m_pMaterial(pMaterial)
{
}

MaterialInstance::~MaterialInstance()
{

}

uint MaterialInstance::GetNumPass() const
{
	return m_pMaterial->GetNumPass();
}

void MaterialInstance::UpdateShaderConst(ShaderConstProvider* pProvider)
{
	uint nNumPass = GetNumPass();
	for (uint i = 0; i < nNumPass; i++)
	{
		m_pVertexSCT[i]->UpdateShaderConst(m_pVertexSCB[i].get(), pProvider);
		m_pPixelSCT[i]->UpdateShaderConst(m_pPixelSCB[i].get(), pProvider);
	}
}

void MaterialInstance::SetValueF(const string& name, const float* pValue, uint numFloats)
{
	uint nNumPass = GetNumPass();
	for (uint i = 0; i < nNumPass; i++)
	{
		uint offset = 0, numReg = 0;
		if (m_pVertexSCT[i]->FindShaderConst(name, offset, numReg))
		{
			float* pBuffer = m_pVertexSCB[i]->GetBufferPointer(offset);
			memcpy_s(pBuffer, sizeof(float) * numReg * 4, pValue, numFloats * sizeof(float));
		}

		if (m_pPixelSCT[i]->FindShaderConst(name, offset, numReg))
		{
			float* pBuffer = m_pPixelSCB[i]->GetBufferPointer(offset);
			memcpy_s(pBuffer, sizeof(float) * numReg * 4, pValue, numFloats * sizeof(float));
		}
	}
}



ShaderConstParamPtr MaterialInstance::CreateShaderConstParam(const string& name)
{
	uint numReg = 0;
	vector<float*> pointers;

	uint nNumPass = GetNumPass();
	for (uint i = 0; i < nNumPass; i++)
	{
		uint offset = 0;
		if (m_pVertexSCT[i]->FindShaderConst(name, offset, numReg))
		{
			assert(numReg != 0);
			float* pBuffer = m_pVertexSCB[i]->GetBufferPointer(offset);;
			pointers.push_back(pBuffer);
		}

		if (m_pPixelSCT[i]->FindShaderConst(name, offset, numReg))
		{
			assert(numReg != 0);
			float* pBuffer = m_pPixelSCB[i]->GetBufferPointer(offset);
			pointers.push_back(pBuffer);
		}
	}

	ShaderConstParamPtr pParam(new ShaderConstParam(numReg));
	if (numReg > 0)
	{
		pParam->AddBufferPointer(pointers);
	}

	return pParam;
}

void MaterialInstance::SetTexture(const string& name, const ResourcePtr<Texture>& pTex)
{
	uint nNumPass = GetNumPass();
	for (uint i = 0; i < nNumPass; i++)
	{
		uint offset = 0;
		if (m_pVertexSCT[i]->FindTexture(name, offset))
		{
			m_pVertexSCB[i]->SetTexture(offset, pTex);
		}

		if (m_pPixelSCT[i]->FindTexture(name, offset))
		{
			m_pPixelSCB[i]->SetTexture(offset, pTex);
		}
	}
}

// flush shader const to device
void MaterialInstance::FlushShaderConst(uint nPass)
{
	m_pVertexSCT[nPass]->FlushShaderConst(m_pVertexSCB[nPass].get(), Shader::TYPE_VERTEX_SHADER);
	m_pPixelSCT[nPass]->FlushShaderConst(m_pPixelSCB[nPass].get(), Shader::TYPE_PIXEL_SHADER);
}


void MaterialInstance::AddMaterialPass(const ShaderConstTable*& pVertexSCT, const ShaderConstTable*& pPixelSCT)
{
	ShaderConstBuffer* pVertexSCB = pVertexSCT->CreateShaderConstBuffer();
	ShaderConstBuffer* pPixelSCB = pPixelSCT->CreateShaderConstBuffer();

	m_pVertexSCT.push_back(pVertexSCT);
	m_pPixelSCT.push_back(pPixelSCT);
	m_pVertexSCB.push_back(pVertexSCB);
	m_pPixelSCB.push_back(pPixelSCB);
}

//////////////////////////////////////////////////////////////////////////


void ApplyXmlTextures(TiXmlNode* pXmlTextures, MaterialInstance* pMatInst)
{
	string diffuseTex = "../res/asset/default_albedo.dds";
	string bumpTex = "../res/asset/default_normal.dds";

	float bumpniess = 1.0f;

	TiXmlNode* pXmlTex = pXmlTextures->FirstChild();
	while (pXmlTex)
	{
		if (strcmp(pXmlTex->Value(), "Diffuse") == 0)
		{
			string text = pXmlTex->FirstChild()->ToText()->Value();
			diffuseTex = text;
		}
		else if (strcmp(pXmlTex->Value(), "Bump") == 0)
		{
			bumpniess = (float)atof(pXmlTex->ToElement()->Attribute("Amount"));

			string text = pXmlTex->FirstChild()->ToText()->Value();
			bumpTex = text;
		}

		pXmlTex = pXmlTex->NextSibling();
	}

	pMatInst->SetTexture("texAlbedo", Global::GetResourceManager()->GetTexture(diffuseTex));
	pMatInst->SetTexture("texNormal", Global::GetResourceManager()->GetTexture(bumpTex));

	pMatInst->SetValueF("Bumpniess", &bumpniess, 1);
}


#endif

MaterialInstLib::MaterialInstLib()
{

}

MaterialInstLib::~MaterialInstLib()
{
}

void MaterialInstLib::LoadMaterialInstances(const string& pathName)
{
	//m_pOpenedLib = new TiXmlDocument();
	//if (!m_pOpenedLib->LoadFile(pathName.c_str()))
	//{
	//	assert(0);
	//}
}

MaterialInstance* MaterialInstLib::GetMaterialInst(const string& id)
{
	//TiXmlNode* pXmlRoot = m_pOpenedLib->FirstChild("MaterialInstances");

	//TiXmlNode* pXmlMaterialInst = pXmlRoot->FirstChild(id.c_str());
	//assert(pXmlMaterialInst);

	//MaterialInstance* pMatInst = Global::GetResourceManager()->CreateMaterialInst("MatPreNormal");
	//string name = pXmlMaterialInst->Value();
	//pMatInst->SetName(name);

	//TiXmlNode* pElem = pXmlMaterialInst->FirstChild();
	//while (pElem)
	//{
	//	if (strcmp(pElem->Value(), "Diffuse") == 0)
	//	{
	//		string text = pElem->FirstChild()->ToText()->Value();
	//		Color c = ParseColor(text);
	//		pMatInst->SetValueF("DiffuseColor", c.Ptr(), 3);
	//	}
	//	else if (strcmp(pElem->Value(), "Ambient") == 0)
	//	{
	//	}
	//	else if (strcmp(pElem->Value(), "Specular") == 0)
	//	{
	//	}
	//	else if (strcmp(pElem->Value(), "Emissive") == 0)
	//	{
	//		string text = pElem->FirstChild()->ToText()->Value();
	//		Color c = ParseColor(text);
	//		pMatInst->SetValueF("EmissiveColor", c.Ptr(), 3);
	//	}
	//	else if (strcmp(pElem->Value(), "Glossiness") == 0)
	//	{
	//		string text = pElem->FirstChild()->ToText()->Value();
	//		float value = (float)atof(text.c_str());
	//		pMatInst->SetValueF("Glossiness", &value, 1);
	//	}
	//	else if (strcmp(pElem->Value(), "SpecularLevel") == 0)
	//	{
	//		string text = pElem->FirstChild()->ToText()->Value();
	//		float value = (float)atof(text.c_str());
	//		pMatInst->SetValueF("SpecularLevel", &value, 1);
	//	}
	//	else if (strcmp(pElem->Value(), "Textures") == 0)
	//	{
	//		ApplyXmlTextures(pElem, pMatInst.get());
	//	}

	//	pElem = pElem->NextSibling();
	//}

	//return pMatInst;


	MaterialInstance* pMatInst;
	return pMatInst;
}



//===========================

ShaderVariableValue::ShaderVariableValue(ShaderVariable* pVariable):m_pVariable(pVariable)
{

}

ShaderVariableValue::~ShaderVariableValue()
{

}

const string& ShaderVariableValue::GetName() const
{
	return m_pVariable->GetName();
}

uint ShaderVariableValue::GetNumElement() const
{
	return m_pVariable->GetNumElement();
}

int ShaderVariableValue::GetType() const
{
	return m_pVariable->GetType();
}

//===================================

class VectorShaderVarValue : public ShaderVariableValue
{
public:
	VectorShaderVarValue(ShaderVariable* pVariable);
	virtual ~VectorShaderVarValue();


	virtual void		Commit();
	virtual void		SetValue(const void* pValue);

private:
	vector<float>	m_data;
};

VectorShaderVarValue::VectorShaderVarValue(ShaderVariable* pVariable):ShaderVariableValue(pVariable)
{
	m_data.resize(pVariable->GetNumElement());
}

VectorShaderVarValue::~VectorShaderVarValue()
{

}

void VectorShaderVarValue::Commit()
{
	m_pVariable->SetValue((void*)&m_data[0]);
}

void VectorShaderVarValue::SetValue(const void* pValue)
{
	uint sizeInByte = sizeof(float) * m_data.size();
	memcpy_s(&m_data[0], sizeInByte, pValue, sizeInByte);
}

//===================================

class TextureShaderVarValue : public ShaderVariableValue
{
public:
	TextureShaderVarValue(ShaderVariable* pVariable);
	virtual ~TextureShaderVarValue();

	virtual void		Commit();
	virtual void		SetValue(const void* pValue);

private:
	Texture*	m_pTexture;
};

TextureShaderVarValue::TextureShaderVarValue(ShaderVariable* pVariable):ShaderVariableValue(pVariable)
{
}

TextureShaderVarValue::~TextureShaderVarValue()
{
}

void TextureShaderVarValue::Commit()
{
	m_pVariable->SetValue((void*)m_pTexture);
}

void TextureShaderVarValue::SetValue(const void* pValue)
{
	m_pTexture = (Texture*)pValue;
}

//======================================

class WorldMatrixShaderVarValue : public ShaderVariableValue
{
public:
	WorldMatrixShaderVarValue(ShaderVariable* pVariable, ShaderVariableStrategy* pVarStrategy);
	virtual ~WorldMatrixShaderVarValue();

	virtual void		Commit();
	virtual void		SetValue(const void* pValue)	{}

private:
	 ShaderVariableStrategy* m_pVarStrategy;
};

WorldMatrixShaderVarValue::WorldMatrixShaderVarValue(ShaderVariable* pVariable, ShaderVariableStrategy* pVarStrategy)
:ShaderVariableValue(pVariable),m_pVarStrategy(pVarStrategy)
{

}

WorldMatrixShaderVarValue::~WorldMatrixShaderVarValue()
{

}

void WorldMatrixShaderVarValue::Commit()
{
	m_pVariable->SetValue((void*)m_pVarStrategy->GetWorldMatrix().Ptr());
}


//////////////////////////////////////////////////////////////////////////

ShaderVariableStrategy::ShaderVariableStrategy()
{

}

ShaderVariableStrategy::~ShaderVariableStrategy()
{

}

void ShaderVariableStrategy::SetWorldMatrix(const Matrix4f& matWorld)
{	
	m_matWorld = matWorld;	
}

const Matrix4f& ShaderVariableStrategy::GetWorldMatrix() const
{
	return m_matWorld;
}

ShaderVariableStrategy shaderManager;


//////////////////////////////////////////////////////////////////////////

MaterialInstanceManager GMaterialInstanceManager;

MaterialInstanceManager::MaterialInstanceManager()	{}

MaterialInstanceManager::~MaterialInstanceManager()	{}

void MaterialInstanceManager::Initialize()
{
}

void MaterialInstanceManager::Finalize()
{
}


MaterialInstance* MaterialInstanceManager::GetMaterialInstance(const ResourceID& id, const set<string>& MaterialDef)
{
	set<string> instId = MaterialDef;
	instId.insert(id);
	MaterialInstance* pMtlInst = Find(instId);
	if (pMtlInst == NULL)
	{
		return Load(id, MaterialDef);
	}
	return pMtlInst;
}

MaterialInstance* MaterialInstanceManager::Find(const set<string>& id)
{
	map<set<string>, MaterialInstance*>::iterator it = m_ResourcePool.find(id);
	if (it != m_ResourcePool.end())
	{
		return it->second;
	}
	return NULL;
}


MaterialInstance* MaterialInstanceManager::CreateInstanceFromTemplate(MaterialTemplate* pTemplate)
{
	return nullptr;
	//vector<ShaderVariableValue*> variableValues;
	//for (uint i = 0; i < pTemplate->GetNumShaderVariables(); i++)
	//{
	//	ShaderVariable* pVariable = pTemplate->GetShaderVariable(i);

	//	ShaderVariableValue* pValue = CreateDefiendVariable(pVariable);
	//	if (pValue)
	//	{
	//		variableValues.push_back(pValue);
	//	}
	//}

	//MaterialInstance* pInst = new MaterialInstance(pTemplate, variableValues);
	//return pInst;
}


ShaderVariableValue* MaterialInstanceManager::CreateValueFromVarible(ShaderVariable* pVariable)
{
	ShaderVariableValue* pValue = NULL;

	if (pVariable->GetType() == SVT_VECTOR ||
		pVariable->GetType() == SVT_SCALAR ||
		pVariable->GetType() == SVT_MATRIX)
	{
		pValue = new VectorShaderVarValue(pVariable);
	}
	else if (pVariable->GetType() == SVT_TEXTURE)
	{
		pValue = new TextureShaderVarValue(pVariable);		
	}

	return pValue;
}

ShaderVariableValue* MaterialInstanceManager::CreateDefiendVariable(ShaderVariable* pVariable)
{
	ShaderVariableValue* pValue = NULL;
	if (pVariable->GetName() == "mMatWorld")
	{
		pValue = new WorldMatrixShaderVarValue(pVariable, &shaderManager);
	}

	return pValue;
}



static bool ParseValue(float* buffer, const char* pText, int numElement)
{
	int nOk = 0;
	if (numElement == 1)
	{
		nOk = sscanf_s(pText, "%f", buffer);
	}
	else if (numElement == 3)
	{
		nOk = sscanf_s(pText, "%f,%f,%f", buffer, buffer + 1, buffer + 2);
	}
	else if (numElement == 2)
	{
		nOk = sscanf_s(pText, "%f,%f", buffer, buffer + 1);
	}
	else
	{
		assert(0);
	}

	if (nOk == EOF)
	{
		return false;
	}
	return true;
}

MaterialInstance* MaterialInstanceManager::Load(const ResourceID& id, const set<string>& MaterialDef)
{
	return nullptr;

	string pathName = Util::ProcessPathName(id);

	TiXmlDocument doc;
	bool bOk = doc.LoadFile(pathName.c_str());
	
	if (!bOk)
	{
		GLog->Error("open material instance file \"%s\" failed!", id.c_str());
		return NULL;
	}

	TiXmlElement* pXmlMtlInst = doc.FirstChild()->ToElement();
	if (strcmp(pXmlMtlInst->Value(), "MaterialInstance") != 0)
	{
		GLog->Error("file \"%s\"is not a material instance file!", id.c_str());
		return NULL;
	}

//	string name = pXmlMtlInst->Attribute("name");
	string templateName = pXmlMtlInst->Attribute("Template");

	MaterialTemplate* pMtlTemplate = GMaterialTemplateManager.GetMaterialTemplate(templateName, MaterialDef);
	//if (pMtlTemplate == NULL)
	//{
	//	// log can't create material template
	//	return NULL;
	//}

	MaterialInstance* pInstance = CreateInstanceFromTemplate(pMtlTemplate);

	{
		TiXmlElement* pXmlVariableValues = pXmlMtlInst->FirstChildElement("VariableValue");
		TiXmlElement* pXmlValue = pXmlVariableValues->FirstChildElement();
		while (pXmlValue)
		{
			const char* name = pXmlValue->Value();


			string id = pXmlValue->FirstChild()->ToText()->Value();

			//ShaderVariableValue* pValue = pInstance->FindVariableValue(string(name));
			//ShaderVariableValue* pValue = pInstance->CreateVariable(string(name));
			//if (pValue)
			//{
			//	if (pValue->GetType() == SVT_SCALAR ||
			//		pValue->GetType() == SVT_VECTOR ||
			//		pValue->GetType() == SVT_MATRIX)
			//	{
			//		const char* pText = pXmlValue->FirstChild()->ToText()->Value();
			//		float buffer[16];
			//		bOk = ParseValue(buffer, pText, pValue->GetNumElement());
			//		if (bOk)
			//		{
			//			pValue->SetValue((const void*)buffer);
			//		}
			//		else
			//		{
			//			// log parse value error
			//			assert(0);
			//		}

			//	}
			//	else if (pValue->GetType() == SVT_TEXTURE)
			//	{
			//		string id = pXmlValue->FirstChild()->ToText()->Value();

			//		Texture* pTexture = GTextureManager.GetTexture(id);
			//		if (pTexture != NULL)
			//		{
			//			pValue->SetValue(pTexture);
			//		}
			//	}				
			//}
			//else
			//{
			//	// log can not find shader variable
			//	GLog->Error("can not find material variable : %s", name);
			//}
			
			pXmlValue = pXmlValue->NextSiblingElement();
		}
	}

	set<string> instId = MaterialDef;
	instId.insert(id);
	m_ResourcePool.insert(make_pair(instId, pInstance));

	return pInstance;

	return nullptr;
}

//////////////////////////////////////////////////////////////////////////

MaterialInstance::MaterialInstance(MaterialTemplate* pMtlTemplate, const vector<ShaderVariableValue*>& definedVariables):
m_pMtlTemplate(pMtlTemplate),m_VariableValues(definedVariables)
{
	bReciveLight	= 1;
	bTransparent	= 0;
	bAlphaTest		= 0;
	bReciveShadow	= 1;
	bCastShadow		= 1;

	m_pTexDiffuse = NULL;
	m_pTexBump = NULL;
}

MaterialInstance::~MaterialInstance()
{

}

const string& MaterialInstance::GetTemplateName() const
{
	return m_Name;
	//return m_pMtlTemplate->GetName();
}

bool MaterialInstance::SetVariableValue(const string& name, const void* pValue)
{
	ShaderVariableValue* pVariable = FindVariableValue(name);

	if (pVariable == NULL)
	{
		pVariable = CreateVariable(name);
	}

	if (pVariable == NULL)
	{
		return false;
	}

	pVariable->SetValue(pValue);
	return true;
}

void MaterialInstance::AddShaderVariableValue(ShaderVariableValue* pValue)
{
	m_VariableValues.push_back(pValue);
}

ShaderVariableValue* MaterialInstance::FindVariableValue(const string& name)
{
	for (uint i = 0; i < m_VariableValues.size(); i++)
	{
		if (m_VariableValues[i]->GetName() == name)
		{
			return m_VariableValues[i];
		}
	}
	return NULL;
}

void MaterialInstance::Apply(uint nTechnique)
{
	for (uint i = 0; i < m_VariableValues.size(); i++)
	{
		m_VariableValues[i]->Commit();
	}

	m_pMtlTemplate->Apply(nTechnique);
}

void MaterialInstance::Commit()
{
	for (uint i = 0; i < m_VariableValues.size(); i++)
	{
		m_VariableValues[i]->Commit();
	}
}


ShaderVariableValue* MaterialInstance::CreateVariable(const string& name)
{
	for (uint i = 0; i < m_pMtlTemplate->GetNumShaderVariables(); i++)
	{
		ShaderVariable* pVariable = m_pMtlTemplate->GetShaderVariable(i);

		if (pVariable->GetName() == name)
		{
			ShaderVariableValue* pValue = GMaterialInstanceManager.CreateValueFromVarible(pVariable);
			AddShaderVariableValue(pValue);

			return pValue;
		}
	}

	return NULL;
}


}