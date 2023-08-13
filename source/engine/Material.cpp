#include "stdHeader.h"
#include "Material.h"
#include "Renderer.h"
#include "MaterialInstance.h"
#include "Shader.h"
//#include "RendererDX10.h"
//#include <d3d10shader.h>
//#include <d3dx10.h>
#include "Log.h"
//#include "lua.hpp"
#include "Config.h"
#include "Camera.h"

namespace Aurora
{




struct PredefShader
{
	MaterialTemplate* pLPPShading;

	MaterialTemplate* pDirLight;
	MaterialTemplate* pPointLight;
	MaterialTemplate* pSpotLight;

	MaterialTemplate* gDebugShader;
};

PredefShader m_Shaders;

ShaderManager GShaderManager;
ShaderManager::ShaderManager()
{
}

ShaderManager::~ShaderManager()
{
}

void ShaderManager::Initialize()
{
	//string pathName = Config::Get()->GetShaderPath() + "GlobalShaderVariables.fx";

	//ID3D10EffectPool* pEffectPool = nullptr;
	//ID3D10Blob* pError = nullptr;
	//HRESULT hr = D3DX10CreateEffectPoolFromFileA(pathName.c_str(), nullptr, nullptr, "fx_4_0", 
	//												0, 0, D3D9Device, 
	//												nullptr, &pEffectPool, &pError, nullptr);

	//if (FAILED(hr) && pError)
	//{
	//	const char* error = (char*)(pError->GetBufferPointer());
	//	GLog->Error(error);
	//	pError->Release();
	//}
	//m_PoolHandle = (void*)pEffectPool;

	//m_pD3DEffect = pEffectPool->AsEffect();

	//InitShaderVariables();

	//struct svInfo
	//{
	//	EShaderVarible var;
	//	const char* name;
	//};

	//svInfo info [] = {
	//	{SV_MATVIEW, "MatViewProj"},
	//	{SV_MATPROJ, "MatViewProj"},
	//	{SV_MATVIEWPROJ, "MatViewProj"},

	//	{SV_BACKBUFFERSIZE, "BackBufferSize"},
	//	{SV_CAMERAVECX, "vCameraVecX"},
	//	{SV_CAMERAVECY, "vCameraVecY"},
	//	{SV_CAMERAVECZ, "vCameraVecZ"},
	//	{SV_CAMERAPARAM, "vCameraParam"},
	//	{SV_CAMERAPOS, "vCameraPos"},
	//	{SV_CAMERALOOKDIR, "vCameraLookDir"},

	//	{SV_GBUFFER0, "tGbuffer0"},
	//	{SV_GBUFFER1, "tGbuffer1"},
	//	{SV_LIGHTINGBUFFER, "tLightingBuffer"},
	//};

	//int size = sizeof(info) / sizeof(info[0]);
	//for (int i = 0; i < size; i++)
	//{
	//	m_ShaderVars[info[i].var] = FindShaderVariable(info[i].name);
	//}

	//memset(&m_Shaders, 0, sizeof(m_Shaders));
	//m_Shaders.pLPPShading = GMaterialTemplateManager.GetMaterialTemplate("PrePassShading.fx");
	//m_Shaders.pDirLight = GMaterialTemplateManager.GetMaterialTemplate("DirectionalLight.fx");
	//m_Shaders.gDebugShader = GMaterialTemplateManager.GetMaterialTemplate("Debug.fx");




}

void ShaderManager::Finalize()
{
	((ID3D10EffectPool*)m_PoolHandle)->Release();
	m_PoolHandle = nullptr;
}


void ShaderManager::BindGPassShader()
{
	m_Shaders.pLPPShading->Apply(0);
}

void ShaderManager::BindDirLightShader()
{
	m_Shaders.pDirLight->Apply(0);
}

void ShaderManager::BindShadingPassShader()
{
	m_Shaders.pLPPShading->Apply(1);
}

void ShaderManager::BindDebugShader()
{
	m_Shaders.gDebugShader->Apply(0);
}

void ShaderManager::SetTexture(EShaderVarible sv, Texture* pTex)
{
	if (m_ShaderVars[sv] && m_ShaderVars[sv]->GetType() == SVT_TEXTURE)
	{
		m_ShaderVars[sv]->SetValue((void*)pTex);
	}
	else 
	{
		GLog->Error("ShaderManager::SetTexture failed");
	}

}

void ShaderManager::SetMatrix(EShaderVarible sv, const Matrix4f& mat)
{
	if (m_ShaderVars[sv] && m_ShaderVars[sv]->GetType() == SVT_MATRIX)
	{
		m_ShaderVars[sv]->SetValue((void*)mat.Ptr());
	}
	else 
	{
		GLog->Error("ShaderManager::SetMatrix failed");
	}
}


void ShaderManager::SetVector(EShaderVarible sv, const float* v)
{
	if (m_ShaderVars[sv] && m_ShaderVars[sv]->GetType() == SVT_VECTOR)
	{
		m_ShaderVars[sv]->SetValue((void*)v);
	}
	else 
	{
		GLog->Error("ShaderManager::SetVector failed");
	}
}


//////////////////////////////////////////////////////////////////////////


ShaderVariable::ShaderVariable(const string& name):m_Name(name)
{

}

ShaderVariable::~ShaderVariable()
{

}
//==================
ScalarShaderVariable::ScalarShaderVariable(const string& name, ID3D10EffectVariable* pEffectVariable):
ShaderVariable(name)
{
	m_nNumElem = 1;
	m_pD3DEffectVariable = pEffectVariable->AsScalar();

}

ScalarShaderVariable::~ScalarShaderVariable()
{
}

EShaderVarType ScalarShaderVariable::GetType() const
{
	return SVT_SCALAR;
}

void ScalarShaderVariable::SetValue(void* pData)
{
	m_pD3DEffectVariable->SetFloat(*((float*)pData));
}

//==================
VectorShaderVariable::VectorShaderVariable(const string& name, uint numFloats,
					ID3D10EffectVariable* pEffectVariable):ShaderVariable(name)
{
	m_nNumElem = numFloats;
	m_pD3DEffectVariable = pEffectVariable->AsVector();
}

VectorShaderVariable::~VectorShaderVariable()
{

}

EShaderVarType VectorShaderVariable::GetType() const
{
	return SVT_VECTOR;
}

void VectorShaderVariable::SetValue(void* pData)
{

	m_pD3DEffectVariable->SetFloatVector((float*)pData);
}

//======================


MatrixShaderVariable::MatrixShaderVariable(const string& name, uint nNumRows, uint nNumColumns,
								ID3D10EffectVariable* pEffectVariable):ShaderVariable(name)
{
	m_nNumRows = nNumRows;
	m_nNumColumns = nNumColumns;
	m_nNumElem = nNumRows * nNumColumns;
	m_pD3DEffectVariable = pEffectVariable->AsMatrix();
}

MatrixShaderVariable::~MatrixShaderVariable()
{

}

EShaderVarType MatrixShaderVariable::GetType() const
{
	return SVT_MATRIX;
}

void MatrixShaderVariable::SetValue(void* pData)
{
	m_pD3DEffectVariable->SetMatrix((float*)pData);
}

//=====================
TextureShaderVariable::TextureShaderVariable(const string& name, ID3D10EffectVariable* pEffectVariable)
:ShaderVariable(name)
{
	m_pD3DEffectVariable = pEffectVariable->AsShaderResource();
}

TextureShaderVariable::~TextureShaderVariable()	
{

}

EShaderVarType TextureShaderVariable::GetType() const
{
	return SVT_TEXTURE;
}

void TextureShaderVariable::SetValue(void* pData)
{
	Texture* pTexture = (Texture*)pData;

	ID3D10ShaderResourceView* pSRView = nullptr;
	if (pTexture)
	{
		pSRView = (ID3D10ShaderResourceView*)pTexture->HALHandle;
	}
	m_pD3DEffectVariable->SetResource(pSRView);
}

//=====================

MaterialTemplate::MaterialTemplate()
{

}

MaterialTemplate::~MaterialTemplate()
{
	for (vector<ShaderVariable*>::iterator it = m_Variables.begin(); it != m_Variables.end(); ++it)
	{
		delete *it;
	}
	m_Variables.clear();
}

bool MaterialTemplate::Load(const string& pathName, const set<string>& MaterialDef)
{
	//ID3D10Blob* pError = nullptr;
	//ID3D10EffectPool* pEffectPool = (ID3D10EffectPool*)GShaderManager.GetPoolHandle();

	//vector<D3D10_SHADER_MACRO> macros;
	//for (set<string>::const_iterator it = MaterialDef.begin(); it != MaterialDef.end(); ++it) {
	//	D3D10_SHADER_MACRO mac;
	//	mac.Name = it->c_str();
	//	mac.Definition = "1";
	//	macros.push_back(mac);
	//}
	//D3D10_SHADER_MACRO mac = {nullptr, nullptr};
	//macros.push_back(mac);


	//HRESULT hr = D3DX10CreateEffectFromFileA(pathName.c_str(), &macros[0], nullptr, "fx_4_0" , 
	//										0, D3D10_EFFECT_COMPILE_CHILD_EFFECT, 
	//										D3D9Device, pEffectPool,
	//										nullptr, &m_pD3DEffect, &pError, nullptr);

	//if (FAILED(hr))	{
	//	if (pError)		{
	//		const char* error = (char*)(pError->GetBufferPointer());
	//		GLog->Error(error);
	//		pError->Release();
	//	}else {
	//		GLog->Error("MaterialTemplate::Load failed!");
	//	}
	//	return false;
	//}

	//InitShaderVariables();

	//m_pD3DEffect->Optimize();

	return true;
}


void MaterialTemplate::InitShaderVariables()
{
	//HRESULT hr = S_OK;

	//D3D10_EFFECT_DESC desc;
	//m_pD3DEffect->GetDesc(&desc);


	//m_nNumTechnique = desc.Techniques;
	//m_nNumVariable = desc.GlobalVariables;
	//m_nConstBuffer = desc.ConstantBuffers;

	//for (int i = 0; i < m_nConstBuffer; i++)
	//{
	//	ID3D10EffectConstantBuffer* pCBuffer = m_pD3DEffect->GetConstantBufferByIndex(i);
	//	D3D10_EFFECT_VARIABLE_DESC desc;
	//	hr = pCBuffer->GetDesc(&desc);

	//	ConstBuffer cb;
	//	cb.name = desc.Name;
	//	pCBuffer->GetConstantBuffer(&cb.handle);
	//	D3D10_BUFFER_DESC buffDesc;
	//	cb.handle->GetDesc(&buffDesc);
	//	
	//	ID3D10EffectType* pType = pCBuffer->GetType();
	//	D3D10_EFFECT_TYPE_DESC typeDesc;
	//	pType->GetDesc(&typeDesc);
	//	cb.numMember = typeDesc.Members;

	//	m_ConstBuffers.push_back(cb);
	//}

	//for (uint i = 0; i < m_nNumVariable; i++)
	//{
	//	ID3D10EffectVariable* pVariable = m_pD3DEffect->GetVariableByIndex(i);
	//	D3D10_EFFECT_VARIABLE_DESC varDesc;
	//	pVariable->GetDesc(&varDesc);

	//	ID3D10EffectType* pType = pVariable->GetType();
	//	D3D10_EFFECT_TYPE_DESC typeDesc;
	//	pType->GetDesc(&typeDesc);

	//	string name = varDesc.Name;
	//	ShaderVariable* pSV = nullptr;

	//	if (typeDesc.Class == D3D10_SVC_SCALAR)
	//	{
	//		pSV = new ScalarShaderVariable(name, pVariable);
	//	}
	//	else if (typeDesc.Class == D3D10_SVC_VECTOR)
	//	{
	//		assert(typeDesc.Type == D3D10_SVT_FLOAT);
	//		uint nNumElem = typeDesc.Columns;
	//		pSV = new VectorShaderVariable(name, nNumElem, pVariable);
	//	}
	//	else if (typeDesc.Class == D3D10_SVC_MATRIX_COLUMNS)
	//	{
	//		assert(typeDesc.Type == D3D10_SVT_FLOAT);
	//		//uint nNumElem = typeDesc.Columns;
	//		pSV = new MatrixShaderVariable(name, typeDesc.Rows, typeDesc.Columns, pVariable);
	//	}
	//	else if (typeDesc.Class == D3D10_SVC_OBJECT)
	//	{
	//		if (typeDesc.Type == D3D10_SVT_TEXTURE1D ||
	//			typeDesc.Type == D3D10_SVT_TEXTURE2D ||
	//			typeDesc.Type == D3D10_SVT_TEXTURE3D)
	//		{
	//			pSV = new TextureShaderVariable(name, pVariable);
	//		}
	//		else if (typeDesc.Type == D3D10_SVT_SAMPLER)
	//		{
	//		}
	//		else if (typeDesc.Type == D3D10_SVT_STRING)
	//		{
	//		}
	//	}
	//	else 
	//	{
	//		assert(0);
	//	}

	//	if (pSV != nullptr)
	//	{
	//		m_Variables.push_back(pSV);
	//	}
	//}

}

ShaderVariable*	MaterialTemplate::FindShaderVariable(const string& name)
{
	for (vector<ShaderVariable*>::iterator it = m_Variables.begin(); 
		it != m_Variables.end(); ++it)
	{
		if ((*it)->GetName() == name)
		{
			return *it;
		}
	}
	return nullptr;
}

void MaterialTemplate::Apply(uint nTechnique)
{
	m_pD3DEffect->GetTechniqueByIndex(nTechnique)->GetPassByIndex(0)->Apply(0);
}


//////////////////////////////////////////////////////////////////////////

MaterialTemplateManager GMaterialTemplateManager;



void MaterialTemplateManager::Initialize()
{
}

void MaterialTemplateManager::Finalize()
{
}

MaterialTemplateManager::MaterialTemplateManager()
{
}

MaterialTemplateManager::~MaterialTemplateManager()
{
}

MaterialTemplate* MaterialTemplateManager::GetMaterialTemplate(const ResourceID& id, const set<string>& MaterialDef)
{
	return nullptr;
	//set<string> mtlId = MaterialDef;
	//mtlId.insert(id);
	//MaterialTemplate* pMtl = Find(mtlId);
	//if (pMtl == nullptr)
	//{
	//	return Load(id, MaterialDef);
	//}
	//return pMtl;

}


MaterialTemplate* MaterialTemplateManager::GetMaterialTemplate(const ResourceID& id)
{
	return GetMaterialTemplate(id, set<string>());
}

MaterialTemplate* MaterialTemplateManager::Find(const set<string>& id)
{
	map<set<string>, MaterialTemplate*>::iterator it = m_ResourcePool.find(id);
	if (it != m_ResourcePool.end())
	{
		return it->second;
	}
	return nullptr;

}

MaterialTemplate* MaterialTemplateManager::Load(const ResourceID& id, const set<string>& MaterialDef)
{
	string pathName = Config::Get()->GetShaderPath() + id;
	MaterialTemplate* pMtl = new MaterialTemplate();
	if (pMtl->Load(pathName, MaterialDef))
	{
		set<string> mtlId = MaterialDef;
		mtlId.insert(id);
		m_ResourcePool.insert(make_pair(mtlId, pMtl));
		return pMtl;
	}

	return nullptr;
}

}