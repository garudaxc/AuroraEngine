#ifndef GARENDERER_H
#define GARENDERER_H

#include "Types.h"
#include "Geometry.h"
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
			PT_POINTLIST             = 0,
			PT_LINELIST,
			PT_LINESTRIP,
			PT_TRIANGLELIST,
			PT_TRIANGLESTRIP,
		};

		RenderOperator() {}

		RenderOperator(Handle VertexLayout, Handle VertexBuffer, Handle IndexBuffer,
			EPrimitiveType PrimType, int32 BaseVertexIndex, uint StartIndex,
			int32 IndexCount, uint32 VertexStride) 
		{
			VertexLayout_ = VertexLayout;
			VertexBuffer_ = VertexBuffer;
			IndexBuffer_ = IndexBuffer;
			PrimType_ = PrimType;
			BaseVertexIndex_ = BaseVertexIndex;
			StartIndex_ = StartIndex;
			IndexCount_ = IndexCount;
			VertexStride_ = VertexStride;
		}


		Material* pMaterial = nullptr;
		MaterialInstance* pMtlInst = nullptr;

		EPrimitiveType PrimType_ = PT_TRIANGLELIST;

		Handle		VertexLayout_	= -1;
		Handle		VertexBuffer_	= -1;
		Handle		IndexBuffer_	= -1;

		int32	BaseVertexIndex_;
		uint	StartIndex_;
		int32	IndexCount_;
		uint32	VertexStride_;
	};



	template<class type>
	int32 FindAvailableSlot(vector<type*>& v)
	{
		auto it = std::find(v.begin(), v.end(), nullptr);
		if (it == v.end()) {
			v.push_back(nullptr);
			return v.size() - 1;
		}

		return std::distance(v.begin(), it);
	}

	template<class type>
	int32 FindAvailableSlot(vector<type>& v)
	{
		for (auto it = v.begin(); it != v.end(); ++it) {
			if (!it->Occupied) {
				return std::distance(v.begin(), it);
			}
		}

		v.push_back(type());
		return v.size() - 1;
	}


	class File;

	class IRenderDevice
	{
	public:
		enum ClearBufferMask
		{
			CLEAR_DEPTH_BUFFER		= 0x1,
			CLEAR_STENCIL_BUFFER	= 0x2,
			CLEAR_FRAME_BUFFER		= 0x4,
		};


		static IRenderDevice* CreateDevice(int nWidth, int nHeight);

		virtual Handle  CreateShader(const ShaderCode& code) = 0;
 
		virtual Texture* CreateTexture(File* file) = 0;

		virtual Texture* CreateTexture(Texture::Type type, const Texture::Desc& desc) = 0;

		virtual RenderTarget* CreateRenderTarget(const RenderTarget::Desc& desc) = 0;


		//////////////////////////////////////////////////////////////////////////

		virtual void		Clear(int clearMask, const Color& clearColor = Color::BLUE, float fDepth = 1.0f, uint8 nStencil = 0) = 0;
		virtual void		BeginScene() = 0;
		virtual void		EndScene() = 0;
		virtual void		Present() = 0;

		virtual RenderTarget*	GetFrameBuffer() = 0;
		virtual RenderTarget*	GetDepthStecil() = 0;
		virtual void		GetFrameBufferSize(uint& nWidth, uint& nHeight) = 0;
		
		virtual void		SetVertexShader(Shader* pShader) = 0;
		virtual void		SetPixelShader(Shader* pShader) = 0;

		virtual void		SetRenderTarget(uint idx, RenderTarget* pRenderTarget) = 0;
		virtual void		SetDepthStencil(RenderTarget* pDepthStencil) = 0;

		virtual void		SetRenderTarget(uint nRTs, RenderTarget** pRenderTargets, RenderTarget* pDepthStencil) = 0;

		virtual void		ExecuteOperator(const RenderOperator& op) = 0;

		virtual void		OnReset() = 0;
		virtual void		OnLost() = 0;

		virtual void		Reset(int nWidth, int nHeight) = 0;
		virtual void		RestoreFrameBuffer() = 0;

	};

	//void SetPipeline(Handle );



	Handle CreateShaderParameterBinding(Handle shaderHandle, const ShaderParamterBindings& bindings);

	void UpdateShaderParameter(Handle bindingHandle);


	void BindVertexShader(Handle shaderHandle);
	void BindPixelShader(Handle shaderHandle);

	Handle CreateShaderTextureBinding(Handle shaderHandle, const ShaderTextureBinding& bindings);
	void BindTexture(Handle binding, Texture* texture);

	void BindGlobalParameter(Handle handle);

	Handle CreateVertexLayoutHandle(const vector<VertexLayoutItem>& layoutItems);

	
	Handle CreateVertexBufferHandle(const void* data, int32 size);
	Handle CreateIndexBufferHandle(const void* data, int32 size);



	class ResourceBufferMapper
	{
	public:
		ResourceBufferMapper(Handle handle);
		~ResourceBufferMapper();

		void* Ptr();

	private:
		void* pointer = nullptr;
		Handle	handle_ = -1;
	};




}	


#endif