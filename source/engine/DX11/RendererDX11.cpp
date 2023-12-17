#include "stdHeader.h"
#include <d3d11.h>
#include <d3d11_1.h>
#include <d3d11_2.h>
#include <d3d11_3.h>
#include <d3d11_4.h>
#include <DXGI.h>
#include <D3Dcompiler.h>
#include "Renderer.h"
#include "Console.h"
#include "Shader.h"
#include "Log.h"
#include "Util.h"
#include "RendererObject.h"
#include "Pipeline.h"
#include "RenderState.h"
#include "stb/stb_image.h"
#include "FileSystem.h"
#include "DDSTextureLoader.h"

#pragma comment (lib, "d3d11.lib") 
#pragma comment (lib, "DXGI.lib") 
#pragma comment (lib, "d3dcompiler.lib") 
#pragma comment (lib, "DXGUID.lib") 




namespace Aurora
{

	class MyInclude : public ID3DInclude
	{
	public:
		virtual HRESULT DECLSPEC_NOTHROW STDMETHODCALLTYPE Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes) override;
		virtual HRESULT DECLSPEC_NOTHROW STDMETHODCALLTYPE Close(LPCVOID pData) override { return S_OK; }

		vector<unique_ptr<int8[]>>	cache_;
		string	path_;
	};

	void ReplaceShaderInclude(unique_ptr<int8[]>& data, int32* size);



	HRESULT MyInclude::Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes)
	{
		if (_stricmp(pFileName, "Replace.shader") == 0) {
			unique_ptr<int8[]> data;
			int32 size = 0;

			ReplaceShaderInclude(data, &size);

			*ppData = data.get();
			*pBytes = size;

			cache_.push_back(move(data));
			return S_OK;
		}


		string pathName = path_ + pFileName;
		
		FILE* file = nullptr;
		if (fopen_s(&file, pathName.c_str(), "rb")) {
			GLog->Error("open shader include file %s failed!", pathName.c_str());
			return E_FAIL;
		}

		fseek(file, 0, SEEK_END);
		uint32 size = ftell(file);
		fseek(file, 0, SEEK_SET);

		unique_ptr<int8[]> data(new int8[size + 1]());
		fread_s(data.get(), size, 1, size, file);
		fclose(file);

		//auto encode = Util::GetEncoding(data.get());
		*ppData = data.get();
		*pBytes = size;	

		cache_.push_back(move(data));
		return S_OK;
	}

	// wchar_t to string
	void Wchar_tToString(std::string& szDst, wchar_t* wchar)
	{
		wchar_t* wText = wchar;
		DWORD dwNum = WideCharToMultiByte(CP_OEMCP, 0, wText, -1, nullptr, 0, nullptr, FALSE);
		char* psText; //
		psText = new char[dwNum];
		WideCharToMultiByte(CP_OEMCP, 0, wText, -1, psText, dwNum, nullptr, FALSE);
		szDst = psText;
		delete[]psText;
	}

	//Converting a WChar string to a Ansi string   
	std::string WChar2Ansi(LPCWSTR pwszSrc)
	{
		int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, nullptr, 0, nullptr, nullptr);
		if (nLen <= 0) return std::string("");
		char* pszDst = new char[nLen];
		if (nullptr == pszDst) return std::string("");
		WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, nullptr, nullptr);
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
		if (nullptr == pwszDst) return nullptr;
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

		static D3D11_PRIMITIVE_TOPOLOGY PrimitiveTopology(RenderOperator::EPrimitiveType priType);
	};




	D3D11_PRIMITIVE_TOPOLOGY D3D11Mapping::PrimitiveTopology(RenderOperator::EPrimitiveType priType)
	{
		switch (priType)
		{
		case Aurora::RenderOperator::POINT_LIST:
			return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
		case Aurora::RenderOperator::PT_LINELIST:
			return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
		case Aurora::RenderOperator::PT_LINESTRIP:
			return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
		case Aurora::RenderOperator::PT_TRIANGLELIST:
			return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		case Aurora::RenderOperator::PT_TRIANGLESTRIP:
			return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		default:
			assert(0);
			return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		}
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
		virtual ~RendererDx11(void);


		GPUShaderObject* CreateShader(const ShaderCode& code) override;

		Texture* CreateTexture(File* file) override;

		Texture* CreateTexture(Texture::Type type, const Texture::Desc& desc) override;

		RenderTarget* CreateRenderTarget(const RenderTarget::Desc& desc) override;


		void Clear(int clearMask, const Color& clearColor = Color::BLUE, float fDepth = 1.0f, uint8 nStencil = 0) override;

		void Present() override;

		RenderTarget* GetFrameBuffer() override;
		RenderTarget* GetDepthStencil() override;

		virtual void SetVertexDesc(VertexLayout* pVertDesc);

		void SetVertexShader(Shader* pShader) override;
		void SetPixelShader(Shader* pShader) override;


		void SetRenderTarget(uint idx, RenderTarget* pRenderTarget) override;
		void SetDepthStencil(RenderTarget* pDepthStencil) override;

		void SetRenderTarget(uint nRTs, RenderTarget** pRenderTargets, RenderTarget* pDepthStencil) override;

		void ExecuteOperator(const RenderOperator& op) override;

		void GetFrameBufferSize(uint& nWidth, uint& nHeight) override;


		Handle CreateShaderParameterBinding(GPUShaderObject* shaderHandle, const ShaderParamterBindings& bindings) override;

		void UpdateShaderParameter(Handle bindingHandle) override;

		void BindVertexShader(GPUShaderObject* shaderHandle) override;
		void BindPixelShader(GPUShaderObject* shaderHandle) override;

		Handle CreateShaderTextureBinding(GPUShaderObject* shaderHandle, const ShaderTextureBinding& bindings) override;
		void BindTexture(Handle binding, Texture* texture) override;

		void BindGlobalParameter(Handle handle) override;

		Handle CreateVertexLayoutHandle(const vector<VertexLayoutItem>& layoutItems) override;


		Handle CreateVertexBufferHandle(const void* data, int32 size) override;
		Handle CreateIndexBufferHandle(const void* data, int32 size) override;

		CGPUGeometryBuffer* CreateGeometryBuffer(const CGeometry* InGeometry) override;
	};




	extern HWND	GMainHWnd;

	static RendererDx11			GRendererDx11;
	IRenderDevice*				GRenderDevice = nullptr;

	ID3D11Device* D3D11Device = nullptr;
	ID3D11Device3* D3D11Device3 = nullptr;

	ID3D11DeviceContext* ImmediateContext = nullptr;
	static IDXGISwapChain* SwapChain = nullptr;


	static ID3D11RenderTargetView* RenderTargetView = nullptr;
	static ID3D11Texture2D* DepthStencil = nullptr;
	static ID3D11DepthStencilView* DepthStencilView = nullptr;

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
	

	
	class GPUShaderObjectDX11 : public GPUShaderObject
	{
	public:
		GPUShaderObjectDX11() = default;
		
		string Name;
		union
		{
			ID3D11VertexShader* VertexShader = nullptr;
			ID3D11PixelShader* PixelShader;			
		};
		ID3D11ShaderReflection* Reflector = nullptr;
		vector<int32>	constBuffers;
		
	};

	class CGPUGeometryBufferDX11 : public CGPUGeometryBuffer
	{
	public:

		Handle mVertexBufferHandle = 0;
		Handle mIndexBufferHandle = 0;		
	};
	

	CGPUGeometryBufferDX11* AsDX11Type(CGPUGeometryBuffer* InBase)
	{
		return static_cast<CGPUGeometryBufferDX11*>(InBase);
	}

	GPUShaderObjectDX11* AsDX11Type(GPUShaderObject* InBase)
	{
		return static_cast<GPUShaderObjectDX11*>(InBase);
	}
	

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
			D3D_FEATURE_LEVEL_11_1,
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
			hr = D3D11CreateDeviceAndSwapChain(nullptr, driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
				D3D11_SDK_VERSION, &sd, &SwapChain, &D3D11Device, &featureLevel, &ImmediateContext);
			if (SUCCEEDED(hr))
				break;
		}

		if (FAILED(hr)) {
			GLog->Error("D3D11CreateDeviceAndSwapChain failed! hr = %d", hr);
			return false;
		}
		
		const char* featureLevelsName[] = {
			"11.1",
			"11.0",
			"10.1",
			"10.0",
			"unknow"
		};

		auto featureItem = find(featureLevels, featureLevels + numFeatureLevels, featureLevel);
		int featureIndex = distance(featureLevels, featureItem);
		GLog->Info("D3D11 device created with feature level %s", featureLevelsName[featureIndex]);


		hr = D3D11Device->QueryInterface(__uuidof (ID3D11Device3), (void**)&D3D11Device3);
		if (FAILED(hr) || D3D11Device3 == nullptr) {
			GLog->Error("Query ID3D11Device3 interface failed!");
		}

		// Create a render target view
		ID3D11Texture2D* pBackBuffer = nullptr;
		hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		if (FAILED(hr)) {
			GLog->Error("SwapChain->GetBuffer failed! hr = %d", hr);
			SwapChain->Release();
			ImmediateContext->Release();
			return false;
		}

		hr = D3D11Device->CreateRenderTargetView(pBackBuffer, nullptr, &RenderTargetView);
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
		hr = D3D11Device->CreateTexture2D(&descDepth, nullptr, &DepthStencil);
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

	
	bool InitializeOpenGLDevice(HWND hwnd, int InWidth, int InHeight);

	
	IRenderDevice* IRenderDevice::CreateDevice(int nWidth, int nHeight)
	{

		
		if (!CreateDX11Device(GMainHWnd)) {
			return nullptr;
		}

		GRenderDevice = &GRendererDx11;

		return GRenderDevice;
	}

	bool IRenderDevice::Initialized()
	{
		return  GRenderDevice != nullptr;
	}


	RendererDx11::RendererDx11()
	{
	}

	RendererDx11::~RendererDx11(void)
	{
	}


	GPUShaderObject* RendererDx11::CreateShader(const ShaderCode& code)
	{
		vector<D3D_SHADER_MACRO> macros;
		for (auto it = code.defines.begin(); it != code.defines.end(); ++it) {
			D3D_SHADER_MACRO m = { it->first.c_str(), it->second.c_str() };
			macros.push_back(m);
		}
		D3D_SHADER_MACRO m = { nullptr, nullptr };
		macros.push_back(m);


		ID3DBlob* pCompiledShader = nullptr;
		ID3DBlob* pError = nullptr;

		UINT flag = 0;
#ifdef _DEBUG
		flag |= D3DCOMPILE_DEBUG;
#endif

		char* target = "vs_4_0";
		if (code.type == BaseShader::PIXEL_SHADER) {
			target = "ps_4_0";
		}
		else if (code.type == BaseShader::GEOM_SHADER) {

		}
		else if (code.type == BaseShader::COMPUTE_SHADER) {

		}

		MyInclude includeObject;
		includeObject.path_ = Util::GetPath(code.name);

		//HRESULT hr = D3DCompile(code.text.c_str(), code.text.length(), code.name.c_str(),
		//	&macros[0], D3D_COMPILE_STANDARD_FILE_INCLUDE, "Main",
		//	target, flag, 0, &pCompiledShader, &pError);

		HRESULT hr = D3DCompile(code.text.c_str(), code.text.length(), code.name.c_str(),
			&macros[0], &includeObject, "Main",
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

			return nullptr;
		}

		auto obj = new GPUShaderObjectDX11();
		obj->Name = code.name;
		if (code.type == BaseShader::VERTEX_SHADER) {

			ID3D11VertexShader* pVertexShader = nullptr;
			hr = D3D11Device->CreateVertexShader(pCompiledShader->GetBufferPointer(),
				pCompiledShader->GetBufferSize(), nullptr, &pVertexShader);
			assert(SUCCEEDED(hr));
			obj->VertexShader = pVertexShader;
		}
		else if (code.type == BaseShader::PIXEL_SHADER) {
			ID3D11PixelShader* pixelShader = nullptr;
			hr = D3D11Device->CreatePixelShader(pCompiledShader->GetBufferPointer(),
				pCompiledShader->GetBufferSize(), nullptr, &pixelShader);
			assert(SUCCEEDED(hr));
			obj->PixelShader = pixelShader;
		}

		ID3D11ShaderReflection* pReflector = nullptr;
		D3DReflect(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(),
			IID_ID3D11ShaderReflection, (void**)&pReflector);
		pCompiledShader->Release();
		pCompiledShader = nullptr;


		obj->Reflector = pReflector;

		return obj;
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



	string GlobalParameterDef = "\
												 \
		cbuffer GlobalParameter: register(b0)	 \
		{										 \
			float4x4 matView;					 \
			float4x4 matProj;					 \
			float4x4 matViewProj;				 \
			float3	DirectionLight0;			 \
			float4	LightColor0;				 \
		};										 \
";
	

	void ReplaceShaderInclude(unique_ptr<int8[]>& data, int32* size)
	{
		*size = GlobalParameterDef.size();
		data.reset(new int8[*size]);
		memcpy_s(data.get(), *size, GlobalParameterDef.c_str(), *size);
	}


	static vector<ShaderParameterBuffer*>  ShaderParameterBuffers_;

	Handle RendererDx11::CreateShaderParameterBinding(GPUShaderObject* shaderHandle, const ShaderParamterBindings& bindings)
	{
		GPUShaderObjectDX11* shader = AsDX11Type(shaderHandle);

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


	Handle RendererDx11::CreateShaderTextureBinding(GPUShaderObject* shaderHandle, const ShaderTextureBinding& bindings)
	{
		auto shader = AsDX11Type(shaderHandle);
		ID3D11ShaderReflection* Reflector = shader->Reflector;
				
		D3D11_SHADER_DESC Desc;
		Reflector->GetDesc(&Desc);

		GLog->Info("shader %d InstructionCount %d BoundResources %d", Desc.Creator, Desc.InstructionCount, Desc.BoundResources);

		// for(int Index = 0; Index < Desc.BoundResources; Index++)
		// {
		// 	
		// 	D3D11_SHADER_INPUT_BIND_DESC bindDesc;
		// 	HRESULT hr = Reflector->GetResourceBindingDesc(Index, &bindDesc);
		// 	GLog->Info("bound resource %d %s", Index, bindDesc.Name);
		// }
		
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


	void RendererDx11::UpdateShaderParameter(Handle bindingHandle)
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


	void RendererDx11::BindGlobalParameter(Handle handle)
	{
		auto binding = ShaderParameterBuffers_[handle];
		ImmediateContext->VSSetConstantBuffers(binding->Slot, 1, &binding->D3DBuffer);
		ImmediateContext->PSSetConstantBuffers(binding->Slot, 1, &binding->D3DBuffer);
	}

	void RendererDx11::BindVertexShader(GPUShaderObject* shaderHandle)
	{
		assert(shaderHandle != nullptr);
		auto shader = AsDX11Type(shaderHandle);
		assert(shader->VertexShader != nullptr);

		ImmediateContext->VSSetShader(shader->VertexShader, nullptr, 0);

		for (int i = 0; i < shader->constBuffers.size(); i++) {
			BindVertexShaderParameter(shader->constBuffers[i]);
		}
	}

	void RendererDx11::BindPixelShader(GPUShaderObject* shaderHandle)
	{
		assert(shaderHandle != nullptr);
		auto shader = AsDX11Type(shaderHandle);
		assert(shader->PixelShader != nullptr);

		ImmediateContext->PSSetShader(shader->PixelShader, nullptr, 0);

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

			hr = D3D11Device->CreateShaderResourceView(d3dTexture, nullptr, &pResourceView);
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




	void RendererDx11::BindTexture(Handle binding, Texture* texture)
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
			HRESULT hr = D3D11Device->CreateTexture2D(&d3dDesc, nullptr, &pTexture->m_pD3DTexture);
			assert(SUCCEEDED(hr));

			D3D11_SHADER_RESOURCE_VIEW_DESC resViewDesc;
			resViewDesc.Format = d3dDesc.Format;
			resViewDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
			resViewDesc.Texture2D.MipLevels = d3dDesc.MipLevels;
			resViewDesc.Texture2D.MostDetailedMip = 0;

			ID3D11ShaderResourceView* pD3DResView = nullptr;
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
			HRESULT hr = D3D11Device->CreateTexture3D(&d3dDesc, nullptr, &pTexture->m_pD3DTexture);
			assert(SUCCEEDED(hr));

			D3D11_SHADER_RESOURCE_VIEW_DESC resViewDesc;
			resViewDesc.Format = d3dDesc.Format;
			resViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
			resViewDesc.Texture3D.MipLevels = d3dDesc.MipLevels;
			resViewDesc.Texture3D.MostDetailedMip = 0;

			ID3D11ShaderResourceView* pD3DResView = nullptr;
			hr = D3D11Device->CreateShaderResourceView(pTexture->m_pD3DTexture, &resViewDesc, &pD3DResView);
			assert(SUCCEEDED(hr));
			pTexture->HALHandle = pD3DResView;

			return pTexture;
		}*/

		assert(0);
		return nullptr;
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

		ID3D11Texture2D* pD3DTex = nullptr;
		hr = D3D11Device->CreateTexture2D( &rtDesc, nullptr, &pD3DTex);
		if (FAILED(hr))
		{
			GLog->Error("CreateRenderTarget failed!");
			return nullptr;
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
		ID3D11ShaderResourceView* pShaderResourceView = nullptr;

		hr = D3D11Device->CreateShaderResourceView(pD3DTex, &DescRV, &pShaderResourceView);
		if (FAILED(hr))
		{
			GLog->Error("CreateRenderTarget failed!");
			return nullptr;
		}

		refCnt = pD3DTex->AddRef();
		refCnt = pD3DTex->Release();

		pRT->HALHandle = pShaderResourceView;


		Dx11TextureBind* pBind = new Dx11TextureBind;
		pBind->pTexture = pD3DTex;

		if (desc.nUsage == Surface::USAGE_RENDERTARGET)
		{
			// Create the render target view
			ID3D11RenderTargetView* pRTView = nullptr;
			D3D11_RENDER_TARGET_VIEW_DESC DescRT;
			DescRT.Format = fmt;
			DescRT.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			DescRT.Texture2D.MipSlice = 0;
			hr = D3D11Device->CreateRenderTargetView(pD3DTex, &DescRT, &pRTView);

			if (FAILED(hr))
			{
				GLog->Error("CreateRenderTarget failed!");
				return nullptr;
			}

			pBind->pRTView = pRTView;
		}
		else if (desc.nUsage == Surface::USAGE_DEPTHSTENCIL)
		{
			if (desc.nFormat == Surface::FMT_D16)
			{
				fmt = DXGI_FORMAT_D16_UNORM;
			}
			ID3D11DepthStencilView* pDSView = nullptr;
			D3D11_DEPTH_STENCIL_VIEW_DESC DescRT;
			DescRT.Format = fmt;
			DescRT.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			DescRT.Texture2D.MipSlice = 0;
			hr = D3D11Device->CreateDepthStencilView(pD3DTex, &DescRT, &pDSView);

			if (FAILED(hr))
			{
				GLog->Error("CreateRenderTarget failed!");
				return nullptr;
			}

			pBind->pDSView = pDSView;
		}

		refCnt = pD3DTex->AddRef();
		refCnt = pD3DTex->Release();

		pRT->HALHandleRT = pBind;

		return pRT;
	}

	

	struct GeometryBufferInfo
	{
		int32	Occupied = 0;
		int32	Size = 0;
		D3D11_USAGE Usage;
		UINT		BindFlags;

		ID3D11Buffer* d3dBuffer = nullptr;
	};


	vector<GeometryBufferInfo>		GeometryBufferInfos_;


	Handle RendererDx11::CreateVertexBufferHandle(const void* data, int32 size)
	{
		D3D11_BUFFER_DESC bufferDesc = { 0 };
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.MiscFlags = 0;
		bufferDesc.ByteWidth = size;

		D3D11_SUBRESOURCE_DATA initData;
		D3D11_SUBRESOURCE_DATA* initDataPtr = nullptr;

		if (data == nullptr) {
			// dynamic buffer
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		} else {
			bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			bufferDesc.CPUAccessFlags = 0;

			initData.SysMemPitch = 0;
			initData.SysMemSlicePitch = 0;
			initData.pSysMem = data;

			initDataPtr = &initData;
		}

		ID3D11Buffer* d3dBuffer = nullptr;

		HRESULT hr = D3D11Device->CreateBuffer(&bufferDesc, initDataPtr, &d3dBuffer);
		if (FAILED(hr)) {
			GLog->Error("Create Vertex buffer failed hr = %#X", hr);
			return -1;
		}

		GeometryBufferInfo info = { 0 };
		info.Size = size;
		info.BindFlags = bufferDesc.BindFlags;
		info.Usage = bufferDesc.Usage;
		info.d3dBuffer = d3dBuffer;
		info.Occupied = 1;

		Handle slot = FindAvailableSlot(GeometryBufferInfos_);
		GeometryBufferInfos_[slot] = info;

		return slot;
	}


	Handle RendererDx11::CreateIndexBufferHandle(const void* data, int32 size)
	{
		D3D11_BUFFER_DESC bufferDesc = { 0 };
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.MiscFlags = 0;
		bufferDesc.ByteWidth = size;

		D3D11_SUBRESOURCE_DATA initData;
		D3D11_SUBRESOURCE_DATA* initDataPtr = nullptr;

		if (data == nullptr) {
			// dynamic buffer
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		}
		else {
			bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			bufferDesc.CPUAccessFlags = 0;

			initData.SysMemPitch = 0;
			initData.SysMemSlicePitch = 0;
			initData.pSysMem = data;

			initDataPtr = &initData;
		}

		ID3D11Buffer* d3dBuffer = nullptr;

		HRESULT hr = D3D11Device->CreateBuffer(&bufferDesc, initDataPtr, &d3dBuffer);
		if (FAILED(hr)) {
			GLog->Error("Create index buffer failed hr = %#X", hr);
			return -1;
		}

		GeometryBufferInfo info = { 0 };
		info.Size = size;
		info.BindFlags = bufferDesc.BindFlags;
		info.Usage = bufferDesc.Usage;
		info.d3dBuffer = d3dBuffer;
		info.Occupied = 1;

		Handle slot = FindAvailableSlot(GeometryBufferInfos_);
		GeometryBufferInfos_[slot] = info;

		return slot;
	}
	
	
	CGPUGeometryBuffer* RendererDx11::CreateGeometryBuffer(const CGeometry* InGeometry)
	{
		Handle VertexBufferHandle = 0;
		Handle IndexBufferHandle = 0;
		vector<int8> vertexData;
		InGeometry->PrepareVertexData(vertexData, CGeometry::VertexLayoutPosNormTangentTex);
		VertexBufferHandle = CreateVertexBufferHandle(vertexData.data(), vertexData.size());
		 
		vector<int8> IndexData;
		InGeometry->PrepareIndexData(IndexData);
		if(!IndexData.empty())
		{
			IndexBufferHandle = CreateIndexBufferHandle(IndexData.data(), IndexData.size());
		}

		auto GPUBuffer = new CGPUGeometryBufferDX11();
		GPUBuffer->mVertexBufferHandle = VertexBufferHandle;
		GPUBuffer->mIndexBufferHandle = IndexBufferHandle;

		return  GPUBuffer;
	}




	ResourceBufferMapper::ResourceBufferMapper(Handle handle)
	{
		handle_ = handle;
		auto buffer = GeometryBufferInfos_[handle_];

		if (buffer.Usage != D3D11_USAGE_DYNAMIC) {
			GLog->Error("try to map non dynamic buffer!");
		}
		else {
			D3D11_MAPPED_SUBRESOURCE resource = { 0 };
			HRESULT hr = ImmediateContext->Map(buffer.d3dBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
			if (FAILED(hr)) {
				GLog->Error("map buffer resource  failed! hr = %#X", hr);
			}
			else {
				pointer = resource.pData;
			}
		}

	}

	ResourceBufferMapper::~ResourceBufferMapper()
	{
		if (pointer != nullptr) {
			auto buffer = GeometryBufferInfos_[handle_];
			ImmediateContext->Unmap(buffer.d3dBuffer, 0);
		}
	}

	void* ResourceBufferMapper::Ptr()
	{
		return pointer;
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
	
	void RendererDx11::Present()
	{
		SwapChain->Present(0, 0);
	}

	RenderTarget* RendererDx11::GetFrameBuffer()
	{
		return nullptr;
	}

	RenderTarget* RendererDx11::GetDepthStencil()
	{
		return nullptr;
	}

	void RendererDx11::SetVertexDesc(VertexLayout* pVertDesc)
	{
		ID3D11InputLayout* layout = (ID3D11InputLayout*)pVertDesc->HALHandle;
		ImmediateContext->IASetInputLayout(layout);

	}

	void RendererDx11::SetVertexShader(Shader* pShader)
	{
		ImmediateContext->VSSetShader((ID3D11VertexShader*)pShader->HALHandle, nullptr, 0);
	}

	void RendererDx11::SetPixelShader(Shader* pShader)
	{
		ImmediateContext->PSSetShader((ID3D11PixelShader*)pShader->HALHandle, nullptr, 0);
	}


	void RendererDx11::SetRenderTarget(uint idx, RenderTarget* pRenderTarget)
	{

	}

	void RendererDx11::SetDepthStencil(RenderTarget* pDepthStencil)
	{

	}

	void RendererDx11::SetRenderTarget(uint nRTs, RenderTarget** pRenderTargets, RenderTarget* pDepthStencil)
	{
		ID3D11RenderTargetView * pRTs[] = {nullptr, nullptr, nullptr, nullptr};

		for (uint i = 0; i < nRTs; i++)
		{
			if (pRenderTargets[i])
			{
				auto* pBind = static_cast<Dx11TextureBind*>(pRenderTargets[i]->HALHandleRT);
				pRTs[i] = pBind->pRTView;
			}
		}

		ID3D11DepthStencilView* pDSView = nullptr;
		if (pDepthStencil)
		{
			auto* pDSBind = static_cast<Dx11TextureBind*>(pDepthStencil->HALHandleRT);
			pDSView = pDSBind->pDSView;
		}

		ImmediateContext->OMSetRenderTargets(nRTs, pRTs, pDSView);
		GDeviceState.rts[0] = pRTs[0];
		GDeviceState.rts[1] = pRTs[1];
		GDeviceState.rts[2] = pRTs[2];
		GDeviceState.rts[3] = pRTs[3];
		GDeviceState.ds = pDSView;

	}

	void RendererDx11::GetFrameBufferSize(uint& nWidth, uint& nHeight)
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

		ID3D10Blob* pShader = nullptr;
		ID3D10Blob* pError = nullptr;


		HRESULT hr = S_FALSE;

		hr = D3DCompile(buffer, strlen(buffer), "CreateShaderSignature shader", nullptr, nullptr, "Main",
			"vs_4_0", 0, 0, &pShader, &pError);
		
		if (FAILED(hr)) {
			int code = HRESULT_CODE(hr);
			if (pError) {
				string errorMsg = static_cast<const char*>(pError->GetBufferPointer());
				pError->Release();
				GLog->Error("CreateShaderSignature error : %s", errorMsg.c_str());
			}
			return nullptr;
		}

		ID3DBlob* pInputSig = nullptr;
		hr = D3DGetInputSignatureBlob(pShader->GetBufferPointer(), pShader->GetBufferSize(), &pInputSig);
		assert(SUCCEEDED(hr));

		hr = pShader->Release();
		assert(SUCCEEDED(hr));

		return pInputSig;
	}




	const char* MapDX11SemanticName(Vertex::ElemUsage usage) {

		switch (usage)
		{
		case Aurora::Vertex::USAGE_POSITION:
			return "SV_Position";
		case Aurora::Vertex::USAGE_NORMAL:
			return "NORMAL";
		case Aurora::Vertex::USAGE_COLOR:
			return "COLOR";
		case Aurora::Vertex::USAGE_TEXCOORD:
			return "TEXCOORD";
		case Aurora::Vertex::USAGE_TANGENT:
			return "TANGENT";
		case Aurora::Vertex::USAGE_BINORMAL:
			return "BINORMAL";
		case Aurora::Vertex::USAGE_BLENDINDEX:
			return "BLENDINDICES";
		case Aurora::Vertex::USAGE_BLENDWEIGHT:
			return "BLENDWEIGHT";
		case Aurora::Vertex::USAGE_INDEX:
			assert(0);
			return "";
		default:
			assert(0);
			return "";
		}
	}


	DXGI_FORMAT MapDX11VertexElemType(Vertex::ElemType type)
	{
		switch (type)
		{
		case Aurora::Vertex::TYPE_FLOAT:
			return DXGI_FORMAT_R32_FLOAT;

		case Aurora::Vertex::TYPE_FLOAT2:
			return DXGI_FORMAT_R32G32_FLOAT;

		case Aurora::Vertex::TYPE_FLOAT3:
			return DXGI_FORMAT_R32G32B32_FLOAT;

		case Aurora::Vertex::TYPE_FLOAT4:
			return DXGI_FORMAT_R32G32B32A32_FLOAT;

		case Aurora::Vertex::TYPE_UBYTE4_UINT:
			return DXGI_FORMAT_R8G8B8A8_UINT;

		case Aurora::Vertex::TYPE_UBYTE4_UNORM:
			return DXGI_FORMAT_R8G8B8A8_UNORM;

		case Aurora::Vertex::TYPE_USHORT_UINT:
			return DXGI_FORMAT_R16_UINT;

		case Aurora::Vertex::TYPE_UINT:
			return DXGI_FORMAT_R32_UINT;
		default:
			assert(0);
			return DXGI_FORMAT_R32G32B32_FLOAT;
		}
	}


	const char* MapVertexElementDeclaration(Vertex::ElemUsage usage)
	{
		switch (usage)
		{
		case Aurora::Vertex::USAGE_POSITION:
			return "float4 Position";

		case Aurora::Vertex::USAGE_NORMAL:
			return "float3 Normal";

		case Aurora::Vertex::USAGE_COLOR:
			return "float4 Color";

		case Aurora::Vertex::USAGE_TEXCOORD:
			return "float2 Texcoord";

		case Aurora::Vertex::USAGE_TANGENT:
			return "float4 Tangent";

		case Aurora::Vertex::USAGE_BINORMAL:
			return "float3 Binormal";

		case Aurora::Vertex::USAGE_BLENDINDEX:
			return "float4 BlendIndex";

		case Aurora::Vertex::USAGE_BLENDWEIGHT:
			return "float4 BlendWeight";

		case Aurora::Vertex::USAGE_INDEX:
			assert(0);
			return "";

		default:
			assert(0);
			return "";
		}

	}



	static ID3DBlob* CreateSignatureWithVertexLayout(const vector<VertexLayoutItem>& layoutItems)
	{

		string vsInputCode = "struct GeomVertexInput {\n";
		char line[256] = { 0 };

		for (auto item = layoutItems.begin(); item != layoutItems.end(); ++item) {

			const char* decl = MapVertexElementDeclaration((Vertex::ElemUsage)item->usage);
			const char* semanticName = MapDX11SemanticName((Vertex::ElemUsage)item->usage);
			
			char buff[8];
			auto indexName = (item->usageIndex > 0) ? itoa(item->usageIndex, buff, 10) : "";
			sprintf(line, "%s%s : %s%s;\n", decl, indexName, semanticName, indexName);
			vsInputCode += line;
		}

		vsInputCode += "};\n";

		string signatureCode = vsInputCode;
			   
		signatureCode += "\
			float4 Main(GeomVertexInput vsin) : SV_POSITION	 \
			{										 \
			return float4(0, 0, 0, 0);				 \
			};										 \
		";


		ID3D10Blob* pShader = nullptr;
		ID3D10Blob* pError = nullptr;

		HRESULT hr = S_FALSE;
		hr = D3DCompile(signatureCode.c_str(), signatureCode.size(), "CreateShaderSignature shader", 
						nullptr, nullptr, "Main", "vs_4_0", 0, 0, &pShader, &pError);

		if (FAILED(hr)) {
			int code = HRESULT_CODE(hr);
			if (pError) {
				string errorMsg = (const char*)pError->GetBufferPointer();
				GLog->Error("CreateShaderSignature error : %s", errorMsg);
			}			
			pError->Release();

			return nullptr;
		}

		ID3DBlob* pInputSig = nullptr;
		hr = D3DGetInputSignatureBlob(pShader->GetBufferPointer(), pShader->GetBufferSize(), &pInputSig);
		assert(SUCCEEDED(hr));

		hr = pShader->Release();
		assert(SUCCEEDED(hr));

		return pInputSig;
	}
	   


	struct VertexLayoutInfo
	{
		string	VertexInputCode;
		ID3D11InputLayout* d3d11InputLayout = nullptr;
	};



	static vector<VertexLayoutInfo*>  VertexLayouts_;


	Handle RendererDx11::CreateVertexLayoutHandle(const vector<VertexLayoutItem>& layoutItems)
	{

		vector<D3D11_INPUT_ELEMENT_DESC> elemDesc;
		for (auto item = layoutItems.begin(); item != layoutItems.end(); ++item) {
			D3D11_INPUT_ELEMENT_DESC desc = {0};

			desc.SemanticName = MapDX11SemanticName((Vertex::ElemUsage)item->usage);
			desc.SemanticIndex	= item->usageIndex;
			desc.Format			= MapDX11VertexElemType((Vertex::ElemType)item->type);
			desc.InputSlot		= 0;
			desc.AlignedByteOffset	= D3D11_APPEND_ALIGNED_ELEMENT;
			desc.InputSlotClass		= D3D11_INPUT_PER_VERTEX_DATA;
			desc.InstanceDataStepRate = 0;

			elemDesc.push_back(desc);
		}		

		ID3D10Blob* pInputSig = CreateSignatureWithVertexLayout(layoutItems);
		ID3D11InputLayout* d3dLayout = nullptr;		

		HRESULT hr = D3D11Device->CreateInputLayout(&elemDesc[0], elemDesc.size(),
			pInputSig->GetBufferPointer(), pInputSig->GetBufferSize(), &d3dLayout);
			   		 
		assert(SUCCEEDED(hr));
		pInputSig->Release();

		auto info = new VertexLayoutInfo();
		info->d3d11InputLayout = d3dLayout;

		Handle slot = FindAvailableSlot(VertexLayouts_);
		VertexLayouts_[slot] = info;

		return slot;
	}


	void RendererDx11::ExecuteOperator(const RenderOperator& op)
	{
		ImmediateContext->IASetPrimitiveTopology(D3D11Mapping::PrimitiveTopology(op.PrimType));
		
		auto layout = VertexLayouts_[op.VertexLayout];
		CGPUGeometryBufferDX11* GPUBuffer = AsDX11Type(op.GeometryBuffer);
		auto vertex = GeometryBufferInfos_[GPUBuffer->mVertexBufferHandle];
		auto index = GeometryBufferInfos_[GPUBuffer->mIndexBufferHandle];

		ImmediateContext->IASetIndexBuffer(index.d3dBuffer, DXGI_FORMAT_R32_UINT, 0);

		UINT strides[] = { op.VertexStride };
		UINT offsets[] = { 0 };

		ImmediateContext->IASetVertexBuffers(0, 1, &vertex.d3dBuffer, strides, offsets);
		ImmediateContext->IASetInputLayout(layout->d3d11InputLayout);

		ImmediateContext->DrawIndexed(op.IndexCount, op.StartIndex, op.BaseVertexIndex);
	}


}