#pragma once
#include "Renderer.h"
#include "stdheader.h"
#include "Util.h"


namespace Aurora
{


//struct ShaderSamplerBinding
//{
//	string Name;
//	Handle handle = -1;
//};

    class ShaderParameterBase;
    class GPUShaderObject;
    class CShaderParameterBuffer;
    class GPUShaderParameterBuffer;


    class CShaderParameterContainer
    {
    public:
        CShaderParameterContainer() = default;
        virtual ~CShaderParameterContainer() = default; 


        void AddParameterBinding(ShaderParameterBase* InParameter)
        {            
            mParameterBindings.push_back(InParameter);
        }
        
        void UpdateValue(const uint8* InData, uint16 InOffset, int InSize)
        {
            Util::MemCopy(InData, mBufferMemory.data() + InOffset, InSize);
        }
        
        vector<ShaderParameterBase*>    mParameterBindings;
        vector<int8>    mBufferMemory;

        GPUShaderParameterBuffer*   mGPUBuffer = nullptr;
    };

    class BaseShader : public CShaderParameterContainer
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

        void InitBase(ShaderType type, const string& pathname);

        void CommitShaderParameter();
        void BindShader();

        void AddShaderParameterBuffer(CShaderParameterBuffer* InBuffer)
        {
            mParameterBuffers.push_back(InBuffer);
        }

        ShaderType mType = VERTEX_SHADER;

        GPUShaderObject* mDeviceHandle = nullptr;
        string mName{"test shader"};

        string pathname_;
        
        vector<CShaderParameterBuffer*> mParameterBuffers;

    private:

    };

    

    class CShaderParameterBuffer : public CShaderParameterContainer
    {
    public:
        CShaderParameterBuffer(const string& InName):mName(InName) {}
        ~CShaderParameterBuffer() override = default;
        
        bool CreateDeviceObject();

        void Commit() const;

        string mName;
    };

    

    class ShaderParameterBase
    {
    public:
        enum ShaderParameterType
        {
            FLOAT,
            FLOAT2,
            FLOAT3,
            FLOAT4,
            MATRIX4X4,
            INVALID,
        };
        
        ShaderParameterBase(const string& InName, CShaderParameterContainer* InContainer)
        {
            mName = InName;
            InContainer->AddParameterBinding(this);
            mContainer = InContainer;
        }

        virtual ~ShaderParameterBase() = default;

        bool IsBind() const  {   return mSizeInByte > 0;   }

        void Update(const uint8* InData)
        {
            if(IsBind() && mContainer)
            {
                mContainer->UpdateValue(InData, mOffset, mSizeInByte);
            }
        }

        virtual ShaderParameterType GetType() const = 0;

        uint16  mOffset = 0;
        uint16  mSizeInByte = 0;
        string mName;
        CShaderParameterContainer* mContainer = nullptr;        
    };

    template<int NumFloat>
    class ShaderParameterFloatN : public ShaderParameterBase
    {
        template<int Num, ShaderParameterBase::ShaderParameterType TValue> struct type_trace_base
        {
            constexpr static ShaderParameterBase::ShaderParameterType Type = TValue; 
        };

        template<int Num> struct type_trace : public type_trace_base<Num, ShaderParameterBase::INVALID> {};        
        template<> struct type_trace<1> : public type_trace_base<1, ShaderParameterBase::FLOAT> {};
        template<> struct type_trace<2> : public type_trace_base<2, ShaderParameterBase::FLOAT2> {};
        template<> struct type_trace<3> : public type_trace_base<3, ShaderParameterBase::FLOAT3> {};
        template<> struct type_trace<4> : public type_trace_base<4, ShaderParameterBase::FLOAT4> {};
        
    public:
        
        ShaderParameterFloatN(const string& InName, CShaderParameterContainer* InContainer)
            : ShaderParameterBase(InName, InContainer)
        {
        }

        ~ShaderParameterFloatN() override = default;
        
        ShaderParameterType GetType() const override
        {
            return type_trace<NumFloat>::Type;
        }

        float operator = (float InValue);
        const Vector2f& operator = (const Vector2f& InValue);
        const Vector3f& operator = (const Vector3f& InValue);
        const Vector4f& operator = (const Vector4f& InValue);
    };
    
    template<int NumFloat> float ShaderParameterFloatN<NumFloat>::operator=(float InValue)
    {
        const float TempData[] = {InValue, 0, 0, 0};
        Update(reinterpret_cast<const uint8*>(TempData));
        return  InValue;
    }
    
    template<int NumFloat> const Vector2f& ShaderParameterFloatN<NumFloat>::operator=(const Vector2f& InValue)
    {
        const float TempData[] = {InValue.x, InValue.y, 0, 0};
        Update(reinterpret_cast<const uint8*>(TempData));
        return  InValue;
    }
    
    template<int NumFloat> const Vector3f& ShaderParameterFloatN<NumFloat>::operator=(const Vector3f& InValue)
    {
        const float TempData[] = {InValue.x, InValue.y, InValue.z, 0};
        Update(reinterpret_cast<const uint8*>(TempData));
        return  InValue;
    }
    
    template<int NumFloat> const Vector4f& ShaderParameterFloatN<NumFloat>::operator=(const Vector4f& InValue)
    {
        const float TempData[] = {InValue.x, InValue.y, InValue.z, InValue.w};
        Update(reinterpret_cast<const uint8*>(TempData));
        return  InValue;
    }

    typedef ShaderParameterFloatN<1> ShaderParameterFloat;
    typedef ShaderParameterFloatN<2> ShaderParameterFloat2;
    typedef ShaderParameterFloatN<3> ShaderParameterFloat3;
    typedef ShaderParameterFloatN<4> ShaderParameterFloat4;
    

    class ShaderParameterMatrix : public ShaderParameterBase
    {
    public:

        ShaderParameterMatrix(const string& InName, CShaderParameterContainer* InContainer)
            : ShaderParameterBase(InName, InContainer)
        {
        }

        ~ShaderParameterMatrix() override = default;

        ShaderParameterType GetType() const override
        {
            return ShaderParameterType::MATRIX4X4;
        }

        ShaderParameterMatrix& operator=(const Matrix4f& other)
        {
            Matrix4f Temp = other;
            Temp.TransposeSelf();
            const auto DataPtr = reinterpret_cast<const uint8*>(Temp.Ptr());
            Update(DataPtr);
            return *this;
        }
    };


    #define DEFINE_SHADER_PARAMETER(InParameter, Type) \
    Type InParameter = {#InParameter, this}


    
    class CViewShaderParameterBuffer : public CShaderParameterBuffer
    {
    public:
        static CShaderParameterBuffer* Instance;
        
        CViewShaderParameterBuffer():CShaderParameterBuffer("View")
        {
            Instance = this;
        }
        
        DEFINE_SHADER_PARAMETER(mViewMatrix, ShaderParameterMatrix);
        DEFINE_SHADER_PARAMETER(mViewMatrixInverse, ShaderParameterMatrix);
        DEFINE_SHADER_PARAMETER(mProjectionMatrix, ShaderParameterMatrix);
        DEFINE_SHADER_PARAMETER(mViewProjectionMatrix, ShaderParameterMatrix);
        DEFINE_SHADER_PARAMETER(DirectionLightDirection0, ShaderParameterFloat3);
        DEFINE_SHADER_PARAMETER(DirectionLightColor0, ShaderParameterFloat4);        
    };


    class ModelShaderVS : public BaseShader
    {
    public:
        ModelShaderVS();
        ~ModelShaderVS() override;

        void Initialize();

        DEFINE_SHADER_PARAMETER(mWorldMatrix, ShaderParameterMatrix);

    };


    class ModelShaderPS : public BaseShader
    {
    public:
        ModelShaderPS();
        ~ModelShaderPS() override;

        void Initialize();

    private:
        ShaderTextureBinding texBinding_;

        ShaderTextureBinding texEnvTex_;
    };




}