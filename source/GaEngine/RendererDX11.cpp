#include "stdHeader.h"

#include <d3d11.h>
//#include <d3dx11.h>
#include <DXGI.h>
#include <D3Dcompiler.h>


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

#include "RenderState.h"


#include "stb/stb_image.h"
#include "FileSystem.h"
#include "DDSTextureLoader.h"

#pragma comment (lib, "d3d11.lib") 
#pragma comment (lib, "DXGI.lib") 
#pragma comment (lib, "d3dcompiler.lib") 
#pragma comment (lib, "DXGUID.lib") 

//#ifdef _DEBUG
//#pragma comment (lib, "d3dx11d.lib") 
//#else
//#pragma comment (lib, "d3dx11.lib") 
//#endif



namespace Aurora
{



	// wchar_t to string
	void Wchar_tToString(std::string& szDst, wchar_t* wchar)
	{
		wchar_t* wText = wchar;
		DWORD dwNum = WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, NULL, 0, NULL, FALSE);
		char* psText; //
		psText = new char[dwNum];
		WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, psText, dwNum, NULL, FALSE);
		szDst = psText;
		delete[]psText;
	}

	//Converting a WChar string to a Ansi string   
	std::string WChar2Ansi(LPCWSTR pwszSrc)
	{
		int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);
		if (nLen <= 0) return std::string("");
		char* pszDst = new char[nLen];
		if (NULL == pszDst) return std::string("");
		WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
		pszDst[nLen - 1] = 0;
		std::string strTemp(pszDst);
		delete[] pszDst;
		return strTemp;
	}
	std::string ws2s(std::wstring& inputws)
	{
		return WChar2Ansi(inputws.c_str());
	}

	//Converting a Ansi string to WChar string   
	std::wstring Ansi2WChar(LPCSTR pszSrc, int nLen)
	{
		int nSize = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszSrc, nLen, 0, 0);
		if (nSize <= 0) return std::wstring();
		WCHAR* pwszDst = new WCHAR[nSize + 1];
		if (NULL == pwszDst) return NULL;
		MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszSrc, nLen, pwszDst, nSize);
		pwszDst[nSize] = 0;
		if (pwszDst[0] == 0xFEFF) // skip Oxfeff   
			for (int i = 0; i < nSize; i++)
				pwszDst[i] = pwszDst[i + 1];
		std::wstring wcharString(pwszDst);
		delete pwszDst;
		return wcharString;
	}
	std::wstring s2ws(const std::string& s)
	{
		return Ansi2WChar(s.c_str(), s.size());
	}






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



	class D3D11Mapping
	{
	public:

		static D3D11_PRIMITIVE_TOPOLOGY PrimitiveTopology(RenderOperator::PrimitiveType priType);
	};




	D3D11_PRIMITIVE_TOPOLOGY D3D11Mapping::PrimitiveTopology(RenderOperator::PrimitiveType priType)
	{
		static D3D11_PRIMITIVE_TOPOLOGY table[] = {
			D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
			D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
			D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		};

		return table[priType];
	}


	// for debug
	enum RESOURCE_FLAG:int32
	{
		RESOURCE_FLAG_SHADER	= 0x01000000,
		RESOURCE_FLAG_BUFFER	= 0x02000000,
		RESOURCE_FLAG_TEXTURE	= 0x04000000,
		RESOURCE_FLAG_SAMPLER	= 0x08000000
	};

	enum SHADER_FLAG :int32
	{
		SHADER_FLAG_VERTEX		= 0x00010000,
		SHADER_FLAG_PIXEL		= 0x00020000,
		SHADER_FLAG_GEOMETRY	= 0x00040000,
	};

	class RendererDx11 : public IRenderDevice
	{
	public:
		
		RendererDx11();
		~RendererDx11(void);


		virtual Handle  CreateShader(const ShaderCode& code);


		virtual Texture* CreateTexture(File* file);

		virtual Texture* CreateTexture(Texture::Type type, const Texture::Desc& desc);

		virtual RenderTarget* CreateRenderTarget(const RenderTarget::Desc& desc);

		virtual VertexBuffer* CreateVertexBuffer(Geometry* pGeometry, VertexLayout layout, uint nVert);

		virtual IndexBuffer* CreateIndexBuffer(Geometry* pGeometry, IndexBuffer::Format fmt, uint nNumIndex);

		virtual VertexBuffer* CreateDynamicVertexBuffer(int nStride, int nNumVert);

		//////////////////////////////////////////////////////////////////////////


		virtual void		Clear(int clearMask, const Color& clearColor = Color::BLUE, float fDepth = 1.0f, uint8 nStencil = 0);
		virtual void		BeginScene();
		virtual void		EndScene();
		virtual void		Present();

		RenderTarget* GetFrameBuffer();
		RenderTarget* GetDepthStecil();

		virtual void		SetVertexDesc(VertexDescription* pVertDesc);

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


		void		InitializeInputLayout();
		void		FinalizeInputLayout();
	};







	static RendererDx11			GRendererDx11;

	ID3D11Device* D3D11Device = NULL;
	ID3D11DeviceContext* ImmediateContext = NULL;
	static IDXGISwapChain* SwapChain = NULL;
	static ID3D11RenderTargetView* RenderTargetView = NULL;
	static ID3D11Texture2D* DepthStencil = NULL;
	static ID3D11DepthStencilView* DepthStencilView = NULL;

	struct DeviceState11
	{
		ID3D11RenderTargetView* rts[4];
		ID3D11DepthStencilView* ds;
	};


	static DeviceState11 GDeviceState;

	struct Dx11TextureBind
	{
		ID3D11Resource* pTexture;
		union
		{
			ID3D11RenderTargetView* pRTView;
			ID3D11DepthStencilView* pDSView;
		};
	};


	bool CreateDX11Device(HWND hWnd)
	{
		D3D_DRIVER_TYPE         driverType = D3D_DRIVER_TYPE_NULL;
		D3D_FEATURE_LEVEL       featureLevel = D3D_FEATURE_LEVEL_11_0;

		HRESULT hr = S_OK;

		RECT rc;
		GetClientRect(hWnd, &rc);
		UINT width = rc.right - rc.left;
		UINT height = rc.bottom - rc.top;

		UINT createDeviceFlags = 0;
#ifdef _DEBUG
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		D3D_DRIVER_TYPE driverTypes[] =
		{
			D3D_DRIVER_TYPE_HARDWARE,
			D3D_DRIVER_TYPE_WARP,
			D3D_DRIVER_TYPE_REFERENCE,
		};
		UINT numDriverTypes = ARRAYSIZE(driverTypes);

		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
		};
		UINT numFeatureLevels = ARRAYSIZE(featureLevels);

		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 1;
		sd.BufferDesc.Width = width;
		sd.BufferDesc.Height = height;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hWnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;

		for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++) {
			driverType = driverTypes[driverTypeIndex];
			hr = D3D11CreateDeviceAndSwapChain(NULL, driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
				D3D11_SDK_VERSION, &sd, &SwapChain, &D3D11Device, &featureLevel, &ImmediateContext);
			if (SUCCEEDED(hr))
				break;
		}

		if (FAILED(hr)) {
			GLog->Error("D3D11CreateDeviceAndSwapChain failed! hr = %d", hr);
			return false;
		}

		// Create a render target view
		ID3D11Texture2D* pBackBuffer = NULL;
		hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		if (FAILED(hr)) {
			GLog->Error("SwapChain->GetBuffer failed! hr = %d", hr);
			SwapChain->Release();
			ImmediateContext->Release();
			return false;
		}

		hr = D3D11Device->CreateRenderTargetView(pBackBuffer, NULL, &RenderTargetView);
		pBackBuffer->Release();
		if (FAILED(hr)) {
			GLog->Error("D3D11Device->CreateRenderTargetView failed! hr = %d", hr);
			return false;
		}
		GDeviceState.rts[0] = RenderTargetView;


		// Create depth stencil texture
		D3D11_TEXTURE2D_DESC descDepth;
		ZeroMemory(&descDepth, sizeof(descDepth));
		descDepth.Width = width;
		descDepth.Height = height;
		descDepth.MipLevels = 1;
		descDepth.ArraySize = 1;
		descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		descDepth.SampleDesc.Count = 1;
		descDepth.SampleDesc.Quality = 0;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		descDepth.CPUAccessFlags = 0;
		descDepth.MiscFlags = 0;
		hr = D3D11Device->CreateTexture2D(&descDepth, NULL, &DepthStencil);
		if (FAILED(hr)) {
			GLog->Error("D3D11Device->CreateTexture2D Create depth RT failed! hr = %d", hr);
			return false;
		}

		// Create the depth stencil view
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		ZeroMemory(&descDSV, sizeof(descDSV));
		descDSV.Format = descDepth.Format;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;
		hr = D3D11Device->CreateDepthStencilView(DepthStencil, &descDSV, &DepthStencilView);
		if (FAILED(hr)) {
			GLog->Error("D3D11Device->CreateDepthStencilView failed! hr = %d", hr);
			return false;
		}
		GDeviceState.ds = DepthStencilView;

		ImmediateContext->OMSetRenderTargets(1, &RenderTargetView, DepthStencilView);

		// Setup the viewport
		D3D11_VIEWPORT vp;
		vp.Width = (FLOAT)width;
		vp.Height = (FLOAT)height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		ImmediateContext->RSSetViewports(1, &vp);

		return true;
	}


	void CreateGlobalParameterBuffer();

	IRenderDevice* IRenderDevice::CreateDevice(HWND hWnd, int nWidth, int nHeight)
	{
		if (!CreateDX11Device(hWnd)) {
			return NULL;
		}

		GRenderDevice = &GRendererDx11;

		GRendererDx11.InitializeInputLayout();
		CreateGlobalParameterBuffer();

		return GRenderDevice;
	}




	RendererDx11::RendererDx11()
	{
	}

	RendererDx11::~RendererDx11(void)
	{
	}



	struct ShaderObject
	{
		string Name;
		ID3D11VertexShader* VertexShader = nullptr;
		ID3D11PixelShader* PixelShader = nullptr;
		ID3D11ShaderReflection* Reflector = nullptr;
		vector<int32>	constBuffers;
	};


	static vector<ShaderObject*>	VertexShaderList_;





	Handle RendererDx11::CreateShader(const ShaderCode& code)
	{
		vector<D3D_SHADER_MACRO> macros;
		for (auto it = code.defines.begin(); it != code.defines.end(); ++it) {
			D3D_SHADER_MACRO m = { it->first.c_str(), it->second.c_str() };
			macros.push_back(m);
		}
		D3D_SHADER_MACRO m = { NULL, NULL };
		macros.push_back(m);


		ID3DBlob* pCompiledShader = NULL;
		ID3DBlob* pError = NULL;

		UINT flag = 0;
#ifdef _DEBUG
		flag |= D3DCOMPILE_DEBUG;
#endif

		char* target = "vs_4_0";
		if (code.type == Shader::Category::TYPE_PIXEL_SHADER) {
			target = "ps_4_0";
		}
		else if (code.type == Shader::Category::TYPE_GEOM_SHADER) {

		}
		else if (code.type == Shader::Category::TYPE_COMPUTE_SHADER) {

		}

		HRESULT hr = D3DCompile(code.text.c_str(), code.text.length(), code.name.c_str(),
			&macros[0], D3D_COMPILE_STANDARD_FILE_INCLUDE, "Main",
			target, flag, 0, &pCompiledShader, &pError);


		if (FAILED(hr)) {
			std::string msg = "compile shader error. material: ";
			msg += '\n';
			Console::AppendText(msg.c_str());
			if (pError)
			{
				std::string strError = (const char*)pError->GetBufferPointer();
				Console::AppendText(strError.c_str());
				GLog->Error(strError.c_str());
				pError->Release();
			}

			return -1;
		}

		auto obj = new ShaderObject();
		obj->Name = code.name;
		if (code.type == Shader::Category::TYPE_VERTEX_SHADER) {

			ID3D11VertexShader* pVertexShader = NULL;
			hr = D3D11Device->CreateVertexShader(pCompiledShader->GetBufferPointer(),
				pCompiledShader->GetBufferSize(), NULL, &pVertexShader);
			assert(SUCCEEDED(hr));
			obj->VertexShader = pVertexShader;
		}
		else if (code.type == Shader::Category::TYPE_PIXEL_SHADER) {
			ID3D11PixelShader* pixelShader = NULL;
			hr = D3D11Device->CreatePixelShader(pCompiledShader->GetBufferPointer(),
				pCompiledShader->GetBufferSize(), NULL, &pixelShader);
			assert(SUCCEEDED(hr));
			obj->PixelShader = pixelShader;
		}

		ID3D11ShaderReflection* pReflector = NULL;
		D3DReflect(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(),
			IID_ID3D11ShaderReflection, (void**)&pReflector);
		pCompiledShader->Release();
		pCompiledShader = NULL;

		obj->Reflector = pReflector;

		auto it = std::find(VertexShaderList_.begin(), VertexShaderList_.end(), nullptr);
		if (it == VertexShaderList_.end()) {
			VertexShaderList_.push_back(obj);
			return VertexShaderList_.size() - 1;
		}

		*it = obj;
		return std::distance(VertexShaderList_.begin(), it);
	}



	//template<class type>
	//int32 FindAvailableSlot(vector<type*>& v)
	//{
	//	auto it = std::find(v.begin(), v.end(), nullptr);
	//	if (it == v.end()) {
	//		v.push_back(nullptr);
	//		return v.size() - 1;
	//	}

	//	return std::distance(v.begin(), it);
	//}



	struct ShaderParameterBindInfo
	{
		string	Name;
		int32	Offset;
		int32	Size;
		void* Source;
	};


	struct ShaderParameterBuffer
	{
		string	Name;
		int32	Slot = 0;
		int32	Size = 0;
		ID3D11Buffer* D3DBuffer = nullptr;

		vector<ShaderParameterBindInfo>	Bindings;
	};

	void CreateGlobalParameterBuffer()
	{
		ShaderCode code;
		char* shaderCode = "                     \
												 \
		cbuffer GlobalParameter: register(b0)	 \
		{										 \
			float4x4 matView;					 \
			float4x4 matProj;					 \
			float4x4 matViewProj;				 \
			float3	DirectionLight0;			 \
			float4	LightColor0;				 \
		};										 \
		float4 Main(float4 pos : POSITION) : SV_POSITION		\
		{														\
			return pos + LightColor0;						\
		}														\
			";

		code.name = "Global Buffer";
		code.text = shaderCode;
		code.type = Shader::Category::TYPE_VERTEX_SHADER;

		Handle handle = GRendererDx11.CreateShader(code);

		assert(handle == 0);
	}



	static vector<ShaderParameterBuffer*>  ShaderParameterBuffers_;

	Handle CreateShaderParameterBinding(Handle shaderHandle, const ShaderParamterBindings& bindings)
	{
		auto shader = VertexShaderList_[shaderHandle];

		ID3D11ShaderReflection* Reflector = shader->Reflector;

		D3D11_SHADER_DESC shaderDesc;
		Reflector->GetDesc(&shaderDesc);

		auto constBuffer = Reflector->GetConstantBufferByName(bindings.Name.c_str());

		D3D11_SHADER_BUFFER_DESC desc;
		if (FAILED(constBuffer->GetDesc(&desc))) {
			GLog->Error("can't find shader const buffer by name \"%s\" in shader %s",
				bindings.Name.c_str(), shader->Name.c_str());
			return -1;
		}

		D3D11_SHADER_INPUT_BIND_DESC bindDesc;
		if (FAILED(Reflector->GetResourceBindingDescByName(bindings.Name.c_str(), &bindDesc))) {
			GLog->Error("GetResourceBindingDescByName failed! bind name : %s", bindings.Name.c_str());
			return -1;
		}

		//if (strcmp(desc.Name, "$Globals") != 0)
		//{
		//	continue;
		//}
		
		D3D11_BUFFER_DESC cbDesc;
		cbDesc.ByteWidth = desc.Size;
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0;
		cbDesc.StructureByteStride = 0;

		ID3D11Buffer* d3DBuffer = nullptr;
		if (FAILED(D3D11Device->CreateBuffer(&cbDesc, nullptr, &d3DBuffer))) {
			GLog->Error("create shader const buffer failed! shader : %s", shader->Name.c_str());
			return -1;
		}

		int32 slot = FindAvailableSlot(ShaderParameterBuffers_);
		auto newBuffer = new ShaderParameterBuffer();
		newBuffer->Name = bindings.Name;
		newBuffer->Slot = bindDesc.BindPoint;
		newBuffer->Size = desc.Size;
		newBuffer->D3DBuffer = d3DBuffer;

		for (auto it = bindings.Bindings.begin(); it != bindings.Bindings.end(); ++it) {
			auto variable = constBuffer->GetVariableByName(it->Name.c_str());

			D3D11_SHADER_VARIABLE_DESC varDesc;
			if (FAILED(variable->GetDesc(&varDesc))) {
				GLog->Info("can't find shader variable by name \"%s\" in buffer %s of shader %s",
					it->Name.c_str(), bindings.Name.c_str(), shader->Name.c_str());
				continue;
			}

			//if (!(varDesc.uFlags & D3D_SVF_USED)) {
			//	continue;
			//}

			ShaderParameterBindInfo info{ it->Name, (int32)varDesc.StartOffset, (int32)varDesc.Size, it->Source };
			newBuffer->Bindings.push_back(info);
		}
		
		ShaderParameterBuffers_[slot] = newBuffer;

		shader->constBuffers.push_back(slot);

		return slot;
	}



	Handle CreateShaderTextureBinding(Handle shaderHandle, const ShaderTextureBinding& bindings)
	{
		auto shader = VertexShaderList_[shaderHandle];
		ID3D11ShaderReflection* Reflector = shader->Reflector;

		D3D11_SHADER_INPUT_BIND_DESC bindDesc;
		HRESULT hr = Reflector->GetResourceBindingDescByName(bindings.Name.c_str(), &bindDesc);
		if (FAILED(hr)) {
			GLog->Error("can not find binding texture %s in shader %s", bindings.Name.c_str(), shader->Name.c_str());
			return -1;
		}

		//Reflector->GetResourceBindingDescByName("g_samLinear", &bindDesc);

		int32 handle = 0;
		assert(bindDesc.Type == D3D_SIT_TEXTURE);

		handle = bindDesc.BindPoint;
		handle |= RESOURCE_FLAG_TEXTURE;
		if (shader->VertexShader != nullptr) {
			handle |= SHADER_FLAG_VERTEX;
		}

		if (shader->PixelShader != nullptr) {
			handle |= SHADER_FLAG_PIXEL;
		}

		return handle;
	}


	void UpdateShaderParameter(Handle bindingHandle)
	{
		assert(bindingHandle >= 0);
		auto binding = ShaderParameterBuffers_[bindingHandle];

		D3D11_MAPPED_SUBRESOURCE res = { 0 };

		HRESULT hr = ImmediateContext->Map(
			binding->D3DBuffer,  0,
			D3D11_MAP_WRITE_DISCARD, 0, &res);

		if (FAILED(hr)) {
			GLog->Error("map shader parameter buffer failed. name : %s", binding->Name.c_str());
			return;
		}

		for (auto it = binding->Bindings.begin(); it != binding->Bindings.end(); ++it) {
			memcpy((int8*)res.pData + it->Offset, it->Source, it->Size);
		}

		ImmediateContext->Unmap(binding->D3DBuffer, 0);
	}

	void BindVertexShaderParameter(Handle bindingHandle)
	{
		assert(bindingHandle >= 0);
		auto binding = ShaderParameterBuffers_[bindingHandle];

		ImmediateContext->VSSetConstantBuffers(binding->Slot, 1, &binding->D3DBuffer);
	}

	void BindPixelShaderParameter(Handle bindingHandle)
	{
		assert(bindingHandle >= 0);
		auto binding = ShaderParameterBuffers_[bindingHandle];

		ImmediateContext->PSSetConstantBuffers(binding->Slot, 1, &binding->D3DBuffer);
	}


	void BindGlobalParameter(Handle handle)
	{
		auto binding = ShaderParameterBuffers_[handle];
		ImmediateContext->VSSetConstantBuffers(binding->Slot, 1, &binding->D3DBuffer);
		ImmediateContext->PSSetConstantBuffers(binding->Slot, 1, &binding->D3DBuffer);
	}

	void BindVertexShader(Handle shaderHandle)
	{
		assert(shaderHandle >= 0);
		auto shader = VertexShaderList_[shaderHandle];
		assert(shader->VertexShader != nullptr);

		ImmediateContext->VSSetShader(shader->VertexShader, NULL, 0);

		for (int i = 0; i < shader->constBuffers.size(); i++) {
			BindVertexShaderParameter(shader->constBuffers[i]);
		}
	}

	void BindPixelShader(Handle shaderHandle)
	{
		assert(shaderHandle >= 0);
		auto shader = VertexShaderList_[shaderHandle];
		assert(shader->PixelShader != nullptr);

		ImmediateContext->PSSetShader(shader->PixelShader, NULL, 0);

		for (int i = 0; i < shader->constBuffers.size(); i++) {
			BindPixelShaderParameter(shader->constBuffers[i]);
		}
	}



	struct TextureResourceInfo
	{
		D3D11_SRV_DIMENSION ViewDimension;

		union
		{
			ID3D11Texture1D* d3DTexture1d = nullptr;
			ID3D11Texture2D* d3DTexture2d;
			ID3D11Texture3D*	d3DTexture3d;
		};

		ID3D11ShaderResourceView* pResourceView = nullptr;
	};




	Surface::Format FormatD3DToEngine(uint fmt);
	DXGI_FORMAT FormatEngineToD3D(Surface::Format fmt);




	Texture* RendererDx11::CreateTexture(File* file)
	{
		vector<uint8> buffer;
		buffer.resize(file->Size());
		file->Read(&buffer[0], file->Size());

		HRESULT hr = S_FALSE;
		ID3D11Texture2D* d3dTexture = nullptr;
		ID3D11Resource* pResource = nullptr;
		ID3D11ShaderResourceView* pResourceView = nullptr;

		string ext = Util::GetFileNameExt(file->Pathname());
		if (ext == "dds") {
			hr = DirectX::CreateDDSTextureFromMemory(D3D11Device, &buffer[0], buffer.size(), &pResource, &pResourceView, 0, nullptr);
			if (FAILED(hr)) {
				GLog->Error("create texture from %s failed!", file->Pathname().c_str());
				return nullptr;
			}

		} else {
			
			int x = 0, y = 0, comp = 0;
			uint8* texbuffer = stbi_load_from_memory(&buffer[0], buffer.size(), &x, &y, &comp, 4);

			D3D11_TEXTURE2D_DESC texDesc = { 0 };
			texDesc.Width = x;
			texDesc.Height = y;
			texDesc.MipLevels = 1;
			texDesc.ArraySize = 1;
			texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			texDesc.SampleDesc.Count = 1;
			texDesc.SampleDesc.Quality = 0;
			texDesc.Usage = D3D11_USAGE_DEFAULT;
			texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			texDesc.CPUAccessFlags = 0;
			texDesc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA data = { texbuffer, (UINT)x * 4, 0 };

			hr = D3D11Device->CreateTexture2D(&texDesc, &data, &d3dTexture);
			stbi_image_free(texbuffer);

			if (FAILED(hr)) {
				GLog->Error("create texture from %s failed!", file->Pathname().c_str());
				return nullptr;
			}

			hr = D3D11Device->CreateShaderResourceView(d3dTexture, NULL, &pResourceView);
			if (FAILED(hr)) {
				GLog->Error("create shader resource view from %s failed!", file->Pathname().c_str());
				return nullptr;
			}
		}
		
		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc = { 0 };
		pResourceView->GetDesc(&viewDesc);
		pResourceView->GetResource(&pResource);

		D3D11_RESOURCE_DIMENSION resType;
		pResource->GetType(&resType);
			   
		Texture* pTexture = nullptr;

		if (resType == D3D11_RESOURCE_DIMENSION_TEXTURE2D) {
			d3dTexture = (ID3D11Texture2D*)pResource;


			ID3D11Texture2D* pD3DTex2d = (ID3D11Texture2D*)pResource;
			D3D11_TEXTURE2D_DESC d3dDesc;
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

			pTexture = new Texture(desc);
			auto handle = new TextureResourceInfo();
			handle->ViewDimension = viewDesc.ViewDimension;
			handle->d3DTexture2d = d3dTexture;
			handle->pResourceView = pResourceView;


			if (viewDesc.ViewDimension == D3D11_SRV_DIMENSION_TEXTURE2D) {
				pTexture->type_ = Texture::TP_2D;
				pTexture->HALHandle = handle;
			}

			if (viewDesc.ViewDimension == D3D11_SRV_DIMENSION_TEXTURECUBE) {
				pTexture->type_ = Texture::TP_CUBE;
				pTexture->HALHandle = handle;
			}
		}

		return pTexture;
	}




	void BindTexture(Handle binding, Texture* texture)
	{
		assert((binding & RESOURCE_FLAG_TEXTURE) == RESOURCE_FLAG_TEXTURE);
		assert(texture);
		int32 slot = binding & 0xffff;

		TextureResourceInfo* info = (TextureResourceInfo*)texture->HALHandle;

		if (binding & SHADER_FLAG_VERTEX) {
			ImmediateContext->VSSetShaderResources(slot, 1, &info->pResourceView);
		}

		if (binding & SHADER_FLAG_PIXEL) {
			ImmediateContext->PSSetShaderResources(slot, 1, &info->pResourceView);
		}
	}



	Texture* RendererDx11::CreateTexture(Texture::Type type, const Texture::Desc& desc)
	{
		/*if (type == Texture::TP_2D)
		{
			D3D11_TEXTURE2D_DESC d3dDesc;
			d3dDesc.Width = (UINT)desc.nWidth;
			d3dDesc.Height = (UINT)desc.nHeight;
			d3dDesc.MipLevels = (UINT)desc.nMipLevels;
			d3dDesc.ArraySize = 1;
			d3dDesc.Format = FormatEngineToD3D(desc.nFormat);
			d3dDesc.SampleDesc.Count = 1;
			d3dDesc.SampleDesc.Quality = 0;
			d3dDesc.Usage = desc.bDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
			d3dDesc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
			d3dDesc.CPUAccessFlags = 0;
			d3dDesc.MiscFlags = 0;

			Texture2DDx11* pTexture = new Texture2DDx11(desc);
			HRESULT hr = D3D11Device->CreateTexture2D(&d3dDesc, NULL, &pTexture->m_pD3DTexture);
			assert(SUCCEEDED(hr));

			D3D11_SHADER_RESOURCE_VIEW_DESC resViewDesc;
			resViewDesc.Format = d3dDesc.Format;
			resViewDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
			resViewDesc.Texture2D.MipLevels = d3dDesc.MipLevels;
			resViewDesc.Texture2D.MostDetailedMip = 0;

			ID3D11ShaderResourceView* pD3DResView = NULL;
			hr = D3D11Device->CreateShaderResourceView(pTexture->m_pD3DTexture, &resViewDesc, &pD3DResView);
			assert(SUCCEEDED(hr));
			pTexture->HALHandle = pD3DResView;

			return pTexture;
		}
		else if (type == Texture::TP_3D)
		{
			D3D11_TEXTURE3D_DESC d3dDesc;
			d3dDesc.Width = (UINT)desc.nWidth;
			d3dDesc.Height = (UINT)desc.nHeight;
			d3dDesc.Depth = (UINT)desc.nDepth;
			d3dDesc.MipLevels = (UINT)desc.nMipLevels;
			d3dDesc.Format = FormatEngineToD3D(desc.nFormat);
			d3dDesc.Usage = desc.bDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
			d3dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			d3dDesc.CPUAccessFlags = 0;
			d3dDesc.MiscFlags = 0;

			Texture3DDx11* pTexture = new Texture3DDx11(desc);
			HRESULT hr = D3D11Device->CreateTexture3D(&d3dDesc, NULL, &pTexture->m_pD3DTexture);
			assert(SUCCEEDED(hr));

			D3D11_SHADER_RESOURCE_VIEW_DESC resViewDesc;
			resViewDesc.Format = d3dDesc.Format;
			resViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
			resViewDesc.Texture3D.MipLevels = d3dDesc.MipLevels;
			resViewDesc.Texture3D.MostDetailedMip = 0;

			ID3D11ShaderResourceView* pD3DResView = NULL;
			hr = D3D11Device->CreateShaderResourceView(pTexture->m_pD3DTexture, &resViewDesc, &pD3DResView);
			assert(SUCCEEDED(hr));
			pTexture->HALHandle = pD3DResView;

			return pTexture;
		}*/

		assert(0);
		return NULL;
	}

	RenderTarget* RendererDx11::CreateRenderTarget(const RenderTarget::Desc& desc)
	{		
		RenderTarget* pRT = new RenderTarget(desc);

		D3D11_TEXTURE2D_DESC rtDesc;
		ZeroMemory(&rtDesc, sizeof(D3D11_TEXTURE2D_DESC));
		rtDesc.ArraySize	= 1;
		rtDesc.Usage		= D3D11_USAGE_DEFAULT;
		rtDesc.Format		= FormatEngineToD3D(desc.nFormat);
		rtDesc.Width		= pRT->GetRealWidth();
		rtDesc.Height		= pRT->GetRealHeight();
		rtDesc.MipLevels	= 1;
		rtDesc.SampleDesc.Count = 1;
		rtDesc.BindFlags	= D3D11_BIND_SHADER_RESOURCE;
		if (desc.nUsage == Surface::USAGE_RENDERTARGET)
		{
			rtDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		}
		else if (desc.nUsage == Surface::USAGE_DEPTHSTENCIL)
		{
			rtDesc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
			if (desc.nFormat == Surface::FMT_D16)
			{
				rtDesc.Format = DXGI_FORMAT_R16_TYPELESS;
			}
		}

		HRESULT hr = S_OK;

		ID3D11Texture2D* pD3DTex = NULL;
		hr = D3D11Device->CreateTexture2D( &rtDesc, NULL, &pD3DTex);
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
		D3D11_SHADER_RESOURCE_VIEW_DESC DescRV;
		DescRV.Format = fmt;
		DescRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		DescRV.Texture2D.MipLevels = 1;
		DescRV.Texture2D.MostDetailedMip = 0;
		ID3D11ShaderResourceView* pShaderResourceView = NULL;

		hr = D3D11Device->CreateShaderResourceView(pD3DTex, &DescRV, &pShaderResourceView);
		if (FAILED(hr))
		{
			GLog->Error("CreateRenderTarget failed!");
			return NULL;
		}

		refCnt = pD3DTex->AddRef();
		refCnt = pD3DTex->Release();

		pRT->HALHandle = pShaderResourceView;


		Dx11TextureBind* pBind = new Dx11TextureBind;
		pBind->pTexture = pD3DTex;

		if (desc.nUsage == Surface::USAGE_RENDERTARGET)
		{
			// Create the render target view
			ID3D11RenderTargetView* pRTView = NULL;
			D3D11_RENDER_TARGET_VIEW_DESC DescRT;
			DescRT.Format = fmt;
			DescRT.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			DescRT.Texture2D.MipSlice = 0;
			hr = D3D11Device->CreateRenderTargetView(pD3DTex, &DescRT, &pRTView);

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
			ID3D11DepthStencilView* pDSView = NULL;
			D3D11_DEPTH_STENCIL_VIEW_DESC DescRT;
			DescRT.Format = fmt;
			DescRT.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			DescRT.Texture2D.MipSlice = 0;
			hr = D3D11Device->CreateDepthStencilView(pD3DTex, &DescRT, &pDSView);

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


	//////////////////////////////////////////////////////////////////////////

	class VertexBufferDx11 : public VertexBuffer
	{
	public:
		VertexBufferDx11(uint nStride, uint nNumVertex);
		~VertexBufferDx11();

	private:

		virtual void* Lock();

		virtual void Unlock();
	};

	VertexBufferDx11::VertexBufferDx11(uint nStride, uint nNumVertex):VertexBuffer(nStride, nNumVertex)
	{

	}

	VertexBufferDx11::~VertexBufferDx11()
	{
		ID3D10Buffer* pDxBuffer = (ID3D10Buffer*)HALHandle;
		pDxBuffer->Release();
		HALHandle = NULL;
	}

	void* VertexBufferDx11::Lock()
	{
		assert(0);
		VOID* pointer = NULL;
		//// todo: implement the flags
		//ID3D11Buffer* pDxBuffer = (ID3D11Buffer*)HALHandle;
		//HRESULT hr = pDxBuffer->Map(D3D11_MAP_WRITE_DISCARD, 0, &pointer);
		//assert(SUCCEEDED(hr));
		return pointer;
	}

	void VertexBufferDx11::Unlock()
	{
		assert(0);
		//ID3D11Buffer* pDxBuffer = (ID3D11Buffer*)HALHandle;
		//pDxBuffer->Unmap();
	}




	VertexDescription* GetVertexDescription(VertexLayout layout);
	VertexBuffer* RendererDx11::CreateVertexBuffer(Geometry* pGeometry, VertexLayout layout, uint nVert)
	{
		VertexDescription* pVertexDesc = GetVertexDescription(layout);

		uint nStride = 0;
		for (uint j = 0; j < pVertexDesc->stream.items.size(); j++)
		{
			const VertexDescription::Item& item = pVertexDesc->stream.items[j];
			nStride += Geometry::GetSizeOfType((Geometry::VertexElemType)item.type);
		}

		VertexBuffer* pVertexBuffer = new VertexBufferDx11(nStride, pGeometry->GetNumVertex());
		
		vector<unsigned char> tempBuffer;

		tempBuffer.resize(pVertexBuffer->GetSizeInByte());
		pGeometry->AssembleVertexBuffer(&tempBuffer[0], pVertexDesc->stream);

		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc.Usage		= D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags	= D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags	= 0;
		bufferDesc.ByteWidth	= pVertexBuffer->GetSizeInByte();

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;
		InitData.pSysMem		= &tempBuffer[0];

		HRESULT hr = D3D11Device->CreateBuffer(&bufferDesc, &InitData, (ID3D11Buffer**)(&pVertexBuffer->HALHandle));
		assert(SUCCEEDED(hr));

		return pVertexBuffer;
	}

	//////////////////////////////////////////////////////////////////////////



	class IndexBufferD3D11 : public IndexBuffer
	{
	public:
		IndexBufferD3D11(Geometry* pGeometry, IndexBuffer::Format fmt, uint nNumIndex);
		~IndexBufferD3D11();

		virtual void*			GetBindID() const;
		virtual void			OnReset();
		virtual void			OnLost();

		virtual void*			Lock();
		virtual void			Unlock();

	private:
		ID3D11Buffer*           m_pBuffer;
		Geometry*			m_pGeometry;
	};

	IndexBufferD3D11::IndexBufferD3D11(Geometry* pGeometry, IndexBuffer::Format fmt, uint nNumIndex):
	IndexBuffer(fmt, nNumIndex),m_pBuffer(NULL),m_pGeometry(pGeometry)
	{

	}

	IndexBufferD3D11::~IndexBufferD3D11()
	{
		assert(m_pBuffer == NULL);
	}


	void* IndexBufferD3D11::GetBindID() const
	{
		return m_pBuffer;
	}

	void* IndexBufferD3D11::Lock()
	{
		assert(0);
		VOID* pointer = NULL;
		// todo: implement the flags
		//HRESULT hr = m_pBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, &pointer);
		//assert(SUCCEEDED(hr));
		return pointer;
	}

	void IndexBufferD3D11::Unlock()
	{
		assert(0);
		//m_pBuffer->Unmap();
	}

	void IndexBufferD3D11::OnReset()
	{
		assert(m_pBuffer == NULL);
		D3DFORMAT d3dFmt = (GetFormat() == IndexBuffer::FMT_INDEX16) ? D3DFMT_INDEX16 : D3DFMT_INDEX32;
		uint stride = (GetFormat() == IndexBuffer::FMT_INDEX16) ? 2 : 4;
		UINT length = stride * GetNumIndex();

		uint8* pMemBuffer = m_pGeometry->GetStreamPointer(Geometry::USAGE_INDEX, 0);
		assert(pMemBuffer != NULL);


		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc.ByteWidth	= length;
		bufferDesc.Usage		= D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags	= D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags	= 0;

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;
		InitData.pSysMem = pMemBuffer;

		HRESULT hr = D3D11Device->CreateBuffer(&bufferDesc, &InitData, &m_pBuffer);
		assert(SUCCEEDED(hr));

		// Set index buffer
		//g_pd3dDevice->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	}

	void IndexBufferD3D11::OnLost()
	{
		assert(m_pBuffer != NULL);
		m_pBuffer->Release();
		m_pBuffer = NULL;
	}


	IndexBuffer* RendererDx11::CreateIndexBuffer(Geometry* pGeometry, IndexBuffer::Format fmt, uint nNumIndex)
	{
		return new IndexBufferD3D11(pGeometry, fmt, nNumIndex);
	}

	VertexBuffer* RendererDx11::CreateDynamicVertexBuffer(int nStride, int nNumVert)
	{
		return nullptr;
	}

	//////////////////////////////////////////////////////////////////////////


	void RendererDx11::Clear(int clearMask, const Color& clearColor, float fDepth , uint8 nStencil)
	{
		if (clearMask & IRenderDevice::CLEAR_FRAME_BUFFER)
		{
			for (int i = 0; i < 4; i++)
			{
				if (GDeviceState.rts[i])
					ImmediateContext->ClearRenderTargetView(GDeviceState.rts[i], clearColor.Ptr());
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
				ImmediateContext->ClearDepthStencilView(GDeviceState.ds, nFlag, fDepth, nStencil);
		}
	}
	
	void RendererDx11::BeginScene()
	{

	}

	void RendererDx11::EndScene()
	{

	}

	void RendererDx11::Present()
	{
		SwapChain->Present(0, 0);
	}

	RenderTarget* RendererDx11::GetFrameBuffer()
	{
		return nullptr;
	}

	RenderTarget* RendererDx11::GetDepthStecil()
	{
		return nullptr;
	}

	void RendererDx11::SetVertexDesc(VertexDescription* pVertDesc)
	{
		ID3D11InputLayout* layout = (ID3D11InputLayout*)pVertDesc->HALHandle;
		ImmediateContext->IASetInputLayout(layout);

	}

	void RendererDx11::SetVertexShader(Shader* pShader)
	{
		ImmediateContext->VSSetShader((ID3D11VertexShader*)pShader->HALHandle, NULL, 0);
	}

	void RendererDx11::SetPixelShader(Shader* pShader)
	{
		ImmediateContext->PSSetShader((ID3D11PixelShader*)pShader->HALHandle, NULL, 0);
	}

	void RendererDx11::SetIndexBuffer(IndexBuffer* pIndexBuffer)
	{
		if (pIndexBuffer == NULL)
		{
			ImmediateContext->IASetIndexBuffer(NULL, (DXGI_FORMAT)0, 0);
			return;
		}

		IndexBufferD3D11* pIndexBuffer11 = (IndexBufferD3D11*)pIndexBuffer;

		ImmediateContext->IASetIndexBuffer((ID3D11Buffer*)pIndexBuffer11->GetBindID(), 
			pIndexBuffer11->GetFormat() == IndexBuffer::FMT_INDEX16 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
	}

	void RendererDx11::SetVertexBuffer(VertexBuffer* pVertexBuffer)
	{
		UINT strides[] = {pVertexBuffer->GetStride()};
		UINT offsets[] = {0};
		//ID3D10Buffer* pBuffers[] = {pVertexBuffer10->GetBindPointer(0)};

		ImmediateContext->IASetVertexBuffers(0, 1, (ID3D11Buffer**)(&pVertexBuffer->HALHandle), strides, offsets);
	}

	void RendererDx11::SetRenderTarget(uint idx, RenderTarget* pRenderTarget)
	{

	}

	void RendererDx11::SetDepthStencil(RenderTarget* pDepthStencil)
	{

	}

	void RendererDx11::SetRenderTarget(uint nRTs, RenderTarget** pRenderTargets, RenderTarget* pDepthStencil)
	{
		ID3D11RenderTargetView * pRTs[] = {NULL, NULL, NULL, NULL};

		for (uint i = 0; i < nRTs; i++)
		{
			if (pRenderTargets[i])
			{
				Dx11TextureBind* pBind = (Dx11TextureBind*)pRenderTargets[i]->HALHandleRT;
				pRTs[i] = pBind->pRTView;
			}
		}

		ID3D11DepthStencilView* pDSView = NULL;
		if (pDepthStencil)
		{
			Dx11TextureBind* pDSBind = (Dx11TextureBind*)pDepthStencil->HALHandleRT;
			pDSView = pDSBind->pDSView;
		}

		ImmediateContext->OMSetRenderTargets(nRTs, pRTs, pDSView);
		GDeviceState.rts[0] = pRTs[0];
		GDeviceState.rts[1] = pRTs[1];
		GDeviceState.rts[2] = pRTs[2];
		GDeviceState.rts[3] = pRTs[3];
		GDeviceState.ds = pDSView;

	}

	void RendererDx11::ExecuteOperator(const RenderOperator& op)
	{
		ImmediateContext->IASetPrimitiveTopology(D3D11Mapping::PrimitiveTopology(op.nPrimType));

		VertexDescription* pVertDesc = GetVertexDescription(op.VertLayout);

		//pVertDesc = GetVertexDescription()

		if (op.pIndexBuffer)
		{
			this->SetIndexBuffer(op.pIndexBuffer);
			this->SetVertexBuffer(op.pVertexBuffer);

			this->SetVertexDesc(pVertDesc);

			ImmediateContext->DrawIndexed(op.nPrimitiveCount * 3, op.nStartIndex, op.nBaseVertexIndex);
		}
		else
		{
			this->SetIndexBuffer(NULL);
			this->SetVertexBuffer(op.pVertexBuffer);
			this->SetVertexDesc(pVertDesc);

			ImmediateContext->Draw(op.nNumVertices, op.nBaseVertexIndex);
		}
	}

	void RendererDx11::OnReset()
	{

	}

	void RendererDx11::OnLost()
	{

	}

	void RendererDx11::GetFrameBufferSize(uint& nWidth, uint& nHeight)
	{

	}

	void RendererDx11::Reset(int nWidth, int nHeight)
	{

	}

	void RendererDx11::RestoreFrameBuffer()
	{

	}




	static ID3DBlob* CreateShaderSignature(const char* vsin)
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


		HRESULT hr = S_FALSE;

		hr = D3DCompile(buffer, strlen(buffer), "CreateShaderSignature shader", NULL, NULL, "Main",
			"vs_4_0", 0, 0, &pShader, &pError);

		assert(SUCCEEDED(hr));

		if (FAILED(hr))
		{
			int code = HRESULT_CODE(hr);
			if (pError)
			{
				string errorMsg = (const char*)pError->GetBufferPointer();
				pError->Release();
				GLog->Error("CreateShaderSignature error : %s", errorMsg);
			}
			return nullptr;
		}

		ID3DBlob* pInputSig = NULL;
		hr = D3DGetInputSignatureBlob(pShader->GetBufferPointer(), pShader->GetBufferSize(), &pInputSig);
		assert(SUCCEEDED(hr));

		hr = pShader->Release();
		assert(SUCCEEDED(hr));

		return pInputSig;
	}



	static VertexDescription* CreateInputLayoutPNTT()
	{
		vector<D3D11_INPUT_ELEMENT_DESC> vDecl;

		D3D11_INPUT_ELEMENT_DESC elem;
		elem.SemanticName			= NULL;
		elem.Format					= DXGI_FORMAT_R32G32B32_FLOAT;
		elem.AlignedByteOffset		= 0;
		elem.SemanticIndex			= 0;
		elem.InputSlot				= 0;
		elem.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
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

		HRESULT hr = D3D11Device->CreateInputLayout(&vDecl[0], vDecl.size(), 
			pInputSig->GetBufferPointer(), pInputSig->GetBufferSize(),
			(ID3D11InputLayout**)(&pVertexDesc->HALHandle));

		assert(SUCCEEDED(hr));
		pInputSig->Release();

		return pVertexDesc;
	}


	static VertexDescription* CreateInputLayoutPNTTBB()
	{
		vector<D3D11_INPUT_ELEMENT_DESC> vDecl;

		D3D11_INPUT_ELEMENT_DESC elem;
		elem.SemanticName			= NULL;
		elem.Format					= DXGI_FORMAT_R32G32B32_FLOAT;
		elem.AlignedByteOffset		= 0;
		elem.SemanticIndex			= 0;
		elem.InputSlot				= 0;
		elem.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
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
		ID3DBlob* pInputSig = CreateShaderSignature(vsin);

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


		item.type		= Geometry::TYPE_UBYTE4;
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

		HRESULT hr = D3D11Device->CreateInputLayout(&vDecl[0], vDecl.size(), 
			pInputSig->GetBufferPointer(), pInputSig->GetBufferSize(),
			(ID3D11InputLayout**)(&pVertexDesc->HALHandle));

		assert(SUCCEEDED(hr));
		pInputSig->Release();

		return pVertexDesc;
	}

	static VertexDescription* CreateInputLayoutPT()
	{
		vector<D3D11_INPUT_ELEMENT_DESC> vDecl;

		D3D11_INPUT_ELEMENT_DESC elem;
		elem.SemanticName			= NULL;
		elem.Format					= DXGI_FORMAT_R32G32B32_FLOAT;
		elem.AlignedByteOffset		= 0;
		elem.SemanticIndex			= 0;
		elem.InputSlot				= 0;
		elem.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
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

		ID3DBlob* pInputSig = CreateShaderSignature(vsin);

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

		HRESULT hr = D3D11Device->CreateInputLayout(&vDecl[0], vDecl.size(), 
			pInputSig->GetBufferPointer(), pInputSig->GetBufferSize(),
			(ID3D11InputLayout**)(&pVertexDesc->HALHandle));

		assert(SUCCEEDED(hr));
		pInputSig->Release();

		return pVertexDesc;

	}

	static VertexDescription* CreateInputLayoutP()
	{
		vector<D3D11_INPUT_ELEMENT_DESC> vDecl;

		D3D11_INPUT_ELEMENT_DESC elem;
		elem.SemanticName			= NULL;
		elem.Format					= DXGI_FORMAT_R32G32B32_FLOAT;
		elem.AlignedByteOffset		= 0;
		elem.SemanticIndex			= 0;
		elem.InputSlot				= 0;
		elem.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
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

		HRESULT hr = D3D11Device->CreateInputLayout(&vDecl[0], vDecl.size(), 
			pInputSig->GetBufferPointer(), pInputSig->GetBufferSize(),
			(ID3D11InputLayout**)(&pVertexDesc->HALHandle));

		assert(SUCCEEDED(hr));
		pInputSig->Release();

		return pVertexDesc;
	}

	
	static VertexDescription* gInputLayout[VertexLayout_NUM];
	static VertexDescription* GetVertexDescription(VertexLayout layout)
	{
		return gInputLayout[layout];
	}

	void RendererDx11::InitializeInputLayout()
	{
		gInputLayout[VertexLayout_PNTT]		= CreateInputLayoutPNTT();
		gInputLayout[VertexLayout_PT]		= CreateInputLayoutPT();
		gInputLayout[VertexLayout_P]		= CreateInputLayoutP();
		gInputLayout[VertexLayout_PNTTBB]	= CreateInputLayoutPNTTBB();
	}


	void RendererDx11::FinalizeInputLayout()
	{
		for (int i = 0; i < VertexLayout_NUM; i++) {
			ID3D11InputLayout* pD3DObj = (ID3D11InputLayout*)gInputLayout[i]->HALHandle;
			pD3DObj->Release();
			delete gInputLayout[i];
			gInputLayout[i] = NULL;
		}
	}
}