#pragma once
#include "stdheader.h"


namespace Aurora
{

class ShaderConstTable;
class ShaderConstBuffer;
class Texture;


template <class ResType> class ResourcePtr;

typedef int32		ShaderObjectHandle;


class ShaderConstTable
{
public:

	class ShaderConst
	{
	public:
		string				name;
		uint				startReg;
		uint				numReg;
		uint				offsetInBuffer;
		uint				id;
	};
/*
	class ConstBuffer
	{
	public:

		string				name;
		int					index;
		int					numVariable;
		int					sizeInByte;
	};*/

	ShaderConstBuffer*	CreateShaderConstBuffer() const;

	void					AddConstBuffer(const string& name, int index, int numVariable, int sizeInByte);

	void					AddShaderConstF(const string& name, uint startReg, uint numReg, uint id);

	void					AddSampler(const string& name, uint nRegister);

	uint					GetNumRegisterF() const;

	void					FlushShaderConst(ShaderConstBuffer* pBuffer, uint category);

	bool					FindShaderConst(const string& name, uint& offsetInBuffer, uint& numReg);

	bool					FindTexture(const string& name, uint& offsetInBuffer);
	bool					FindTextureReg(const string& name, uint& nRegIndex);

	void					UpdateShaderConst(ShaderConstBuffer* pBuffer, ShaderConstProvider* pProvider);

private:
	vector<ShaderConst>		m_table;
	vector<ShaderConst>		m_sampler;

	vector<ShaderConstBuffer>	m_ShaderConstBuffers;
};




class ShaderConstBuffer
{
public:
	ShaderConstBuffer(uint numFloatRegister, uint numTexture);
	~ShaderConstBuffer();

	float*			GetBufferPointer(uint nRegOffset);
	void			SetTexture(uint idx, const ResourcePtr<Texture>& pTex);
	Texture*		GetTexture(uint idx) const;

private:
	uint			m_nNumFloatRegister;
	uint			m_nNumTexture;
	float*			m_FloatBuffer;
	//shared_array<ResourcePtr<Texture>>	m_Textures;

};


class Shader
{
public:



	enum Category
	{
		TYPE_VERTEX_SHADER,
		TYPE_PIXEL_SHADER,
		TYPE_GEOM_SHADER,
		TYPE_COMPUTE_SHADER,

	};

	Shader(void);
	Shader(void* handle, uint instructionCount);
	~Shader(void);

	static uint				MapShaderConstNameToID(const string& name);

	ShaderConstTable*		GetShaderConstTable() const;

	virtual void*			GetBindID() const		{	return NULL;	}


	void*					HALHandle;
private:
	uint					m_nInstructionCount;
	ShaderConstTable*		m_pShaderConstTable;

};



//////////////////////////////////////////////////////////////////////////

struct ShaderCode
{
	Shader::Category	type;
	string				name;
	string				text;
	vector<string>		includes;
	vector<pair<string, string>>	 defines;
};


struct ShaderParamterBindingItem
{
	string	Name;
	int32	Size;
	void* Source;
};

struct ShaderParamterBindings
{
	string Name;
	vector<ShaderParamterBindingItem>	Bindings;
	Handle  handle = -1;
};

struct ShaderTextureBinding
{
	string Name;
	Handle handle = -1;
};

//struct ShaderSamplerBinding
//{
//	string Name;
//	Handle handle = -1;
//};


class BaseShader
{
public:
	BaseShader();
	~BaseShader();

	void	CommitShaderParameter();
	void	BindShader();

	Shader::Category	type_ = Shader::Category::TYPE_VERTEX_SHADER;

	ShaderParamterBindings	bindings_;
	Handle	handle_ = -1;
	string	name_{ "test shader" };

private:

};



class ModelShaderVS : public BaseShader
{
public:
	ModelShaderVS();
	~ModelShaderVS();

	void	Initialize();

	void	CreateBindings();

	string	pathname_{"..\\dev\\data\\shader\\testVS.shd"};

	Matrix4f	matWorld;
	Matrix4f	matViewProj;
private:

};


class ModelShaderPS : public BaseShader
{
public:
	ModelShaderPS();
	~ModelShaderPS();

	void	Initialize();

	void	CreateBindings();

	void	BindTextureToRender(Texture* texture);

	string	pathname_{ "..\\dev\\data\\shader\\testPS2.shder" };

private:

	ShaderTextureBinding texBinding_;

};




}