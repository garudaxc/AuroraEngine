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
	int8			type = 0;
	string			name;
	string			text;
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


	enum ShaderType : int8
	{
		VERTEX_SHADER,
		PIXEL_SHADER,
		GEOM_SHADER,
		COMPUTE_SHADER,
	};

	BaseShader();
	~BaseShader();

	void	InitBase(ShaderType type, const string& pathname);

	void	CommitShaderParameter();
	void	BindShader();

	ShaderType	type_ = VERTEX_SHADER;

	ShaderParamterBindings	bindings_;
	Handle	handle_ = -1;
	string	name_{ "test shader" };

	string	pathname_;

private:

};



class ModelShaderVS : public BaseShader
{
public:
	ModelShaderVS();
	~ModelShaderVS();

	void	Initialize();

	void	CreateBindings();

	Matrix4f	matWorld;
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
	
private:

	ShaderTextureBinding texBinding_;
};




}