#pragma once
#include "Renderer.h"
#include "stdheader.h"


namespace Aurora
{

    class ShaderConstTable;
    class ShaderConstBuffer;
    class Texture;


    template<class ResType> class ResourcePtr;

    typedef int32 ShaderObjectHandle;


    class ShaderConstTable
    {
    public:
        class ShaderConst
        {
        public:
            string name;
            uint startReg;
            uint numReg;
            uint offsetInBuffer;
            uint id;
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

        ShaderConstBuffer* CreateShaderConstBuffer() const;

        void AddConstBuffer(const string& name, int index, int numVariable, int sizeInByte);

        void AddShaderConstF(const string& name, uint startReg, uint numReg, uint id);

        void AddSampler(const string& name, uint nRegister);

        uint GetNumRegisterF() const;

        void FlushShaderConst(ShaderConstBuffer* pBuffer, uint category);

        bool FindShaderConst(const string& name, uint& offsetInBuffer, uint& numReg);

        bool FindTexture(const string& name, uint& offsetInBuffer);
        bool FindTextureReg(const string& name, uint& nRegIndex);

        void UpdateShaderConst(ShaderConstBuffer* pBuffer, ShaderConstProvider* pProvider);

    private:
        vector<ShaderConst> m_table;
        vector<ShaderConst> m_sampler;

        vector<ShaderConstBuffer> m_ShaderConstBuffers;
    };




    class ShaderConstBuffer
    {
    public:
        ShaderConstBuffer(uint numFloatRegister, uint numTexture);
        ~ShaderConstBuffer();

        float* GetBufferPointer(uint nRegOffset);
        void SetTexture(uint idx, const ResourcePtr<Texture>& pTex);
        Texture* GetTexture(uint idx) const;

    private:
        uint m_nNumFloatRegister;
        uint m_nNumTexture;
        float* m_FloatBuffer;
        //shared_array<ResourcePtr<Texture>>	m_Textures;

    };


    class Shader
    {
    public:
        Shader(void);
        Shader(void* handle, uint instructionCount);
        ~Shader(void);

        static uint MapShaderConstNameToID(const string& name);

        ShaderConstTable* GetShaderConstTable() const;

        virtual void* GetBindID() const { return nullptr; }


        void* HALHandle;

    private:
        uint m_nInstructionCount;
        ShaderConstTable* m_pShaderConstTable;

    };

//////////////////////////////////////////////////////////////////////////


//struct ShaderSamplerBinding
//{
//	string Name;
//	Handle handle = -1;
//};

    class ShaderParameterBase;
    class GPUShaderObject;
    class CShaderParameterBuffer;
    class GPUShaderParameterBuffer;


    class IShaderParameterContainer
    {
    public:
        IShaderParameterContainer() = default;
        virtual ~IShaderParameterContainer() = default; 


        virtual void    AddParameterBinding(ShaderParameterBase* InParameter) = 0;
    };

    class BaseShader : public IShaderParameterContainer
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
        ~BaseShader() override;

        void    AddParameterBinding(ShaderParameterBase* InParameter) override
        {
            mParameterBindings.push_back(InParameter);
        }

        void InitBase(ShaderType type, const string& pathname);

        void CommitShaderParameter();
        void BindShader();

        ShaderType type_ = VERTEX_SHADER;

        ShaderParameterBindings bindings_;
        GPUShaderObject* mDeviceHandle = nullptr;
        string name_{"test shader"};

        string pathname_;

        vector<ShaderParameterBase*>    mParameterBindings;
        vector<CShaderParameterBuffer*> mParamterBuffers;

    private:

    };

    

    class CShaderParameterBuffer : public IShaderParameterContainer
    {
    public:
        CShaderParameterBuffer(const string& InName):mName(InName) {}
        virtual ~CShaderParameterBuffer() = default;

        
        void    AddParameterBinding(ShaderParameterBase* InParameter) override
        {
            mParameterBindings.push_back(InParameter);
        }

        string mName;
        vector<ShaderParameterBase*>    mParameterBindings;
        vector<int8>    mBufferMemory;

        GPUShaderParameterBuffer*   mGPUBuffer = nullptr;
    };

    


    class ShaderParameterBase
    {
    public:
        enum ShaderParameterType
        {
            FLOAT,
            FLOAT2,
            FLOAT4,
            MATRIX4X4,
                        
        };
        
        ShaderParameterBase(const string& InName, IShaderParameterContainer* InContainer)
        {
            mName = InName;
            InContainer->AddParameterBinding(this);
        }

        bool IsBind() const  {   return false;   }

        virtual ShaderParameterType GetType() const = 0;

        uint16  mOffset = 0;
        uint16  mSizeInByte = 0;
        string mName;
    };

    class ShaderParameterMatrix : public ShaderParameterBase
    {
    public:

        ShaderParameterMatrix(const string& InName, IShaderParameterContainer* InContainer)
            : ShaderParameterBase(InName, InContainer)
        {
        }

        ShaderParameterType GetType() const override
        {
            return ShaderParameterType::MATRIX4X4;
        }

        ShaderParameterMatrix& operator=(const Matrix4f& other)
        {
            return *this;
        }

    };


    #define DEFINE_SHADER_PARAMETER(InParameter, Type) \
    Type InParameter = {#InParameter, this};


    
    class CViewShaderParameterBuffer : public CShaderParameterBuffer
    {
    public:
        CViewShaderParameterBuffer():CShaderParameterBuffer("View") {}
        
        DEFINE_SHADER_PARAMETER(mViewMatrix, ShaderParameterMatrix)
        DEFINE_SHADER_PARAMETER(mProjectionMatrix, ShaderParameterMatrix)        
        DEFINE_SHADER_PARAMETER(mViewProjectionMatrix, ShaderParameterMatrix)
        
    };


    class ModelShaderVS : public BaseShader
    {
    public:
        ModelShaderVS();
        ~ModelShaderVS() override;

        void Initialize();

        void CreateBindings();

        Matrix4f matWorld;

    private:
        DEFINE_SHADER_PARAMETER(mWorldMatrix, ShaderParameterMatrix)

    };


    class ModelShaderPS : public BaseShader
    {
    public:
        ModelShaderPS();
        ~ModelShaderPS() override;

        void Initialize();

        void CreateBindings();

        void BindTextureToRender(Texture* texture, Texture* envTex);

    private:
        ShaderTextureBinding texBinding_;

        ShaderTextureBinding texEnvTex_;
    };




}