#include "stdHeader.h"
#include "Shader.h"

#include "DeferredLightingRender.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "FileSystem.h"

namespace Aurora
{


void ShaderConstTable::AddShaderConstF(const string& name, uint startReg, uint numReg, uint id)
{
	vector<ShaderConst>::iterator it = m_table.begin();
	for (;it != m_table.end(); ++it)
	{
		if (it->startReg > startReg)
		{
			break;
		}
	}

	uint offset = 0;
	if (it != m_table.end())
	{
		offset = it->offsetInBuffer;
	}
	else
	{
		offset = GetNumRegisterF();
	}
	
	ShaderConst sc;
	sc.name		= name;
	sc.startReg	= startReg;
	sc.numReg	= numReg;
	sc.id		= id;
	sc.offsetInBuffer = 999;

	it = m_table.insert(it, sc);
	for (;it != m_table.end(); ++it)
	{
		it->offsetInBuffer = offset;
		offset += it->numReg;
	}
}

uint ShaderConstTable::GetNumRegisterF() const
{
	if (m_table.empty())
	{
		return 0;
	}
	return m_table.back().offsetInBuffer + m_table.back().numReg;
}

void ShaderConstTable::AddSampler(const string& name, uint nRegister)
{	
	ShaderConst sc;
	sc.name		= name;
	sc.startReg	= nRegister;
	sc.numReg	= 1;
	sc.id		= 999;
	sc.offsetInBuffer = (uint)m_sampler.size();

	m_sampler.push_back(sc);	
}

void ShaderConstTable::FlushShaderConst(ShaderConstBuffer* pBuffer, uint category)
{
	//if (category == Shader::TYPE_VERTEX_SHADER)
	//{
	//	for (vector<ShaderConst>::iterator it = m_table.begin();
	//		it != m_table.end(); ++it)
	//	{
	//		HALSetVertexShaderConstantF(
	//			pBuffer->GetBufferPointer(it->offsetInBuffer),
	//			it->startReg, it->numReg);
	//	}
	//}
	//else if (category == Shader::TYPE_PIXEL_SHADER)
	//{
	//	for (vector<ShaderConst>::iterator it = m_table.begin();
	//		it != m_table.end(); ++it)
	//	{
	//		HALSetPixelShaderConstantF(
	//			pBuffer->GetBufferPointer(it->offsetInBuffer),
	//			it->startReg, it->numReg);
	//	}
	//}

	//for (vector<ShaderConst>::iterator it = m_sampler.begin();
	//	it != m_sampler.end(); ++it)
	//{
	//	HALSetTexture(it->startReg,
	//		pBuffer->GetTexture(it->offsetInBuffer));
	//}
}

ShaderConstBuffer* ShaderConstTable::CreateShaderConstBuffer() const
{
	ShaderConstBuffer* pBuffer(new ShaderConstBuffer(GetNumRegisterF(), (uint)m_sampler.size()));

	for (vector<ShaderConst>::const_iterator it = m_sampler.begin(); it != m_sampler.end(); ++it)
	{
		//ResourcePtr<RenderTarget> pRT = Global::GetResourceManager()->GetRenderTarget(it->name);
		//if (pRT)
		//{
		//	assert(0);
		//	//pBuffer->SetTexture(it->offsetInBuffer, pRT->AsTexture());
		//}
	}

	return pBuffer;
}


bool ShaderConstTable::FindShaderConst(const string& name, uint& offsetInBuffer, uint& numReg)
{
	for (vector<ShaderConst>::iterator it = m_table.begin();
		it != m_table.end(); ++it)
	{
		if (it->name == name)
		{
			offsetInBuffer = it->offsetInBuffer;
			numReg = it->numReg;
			return true;
		}
	}

	return false;
}

bool ShaderConstTable::FindTexture(const string& name, uint& offsetInBuffer)
{
	for (vector<ShaderConst>::iterator it = m_sampler.begin();
		it != m_sampler.end(); ++it)
	{
		if (it->name == name)
		{
			offsetInBuffer = it->offsetInBuffer;
			return true;
		}
	}

	return false;
}


bool ShaderConstTable::	FindTextureReg(const string& name, uint& nRegIndex)
{
	for (vector<ShaderConst>::iterator it = m_sampler.begin();
		it != m_sampler.end(); ++it)
	{
		if (it->name == name)
		{
			nRegIndex = it->startReg;
			return true;
		}
	}

	return false;
}

void ShaderConstTable::UpdateShaderConst(ShaderConstBuffer* pBuffer, ShaderConstProvider* pProvider)
{
	for (vector<ShaderConst>::iterator it = m_table.begin();
		it != m_table.end(); ++it)
	{
		pProvider->OnUpdateShaderConstF(it->id, 
			pBuffer->GetBufferPointer(it->offsetInBuffer),
			it->numReg);
	}
}
//////////////////////////////////////////////////////////////////////////


ShaderConstBuffer::ShaderConstBuffer(uint numFloatRegister, uint numTexture):
m_nNumFloatRegister(numFloatRegister), m_nNumTexture(numTexture)
{
	if (numFloatRegister > 0)
	{
		m_FloatBuffer = new float[numFloatRegister * 4];
	}

	if (numTexture > 0)
	{
		//m_Textures.reset(new ResourcePtr<Texture>[numTexture]);
	}
}

ShaderConstBuffer::~ShaderConstBuffer()
{

}


float* ShaderConstBuffer::GetBufferPointer(uint nRegOffset)
{
	return m_FloatBuffer + nRegOffset * 4;
}


void ShaderConstBuffer::SetTexture(uint idx, const ResourcePtr<Texture>& pTex)
{
	//m_Textures[idx] = pTex;
}


Texture* ShaderConstBuffer::GetTexture(uint idx) const
{
	return nullptr;
	//return m_Textures[idx].Get();
}

//////////////////////////////////////////////////////////////////////////

Shader::Shader(void):HALHandle(nullptr)
{
}


Shader::Shader(void* handle, uint instructionCount):
HALHandle(handle),m_nInstructionCount(instructionCount)
{
	m_pShaderConstTable = new ShaderConstTable;
}

Shader::~Shader(void)
{
}

uint Shader::MapShaderConstNameToID(const string& name)
{
	for (int i = 0; i < SHADER_CONST_USERDEFINE; i++)
	{
		if (name == ShaderConstName[i].name)
		{
			return i;
		}
	}
	return SHADER_CONST_USERDEFINE;
}


ShaderConstTable* Shader::GetShaderConstTable() const
{
	return m_pShaderConstTable;
}

//////////////////////////////////////////////////////////////////////////






BaseShader::BaseShader()
{
}

BaseShader::~BaseShader()
{
}

void BaseShader::InitBase(ShaderType type, const string& pathname)
{
	type_ = type;
	pathname_ = pathname;

	FilePtr file(GFileSys->OpenFile(pathname_));

	ShaderCode code;
	code.text = file->ReadAsString();
	code.type = type_;
	code.name = pathname_;
	
	if (type_ == VERTEX_SHADER) {
		code.defines.push_back(make_pair("VERTEX_SHADER", "1"));
	}

	if (type_ == PIXEL_SHADER) {
		code.defines.push_back(make_pair("PIXEL_SHADER", "1"));
	}

	mDeviceHandle = GRenderDevice->CreateShader(code);
}

void BaseShader::CommitShaderParameter()
{
	if (bindings_.handle >= 0) {
		GRenderDevice->UpdateShaderParameter(bindings_.handle);
	}
}

void BaseShader::BindShader()
{
	assert(mDeviceHandle >= 0);
	if (type_ == VERTEX_SHADER) {
		GRenderDevice->BindVertexShader(mDeviceHandle);
	}

	if (type_ == PIXEL_SHADER) {
		GRenderDevice->BindPixelShader(mDeviceHandle);
	}
}



ModelShaderVS::ModelShaderVS()
{
}

ModelShaderVS::~ModelShaderVS()
{
}

void ModelShaderVS::Initialize()
{	
	InitBase(VERTEX_SHADER, "..\\dev\\data\\shader\\testVS.shd");
	CreateBindings();
}

void ModelShaderVS::CreateBindings()
{
	bindings_.Name = "$Globals";
	
	//ShaderParamterBindingItem 
	bindings_.Bindings.push_back(ShaderParamterBindingItem{ "matWorld", 0, matWorld.Ptr() });

	bindings_.handle = GRenderDevice->CreateShaderParameterBinding(mDeviceHandle, bindings_);	
}

ModelShaderPS::ModelShaderPS()
{
}

ModelShaderPS::~ModelShaderPS()
{
}

void ModelShaderPS::Initialize()
{
	InitBase(PIXEL_SHADER, "..\\dev\\data\\shader\\testPS2.shder");
	CreateBindings();
}

void ModelShaderPS::CreateBindings()
{
	if (mDeviceHandle < 0) {
		return;
	}
	//bindings_.Name = "$Globals";

	////ShaderParamterBindingItem 
	//bindings_.Bindings.push_back(ShaderParamterBindingItem{ "matWorld", 0, matWorld.Ptr() });
	//bindings_.Bindings.push_back(ShaderParamterBindingItem{ "matViewProj", 0, matViewProj.Ptr() });

	//bindings_.handle = CreateShaderParameterBinding(handle_, bindings_);
	texBinding_.Name = "g_txDiffuse";
	texBinding_.handle = GRenderDevice->CreateShaderTextureBinding(mDeviceHandle, texBinding_);

	texEnvTex_.Name = "g_txEnv";
	texEnvTex_.handle = GRenderDevice->CreateShaderTextureBinding(mDeviceHandle, texEnvTex_);
}


void ModelShaderPS::BindTextureToRender(Texture* texture, Texture* envTex)
{
	if (texBinding_.handle >= 0) {
		GRenderDevice->BindTexture(texBinding_.handle, texture);
	}

	if (texEnvTex_.handle >= 0) {
		GRenderDevice->BindTexture(texEnvTex_.handle, envTex);
	}
}


}