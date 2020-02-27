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
	class VertexDescription;
	class RenderTarget;
	class IndexBuffer;
	class VertexBuffer;
	class StateValuePair;


	extern IRenderDevice* GRenderDevice;


	class RenderOperator
	{
	public:
		enum PrimitiveType
		{
			PT_POINTLIST             = 0,
			PT_LINELIST,
			PT_LINESTRIP,
			PT_TRIANGLELIST,
			PT_TRIANGLESTRIP,
		};

		RenderOperator()
		{
			memset(this, 0, sizeof(RenderOperator));
		}

		Material* pMaterial;

		MaterialInstance* pMtlInst;

		IndexBuffer* pIndexBuffer;

		VertexBuffer* pVertexBuffer;

		VertexLayout VertLayout;

		PrimitiveType nPrimType;

		int nBaseVertexIndex;

		uint nNumVertices;

		uint nStartIndex;

		uint nPrimitiveCount;

		uint nVertexStride;

		//float*					pSystemVertBuff;
		//ushort*					pSystemIndexBuff;
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


		static IRenderDevice* CreateDevice(HWND hWnd, int nWidth, int nHeight);

		virtual uint32  CreateShader(const ShaderCode& code) = 0;

		// resourec creation
		virtual Shader* CreateVertexShader(const string& pathname, const vector<pair<string, string>>& defines) = 0;
		virtual Shader* CreatePixelShader(const string& pathname, const vector<pair<string, string>>& defines) = 0;
 
		virtual Texture* CreateTexture(File* file) = 0;

		virtual Texture* CreateTexture(Texture::Type type, const Texture::Desc& desc) = 0;

		virtual RenderTarget* CreateRenderTarget(const RenderTarget::Desc& desc) = 0;

		virtual VertexBuffer* CreateVertexBuffer(Geometry* pGeometry, VertexLayout layout, uint nVert) = 0;

		virtual VertexBuffer* CreateDynamicVertexBuffer(int nStride, int nNumVert) = 0;

		virtual IndexBuffer* CreateIndexBuffer(Geometry* pGeometry, IndexBuffer::Format fmt, uint nNumIndex) = 0;


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

		virtual void		SetIndexBuffer(IndexBuffer* pIndexBuffer) = 0;
		virtual void		SetVertexBuffer(VertexBuffer* pVertexBuffer) = 0;

		virtual void		SetRenderTarget(uint idx, RenderTarget* pRenderTarget) = 0;
		virtual void		SetDepthStencil(RenderTarget* pDepthStencil) = 0;

		virtual void		SetRenderTarget(uint nRTs, RenderTarget** pRenderTargets, RenderTarget* pDepthStencil) = 0;

		virtual void		ExecuteOperator(const RenderOperator& op) = 0;

		virtual void		OnReset() = 0;
		virtual void		OnLost() = 0;

		virtual void		Reset(int nWidth, int nHeight) = 0;
		virtual void		RestoreFrameBuffer() = 0;

	};


	Handle CreateShaderParameterBinding(Handle shaderHandle, const ShaderParamterBindings& bindings);

	void UpdateShaderParameter(Handle bindingHandle);


	void BindVertexShader(Handle shaderHandle);
	void BindPixelShader(Handle shaderHandle);

	Handle CreateShaderTextureBinding(Handle shaderHandle, const ShaderTextureBinding& bindings);
	void BindTexture(Handle binding, Texture* texture);






}


#endif