#ifndef GARENDERER_H
#define GARENDERER_H

#include "Types.h"
#include "CGeometry.h"
#include "Texture.h"

namespace Aurora
{

    class BaseShader;
    class ShaderConstTable;
    class Texture;
    class RenderOperator;
    class VertexLayout;
    class RenderTarget;
    class StateValuePair;
    class CShaderParameterContainer;
    class CVertexFactory;
    class CScreen;

    enum RendererType
    {
        DirectX11,
        OpenGL,
    };


    extern IRenderDevice* GRenderDevice;
    
    class RenderOperator
    {
    public:
        enum EPrimitiveType : int8
        {
            POINT_LIST = 0,
            PT_LINELIST,
            PT_LINESTRIP,
            PT_TRIANGLELIST,
            PT_TRIANGLESTRIP,
        };

        RenderOperator() = default;

        RenderOperator(CGPUGeometryBuffer* InGeometryBuffer,
                       EPrimitiveType InPrimType, int32 InBaseVertexIndex, uint InStartIndex,
                       int32 InIndexCount, CVertexFactory* InVertexFactory)
        {
            GeometryBuffer = InGeometryBuffer;
            PrimType = InPrimType;
            BaseVertexIndex = InBaseVertexIndex;
            StartIndex = InStartIndex;
            IndexCount = InIndexCount;
            mVertexFactory = InVertexFactory;
        }


        Material* pMaterial = nullptr;
        MaterialInstance* pMtlInst = nullptr;
        CGPUGeometryBuffer* GeometryBuffer = nullptr;
        CVertexFactory* mVertexFactory = nullptr;

        EPrimitiveType PrimType = PT_TRIANGLELIST;

        int32 BaseVertexIndex;
        uint StartIndex;
        int32 IndexCount;
    };


    template<class type>
    int32 FindAvailableSlot(vector<type*>& v)
    {
        auto it = std::find(v.begin(), v.end(), nullptr);
        if (it == v.end())
        {
            v.push_back(nullptr);
            return v.size() - 1;
        }

        return std::distance(v.begin(), it);
    }

    template<class type>
    int32 FindAvailableSlot(vector<type>& v)
    {
        for (auto it = v.begin(); it != v.end(); ++it)
        {
            if (!it->Occupied)
            {
                return std::distance(v.begin(), it);
            }
        }

        v.push_back(type());
        return v.size() - 1;
    }


    class File;
    class CShaderParameterBuffer;
    
    class GPUShaderObject
    {
    public:
        
    };

    class GPUShaderParameterBuffer
    {
    public:
        
    };

    
    struct ShaderCode
    {
        int8 type = 0;
        string name;
        string text;
        vector<string> includes;
        vector<pair<string, string>> defines;
    };

    
    struct ShaderTextureBinding
    {
        string Name;
        Handle handle = -1;
    };

    

    class IRenderDevice
    {
    public:
        enum ClearBufferMask
        {
            CLEAR_DEPTH_BUFFER   = 0x1,
            CLEAR_STENCIL_BUFFER = 0x2,
            CLEAR_FRAME_BUFFER   = 0x4,
        };

        virtual ~IRenderDevice() =default;


        static IRenderDevice* CreateDevice(RendererType InType, CScreen* InScreen);

        static bool Initialized();

        virtual GPUShaderObject* CreateGPUShaderObject(const ShaderCode& code, BaseShader* InShader) = 0;

        virtual Texture* CreateTexture(File* file) = 0;

        virtual Texture* CreateTexture(Texture::Type type, const Texture::Desc& desc) = 0;

        virtual RenderTarget* CreateRenderTarget(const RenderTarget::Desc& desc) = 0;


        //////////////////////////////////////////////////////////////////////////

        virtual void Clear(int clearMask, const Color& clearColor = Color::BLUE, float fDepth = 1.0f, uint8 nStencil = 0) = 0;
        virtual void Present() = 0;

        virtual RenderTarget* GetFrameBuffer() = 0;
        virtual RenderTarget* GetDepthStencil() = 0;
        virtual void GetFrameBufferSize(uint& nWidth, uint& nHeight) = 0;

        virtual void SetRenderTarget(uint idx, RenderTarget* pRenderTarget) = 0;
        virtual void SetDepthStencil(RenderTarget* pDepthStencil) = 0;

        virtual void SetRenderTarget(uint nRTs, RenderTarget** pRenderTargets, RenderTarget* pDepthStencil) = 0;

        virtual void ExecuteOperator(const RenderOperator& op) = 0;

        virtual void BindVertexShader(GPUShaderObject* shaderHandle) = 0;
        virtual void BindPixelShader(GPUShaderObject* shaderHandle) = 0;

        virtual Handle CreateShaderTextureBinding(GPUShaderObject* shaderHandle, const ShaderTextureBinding& bindings) = 0;
        virtual void BindTexture(Handle binding, Texture* texture) = 0;

        virtual CGPUGeometryBuffer* CreateGeometryBuffer(const CGeometry* InGeometry, CVertexFactory* InVertexFactory) { return nullptr; }

        virtual GPUShaderParameterBuffer*   CreateShaderParameterBuffer(CShaderParameterBuffer* InBuffer) { return nullptr; }

        virtual void UpdateGPUShaderParameterBuffer(GPUShaderParameterBuffer* InBuffer, const Array<int8>& InData) {};
    };


    class ResourceBufferMapper
    {
    public:
        ResourceBufferMapper(Handle handle);
        ~ResourceBufferMapper();

        void* Ptr();

    private:
        void* pointer = nullptr;
        Handle handle_ = -1;
    };




}


#endif