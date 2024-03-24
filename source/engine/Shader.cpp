#include "stdHeader.h"
#include "Shader.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "FileSystem.h"

namespace Aurora
{

    CShaderParameterBuffer* CViewShaderParameterBuffer::Instance = nullptr;


    ShaderParameterBase* CShaderParameterContainer::FindParameter(const String& InName)
    {
        for (auto Parameter : mParameterBindings)
        {
            if(Parameter->mName == InName)
            {
                return Parameter;
            }
        }
        return nullptr;
    }
    
    BaseShader::BaseShader()
    {
    }

    BaseShader::~BaseShader()
    {
    }

    void BaseShader::InitBase(ShaderType type, const String& pathname)
    {
        mType = type;
        pathname_ = pathname;

        FilePtr file(GFileSys->OpenFile(pathname_, false));

        ShaderCode code;
        code.text = file->ReadAsString();
        code.type = mType;
        code.name = pathname_;

        if (mType == VERTEX_SHADER)
        {
            code.defines.push_back(make_pair("VERTEX_SHADER", "1"));
        }

        if (mType == PIXEL_SHADER)
        {
            code.defines.push_back(make_pair("PIXEL_SHADER", "1"));
        }

        mDeviceHandle = GRenderDevice->CreateGPUShaderObject(code, this);
    }

    void BaseShader::CommitShaderParameter()
    {
        if (!mBufferMemory.empty() && mGPUBuffer != nullptr)
        {
            GRenderDevice->UpdateGPUShaderParameterBuffer(mGPUBuffer, mBufferMemory);
        }
    }

    void BaseShader::BindShader()
    {
        assert(mDeviceHandle >= 0);
        if (mType == VERTEX_SHADER)
        {
            GRenderDevice->BindVertexShader(mDeviceHandle);
        }

        if (mType == PIXEL_SHADER)
        {
            GRenderDevice->BindPixelShader(mDeviceHandle);
        }
    }

    bool CShaderParameterBuffer::CreateDeviceObject()
    {
        assert(mGPUBuffer == nullptr);
        mGPUBuffer = GRenderDevice->CreateShaderParameterBuffer(this);

        return mGPUBuffer != nullptr;
    }
    void CShaderParameterBuffer::Commit() const
    {
        if (!mBufferMemory.empty() && mGPUBuffer != nullptr)
        {
            GRenderDevice->UpdateGPUShaderParameterBuffer(mGPUBuffer, mBufferMemory);
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
        AddShaderParameterBuffer(CViewShaderParameterBuffer::Instance);
        InitBase(VERTEX_SHADER, "..\\dev\\data\\shader\\testVS.shader");
    }

    ModelShaderPS::ModelShaderPS()
    {
    }

    ModelShaderPS::~ModelShaderPS()
    {
    }

    void ModelShaderPS::Initialize()
    {
        AddShaderParameterBuffer(CViewShaderParameterBuffer::Instance);
        InitBase(PIXEL_SHADER, "..\\dev\\data\\shader\\testPS2.shader");
    }

    // void ModelShaderPS::CreateBindings()
    // {
    //     if (mDeviceHandle < 0)
    //     {
    //         return;
    //     }
    //
    //     texBinding_.Name = "g_txDiffuse";
    //     texBinding_.handle = GRenderDevice->CreateShaderTextureBinding(mDeviceHandle, texBinding_);
    //
    //     texEnvTex_.Name = "g_txEnv";
    //     texEnvTex_.handle = GRenderDevice->CreateShaderTextureBinding(mDeviceHandle, texEnvTex_);
    // }
    //
    //
    // void ModelShaderPS::BindTextureToRender(Texture* texture, Texture* envTex)
    // {
    //     if (texBinding_.handle >= 0)
    //     {
    //         GRenderDevice->BindTexture(texBinding_.handle, texture);
    //     }
    //
    //     if (texEnvTex_.handle >= 0)
    //     {
    //         GRenderDevice->BindTexture(texEnvTex_.handle, envTex);
    //     }
    // }


}