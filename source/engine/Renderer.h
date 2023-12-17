#ifndef GARENDERER_H
#define GARENDERER_H

#include "Types.h"
#include "CGeometry.h"
#include "Texture.h"
#include "Shader.h"

namespace Aurora
{

    class Shader;
    class ShaderConstTable;
    class Texture;
    class RenderOperator;
    class VertexLayout;
    class RenderTarget;
    class StateValuePair;


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

        RenderOperator(Handle InVertexLayout, CGPUGeometryBuffer* InGeometryBuffer,
                       EPrimitiveType InPrimType, int32 InBaseVertexIndex, uint InStartIndex,
                       int32 InIndexCount, uint32 InVertexStride)
        {
            VertexLayout = InVertexLayout;
            GeometryBuffer = InGeometryBuffer;
            PrimType = InPrimType;
            BaseVertexIndex = InBaseVertexIndex;
            StartIndex = InStartIndex;
            IndexCount = InIndexCount;
            VertexStride = InVertexStride;
        }


        Material* pMaterial = nullptr;
        MaterialInstance* pMtlInst = nullptr;
        CGPUGeometryBuffer* GeometryBuffer = nullptr;

        EPrimitiveType PrimType = PT_TRIANGLELIST;

        Handle VertexLayout = -1;

        int32 BaseVertexIndex;
        uint StartIndex;
        int32 IndexCount;
        uint32 VertexStride;
        
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

    class GPUShaderObject
    {
    public:
        
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


        static IRenderDevice* CreateDevice(int nWidth, int nHeight);

        static bool Initialized();

        virtual GPUShaderObject* CreateShader(const ShaderCode& code) = 0;

        virtual Texture* CreateTexture(File* file) = 0;

        virtual Texture* CreateTexture(Texture::Type type, const Texture::Desc& desc) = 0;

        virtual RenderTarget* CreateRenderTarget(const RenderTarget::Desc& desc) = 0;


        //////////////////////////////////////////////////////////////////////////

        virtual void Clear(int clearMask, const Color& clearColor = Color::BLUE, float fDepth = 1.0f, uint8 nStencil = 0) = 0;
        virtual void Present() = 0;

        virtual RenderTarget* GetFrameBuffer() = 0;
        virtual RenderTarget* GetDepthStencil() = 0;
        virtual void GetFrameBufferSize(uint& nWidth, uint& nHeight) = 0;

        virtual void SetVertexShader(Shader* pShader) = 0;
        virtual void SetPixelShader(Shader* pShader) = 0;

        virtual void SetRenderTarget(uint idx, RenderTarget* pRenderTarget) = 0;
        virtual void SetDepthStencil(RenderTarget* pDepthStencil) = 0;

        virtual void SetRenderTarget(uint nRTs, RenderTarget** pRenderTargets, RenderTarget* pDepthStencil) = 0;

        virtual void ExecuteOperator(const RenderOperator& op) = 0;

        virtual Handle CreateShaderParameterBinding(GPUShaderObject* shaderHandle, const ShaderParamterBindings& bindings) = 0;
        virtual void UpdateShaderParameter(Handle bindingHandle) = 0;

        virtual void BindVertexShader(GPUShaderObject* shaderHandle) = 0;
        virtual void BindPixelShader(GPUShaderObject* shaderHandle) = 0;

        virtual Handle CreateShaderTextureBinding(GPUShaderObject* shaderHandle, const ShaderTextureBinding& bindings) = 0;
        virtual void BindTexture(Handle binding, Texture* texture) = 0;

        virtual void BindGlobalParameter(Handle handle) = 0;

        virtual Handle CreateVertexLayoutHandle(const vector<VertexLayoutItem>& layoutItems) = 0;


        virtual Handle CreateVertexBufferHandle(const void* data, int32 size) = 0;
        virtual Handle CreateIndexBufferHandle(const void* data, int32 size) = 0;

        virtual CGPUGeometryBuffer* CreateGeometryBuffer(const CGeometry* InGeometry) { return nullptr; }
    };

    //void SetPipeline(Handle );



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