#include "stdHeader.h"

#include <d3d10.h>
#include <d3dx10.h>
#include <DXGI.h>

#include "Renderer.h"
#include "Types.h"
#include "Geometry.h"
#include "Texture.h"
#include "Console.h"
#include "Engine.h"
#include "Material.h"
#include "Shader.h"
#include "Log.h"
#include "RendererObject.h"
#include "Shader.h"
#include "Util.h"
#include "RendererObject.h"
#include "Pipeline.h"
#include "MaterialInstance.h"

#pragma comment (lib, "d3d10.lib") 
#pragma comment (lib, "DXGI.lib") 

#ifdef _DEBUG
#pragma comment (lib, "d3dx10d.lib") 
#else
#pragma comment (lib, "d3dx10.lib") 
#endif



namespace Aurora
{


	class RendererDx10 : public IRenderDevice
	{
	public:

		RendererDx10(HWND hWnd, int nWidth, int nHeight);
		virtual ~RendererDx10(void);

		// resourec creation
		virtual Shader* CreateVertexShader(const string& pathname, const vector<pair<string, string>>& defines);

		virtual Shader* CreatePixelShader(const string& pathname, const vector<pair<string, string>>& defines);

		virtual Texture* CreateTexture(const string& name);

		virtual Texture* CreateTexture(Texture::Type type, const Texture::Desc& desc);

		virtual RenderTarget* CreateRenderTarget(const RenderTarget::Desc& desc);

		virtual VertexBuffer* CreateVertexBuffer(Geometry* pGeometry, VertexLayout layout, uint nVert);

		virtual IndexBuffer* CreateIndexBuffer(Geometry* pGeometry, IndexBuffer::Format fmt, uint nNumIndex);

		virtual VertexBuffer* CreateDynamicVertexBuffer(int nStride, int nNumVert);


		//////////////////////////////////////////////////////////////////////////


		virtual void		Clear(int clearMask, const Color& clearColor = Color::BLUE, float fDepth = 1.0f, ubyte nStencil = 0);
		virtual void		BeginScene();
		virtual void		EndScene();
		virtual void		Present();

		virtual RenderTarget*	GetFrameBuffer();
		virtual RenderTarget*	GetDepthStecil();

		void		SetVertexDesc(VertexDescription* pVertDesc);

		virtual void		SetVertexShader(Shader* pShader);
		virtual void		SetPixelShader(Shader* pShader);

		virtual void		SetIndexBuffer(IndexBuffer* pIndexBuffer);
		virtual void		SetVertexBuffer(VertexBuffer* pVertexBuffer);

		virtual void		SetRenderTarget(uint idx, RenderTarget* pRenderTarget);
		virtual void		SetDepthStencil(RenderTarget* pDepthStencil);

		virtual void		SetRenderTarget(uint nRTs, RenderTarget** pRenderTargets, RenderTarget* pDepthStencil);

		virtual void		ExecuteOperator(const RenderOperator& op);

		virtual void		OnReset();
		virtual void		OnLost();
		virtual void		GetFrameBufferSize(uint& nWidth, uint& nHeight);

		virtual void		Reset(int nWidth, int nHeight);
		virtual void		RestoreFrameBuffer();


	protected:
		void			InitializeInputLayout();
		void			FinalizeInputLayout();
	};














	Surface::Format FormatD3DToEngine(uint fmt)
	{
		static Surface::Format table[] = 
		{
			(Surface::Format)0,					//	= 0,
			(Surface::Format)0,					//	= 1,
			Surface::FMT_A32B32G32R32F,			//	= 2,
			(Surface::Format)0,					//	= 3,
			(Surface::Format)0,					//	= 4,
			(Surface::Format)0,					//	= 5,
			(Surface::Format)0,					//	= 6,
			(Surface::Format)0,					//	= 7,
			(Surface::Format)0,					//	= 8,
			(Surface::Format)0,					//	= 9,
			Surface::FMT_A16B16G16R16F,			//	= 10,
			Surface::FMT_A16B16G16R16,			//	= 11,
			(Surface::Format)0,					//	= 12,
			(Surface::Format)0,					//	= 13,
			(Surface::Format)0,					//	= 14,
			(Surface::Format)0,					//	= 15,
			(Surface::Format)0,					//	= 16,
			(Surface::Format)0,					//	= 17,
			(Surface::Format)0,					//	= 18,
			(Surface::Format)0,					//	= 19,
			(Surface::Format)0,					//	= 20,
			(Surface::Format)0,					//	= 21,
			(Surface::Format)0,					//	= 22,
			(Surface::Format)0,					//	= 23,
			(Surface::Format)0,					//	= 24,
			(Surface::Format)0,					//	= 25,
			(Surface::Format)0,					//	= 26,
			(Surface::Format)0,					//	= 27,
			Surface::FMT_R8G8B8A8,				//	= 28,
			(Surface::Format)0,					//	= 29,
			(Surface::Format)0,					//	= 30,
			(Surface::Format)0,					//	= 31,
			(Surface::Format)0,					//	= 32,
			(Surface::Format)0,					//	= 33,
			(Surface::Format)0,					//	= 34,
			(Surface::Format)0,					//	= 35,
			(Surface::Format)0,					//	= 36,
			(Surface::Format)0,					//	= 37,
			(Surface::Format)0,					//	= 38,
			(Surface::Format)0,					//	= 39,
			(Surface::Format)0,					//	= 40,
			Surface::FMT_R32F,					//	= 41,	
			(Surface::Format)0,					//	= 42,
			(Surface::Format)0,					//	= 43,
			(Surface::Format)0,					//	= 44,
			Surface::FMT_D24S8,					//	= 45,
			(Surface::Format)0,					//	= 46,
			(Surface::Format)0,					//	= 47,
			(Surface::Format)0,					//	= 48,
			(Surface::Format)0,					//	= 49,
			(Surface::Format)0,					//	= 50,
			(Surface::Format)0,					//	= 51,
			(Surface::Format)0,					//	= 52,
			(Surface::Format)0,					//	= 53,
			Surface::FMT_R16F,					//	= 54,
			Surface::FMT_D16,					//	= 55,
			(Surface::Format)0,					//	= 56,	
		};

		return table[fmt];
	}

	DXGI_FORMAT FormatEngineToD3D(Surface::Format fmt)
	{
		static DXGI_FORMAT table[] = 
		{
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			DXGI_FORMAT_R16G16B16A16_FLOAT,
			DXGI_FORMAT_R16G16B16A16_UNORM,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			DXGI_FORMAT_R32_FLOAT,
			DXGI_FORMAT_R32_TYPELESS,
			DXGI_FORMAT_R16_FLOAT,
			DXGI_FORMAT_D24_UNORM_S8_UINT,
			DXGI_FORMAT_D16_UNORM,
			DXGI_FORMAT_UNKNOWN,
			DXGI_FORMAT_UNKNOWN,
		};

		return table[(int)fmt];
	}



	class D3D10Mapping
	{
	public:

		static D3D10_CULL_MODE			CullMode(RenderStateObject::CullMode mode);
		static D3D10_COMPARISON_FUNC	CmpFunc(RenderStateObject::CompareFunc fuc);
		static D3D10_FILL_MODE			FillMode(RenderStateObject::FillMode mode);
		static D3D10_BLEND				BlendFactor(RenderStateObject::BlendFactor factor);
		static D3D10_BLEND_OP			BlendOp(RenderStateObject::BlendOp op);
		static D3D10_STENCIL_OP			StencilOp(RenderStateObject::StencilOp op);
		static D3D10_PRIMITIVE_TOPOLOGY PrimitiveTopology(RenderOperator::PrimitiveType priType);
	};


	D3D10_CULL_MODE D3D10Mapping::CullMode(RenderStateObject::CullMode mode)
	{
		static D3D10_CULL_MODE table[] = {D3D10_CULL_FRONT, D3D10_CULL_BACK, D3D10_CULL_NONE};
		return table[mode];
	}

	D3D10_COMPARISON_FUNC D3D10Mapping::CmpFunc(RenderStateObject::CompareFunc fuc)
	{
		static D3D10_COMPARISON_FUNC table[] = {D3D10_COMPARISON_NEVER, 
			D3D10_COMPARISON_EQUAL, D3D10_COMPARISON_NOT_EQUAL, D3D10_COMPARISON_LESS,
			D3D10_COMPARISON_LESS_EQUAL, D3D10_COMPARISON_GREATER,
			D3D10_COMPARISON_GREATER_EQUAL, D3D10_COMPARISON_ALWAYS};
		return table[fuc];
	}

	D3D10_FILL_MODE D3D10Mapping::FillMode(RenderStateObject::FillMode mode)
	{
		static D3D10_FILL_MODE table[] = {D3D10_FILL_WIREFRAME, D3D10_FILL_SOLID};
		return table[mode];
	}

	D3D10_BLEND D3D10Mapping::BlendFactor(RenderStateObject::BlendFactor factor)
	{
		static D3D10_BLEND table[] = {D3D10_BLEND_ZERO, D3D10_BLEND_ONE, D3D10_BLEND_SRC_COLOR, 
			D3D10_BLEND_INV_SRC_COLOR,D3D10_BLEND_DEST_COLOR, D3D10_BLEND_INV_DEST_COLOR, 
			D3D10_BLEND_SRC_ALPHA, D3D10_BLEND_INV_SRC_ALPHA, D3D10_BLEND_DEST_ALPHA,
			D3D10_BLEND_INV_DEST_ALPHA, D3D10_BLEND_BLEND_FACTOR, D3D10_BLEND_INV_BLEND_FACTOR};

		return table[factor];
	}

	D3D10_BLEND_OP D3D10Mapping::BlendOp(RenderStateObject::BlendOp op)
	{
		static D3D10_BLEND_OP table[] = {D3D10_BLEND_OP_ADD, D3D10_BLEND_OP_SUBTRACT, 
			D3D10_BLEND_OP_REV_SUBTRACT, D3D10_BLEND_OP_MIN, D3D10_BLEND_OP_MAX};

		return table[op];
	}

	D3D10_STENCIL_OP D3D10Mapping::StencilOp(RenderStateObject::StencilOp op)
	{
		static D3D10_STENCIL_OP table[] = {D3D10_STENCIL_OP_KEEP, D3D10_STENCIL_OP_ZERO, 
			D3D10_STENCIL_OP_REPLACE, D3D10_STENCIL_OP_INCR_SAT, D3D10_STENCIL_OP_DECR_SAT, 
			D3D10_STENCIL_OP_INVERT, D3D10_STENCIL_OP_INCR, D3D10_STENCIL_OP_DECR};

		return table[op];
	}


	D3D10_PRIMITIVE_TOPOLOGY D3D10Mapping::PrimitiveTopology(RenderOperator::PrimitiveType priType)
	{
		static D3D10_PRIMITIVE_TOPOLOGY table[] = {
			D3D10_PRIMITIVE_TOPOLOGY_POINTLIST,
			D3D10_PRIMITIVE_TOPOLOGY_LINELIST,
			D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP,
			D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		};

		return table[priType];
	}


//////////////////////////////////////////////////////////////////////////


	ID3D10Device*						D3D9Device = NULL;
	static ID3D10Device*				m_pd3dDevice = NULL;
	static IDXGISwapChain*				m_pSwapChain = NULL;
	static ID3D10RenderTargetView*		m_pRenderTargetView = NULL;
	static ID3D10Texture2D*				m_pDepthStencil = NULL;
	static ID3D10DepthStencilView*		m_pDepthStencilView = NULL;

	static RenderTarget* m_pRTFrameBuffer;
	static RenderTarget* m_pRTDepthBuffer;

	static HWND m_hWnd;			//window handle
	static uint m_nWidth;		//back buffer size
	static uint m_nHeight;		//back buffer size

	struct DeviceState
	{
		ID3D10RenderTargetView* rts[4];
		ID3D10DepthStencilView* ds;
	};


	DeviceState GDeviceState;



	struct Dx10TextureBind
	{
		ID3D10Resource* pTexture;
		union
		{
			ID3D10RenderTargetView*	pRTView;
			ID3D10DepthStencilView* pDSView;
		};
	};



//////////////////////////////////////////////////////////////////////////


	class IndexBufferD3D10 : public IndexBuffer
	{
	public:
		IndexBufferD3D10(Geometry* pGeometry, IndexBuffer::Format fmt, uint nNumIndex);
		~IndexBufferD3D10();

		virtual void*			GetBindID() const;
		virtual void			OnReset();
		virtual void			OnLost();

		virtual void*			Lock();
		virtual void			Unlock();

	private:
		ID3D10Buffer*           m_pBuffer;
		Geometry*			m_pGeometry;
	};

	IndexBufferD3D10::IndexBufferD3D10(Geometry* pGeometry, IndexBuffer::Format fmt, uint nNumIndex):
	IndexBuffer(fmt, nNumIndex),m_pBuffer(NULL),m_pGeometry(pGeometry)
	{

	}

	IndexBufferD3D10::~IndexBufferD3D10()
	{
		assert(m_pBuffer == NULL);
	}


	void* IndexBufferD3D10::GetBindID() const
	{
		return m_pBuffer;
	}

	void* IndexBufferD3D10::Lock()
	{
		VOID* pointer = NULL;
		// todo: implement the flags
		HRESULT hr = m_pBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, &pointer);
		assert(SUCCEEDED(hr));
		return pointer;
	}

	void IndexBufferD3D10::Unlock()
	{
		m_pBuffer->Unmap();
	}

	void IndexBufferD3D10::OnReset()
	{
		assert(m_pBuffer == NULL);
		D3DFORMAT d3dFmt = (GetFormat() == IndexBuffer::FMT_INDEX16) ? D3DFMT_INDEX16 : D3DFMT_INDEX32;
		uint stride = (GetFormat() == IndexBuffer::FMT_INDEX16) ? 2 : 4;
		UINT length = stride * GetNumIndex();

		ubyte* pMemBuffer = m_pGeometry->GetStreamPointer(Geometry::USAGE_INDEX, 0);
		assert(pMemBuffer != NULL);


		D3D10_BUFFER_DESC bufferDesc;
		bufferDesc.ByteWidth	= length;
		bufferDesc.Usage		= D3D10_USAGE_DEFAULT;
		bufferDesc.BindFlags	= D3D10_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags	= 0;

		D3D10_SUBRESOURCE_DATA InitData;
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;
		InitData.pSysMem = pMemBuffer;

		HRESULT hr = D3D9Device->CreateBuffer(&bufferDesc, &InitData, &m_pBuffer);
		assert(SUCCEEDED(hr));

		// Set index buffer
		//g_pd3dDevice->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	}

	void IndexBufferD3D10::OnLost()
	{
		assert(m_pBuffer != NULL);
		m_pBuffer->Release();
		m_pBuffer = NULL;
	}

	IndexBuffer* RendererDx10::CreateIndexBuffer(Geometry* pGeometry, IndexBuffer::Format fmt, uint nNumIndex)
	{
		return new IndexBufferD3D10(pGeometry, fmt, nNumIndex);
	}



//////////////////////////////////////////////////////////////////////////

	class VertexBufferDx10 : public VertexBuffer
	{
	public:
		VertexBufferDx10(uint nStride, uint nNumVertex);
		~VertexBufferDx10();

	private:

		virtual void* Lock();

		virtual void Unlock();
	};

	VertexBufferDx10::VertexBufferDx10(uint nStride, uint nNumVertex):VertexBuffer(nStride, nNumVertex)
	{

	}

	VertexBufferDx10::~VertexBufferDx10()
	{
		ID3D10Buffer* pDxBuffer = (ID3D10Buffer*)HALHandle;
		pDxBuffer->Release();
		HALHandle = NULL;
	}

	void* VertexBufferDx10::Lock()
	{
		VOID* pointer = NULL;
		// todo: implement the flags
		ID3D10Buffer* pDxBuffer = (ID3D10Buffer*)HALHandle;
		HRESULT hr = pDxBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, &pointer);
		assert(SUCCEEDED(hr));
		return pointer;
	}

	void VertexBufferDx10::Unlock()
	{
		ID3D10Buffer* pDxBuffer = (ID3D10Buffer*)HALHandle;
		pDxBuffer->Unmap();
	}


	VertexDescription* GetVertexDescription(EVertexLayout layout);
	VertexBuffer* RendererDx10::CreateVertexBuffer(Geometry* pGeometry, EVertexLayout layout, uint nVert)
	{
		VertexDescription* pVertexDesc = GetVertexDescription(layout);

		uint nStride = 0;
		for (uint j = 0; j < pVertexDesc->stream.items.size(); j++)
		{
			const VertexDescription::Item& item = pVertexDesc->stream.items[j];
			nStride += Geometry::GetSizeOfType((Geometry::VertexElemType)item.type);
		}

		VertexBuffer* pVertexBuffer = new VertexBufferDx10(nStride, pGeometry->GetNumVertex());

		D3D10_BUFFER_DESC bufferDesc;
		bufferDesc.Usage		= D3D10_USAGE_DEFAULT;
		bufferDesc.BindFlags	= D3D10_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags	= 0;

		D3D10_SUBRESOURCE_DATA InitData;
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;

		vector<unsigned char> tempBuffer;

		tempBuffer.resize(pVertexBuffer->GetSizeInByte());
		pGeometry->AssembleVertexBuffer(&tempBuffer[0], pVertexDesc->stream);

		bufferDesc.ByteWidth	= pVertexBuffer->GetSizeInByte();
		InitData.pSysMem		= &tempBuffer[0];

		HRESULT hr = D3D9Device->CreateBuffer(&bufferDesc, &InitData, (ID3D10Buffer**)(&pVertexBuffer->HALHandle));
		assert(SUCCEEDED(hr));

		return pVertexBuffer;
	}

	VertexBuffer* RendererDx10::CreateDynamicVertexBuffer(int nStride, int nNumVert)
	{
		int nSizeInByte = nStride * nNumVert;

		D3D10_BUFFER_DESC bufferDesc;
		bufferDesc.ByteWidth = nSizeInByte;
		bufferDesc.Usage = D3D10_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;

		bufferDesc.MiscFlags	= 0;
		VertexBuffer* pVertexBuffer = new VertexBufferDx10(nStride, nNumVert);

		HRESULT hr = D3D9Device->CreateBuffer(&bufferDesc, NULL, (ID3D10Buffer**)(&pVertexBuffer->HALHandle));
		assert(SUCCEEDED(hr));

		return pVertexBuffer;
	}


	Shader* RendererDx10::CreateVertexShader(const string& pathname, const vector<pair<string, string>>& defines)
	{
		vector<D3D10_SHADER_MACRO> macros;
		for (vector<pair<string, string>>::const_iterator it = defines.begin(); it != defines.end(); ++it)
		{
			D3D10_SHADER_MACRO m = {it->first.c_str(), it->second.c_str()};
			macros.push_back(m);
		}
		D3D10_SHADER_MACRO m = {NULL, NULL};
		macros.push_back(m);


		ID3D10Blob* pCompiledShader = NULL;
		ID3D10Blob* pError = NULL;

		UINT flag = 0;
#ifdef _DEBUG
		flag |= D3D10_SHADER_DEBUG;
#endif

		HRESULT hr = D3DX10CompileFromFileA(pathname.c_str(), &macros[0], NULL,
											"Main", "vs_4_0", flag, 0, NULL,
											&pCompiledShader, &pError, NULL);

		if (FAILED(hr))
		{
			std::string msg = "compile vertex shader error. material: ";
			msg += '\n';
			Console::AppendText(msg.c_str());
			if (pError)
			{
				std::string strError = (const char*)pError->GetBufferPointer();
				Console::AppendText(strError.c_str());
				pError->Release();
			}

			return NULL;
		}

		ID3D10VertexShader* pVertexShader = NULL;
		hr = m_pd3dDevice->CreateVertexShader(pCompiledShader->GetBufferPointer(), 
									pCompiledShader->GetBufferSize(), &pVertexShader);
		assert(SUCCEEDED(hr));


		ID3D10ShaderReflection* pReflector = NULL;
		hr = D3D10ReflectShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), &pReflector);
		pCompiledShader->Release();
		pCompiledShader = NULL;


		D3D10_SHADER_DESC shaderDesc;
		pReflector->GetDesc(&shaderDesc);

		Shader* pShader = new Shader(pVertexShader, shaderDesc.InstructionCount);
		ShaderConstTable* pShaderConstTable = pShader->GetShaderConstTable();

		for (uint i = 0; i < shaderDesc.ConstantBuffers; i++)
		{
			ID3D10ShaderReflectionConstantBuffer* pConstBuffer = pReflector->GetConstantBufferByIndex(i);
			D3D10_SHADER_BUFFER_DESC desc;
			pConstBuffer->GetDesc(&desc);

			//if (strcmp(desc.Name, "$Globals") != 0)
			//{
			//	continue;
			//}
			for (uint j = 0; j < desc.Variables; j++)
			{		
				ID3D10ShaderReflectionVariable* pVariable = pConstBuffer->GetVariableByIndex(j);

				D3D10_SHADER_VARIABLE_DESC varDesc;
				pVariable->GetDesc(&varDesc);

				if (!(varDesc.uFlags & D3D10_SVF_USED))
				{
					continue;
				}

				assert(varDesc.StartOffset % 16 == 0);
				uint regIndex = varDesc.StartOffset / 16;
				uint regCount = varDesc.Size % 16 == 0 ? varDesc.Size / 16 :  varDesc.Size / 16 + 1;

				//if (!pContext->TestReservedShaderConstVS(varDesc.Name, regIndex, regCount))
				{
					uint id = Shader::MapShaderConstNameToID(varDesc.Name);
					pShaderConstTable->AddShaderConstF(varDesc.Name, regIndex, regCount, id);
				}
			}
		}

		/*for (uint i = 0; i < shaderDesc.BoundResources; i++)
		{
			D3D10_SHADER_INPUT_BIND_DESC desc;
			pReflector->GetResourceBindingDesc(i, &desc);

			const char* pName = desc.Name;		
		}*/
		pReflector->Release();
		pReflector = NULL;

		return pShader;
	}

	Shader* RendererDx10::CreatePixelShader(const string& pathname, const vector<pair<string, string>>& defines)
	{
		vector<D3D10_SHADER_MACRO> macros;
		for (vector<pair<string, string>>::const_iterator it = defines.begin(); it != defines.end(); ++it)
		{
			D3D10_SHADER_MACRO m = {it->first.c_str(), it->second.c_str()};
			macros.push_back(m);
		}
		D3D10_SHADER_MACRO m = {NULL, NULL};
		macros.push_back(m);


		ID3D10Blob* pCompiledShader = NULL;
		ID3D10Blob* pError = NULL;

		UINT flag = 0;
#ifdef _DEBUG
		flag |= D3D10_SHADER_DEBUG;
#endif

		HRESULT hr = D3DX10CompileFromFileA(pathname.c_str(), &macros[0], NULL,
			"Main", "ps_4_0", flag, 0, NULL,
			&pCompiledShader, &pError, NULL);

		if (FAILED(hr))
		{
			std::string msg = "compile pixel shader error. material: ";
			msg += '\n';
			Console::AppendText(msg.c_str());
			if (pError)
			{
				std::string strError = (const char*)pError->GetBufferPointer();
				Console::AppendText(strError.c_str());
				pError->Release();
			}

			return NULL;
		}

		ID3D10PixelShader* pPixelShader = NULL;
		hr = m_pd3dDevice->CreatePixelShader(pCompiledShader->GetBufferPointer(), 
			pCompiledShader->GetBufferSize(), &pPixelShader);
		assert(SUCCEEDED(hr));


		ID3D10ShaderReflection* pReflector = NULL;
		hr = D3D10ReflectShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), &pReflector);
		pCompiledShader->Release();
		pCompiledShader = NULL;


		D3D10_SHADER_DESC shaderDesc;
		pReflector->GetDesc(&shaderDesc);

		Shader* pShader = new Shader(pPixelShader, shaderDesc.InstructionCount);

		//RenderingContext* pContext = Global::GetPipeline()->GetRenderingContext();


		ShaderConstTable* pShaderConstTable = pShader->GetShaderConstTable();
		//pShaderConstTable.reset(new ShaderConstTable());

		for (uint i = 0; i < shaderDesc.ConstantBuffers; i++)
		{
			ID3D10ShaderReflectionConstantBuffer* pConstBuffer = pReflector->GetConstantBufferByIndex(i);
			D3D10_SHADER_BUFFER_DESC desc;
			pConstBuffer->GetDesc(&desc);

			if (strcmp(desc.Name, "$Globals") != 0)
			{
				continue;
			}
			for (uint j = 0; j < desc.Variables; j++)
			{		
				ID3D10ShaderReflectionVariable* pVariable = pConstBuffer->GetVariableByIndex(j);

				D3D10_SHADER_VARIABLE_DESC varDesc;
				pVariable->GetDesc(&varDesc);

				if (!(varDesc.uFlags & D3D10_SVF_USED))
				{
					continue;
				}

				assert(varDesc.StartOffset % 16 == 0);
				uint regIndex = varDesc.StartOffset / 16;
				uint regCount = varDesc.Size % 16 == 0 ? varDesc.Size / 16 :  varDesc.Size / 16 + 1;

				//if (!pContext->TestReservedShaderConstVS(varDesc.Name, regIndex, regCount))
				{
					uint id = Shader::MapShaderConstNameToID(varDesc.Name);
					pShaderConstTable->AddShaderConstF(varDesc.Name, regIndex, regCount, id);
				}
			}
		}

		for (uint i = 0; i < shaderDesc.BoundResources; i++)
		{
			D3D10_SHADER_INPUT_BIND_DESC desc;
			pReflector->GetResourceBindingDesc(i, &desc);

			const char* pName = desc.Name;		
		}
		pReflector->Release();
		pReflector = NULL;

		return pShader;
	}


	class Texture2DDx10 : public Texture2D
	{
	public:
		Texture2DDx10(const Texture::Desc& desc);
		~Texture2DDx10();

		virtual void Update(int nMip, const void* pData);
		
		ID3D10Texture2D* m_pD3DTexture;
	};

	Texture2DDx10::Texture2DDx10(const Texture::Desc& desc):
	Texture2D(desc),m_pD3DTexture(NULL)
	{
	}

	Texture2DDx10::~Texture2DDx10()
	{
		if (HALHandle)
		{
			ID3D10ShaderResourceView* pView = (ID3D10ShaderResourceView*)HALHandle;
			pView->Release();
			HALHandle = NULL;
		}

		if (m_pD3DTexture)
		{
			m_pD3DTexture->Release();
			m_pD3DTexture = NULL;
		}
	}

	void Texture2DDx10::Update(int nMip, const void* pData)
	{
		UINT nSubResource = (UINT)nMip;
		D3D10_BOX box;
		box.left = 0;
		box.right = m_Desc.nWidth;
		box.top = 0;
		box.bottom = m_Desc.nHeight;
		box.front = 0;
		box.back = 1;

		uint stride = Surface::GetStride(m_Desc.nFormat);
		UINT rowPitch = stride * m_Desc.nWidth;

		D3D9Device->UpdateSubresource(m_pD3DTexture, nSubResource,
														&box, pData, rowPitch, 0);

	}


	class Texture3DDx10 : public Texture3D
	{
	public:
		Texture3DDx10(const Texture::Desc& desc);
		~Texture3DDx10();

		virtual void Update(int nMip, const void* pData);

		ID3D10Texture3D* m_pD3DTexture;
	};

	Texture3DDx10::Texture3DDx10(const Texture::Desc& desc):
	Texture3D(desc),m_pD3DTexture(NULL)
	{
	}

	Texture3DDx10::~Texture3DDx10()
	{
		if (HALHandle)
		{
			ID3D10ShaderResourceView* pView = (ID3D10ShaderResourceView*)HALHandle;
			pView->Release();
			HALHandle = NULL;
		}

		if (m_pD3DTexture)
		{
			m_pD3DTexture->Release();
			m_pD3DTexture = NULL;
		}
	}

	void Texture3DDx10::Update(int nMip, const void* pData)
	{
		UINT nSubResource = (UINT)nMip;
		D3D10_BOX box;
		box.left = 0;
		box.right = m_Desc.nWidth;
		box.top = 0;
		box.bottom = m_Desc.nHeight;
		box.front = 0;
		box.back = m_Desc.nDepth;

		uint stride = Surface::GetStride(m_Desc.nFormat);
		UINT rowPitch = stride * m_Desc.nWidth;
		UINT depthPitch = rowPitch * m_Desc.nHeight;

		D3D9Device->UpdateSubresource(m_pD3DTexture, nSubResource,
										&box, pData, rowPitch, depthPitch);
	}


	Texture* RendererDx10::CreateTexture(const string& name)
	{
		ID3D10ShaderResourceView* pResourceView = NULL;
		HRESULT hr = D3DX10CreateShaderResourceViewFromFileA(D3D9Device, 
										name.c_str(), NULL, NULL, &pResourceView, NULL);
		assert(SUCCEEDED(hr));
		if (FAILED(hr))
		{
			return NULL;
		}

		ID3D10Resource* pResource = NULL;
		pResourceView->GetResource(&pResource);

		D3D10_RESOURCE_DIMENSION resType;
		pResource->GetType(&resType);
		if (resType == D3D10_RESOURCE_DIMENSION_TEXTURE2D)
		{
			ID3D10Texture2D* pD3DTex2d = (ID3D10Texture2D*)pResource;
			D3D10_TEXTURE2D_DESC d3dDesc;
			pD3DTex2d->GetDesc(&d3dDesc);

			Texture::Desc desc;
			memset(&desc, 0, sizeof(desc));
			desc.nWidth		= d3dDesc.Width;
			desc.nHeight	= d3dDesc.Height;
			desc.nMipLevels	= d3dDesc.MipLevels;
			desc.nFormat	= FormatD3DToEngine(d3dDesc.Format);
			desc.nUsage		= Surface::USAGE_DEFAULT;
			desc.nDepth		= 1;
			desc.bMultiSample = d3dDesc.SampleDesc.Quality > 0;
			desc.bDynamic = (d3dDesc.Usage == D3D10_USAGE_DYNAMIC);

			bool bIsCube = (d3dDesc.MiscFlags & D3D10_RESOURCE_MISC_TEXTURECUBE) == D3D10_RESOURCE_MISC_TEXTURECUBE;

			if (bIsCube)
			{
				Texture* pTexture = new TextureCube(desc);
				pTexture->HALHandle = pResourceView;
				pD3DTex2d->Release();
				return pTexture;
			}
			else
			{
				Texture2DDx10* pTexture = new Texture2DDx10(desc);
				pTexture->HALHandle = pResourceView;
				pTexture->m_pD3DTexture = pD3DTex2d;
				return pTexture;
			}
		}

		if (resType == D3D10_RESOURCE_DIMENSION_TEXTURE3D)
		{
			ID3D10Texture3D* pD3DTex3d = (ID3D10Texture3D*)pResource;
			D3D10_TEXTURE3D_DESC d3dDesc;
			pD3DTex3d->GetDesc(&d3dDesc);

			Texture::Desc desc;
			memset(&desc, 0, sizeof(desc));
			desc.nWidth		= d3dDesc.Width;
			desc.nHeight	= d3dDesc.Height;
			desc.nDepth		= d3dDesc.Depth;
			desc.nMipLevels	= d3dDesc.MipLevels;
			desc.nFormat	= FormatD3DToEngine(d3dDesc.Format);
			desc.nUsage		= Surface::USAGE_DEFAULT;
			desc.bDynamic = (d3dDesc.Usage == D3D10_USAGE_DYNAMIC);
			desc.bMultiSample = 0;

			Texture3DDx10* pTexture = new Texture3DDx10(desc);
			pTexture->HALHandle = pResourceView;
			pTexture->m_pD3DTexture = pD3DTex3d;
			return pTexture;
		}

	/*	ID3D10Resource* pRes = NULL;
		ID3D10Texture2D* pCubeTexture = NULL;
		ID3D10ShaderResourceView* pCubeRV = NULL;

		D3DX10_IMAGE_LOAD_INFO LoadInfo;
		LoadInfo.MiscFlags = D3D10_RESOURCE_MISC_TEXTURECUBE;

		hr = D3DX10CreateTextureFromFile(D3D9Device, name.c_str(), &LoadInfo, NULL, &pRes, NULL);
		assert(SUCCEEDED(hr));
		if( pRes )
		{
			D3D10_TEXTURE2D_DESC desc;
			ZeroMemory( &desc, sizeof( D3D10_TEXTURE2D_DESC ) );
			pRes->QueryInterface( __uuidof( ID3D10Texture2D ), ( LPVOID* )&pCubeTexture );
			pCubeTexture->GetDesc( &desc );
			SAFE_RELEASE( pRes );

			D3D10_SHADER_RESOURCE_VIEW_DESC SRVDesc;
			ZeroMemory( &SRVDesc, sizeof( SRVDesc ) );
			SRVDesc.Format = desc.Format;
			SRVDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURECUBE;
			SRVDesc.TextureCube.MipLevels = desc.MipLevels;
			SRVDesc.TextureCube.MostDetailedMip = 0;
			V_RETURN( pd3dDevice->CreateShaderResourceView( pCubeTexture, &SRVDesc, &pCubeRV ) );
		}*/

		return NULL;
	}

	Texture* RendererDx10::CreateTexture(Texture::Type type, const Texture::Desc& desc)
	{
		if (type == Texture::TP_2D)
		{
			D3D10_TEXTURE2D_DESC d3dDesc;
			d3dDesc.Width = (UINT)desc.nWidth;
			d3dDesc.Height = (UINT)desc.nHeight;
			d3dDesc.MipLevels = (UINT)desc.nMipLevels;
			d3dDesc.ArraySize = 1;
			d3dDesc.Format = FormatEngineToD3D(desc.nFormat);
			d3dDesc.SampleDesc.Count = 1;
			d3dDesc.SampleDesc.Quality = 0;
			d3dDesc.Usage = desc.bDynamic ? D3D10_USAGE_DYNAMIC : D3D10_USAGE_DEFAULT;
			d3dDesc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
			d3dDesc.CPUAccessFlags = 0;
			d3dDesc.MiscFlags = 0;

			Texture2DDx10* pTexture = new Texture2DDx10(desc);
			HRESULT hr = m_pd3dDevice->CreateTexture2D(&d3dDesc, NULL, &pTexture->m_pD3DTexture);
			assert(SUCCEEDED(hr));

			D3D10_SHADER_RESOURCE_VIEW_DESC resViewDesc;
			resViewDesc.Format = d3dDesc.Format;
			resViewDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
			resViewDesc.Texture2D.MipLevels = d3dDesc.MipLevels;
			resViewDesc.Texture2D.MostDetailedMip = 0;

			ID3D10ShaderResourceView* pD3DResView = NULL;
			hr = m_pd3dDevice->CreateShaderResourceView(pTexture->m_pD3DTexture, &resViewDesc, &pD3DResView);
			assert(SUCCEEDED(hr));
			pTexture->HALHandle = pD3DResView;

			return pTexture;
		}
		else if (type == Texture::TP_3D)
		{
			D3D10_TEXTURE3D_DESC d3dDesc;
			d3dDesc.Width = (UINT)desc.nWidth;
			d3dDesc.Height = (UINT)desc.nHeight;
			d3dDesc.Depth = (UINT)desc.nDepth;
			d3dDesc.MipLevels = (UINT)desc.nMipLevels;
			d3dDesc.Format = FormatEngineToD3D(desc.nFormat);
			d3dDesc.Usage = desc.bDynamic ? D3D10_USAGE_DYNAMIC : D3D10_USAGE_DEFAULT;
			d3dDesc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
			d3dDesc.CPUAccessFlags = 0;
			d3dDesc.MiscFlags = 0;

			Texture3DDx10* pTexture = new Texture3DDx10(desc);
			HRESULT hr = m_pd3dDevice->CreateTexture3D(&d3dDesc, NULL, &pTexture->m_pD3DTexture);
			assert(SUCCEEDED(hr));

			D3D10_SHADER_RESOURCE_VIEW_DESC resViewDesc;
			resViewDesc.Format = d3dDesc.Format;
			resViewDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE3D;
			resViewDesc.Texture3D.MipLevels = d3dDesc.MipLevels;
			resViewDesc.Texture3D.MostDetailedMip = 0;

			ID3D10ShaderResourceView* pD3DResView = NULL;
			hr = m_pd3dDevice->CreateShaderResourceView(pTexture->m_pD3DTexture, &resViewDesc, &pD3DResView);
			assert(SUCCEEDED(hr));
			pTexture->HALHandle = pD3DResView;

			return pTexture;
		}

		assert(0);
		return NULL;
	}


	RenderTarget* RendererDx10::CreateRenderTarget(const RenderTarget::Desc& desc)
	{
		RenderTarget* pRT = new RenderTarget(desc);

		D3D10_TEXTURE2D_DESC rtDesc;
		ZeroMemory(&rtDesc, sizeof(D3D10_TEXTURE2D_DESC));
		rtDesc.ArraySize	= 1;
		rtDesc.Usage		= D3D10_USAGE_DEFAULT;
		rtDesc.Format		= FormatEngineToD3D(desc.nFormat);
		rtDesc.Width		= pRT->GetRealWidth();
		rtDesc.Height		= pRT->GetRealHeight();
		rtDesc.MipLevels	= 1;
		rtDesc.SampleDesc.Count = 1;
		rtDesc.BindFlags	= D3D10_BIND_SHADER_RESOURCE;
		if (desc.nUsage == Surface::USAGE_RENDERTARGET)
		{
			rtDesc.BindFlags |= D3D10_BIND_RENDER_TARGET;
		}
		else if (desc.nUsage == Surface::USAGE_DEPTHSTENCIL)
		{
			rtDesc.BindFlags |= D3D10_BIND_DEPTH_STENCIL;
			if (desc.nFormat == Surface::FMT_D16)
			{
				rtDesc.Format = DXGI_FORMAT_R16_TYPELESS;
			}
		}

		HRESULT hr = S_OK;

		ID3D10Texture2D* pD3DTex = NULL;
		hr = D3D9Device->CreateTexture2D( &rtDesc, NULL, &pD3DTex);
		if (FAILED(hr))
		{
			GLog->Error("CreateRenderTarget failed!");
			return NULL;
		}

		ULONG refCnt = pD3DTex->AddRef();
		refCnt = pD3DTex->Release();

		DXGI_FORMAT fmt = rtDesc.Format;
		if (desc.nFormat == Surface::FMT_D16)
		{
			fmt = DXGI_FORMAT_R16_UNORM;
		}

		// Create the shader resource view
		D3D10_SHADER_RESOURCE_VIEW_DESC DescRV;
		DescRV.Format = fmt;
		DescRV.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
		DescRV.Texture2D.MipLevels = 1;
		DescRV.Texture2D.MostDetailedMip = 0;
		ID3D10ShaderResourceView* pShaderResourceView = NULL;

		hr = D3D9Device->CreateShaderResourceView(pD3DTex, &DescRV, &pShaderResourceView);
		if (FAILED(hr))
		{
			GLog->Error("CreateRenderTarget failed!");
			return NULL;
		}

		refCnt = pD3DTex->AddRef();
		refCnt = pD3DTex->Release();

		pRT->HALHandle = pShaderResourceView;


		Dx10TextureBind* pBind = new Dx10TextureBind;
		pBind->pTexture = pD3DTex;

		if (desc.nUsage == Surface::USAGE_RENDERTARGET)
		{
			// Create the render target view
			ID3D10RenderTargetView* pRTView = NULL;
			D3D10_RENDER_TARGET_VIEW_DESC DescRT;
			DescRT.Format = fmt;
			DescRT.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;
			DescRT.Texture2D.MipSlice = 0;
			hr = D3D9Device->CreateRenderTargetView(pD3DTex, &DescRT, &pRTView);

			if (FAILED(hr))
			{
				GLog->Error("CreateRenderTarget failed!");
				return NULL;
			}

			pBind->pRTView = pRTView;
		}
		else if (desc.nUsage == Surface::USAGE_DEPTHSTENCIL)
		{
			if (desc.nFormat == Surface::FMT_D16)
			{
				fmt = DXGI_FORMAT_D16_UNORM;
			}
			ID3D10DepthStencilView* pDSView = NULL;
			D3D10_DEPTH_STENCIL_VIEW_DESC DescRT;
			DescRT.Format = fmt;
			DescRT.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
			DescRT.Texture2D.MipSlice = 0;
			hr = D3D9Device->CreateDepthStencilView(pD3DTex, &DescRT, &pDSView);

			if (FAILED(hr))
			{
				GLog->Error("CreateRenderTarget failed!");
				return NULL;
			}

			pBind->pDSView = pDSView;
		}

		refCnt = pD3DTex->AddRef();
		refCnt = pD3DTex->Release();

		pRT->HALHandleRT = pBind;
		
		return pRT;
	}


	static ID3D10Blob* CreateShaderSignature(const char* vsin)
	{
		//"float4 pos		: POSITION;"
		//"float3 normal	: NORMAL;"
		//"float2 tex		: TEXCOORD0;"

		const char* pCode =	\
			"struct VSIn"
			"{"
				"%s"
			"};"
			"float4 Main(VSIn vsin) : SV_POSITION"
			"{"
			"return float4(0, 0, 0, 0);"
			"}";

		char buffer[512];
		sprintf_s(buffer, sizeof(buffer), pCode, vsin);

		ID3D10Blob* pShader = NULL;
		ID3D10Blob* pError = NULL;

		HRESULT hr = D3DX10CompileFromMemory(buffer, strlen(buffer), "Dummy", NULL, NULL, "Main",
			"vs_4_0", 0, 0, NULL, &pShader, &pError, NULL);
		assert(SUCCEEDED(hr));

		//if (FAILED(hr))
		//{
		//	int code = HRESULT_CODE(hr);
		//	if (pError)
		//	{
		//		string errorMsg = (const char*)pError->GetBufferPointer();
		//		pError->Release();
		//		::MessageBoxA(NULL, errorMsg.c_str(), "", MB_OK);
		//	}
		//	return;
		//}

		ID3D10Blob* pInputSig = NULL;
		hr = D3D10GetInputSignatureBlob(pShader->GetBufferPointer(), pShader->GetBufferSize(), &pInputSig);
		assert(SUCCEEDED(hr));

		hr = pShader->Release();
		assert(SUCCEEDED(hr));
		
		return pInputSig;
	}



//////////////////////////////////////////////////////////////////////////

	void OutputDeviceInfo(IDXGIAdapter* pAdapter)
	{
		DXGI_ADAPTER_DESC desc;
		pAdapter->GetDesc(&desc);

		char buffer[256];
		sprintf_s(buffer, sizeof(buffer), "adapter desc : %s", desc.Description);
		GLog->Info(buffer);

		sprintf_s(buffer, sizeof(buffer), "Dedicated Video Memory : %d", desc.DedicatedVideoMemory);
		GLog->Info(buffer);

		sprintf_s(buffer, sizeof(buffer), "Dedicated System Memory : %d", desc.DedicatedSystemMemory);
		GLog->Info(buffer);


	}




	static void CreateDx10Device(int nWidth, int nHeight, HWND hWnd, ID3D10Device** pd3dDevice, IDXGISwapChain** pSwapChain)
	{
		IDXGIFactory *pDXGIFactory = NULL;
		HRESULT hRes;
		hRes = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pDXGIFactory);
		if (FAILED(hRes))
		{
			GLog->Error("CreateDXGIFactory failed! hr = %d", hRes);
			return;
		}
		// Search for a PerfHUD adapter.
		UINT nAdapter = 0;
		IDXGIAdapter* adapter = NULL;
		IDXGIAdapter* selectedAdapter = NULL;
		D3D10_DRIVER_TYPE driverType = D3D10_DRIVER_TYPE_HARDWARE;
		while (pDXGIFactory->EnumAdapters(nAdapter, &adapter) != DXGI_ERROR_NOT_FOUND)
		{
			if (adapter)
			{
				DXGI_ADAPTER_DESC adaptDesc;
				if (SUCCEEDED(adapter->GetDesc(&adaptDesc)))
				{
					const bool isPerfHUD = wcscmp(adaptDesc.Description, L"NVIDIA PerfHUD") == 0;
					// Select the first adapter in normal circumstances or the PerfHUD one if it exists.
					if(nAdapter == 0 || isPerfHUD)
						selectedAdapter = adapter;
					if(isPerfHUD)
						driverType = D3D10_DRIVER_TYPE_REFERENCE;
				}
			}
			++nAdapter;
		}

		UINT flag = D3D10_CREATE_DEVICE_SINGLETHREADED;
#ifdef _DEBUG
		//flag |= D3D10_CREATE_DEVICE_DEBUG;
#endif

		if(FAILED(hRes = D3D10CreateDevice( selectedAdapter, driverType, NULL,
			flag, D3D10_SDK_VERSION, pd3dDevice)))
		{
			GLog->Error("D3D10CreateDevice failed! hr=%d", hRes);
			return;
		}

		OutputDeviceInfo(selectedAdapter);
		if (*pd3dDevice)
		{
			DXGI_SWAP_CHAIN_DESC sd;
			ZeroMemory( &sd, sizeof( sd ) );
			sd.BufferCount			= 1;
			sd.BufferDesc.Width		= nWidth;
			sd.BufferDesc.Height	= nHeight;
			sd.BufferDesc.Format	= DXGI_FORMAT_R8G8B8A8_UNORM;
			sd.BufferDesc.RefreshRate.Numerator = 60;
			sd.BufferDesc.RefreshRate.Denominator = 1;
			sd.BufferUsage			= DXGI_USAGE_RENDER_TARGET_OUTPUT;
			sd.OutputWindow			= hWnd;
			sd.SampleDesc.Count		= 1;
			sd.SampleDesc.Quality	= 0;
			sd.Windowed				= TRUE;

			pDXGIFactory->CreateSwapChain(*pd3dDevice, &sd, pSwapChain);
		}

		pDXGIFactory->Release();
	}

	RendererDx10* gpDx10Renderer = NULL;


	IRenderDevice* IRenderDevice::CreateDevice(HWND hWnd, int nWidth, int nHeight)
	{
		gpDx10Renderer = new RendererDx10(hWnd, nWidth, nHeight);
		return gpDx10Renderer;
	}


	RendererDx10::RendererDx10(HWND hWnd, int nWidth, int nHeight)
	{
		m_pd3dDevice			= NULL;
		m_pSwapChain			= NULL;
		m_pRenderTargetView		= NULL;
		m_pDepthStencil			= NULL;
		m_pDepthStencilView		= NULL;
		m_hWnd					= hWnd;

		m_nWidth				= nWidth;
		m_nHeight				= nHeight;


		CreateDx10Device(nWidth, nHeight, hWnd, &m_pd3dDevice, &m_pSwapChain);
		assert(m_pd3dDevice);
		assert(m_pSwapChain);

		D3D9Device = m_pd3dDevice;

		HRESULT hr = S_OK;

		// Create a render target view
		ID3D10Texture2D* pBuffer;
		hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&pBuffer);
		assert(SUCCEEDED(hr));

		hr = m_pd3dDevice->CreateRenderTargetView(pBuffer, NULL, &m_pRenderTargetView);
		pBuffer->Release();

		
		RenderTarget::Desc desc;
		memset(&desc, 0, sizeof(RenderTarget::Desc));

		desc.nWidth		= RenderTarget::DS_FRAMEBUFFER_WIDTH;
		desc.nHeight	= RenderTarget::DS_FRAMEBUFFER_HEIGHT;
		desc.nUsage		= Surface::USAGE_RENDERTARGET;
		desc.nFormat	= Surface::FMT_R8G8B8A8;

		m_pRTFrameBuffer = new RenderTarget(desc);
		Dx10TextureBind* pBind = new Dx10TextureBind;
		pBind->pTexture = pBuffer;
		pBind->pRTView = m_pRenderTargetView;
		m_pRTFrameBuffer->HALHandleRT = pBind;

		assert(SUCCEEDED(hr));

		GRenderDevice = this;

		InitializeInputLayout();
	}

	RendererDx10::~RendererDx10()
	{
		FinalizeInputLayout();

		m_pd3dDevice->ClearState();
		
		m_pRenderTargetView->Release();
		m_pSwapChain->Release();
		m_pd3dDevice->Release();
	}

	RenderTarget* RendererDx10::GetFrameBuffer()
	{
		return m_pRTFrameBuffer;
	}

	RenderTarget* RendererDx10::GetDepthStecil()
	{
		return m_pRTDepthBuffer;
	}


	//bool RendererDx10::CreateVertexDesc(const char* strDesc, VertexDescription* pVertexDesc)
	//{
	//	char* vsin = NULL;
	//	vector<D3D10_INPUT_ELEMENT_DESC> vDecl;

	//	D3D10_INPUT_ELEMENT_DESC elem;
	//	elem.SemanticName			= NULL;
	//	elem.Format					= DXGI_FORMAT_R32G32B32_FLOAT;
	//	elem.AlignedByteOffset		= 0;
	//	elem.SemanticIndex			= 0;
	//	elem.InputSlot				= 0;
	//	elem.InputSlotClass			= D3D10_INPUT_PER_VERTEX_DATA;
	//	elem.InstanceDataStepRate	= 0;

	//	if (strDesc == ResourceManager::VEXDESC_PNTT)
	//	{
	//		vsin = 	"float4 pos		: POSITION;"
	//			"float3 normal	: NORMAL;"
	//			"float2 tex		: TEXCOORD0;"
	//			"float4 tangent	: TANGENT;";

	//		elem.SemanticName			= "POSITION";
	//		elem.Format					= DXGI_FORMAT_R32G32B32_FLOAT;
	//		elem.AlignedByteOffset		= 0;
	//		vDecl.push_back(elem);

	//		elem.SemanticName			= "NORMAL";
	//		elem.Format					= DXGI_FORMAT_R32G32B32_FLOAT;
	//		elem.AlignedByteOffset		= 12;
	//		vDecl.push_back(elem);

	//		elem.SemanticName			= "TEXCOORD";
	//		elem.Format					= DXGI_FORMAT_R32G32_FLOAT;
	//		elem.AlignedByteOffset		= 24;
	//		vDecl.push_back(elem);

	//		elem.SemanticName			= "TANGENT";
	//		elem.Format					= DXGI_FORMAT_R32G32B32A32_FLOAT;
	//		elem.AlignedByteOffset		= 32;
	//		vDecl.push_back(elem);

	//	}
	//	else  if (strDesc == ResourceManager::VEXDESC_PT)
	//	{
	//		vsin = 	"float4 pos		: POSITION;"
	//			"float2 tex		: TEXCOORD0;";

	//		elem.SemanticName			= "POSITION";
	//		elem.Format					= DXGI_FORMAT_R32G32B32_FLOAT;
	//		elem.AlignedByteOffset		= 0;
	//		vDecl.push_back(elem);

	//		elem.SemanticName			= "TEXCOORD";
	//		elem.Format					= DXGI_FORMAT_R32G32_FLOAT;
	//		elem.AlignedByteOffset		= 12;
	//		vDecl.push_back(elem);
	//	}
	//	else  if (strDesc == ResourceManager::VEXDESC_P)
	//	{
	//		vsin = 	"float4 pos		: POSITION;";

	//		elem.SemanticName			= "POSITION";
	//		elem.Format					= DXGI_FORMAT_R32G32B32_FLOAT;
	//		elem.AlignedByteOffset		= 0;
	//		vDecl.push_back(elem);
	//	}

	//	ID3D10Blob* pInputSig = CreateShaderSignature(vsin);

	//	HRESULT hr = D3D9Device->CreateInputLayout(&vDecl[0], vDecl.size(), pInputSig->GetBufferPointer(),
	//		pInputSig->GetBufferSize(),(ID3D10InputLayout**)(&pVertexDesc->HALHandle));

	//	assert(SUCCEEDED(hr));
	//	pInputSig->Release();

	//	return true;
	//}

	//void RendererDx10::ReleaseVertexDecl(VertexDescription* pDecl)
	//{

	//}


	static VertexDescription* CreateInputLayoutPNTT()
	{
		vector<D3D10_INPUT_ELEMENT_DESC> vDecl;

		D3D10_INPUT_ELEMENT_DESC elem;
		elem.SemanticName			= NULL;
		elem.Format					= DXGI_FORMAT_R32G32B32_FLOAT;
		elem.AlignedByteOffset		= 0;
		elem.SemanticIndex			= 0;
		elem.InputSlot				= 0;
		elem.InputSlotClass			= D3D10_INPUT_PER_VERTEX_DATA;
		elem.InstanceDataStepRate	= 0;


		elem.SemanticName			= "POSITION";
		elem.Format					= DXGI_FORMAT_R32G32B32_FLOAT;
		elem.AlignedByteOffset		= 0;
		vDecl.push_back(elem);

		elem.SemanticName			= "NORMAL";
		elem.Format					= DXGI_FORMAT_R32G32B32_FLOAT;
		elem.AlignedByteOffset		= 12;
		vDecl.push_back(elem);

		elem.SemanticName			= "TEXCOORD";
		elem.Format					= DXGI_FORMAT_R32G32_FLOAT;
		elem.AlignedByteOffset		= 24;
		vDecl.push_back(elem);

		elem.SemanticName			= "TANGENT";
		elem.Format					= DXGI_FORMAT_R32G32B32A32_FLOAT;
		elem.AlignedByteOffset		= 32;
		vDecl.push_back(elem);


		char* 	vsin = 	"float4 pos		: POSITION;"
						"float3 normal	: NORMAL;"
						"float2 tex		: TEXCOORD0;"
						"float4 tangent	: TANGENT;";
		ID3D10Blob* pInputSig = CreateShaderSignature(vsin);

		VertexDescription* pVertexDesc = new VertexDescription();
		VertexDescription::Stream stream;
		uint nStride = 0;

		VertexDescription::Item item;
		item.type		= Geometry::TYPE_FLOAT3;
		item.usage		= Geometry::USAGE_POSITION;
		item.usageIndex	= 0;
		stream.items.push_back(item);
		nStride += Geometry::GetSizeOfType(Geometry::TYPE_FLOAT3);


		item.type		= Geometry::TYPE_FLOAT3;
		item.usage		= Geometry::USAGE_NORMAL;
		item.usageIndex	= 0;
		stream.items.push_back(item);
		nStride += Geometry::GetSizeOfType(Geometry::TYPE_FLOAT3);

		item.type		= Geometry::TYPE_FLOAT2;
		item.usage		= Geometry::USAGE_TEXCOORD;
		item.usageIndex	= 0;
		stream.items.push_back(item);
		nStride += Geometry::GetSizeOfType(Geometry::TYPE_FLOAT2);


		item.type		= Geometry::TYPE_FLOAT4;
		item.usage		= Geometry::USAGE_TANGENT;
		item.usageIndex	= 0;
		stream.items.push_back(item);
		nStride += Geometry::GetSizeOfType(Geometry::TYPE_FLOAT4);
		stream.nVertexStride = nStride;
		pVertexDesc->stream = stream;

		HRESULT hr = D3D9Device->CreateInputLayout(&vDecl[0], vDecl.size(), 
									pInputSig->GetBufferPointer(), pInputSig->GetBufferSize(),
									(ID3D10InputLayout**)(&pVertexDesc->HALHandle));

		assert(SUCCEEDED(hr));
		pInputSig->Release();

		return pVertexDesc;
	}


	static VertexDescription* CreateInputLayoutPNTTBB()
	{
		vector<D3D10_INPUT_ELEMENT_DESC> vDecl;

		D3D10_INPUT_ELEMENT_DESC elem;
		elem.SemanticName			= NULL;
		elem.Format					= DXGI_FORMAT_R32G32B32_FLOAT;
		elem.AlignedByteOffset		= 0;
		elem.SemanticIndex			= 0;
		elem.InputSlot				= 0;
		elem.InputSlotClass			= D3D10_INPUT_PER_VERTEX_DATA;
		elem.InstanceDataStepRate	= 0;


		elem.SemanticName			= "POSITION";
		elem.Format					= DXGI_FORMAT_R32G32B32_FLOAT;
		elem.AlignedByteOffset		= 0;
		vDecl.push_back(elem);

		elem.SemanticName			= "NORMAL";
		elem.Format					= DXGI_FORMAT_R32G32B32_FLOAT;
		elem.AlignedByteOffset		= 12;
		vDecl.push_back(elem);

		elem.SemanticName			= "TEXCOORD";
		elem.Format					= DXGI_FORMAT_R32G32_FLOAT;
		elem.AlignedByteOffset		= 24;
		vDecl.push_back(elem);

		elem.SemanticName			= "TANGENT";
		elem.Format					= DXGI_FORMAT_R32G32B32A32_FLOAT;
		elem.AlignedByteOffset		= 32;
		vDecl.push_back(elem);

		elem.SemanticName			= "BLENDINDICES";
		elem.Format					= DXGI_FORMAT_R8G8B8A8_UINT;
		elem.AlignedByteOffset		= 48;
		vDecl.push_back(elem);

		elem.SemanticName			= "BLENDWEIGHT";
		elem.Format					= DXGI_FORMAT_R32G32B32_FLOAT;
		elem.AlignedByteOffset		= 52;
		vDecl.push_back(elem);


		char* 	vsin = 	"float4 pos			: POSITION;"
						"float3 normal		: NORMAL;"
						"float2 tex			: TEXCOORD0;"
						"float4 tangent		: TANGENT;"
						"uint4 blendIndex	: BLENDINDICES;"
						"float3 blendWeight : BLENDWEIGHT;";
		ID3D10Blob* pInputSig = CreateShaderSignature(vsin);

		VertexDescription* pVertexDesc = new VertexDescription();
		VertexDescription::Stream stream;
		uint nStride = 0;

		VertexDescription::Item item;
		item.type		= Geometry::TYPE_FLOAT3;
		item.usage		= Geometry::USAGE_POSITION;
		item.usageIndex	= 0;
		stream.items.push_back(item);
		nStride += Geometry::GetSizeOfType(Geometry::TYPE_FLOAT3);


		item.type		= Geometry::TYPE_FLOAT3;
		item.usage		= Geometry::USAGE_NORMAL;
		item.usageIndex	= 0;
		stream.items.push_back(item);
		nStride += Geometry::GetSizeOfType((Geometry::VertexElemType)item.type);

		item.type		= Geometry::TYPE_FLOAT2;
		item.usage		= Geometry::USAGE_TEXCOORD;
		item.usageIndex	= 0;
		stream.items.push_back(item);
		nStride += Geometry::GetSizeOfType((Geometry::VertexElemType)item.type);

		item.type		= Geometry::TYPE_FLOAT4;
		item.usage		= Geometry::USAGE_TANGENT;
		item.usageIndex	= 0;
		stream.items.push_back(item);
		nStride += Geometry::GetSizeOfType((Geometry::VertexElemType)item.type);


		item.type		= Geometry::TYPE_UBYTE4_UINT;
		item.usage		= Geometry::USAGE_BLENDINDEX;
		item.usageIndex	= 0;
		stream.items.push_back(item);
		nStride += Geometry::GetSizeOfType((Geometry::VertexElemType)item.type);

		item.type		= Geometry::TYPE_FLOAT3;
		item.usage		= Geometry::USAGE_BLENDWEIGHT;
		item.usageIndex	= 0;
		stream.items.push_back(item);
		nStride += Geometry::GetSizeOfType((Geometry::VertexElemType)item.type);

		stream.nVertexStride = nStride;
		pVertexDesc->stream = stream;

		HRESULT hr = D3D9Device->CreateInputLayout(&vDecl[0], vDecl.size(), 
			pInputSig->GetBufferPointer(), pInputSig->GetBufferSize(),
			(ID3D10InputLayout**)(&pVertexDesc->HALHandle));

		assert(SUCCEEDED(hr));
		pInputSig->Release();

		return pVertexDesc;
	}

	static VertexDescription* CreateInputLayoutPT()
	{
		vector<D3D10_INPUT_ELEMENT_DESC> vDecl;

		D3D10_INPUT_ELEMENT_DESC elem;
		elem.SemanticName			= NULL;
		elem.Format					= DXGI_FORMAT_R32G32B32_FLOAT;
		elem.AlignedByteOffset		= 0;
		elem.SemanticIndex			= 0;
		elem.InputSlot				= 0;
		elem.InputSlotClass			= D3D10_INPUT_PER_VERTEX_DATA;
		elem.InstanceDataStepRate	= 0;

		elem.SemanticName			= "POSITION";
		elem.Format					= DXGI_FORMAT_R32G32B32_FLOAT;
		elem.AlignedByteOffset		= 0;
		vDecl.push_back(elem);

		elem.SemanticName			= "TEXCOORD";
		elem.Format					= DXGI_FORMAT_R32G32_FLOAT;
		elem.AlignedByteOffset		= 12;
		vDecl.push_back(elem);

		char* vsin =	"float4 pos		: POSITION;"
						"float2 tex		: TEXCOORD0;";

		ID3D10Blob* pInputSig = CreateShaderSignature(vsin);

		VertexDescription* pVertexDesc = new VertexDescription();
		VertexDescription::Stream stream;
		uint nStride = 0;

		VertexDescription::Item item;
		item.type		= Geometry::TYPE_FLOAT3;
		item.usage		= Geometry::USAGE_POSITION;
		item.usageIndex	= 0;
		stream.items.push_back(item);


		item.type		= Geometry::TYPE_FLOAT2;
		item.usage		= Geometry::USAGE_TEXCOORD;
		item.usageIndex	= 0;
		stream.items.push_back(item);
		nStride += Geometry::GetSizeOfType(Geometry::TYPE_FLOAT2);
		stream.nVertexStride = nStride;
		pVertexDesc->stream = stream;

		HRESULT hr = D3D9Device->CreateInputLayout(&vDecl[0], vDecl.size(), 
								pInputSig->GetBufferPointer(), pInputSig->GetBufferSize(),
								(ID3D10InputLayout**)(&pVertexDesc->HALHandle));

		assert(SUCCEEDED(hr));
		pInputSig->Release();

		return pVertexDesc;

	}

	static VertexDescription* CreateInputLayoutP()
	{
		vector<D3D10_INPUT_ELEMENT_DESC> vDecl;

		D3D10_INPUT_ELEMENT_DESC elem;
		elem.SemanticName			= NULL;
		elem.Format					= DXGI_FORMAT_R32G32B32_FLOAT;
		elem.AlignedByteOffset		= 0;
		elem.SemanticIndex			= 0;
		elem.InputSlot				= 0;
		elem.InputSlotClass			= D3D10_INPUT_PER_VERTEX_DATA;
		elem.InstanceDataStepRate	= 0;

		elem.SemanticName			= "POSITION";
		elem.Format					= DXGI_FORMAT_R32G32B32_FLOAT;
		elem.AlignedByteOffset		= 0;
		vDecl.push_back(elem);

		char* vsin =	"float4 pos		: POSITION;";

		ID3D10Blob* pInputSig = CreateShaderSignature(vsin);

		VertexDescription* pVertexDesc = new VertexDescription;
		VertexDescription::Stream stream;
		uint nStride = 0;

		VertexDescription::Item item;
		item.type		= Geometry::TYPE_FLOAT3;
		item.usage		= Geometry::USAGE_POSITION;
		item.usageIndex	= 0;
		stream.items.push_back(item);
		nStride += Geometry::GetSizeOfType(Geometry::TYPE_FLOAT3);
		stream.nVertexStride = nStride;
		pVertexDesc->stream = stream;

		HRESULT hr = D3D9Device->CreateInputLayout(&vDecl[0], vDecl.size(), 
										pInputSig->GetBufferPointer(), pInputSig->GetBufferSize(),
										(ID3D10InputLayout**)(&pVertexDesc->HALHandle));

		assert(SUCCEEDED(hr));
		pInputSig->Release();

		return pVertexDesc;
	}


	
	static VertexDescription* gInputLayout[VertexLayout_NUM];

	static VertexDescription* GetVertexDescription(VertexLayout layout)
	{
		return gInputLayout[layout];
	}

	void RendererDx10::InitializeInputLayout()
	{
		gInputLayout[VertexLayout_PNTT]		= CreateInputLayoutPNTT();
		gInputLayout[VertexLayout_PT]		= CreateInputLayoutPT();
		gInputLayout[VertexLayout_P]		= CreateInputLayoutP();
		gInputLayout[VertexLayout_PNTTBB]	= CreateInputLayoutPNTTBB();
	}

	static void ReleaseInputLayout(VertexDescription* pInputLayout)
	{
		ID3D10InputLayout* pD3DObj = (ID3D10InputLayout*)pInputLayout->HALHandle;
		pD3DObj->Release();
		delete pInputLayout;
	}

	void RendererDx10::FinalizeInputLayout()
	{
		for (int i = 0; i < VertexLayout_NUM; i++) {
			ReleaseInputLayout(gInputLayout[i]);
			gInputLayout[i] = NULL;
		}
	}
	

	void RendererDx10::Clear(int clearMask, const Color& clearColor, float fDepth, ubyte nStencil)
	{
		if (clearMask & IRenderDevice::CLEAR_FRAME_BUFFER)
		{
			for (int i = 0; i < 4; i++)
			{
				if (GDeviceState.rts[i])
					m_pd3dDevice->ClearRenderTargetView(GDeviceState.rts[i], clearColor.Ptr());
			}
		}

		UINT nFlag = 0;
		if (clearMask & IRenderDevice::CLEAR_DEPTH_BUFFER)
		{
			nFlag |= D3D10_CLEAR_DEPTH;
		}

		if (clearMask & IRenderDevice::CLEAR_STENCIL_BUFFER)
		{
			nFlag |= D3D10_CLEAR_STENCIL;
		}

		if (nFlag != 0)
		{
			if (GDeviceState.ds)
				m_pd3dDevice->ClearDepthStencilView(GDeviceState.ds, nFlag, fDepth, nStencil);
		}
	}

	void RendererDx10::BeginScene()
	{
		// nothing to do in dx10
		RestoreFrameBuffer();

		m_pd3dDevice->GSSetShader(NULL);
	}

	void RendererDx10::EndScene()
	{

	}

	void RendererDx10::Present()
	{
		m_pSwapChain->Present(0, 0);
	}

	void RendererDx10::SetVertexShader(Shader* pShader)
	{
		m_pd3dDevice->VSSetShader((ID3D10VertexShader*)pShader->HALHandle);
	}

	void RendererDx10::SetPixelShader(Shader* pShader)
	{
		m_pd3dDevice->PSSetShader((ID3D10PixelShader*)pShader->HALHandle);
	}

	void RendererDx10::SetRenderTarget(uint idx, RenderTarget* pRenderTarget)
	{
		assert(0);		
	}

	void RendererDx10::SetRenderTarget(uint nRTs, RenderTarget** pRenderTargets, RenderTarget* pDepthStencil)
	{
		ID3D10RenderTargetView * pRTs[] = {NULL, NULL, NULL, NULL};

		for (uint i = 0; i < nRTs; i++)
		{
			if (pRenderTargets[i])
			{
				Dx10TextureBind* pBind = (Dx10TextureBind*)pRenderTargets[i]->HALHandleRT;
				pRTs[i] = pBind->pRTView;
			}
		}

		ID3D10DepthStencilView* pDSView = NULL;
		if (pDepthStencil)
		{
			Dx10TextureBind* pDSBind = (Dx10TextureBind*)pDepthStencil->HALHandleRT;
			pDSView = pDSBind->pDSView;
		}

		m_pd3dDevice->OMSetRenderTargets(nRTs, pRTs, pDSView);
		GDeviceState.rts[0] = pRTs[0];
		GDeviceState.rts[1] = pRTs[1];
		GDeviceState.rts[2] = pRTs[2];
		GDeviceState.rts[3] = pRTs[3];
		GDeviceState.ds = pDSView;
	}

	void RendererDx10::SetDepthStencil(RenderTarget* pDepthStencil)
	{

	}

	void RendererDx10::ExecuteOperator(const RenderOperator& op)
	{
		m_pd3dDevice->IASetPrimitiveTopology(D3D10Mapping::PrimitiveTopology(op.nPrimType));

		VertexDescription* pVertDesc = GetVertexDescription(op.VertLayout);
		if (op.pIndexBuffer)
		{
			this->SetIndexBuffer(op.pIndexBuffer);
			this->SetVertexBuffer(op.pVertexBuffer);

			this->SetVertexDesc(pVertDesc);

			m_pd3dDevice->DrawIndexed(op.nPrimitiveCount * 3, op.nStartIndex, op.nBaseVertexIndex);
		}
		else
		{
			this->SetIndexBuffer(NULL);
			this->SetVertexBuffer(op.pVertexBuffer);
			this->SetVertexDesc(pVertDesc);

			m_pd3dDevice->Draw(op.nNumVertices, op.nBaseVertexIndex);
		}
	}

	void RendererDx10::OnReset()
	{
		// create depth stencil buffer
		D3D10_TEXTURE2D_DESC descDepth;
		memset(&descDepth, 0, sizeof(descDepth));

		descDepth.Width				= m_nWidth;
		descDepth.Height			= m_nHeight;
		descDepth.MipLevels			= 1;
		descDepth.ArraySize			= 1;
		descDepth.Format			= DXGI_FORMAT_D24_UNORM_S8_UINT;
		descDepth.SampleDesc.Count	= 1;
		descDepth.SampleDesc.Quality = 0;
		descDepth.Usage				= D3D10_USAGE_DEFAULT;
		descDepth.BindFlags			= D3D10_BIND_DEPTH_STENCIL;
		descDepth.CPUAccessFlags	= 0;
		descDepth.MiscFlags			= 0;
		HRESULT hr = m_pd3dDevice->CreateTexture2D(&descDepth, NULL, &m_pDepthStencil);
		assert(SUCCEEDED(hr));

		hr = m_pd3dDevice->CreateDepthStencilView(m_pDepthStencil, NULL, &m_pDepthStencilView); 
		assert(SUCCEEDED(hr));

		RenderTarget::Desc desc;
		memset(&desc, 0, sizeof(RenderTarget::Desc));

		desc.nWidth		= RenderTarget::DS_FRAMEBUFFER_WIDTH;
		desc.nHeight	= RenderTarget::DS_FRAMEBUFFER_HEIGHT;
		desc.nUsage		= Surface::USAGE_DEPTHSTENCIL;
		desc.nFormat	= Surface::FMT_D24S8;

		m_pRTDepthBuffer = new RenderTarget(desc);
		Dx10TextureBind* pBind = new Dx10TextureBind;
		pBind->pTexture = m_pDepthStencil;
		pBind->pDSView = m_pDepthStencilView;
		m_pRTDepthBuffer->HALHandleRT = pBind;


		m_pd3dDevice->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

		GDeviceState.rts[0] = m_pRenderTargetView;
		GDeviceState.rts[1] = NULL;
		GDeviceState.rts[2] = NULL;
		GDeviceState.rts[3] = NULL;
		GDeviceState.ds = m_pDepthStencilView;


		// Setup the viewport
		D3D10_VIEWPORT vp;
		vp.Width	= m_nWidth;
		vp.Height	= m_nHeight;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		m_pd3dDevice->RSSetViewports(1, &vp);

	}

	void RendererDx10::OnLost()
	{
		m_pDepthStencilView->Release();
		m_pDepthStencilView = NULL;

		m_pDepthStencil->Release();
		m_pDepthStencil = NULL;

		if (m_pRTDepthBuffer)
		{
			delete m_pRTDepthBuffer;
			m_pRTDepthBuffer = NULL;
		}
	}

	void RendererDx10::GetFrameBufferSize(uint& nWidth, uint& nHeight)
	{
		nWidth	= m_nWidth;
		nHeight	= m_nHeight;
	}

	void RendererDx10::Reset(int nWidth, int nHeight)
	{
	}

	void RendererDx10::RestoreFrameBuffer()
	{
		RenderTarget* rt[] = {GetFrameBuffer(), NULL, NULL, NULL};
		RenderTarget* pDepthBuffer = GRenderDevice->GetDepthStecil();
		this->SetRenderTarget(4, rt, pDepthBuffer);

		ID3D10ShaderResourceView* rv[] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
		m_pd3dDevice->PSSetShaderResources(0, 8, rv);
	}

	void RendererDx10::SetVertexDesc(VertexDescription* pVertDesc)
	{
		m_pd3dDevice->IASetInputLayout((ID3D10InputLayout*)pVertDesc->HALHandle);
	}

	void RendererDx10::SetIndexBuffer(IndexBuffer* pIndexBuffer)
	{
		if (pIndexBuffer == NULL)
		{
			m_pd3dDevice->IASetIndexBuffer(NULL, (DXGI_FORMAT)0, 0);
			return;
		}

		IndexBufferD3D10* pIndexBuffer10 = (IndexBufferD3D10*)pIndexBuffer;

		m_pd3dDevice->IASetIndexBuffer((ID3D10Buffer*)pIndexBuffer10->GetBindID(), 
			pIndexBuffer10->GetFormat() == IndexBuffer::FMT_INDEX16 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
	}

	void RendererDx10::SetVertexBuffer(VertexBuffer* pVertexBuffer)
	{
		UINT strides[] = {pVertexBuffer->GetStride()};
		UINT offsets[] = {0};
		//ID3D10Buffer* pBuffers[] = {pVertexBuffer10->GetBindPointer(0)};

		m_pd3dDevice->IASetVertexBuffers(0, 1, (ID3D10Buffer**)(&pVertexBuffer->HALHandle), strides, offsets);
	}



}