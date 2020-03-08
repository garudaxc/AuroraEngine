#include "stdHeader.h"
#include "GaRendererDX9.h"
#include "GaConsole.h"
#include "GaEngine.h"
#include "GaMaterial.h"
#include "GaShader.h"
#include "GaLog.h"
#include "GaRendererObject.h"
#include "GaShader.h"
#include "GaUtil.h"
#include "GaRendererObject.h"
#include "GaPipeline.h"


namespace Ga
{
	// for use in impl
	static IDirect3DDevice9* s_pd3dDevice9 = NULL;


	static D3DFORMAT g_d3dFormat[] = 
	{
		D3DFMT_R8G8B8,
		D3DFMT_A8R8G8B8,
		D3DFMT_X8R8G8B8,
		D3DFMT_G16R16,
		D3DFMT_A2R10G10B10,
		D3DFMT_D16_LOCKABLE,
		D3DFMT_D32,
		D3DFMT_D24S8,
		D3DFMT_D24X8,
		D3DFMT_D16,
		D3DFMT_D32F_LOCKABLE,
		D3DFMT_D24FS8,
		D3DFMT_R16F,
		D3DFMT_G16R16F,
		D3DFMT_R32F,
		D3DFMT_G32R32F,
		D3DFMT_A16B16G16R16F,
		D3DFMT_A32B32G32R32F,
		D3DFMT_DXT1,
		D3DFMT_DXT2,
		D3DFMT_DXT3,
		D3DFMT_DXT4,
		D3DFMT_DXT5,
		(D3DFORMAT)MAKEFOURCC('N', 'U', 'L', 'L'),
		(D3DFORMAT)MAKEFOURCC('I', 'N', 'T', 'Z'),
	};

	



	Surface::Format MapFromatFromD3D(D3DFORMAT fmt)
	{
		int count = sizeof(g_d3dFormat) / sizeof(g_d3dFormat[0]);
		for (int i = 0; i < count; i++)
		{
			if (fmt == g_d3dFormat[i])
			{
				return (Surface::Format)i;
			}
		}

		assert(0);
		return (Surface::Format)0;
	}

	

	class D3D9Mapping
	{
	public:

		static D3DCULL			CullMode(RenderStateObject::CullMode mode);
		static D3DCMPFUNC		CmpFunc(RenderStateObject::CompareFunc fuc);
		static D3DFILLMODE		FillMode(RenderStateObject::FillMode mode);
		static D3DBLEND			BlendFactor(RenderStateObject::BlendFactor factor);
		static D3DBLENDOP		BlendOp(RenderStateObject::BlendOp op);
		static D3DSTENCILOP		StencilOp(RenderStateObject::StencilOp op);
	};


	D3DCULL D3D9Mapping::CullMode(RenderStateObject::CullMode mode)
	{
		static D3DCULL table[] = {D3DCULL_CCW, D3DCULL_CW, D3DCULL_NONE};
		return table[mode];
	}

	D3DCMPFUNC D3D9Mapping::CmpFunc(RenderStateObject::CompareFunc fuc)
	{
		static D3DCMPFUNC table[] = {D3DCMP_NEVER, D3DCMP_EQUAL, D3DCMP_NOTEQUAL, 
										D3DCMP_LESS, D3DCMP_LESSEQUAL, D3DCMP_GREATER,
										D3DCMP_GREATEREQUAL, D3DCMP_ALWAYS};
		return table[fuc];
	}

	D3DFILLMODE D3D9Mapping::FillMode(RenderStateObject::FillMode mode)
	{
		static D3DFILLMODE table[] = {D3DFILL_WIREFRAME, D3DFILL_SOLID};
		return table[mode];
	}

	D3DBLEND D3D9Mapping::BlendFactor(RenderStateObject::BlendFactor factor)
	{
		
		static D3DBLEND table[] = {D3DBLEND_ZERO, D3DBLEND_ONE, D3DBLEND_SRCCOLOR, 
			D3DBLEND_INVSRCCOLOR,D3DBLEND_DESTCOLOR, D3DBLEND_INVDESTCOLOR, 
			D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA, D3DBLEND_DESTALPHA,
		D3DBLEND_INVDESTALPHA, D3DBLEND_BLENDFACTOR, D3DBLEND_INVBLENDFACTOR};

		return table[factor];
	}

	D3DBLENDOP D3D9Mapping::BlendOp(RenderStateObject::BlendOp op)
	{
		static D3DBLENDOP table[] = {D3DBLENDOP_ADD, D3DBLENDOP_SUBTRACT, 
		D3DBLENDOP_REVSUBTRACT, D3DBLENDOP_MIN, D3DBLENDOP_MAX};

		return table[op];
	}

	D3DSTENCILOP D3D9Mapping::StencilOp(RenderStateObject::StencilOp op)
	{
		
		static D3DSTENCILOP table[] = {D3DSTENCILOP_KEEP, D3DSTENCILOP_ZERO, 
		D3DSTENCILOP_REPLACE, D3DSTENCILOP_INCRSAT, D3DSTENCILOP_DECRSAT, 
		D3DSTENCILOP_INVERT, D3DSTENCILOP_INCR, D3DSTENCILOP_DECR};

		return table[op];
	}






D3DCOLOR ToD3DColor(const Color& c)
{
	return D3DCOLOR_ARGB((int)(c.a * 255), (int)(c.r * 255), (int)(c.g * 255), (int)(c.b * 255));
}




//////////////////////////////////////////////////////////////////////////
static IDirect3DDevice9*					m_pd3dDevice9;

static LPDIRECT3D9							m_pDirect3D9;
static D3DCAPS9								m_d3dCaps;

static HWND									m_hWnd;			//window handle
static uint									m_nWidth;		//back buffer size
static uint									m_nHeight;		//back buffer size

static IDirect3DSurface9*					m_pFrameBuffer;
static IDirect3DSurface9*					m_pDepthStencil;

static IDirect3DSwapChain9*					m_pSwapChain;


//////////////////////////////////////////////////////////////////////////




#ifdef DIRECTX_VERSION_DX9

class VertexShaderD3D9 : public Shader
{
public:
	VertexShaderD3D9(IDirect3DVertexShader9*	pShader);
	~VertexShaderD3D9();
	virtual void*			GetBindID() const;

private:
	IDirect3DVertexShader9*		m_pShader;


};


VertexShaderD3D9::VertexShaderD3D9(IDirect3DVertexShader9*	pShader):m_pShader(pShader)
{
}

VertexShaderD3D9::~VertexShaderD3D9()
{
	m_pShader->Release();
}

void* VertexShaderD3D9::GetBindID() const
{
	return (void*)m_pShader;
}



class PixelShaderD3D9 : public Shader
{
public:
	PixelShaderD3D9(IDirect3DPixelShader9*	pShader);
	~PixelShaderD3D9();

	virtual void*			GetBindID() const;

private:
	IDirect3DPixelShader9*		m_pShader;


};


PixelShaderD3D9::PixelShaderD3D9(IDirect3DPixelShader9*	pShader):m_pShader(pShader)
{
}

PixelShaderD3D9::~PixelShaderD3D9()
{
	m_pShader->Release();
}

void* PixelShaderD3D9::GetBindID() const
{
	return (void*)m_pShader;
}




//////////////////////////////////////////////////////////////////////////








class Texture2DD3D9 : public Texture2D
{
public:
	Texture2DD3D9(IDirect3DTexture9* pTexture, const Texture::Desc& desc);
	~Texture2DD3D9();

	virtual void*			GetBindID() const;
private:
	IDirect3DTexture9*		m_pTexture;
};

Texture2DD3D9::Texture2DD3D9(IDirect3DTexture9* pTexture, const Texture::Desc& desc):Texture2D(desc),m_pTexture(pTexture)
{

}
Texture2DD3D9::~Texture2DD3D9()
{
	m_pTexture->Release();
}


void* Texture2DD3D9::GetBindID() const
{
	return m_pTexture;
}

//////////////////////////////////////////////////////////////////////////

class TextureCubeD3D9 : public TextureCube
{
public:
	TextureCubeD3D9(IDirect3DCubeTexture9* pTexture, const Texture::Desc& desc);
	~TextureCubeD3D9();

	virtual void*			GetBindID() const;
private:
	IDirect3DCubeTexture9*		m_pTexture;
};

TextureCubeD3D9::TextureCubeD3D9(IDirect3DCubeTexture9* pTexture, const Texture::Desc& desc):TextureCube(desc),m_pTexture(pTexture)
{

}
TextureCubeD3D9::~TextureCubeD3D9()
{
	m_pTexture->Release();
}


void* TextureCubeD3D9::GetBindID() const
{
	return m_pTexture;
}

//////////////////////////////////////////////////////////////////////////

class RenderTargetD3D9 : public RenderTarget
{
public:
	RenderTargetD3D9(IDirect3DSurface9* pSurface, const RenderTarget::Desc& desc);
	RenderTargetD3D9(const RenderTarget::Desc& desc);
	~RenderTargetD3D9();
	virtual void*						GetBindID() const;
	virtual void						OnReset();
	virtual void						OnLost();

	virtual ResourcePtr<Texture>		AsTexture();
private:

	IDirect3DSurface9*					m_pRT;
	ResPtrHolder*						m_pTextureHolder;

};

RenderTargetD3D9::RenderTargetD3D9(const RenderTarget::Desc& desc):
RenderTarget(desc),m_pRT(NULL),m_pTextureHolder(new ResPtrHolder(NULL))
{
}


RenderTargetD3D9::RenderTargetD3D9(IDirect3DSurface9* pSurface, const RenderTarget::Desc& desc):
RenderTarget(desc),m_pRT(pSurface),m_pTextureHolder(new ResPtrHolder(NULL))
{
}


RenderTargetD3D9::~RenderTargetD3D9()
{
	assert(m_pRT == NULL);
	delete m_pTextureHolder;
	m_pTextureHolder = NULL;
}

void* RenderTargetD3D9::GetBindID() const
{
	return m_pRT;
}


ResourcePtr<Texture> RenderTargetD3D9::AsTexture()
{
	assert(m_pTextureHolder != NULL);
	return ResourcePtr<Texture>(m_pTextureHolder);
}

void RenderTargetD3D9::OnReset()
{
	D3DPOOL d3dPool = D3DPOOL_DEFAULT;

	DWORD d3dUsage = (m_Desc.nUsage == Surface::USAGE_RENDERTARGET) ? D3DUSAGE_RENDERTARGET : D3DUSAGE_DEPTHSTENCIL;

	IDirect3DTexture9* pd3dTex = NULL;

	uint width = GetRealWidth();
	uint height = GetRealHeight();

	if (SUCCEEDED(s_pd3dDevice9->CreateTexture((UINT)width, (UINT)height, 1,
		d3dUsage, g_d3dFormat[m_Desc.nFormat], d3dPool, &pd3dTex, NULL)))
	{
		pd3dTex->GetSurfaceLevel(0, &m_pRT);
		assert(m_pRT != NULL);
	}
	// create as texture failed, try surface
	else
	{
		if (m_Desc.nUsage == Surface::USAGE_RENDERTARGET)
		{
			s_pd3dDevice9->CreateRenderTarget((UINT)width, height, 
				g_d3dFormat[m_Desc.nFormat], D3DMULTISAMPLE_NONE, 0, 
				FALSE, &m_pRT, NULL);
		}
		else if (m_Desc.nUsage == Surface::USAGE_DEPTHSTENCIL)
		{
			s_pd3dDevice9->CreateDepthStencilSurface((UINT)width, height, 
				g_d3dFormat[m_Desc.nFormat], D3DMULTISAMPLE_NONE, 0,
				FALSE, &m_pRT, NULL);
		}
		assert(m_pRT != NULL);
	}

	if (!m_pRT)
	{
		char msg[255];
		::sprintf_s(msg, sizeof(msg), "Create render target failed. width = %d height = %d\n", 
			width, height);
		Console::AppendText(msg);

		if (pd3dTex)
		{
			pd3dTex->Release();
		}
		return;
	}

	Texture* pTexture = NULL;
	if (pd3dTex)
	{
		Texture::Desc texDesc;
		texDesc.nMipLevels	= 1;
		texDesc.nDepth	= 1;
		texDesc.nFormat	= m_Desc.nFormat;
		texDesc.nWidth	= width;
		texDesc.nHeight	= height;
		texDesc.nUsage	= m_Desc.nUsage;
		texDesc.nPool	= D3DPOOL_DEFAULT;

		pTexture = new Texture2DD3D9(pd3dTex, texDesc);
		m_pTextureHolder->ResetResource(pTexture);
	}

}

void RenderTargetD3D9::OnLost()
{
	assert(m_pRT != NULL);
	m_pRT->Release();
	m_pRT = NULL;
	m_pTextureHolder->ResetResource(NULL);
}




Texture* RendererDX9::CreateTexture(const string& name)
{
	Texture* pTexture = NULL;
	D3DXIMAGE_INFO imageInfo;
	Texture::Desc desc;

	// first try cube texture
	if (pTexture == NULL)
	{
		IDirect3DCubeTexture9* pCubeTex = NULL;
		if (SUCCEEDED(D3DXCreateCubeTextureFromFileExA(m_pd3dDevice9,  name.c_str(), 
			D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 
			D3DX_DEFAULT, D3DX_DEFAULT, 0, &imageInfo, NULL, &pCubeTex)))
		{
			desc.nWidth		= (uint)imageInfo.Width;
			desc.nHeight	= (uint)imageInfo.Height;
			desc.nDepth		= 0;
			desc.nFormat	= MapFromatFromD3D(imageInfo.Format);
			desc.nMipLevels	= (uint)imageInfo.MipLevels;
			desc.nPool		= D3DPOOL_MANAGED;
			desc.nUsage		= Surface::USAGE_DEFAULT;

			pTexture = new TextureCubeD3D9(pCubeTex, desc);
		}
	}

	// then try 2d texture
	if (pTexture == NULL)
	{
		IDirect3DTexture9* pd3dTex = NULL;
		if ( SUCCEEDED( D3DXCreateTextureFromFileExA(m_pd3dDevice9,
			name.c_str(), D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, D3DX_FROM_FILE, 0,
			D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, &imageInfo,
			NULL, &pd3dTex) ) )
		{
			desc.nWidth		= (uint)imageInfo.Width;
			desc.nHeight	= (uint)imageInfo.Height;
			desc.nDepth		= 0;
			desc.nFormat	= MapFromatFromD3D(imageInfo.Format);
			desc.nMipLevels	= (uint)imageInfo.MipLevels;
			desc.nPool		= D3DPOOL_MANAGED;
			desc.nUsage		= Surface::USAGE_DEFAULT;

			pTexture = new Texture2DD3D9(pd3dTex, desc);
		}
	}


	if (!pTexture)
	{
		std::string msg = "Create texture failed. Filename: " + name;
		msg += '\n';
		Console::AppendText(msg.c_str());
	}

	return pTexture;
}



Texture* RendererDX9::CreateTexture(Texture::Type type, const Texture::Desc& desc)
{
	Texture* pTexture = NULL;

	DWORD d3dUsage = desc.nUsage == Surface::USAGE_RENDERTARGET ? D3DUSAGE_RENDERTARGET : 0;
	D3DPOOL d3dPool = (desc.nUsage == Surface::USAGE_RENDERTARGET) ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;

	if (type == Texture::TP_2D)
	{
		LPDIRECT3DTEXTURE9 pd3dTex = NULL;
		if (SUCCEEDED(D3DXCreateTexture(m_pd3dDevice9, (UINT)desc.nWidth, (UINT)desc.nHeight, desc.nMipLevels,
			d3dUsage, g_d3dFormat[desc.nFormat], d3dPool, &pd3dTex)))
		{
			pTexture = new Texture2DD3D9(pd3dTex, desc);
		}
	}

	if (!pTexture)
	{
		char msg[255];
		::sprintf_s(msg, sizeof(msg), "Create texture failed. width = %d height = %d\n", 
			m_nWidth, m_nHeight);
		Console::AppendText(msg);
	}

	return pTexture;
}




RenderTarget* RendererDX9::CreateRenderTarget(const RenderTarget::Desc& desc)
{
	RenderTarget* pRT =  new RenderTargetD3D9(desc);
	return pRT;
}


//////////////////////////////////////////////////////////////////////////




class ShaderCodeIncluder : public ID3DXInclude
{
public:
	STDMETHOD(Open)(D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes);
	STDMETHOD(Close)(LPCVOID pData);
};


HRESULT ShaderCodeIncluder::Open(D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
{
	string fileName = pFileName;



	return S_OK;
}

HRESULT ShaderCodeIncluder::Close(LPCVOID pData)
{

	return S_OK;
}



bool RendererDX9::CreateVertexShader(const string& pathname, ShaderPtr& pShader, ShaderConstTablePtr& pShaderConstTable, 
									 const vector<pair<string, string>>& defines)
{
	LPD3DXBUFFER			pCompiledShader	= NULL;
	LPD3DXBUFFER			pErrorMsgs		= NULL;
	LPD3DXCONSTANTTABLE		pConstantTable	= NULL;

	DWORD flag = 0;
#ifdef _DEBUG
	flag |= D3DXSHADER_DEBUG;
#endif

	vector<D3DXMACRO> macros;
	for (vector<pair<string, string>>::const_iterator it = defines.begin(); it != defines.end(); ++it)
	{
		D3DXMACRO m = {it->first.c_str(), it->second.c_str()};
		macros.push_back(m);
	}
	D3DXMACRO m = {NULL, NULL};
	macros.push_back(m);


	HRESULT hr = D3DXCompileShaderFromFileA(pathname.c_str(), &macros[0], NULL, 
		"Main", "vs_3_0", flag, &pCompiledShader, &pErrorMsgs, &pConstantTable);

	if (FAILED(hr))
	{
		std::string msg = "compile vertex shader error. material: ";
		msg += '\n';
		Console::AppendText(msg.c_str());
		if (pErrorMsgs)
		{
			std::string strError = (char*)pErrorMsgs->GetBufferPointer();
			Console::AppendText(strError.c_str());
			pErrorMsgs->Release();
		}

		return false;
	}

	IDirect3DVertexShader9* pD3DShader = NULL;
	hr = m_pd3dDevice9->CreateVertexShader((const DWORD*)pCompiledShader->GetBufferPointer(), &pD3DShader);
	assert(SUCCEEDED(hr));

	RenderingContext* pContext = Global::GetPipeline()->GetRenderingContext();

	pShader.reset(new VertexShaderD3D9(pD3DShader));
	pShaderConstTable.reset(new ShaderConstTable());

	D3DXCONSTANTTABLE_DESC desc;
	pConstantTable->GetDesc(&desc);
	for (uint i = 0; i < desc.Constants; i++)
	{
		D3DXHANDLE handle = pConstantTable->GetConstant(NULL, i);
		D3DXCONSTANT_DESC SCDesc;
		UINT count = 1;
		pConstantTable->GetConstantDesc(handle, &SCDesc, &count);
		if (!pContext->TestReservedShaderConstVS(SCDesc.Name, SCDesc.RegisterIndex, SCDesc.RegisterCount))
		{
			uint id = Shader::MapShaderConstNameToID(SCDesc.Name);
			if (SCDesc.RegisterSet == D3DXRS_FLOAT4)
			{
				pShaderConstTable->AddShaderConstF(SCDesc.Name, SCDesc.RegisterIndex, SCDesc.RegisterCount, id);
				continue;
			}

			if (SCDesc.RegisterSet == D3DXRS_SAMPLER)
			{
				pShaderConstTable->AddSampler(SCDesc.Name, SCDesc.RegisterIndex);
				continue;
			}

			assert(0);
		}

	}

	pConstantTable->Release();
	return true;
}



bool RendererDX9::CreatePixelShader(const string& pathname, ShaderPtr& pShader, ShaderConstTablePtr& pShaderConstTable, 
									const vector<pair<string, string>>& defines)
{
	LPD3DXBUFFER			pCompiledShader	= NULL;
	LPD3DXBUFFER			pErrorMsgs		= NULL;
	LPD3DXCONSTANTTABLE		pConstantTable	= NULL;

	//ShaderCodeIncluder includer;

	DWORD flag = 0;
#ifdef _DEBUG
	flag |= D3DXSHADER_DEBUG;
#endif

	vector<D3DXMACRO> macros;
	for (vector<pair<string, string>>::const_iterator it = defines.begin(); it != defines.end(); ++it)
	{
		D3DXMACRO m = {it->first.c_str(), it->second.c_str()};
		macros.push_back(m);
	}
	D3DXMACRO m = {NULL, NULL};
	macros.push_back(m);

	HRESULT hr = D3DXCompileShaderFromFileA(pathname.c_str(), &macros[0], NULL, 
		"Main", "ps_3_0", flag, &pCompiledShader, &pErrorMsgs, &pConstantTable);

	if (FAILED(hr))
	{
		std::string msg = "compile pixel shader error. material: ";
		msg += '\n';
		Console::AppendText(msg.c_str());
		if (pErrorMsgs)
		{
			std::string strError = (char*)pErrorMsgs->GetBufferPointer();
			Console::AppendText(strError.c_str());
			pErrorMsgs->Release();
		}

		return false;
	}

	IDirect3DPixelShader9* pD3DShader = NULL;
	hr = m_pd3dDevice9->CreatePixelShader((const DWORD*)pCompiledShader->GetBufferPointer(), &pD3DShader);
	assert(SUCCEEDED(hr));

	// fixme
	// bad design
	RenderingContext* pContext = Global::GetPipeline()->GetRenderingContext();

	pShader.reset(new PixelShaderD3D9(pD3DShader));
	pShaderConstTable.reset(new ShaderConstTable());

	D3DXCONSTANTTABLE_DESC desc;
	pConstantTable->GetDesc(&desc);
	for (uint i = 0; i < desc.Constants; i++)
	{
		D3DXHANDLE handle = pConstantTable->GetConstant(NULL, i);
		D3DXCONSTANT_DESC SCDesc;
		UINT count = 1;
		pConstantTable->GetConstantDesc(handle, &SCDesc, &count);

		if (!pContext->TestReservedShaderConstPS(SCDesc.Name, SCDesc.RegisterIndex, SCDesc.RegisterCount))
		{
			uint id = Shader::MapShaderConstNameToID(SCDesc.Name);
			if (SCDesc.RegisterSet == D3DXRS_FLOAT4)
			{
				pShaderConstTable->AddShaderConstF(SCDesc.Name, SCDesc.RegisterIndex, SCDesc.RegisterCount, id);
				continue;
			}

			if (SCDesc.RegisterSet == D3DXRS_SAMPLER)
			{
				pShaderConstTable->AddSampler(SCDesc.Name, SCDesc.RegisterIndex);
				continue;
			}
			assert(0);
		}
	}

	pConstantTable->Release();
	return true;

}


//////////////////////////////////////////////////////////////////////////



//
//
//enum VertexElemType
//{
//TYPE_FLOAT = 0,
//TYPE_FLOAT2,
//TYPE_FLOAT3,
//TYPE_FLOAT4,
//TYPE_UBYTE,
//TYPE_UBYTE4_UINT,
//TYPE_USHORT_UINT,
//TYPE_USHORT2,
//TYPE_UINT
//};
//

static BYTE s_MapTypeToD3D[GeometryData::NUM_OF_TYPE] = 
{
	D3DDECLTYPE_FLOAT1,
	D3DDECLTYPE_FLOAT2,
	D3DDECLTYPE_FLOAT3,
	D3DDECLTYPE_FLOAT4,
	D3DDECLTYPE_UNUSED,
	D3DDECLTYPE_UBYTE4,
	D3DDECLTYPE_UNUSED,
	D3DDECLTYPE_UNUSED,
	D3DDECLTYPE_UNUSED
};


//enum ElemUsage
//{
//	USAGE_POSITION = 0,
//	USAGE_NORMAL,
//	USAGE_COLOR,
//	USAGE_TEXCOORD,
//	USAGE_TANGENT,
//	USAGE_BINORMAL,
//	USAGE_BLENDINDEX,
//	USAGE_BLENDWEIGHT,
//	USAGE_INDEX
//};

static BYTE s_MapUsageToD3D[GeometryData::NUM_OF_USAGE] = 
{
	D3DDECLUSAGE_POSITION,
	D3DDECLUSAGE_NORMAL,
	D3DDECLUSAGE_COLOR,
	D3DDECLUSAGE_TEXCOORD,
	D3DDECLUSAGE_TANGENT,
	D3DDECLUSAGE_BINORMAL,
	D3DDECLUSAGE_BLENDINDICES,
	D3DDECLUSAGE_BLENDWEIGHT,
	D3DDECLUSAGE_TESSFACTOR				//index unused for d3d
};



class VertexDescriptionD3D9 : public VertexDescription
{
public:
	~VertexDescriptionD3D9();

	virtual bool			BuildDeviceObject();
	virtual void*			GetBindID() const;

private:

	vector<D3DVERTEXELEMENT9>		m_vDecl;
	LPDIRECT3DVERTEXDECLARATION9	m_pVertexDecl;
};

VertexDescriptionD3D9::~VertexDescriptionD3D9()
{
	m_pVertexDecl->Release();
}

void* VertexDescriptionD3D9::GetBindID() const
{
	return m_pVertexDecl;
}


bool VertexDescriptionD3D9::BuildDeviceObject()
{
	vector<D3DVERTEXELEMENT9> vDecl;

	D3DVERTEXELEMENT9 elem;
	elem.Method		= D3DDECLMETHOD_DEFAULT;


	Stream& s = stream;
	elem.Offset		= 0;
	elem.Stream		= 0;

	for (uint item = 0; item < s.items.size(); item++)
	{
		Item& i = s.items[item];
		elem.Type		= s_MapTypeToD3D[i.type];
		elem.Usage		= s_MapUsageToD3D[i.usage];
		elem.UsageIndex	= i.usageIndex;

		vDecl.push_back(elem);

		elem.Offset += GeometryData::GetSizeOfType((GeometryData::VertexElemType)i.type);
	}


	D3DVERTEXELEMENT9 endElem = {0xFF, 0, D3DDECLTYPE_UNUSED, 0, 0, 0};
	vDecl.push_back(endElem);


	m_pVertexDecl = Global::GetRenderer()->CreateVertexDesc(&vDecl[0]);
	if (!m_pVertexDecl)
	{
		Console::AppendText("Create Vertex Declaration failed!\n");
		return false;
	}

	return true;
}


VertexDescription* RendererDX9::CreateEmptyVertexDesc()
{
	return new VertexDescriptionD3D9();
}

LPDIRECT3DVERTEXDECLARATION9 RendererDX9::CreateVertexDesc(CONST D3DVERTEXELEMENT9* pElem)
{
	LPDIRECT3DVERTEXDECLARATION9 pd3dDecl = NULL;
	if ( FAILED(m_pd3dDevice9->CreateVertexDeclaration(pElem, &pd3dDecl) ) )
	{
		return NULL;
	}
	return pd3dDecl;
}


bool HALCreateVertexDecl(VertexDescription* pDecl)
{
	vector<D3DVERTEXELEMENT9> vDecl;

	D3DVERTEXELEMENT9 elem;
	elem.Method		= D3DDECLMETHOD_DEFAULT;


	VertexDescription::Stream& s = pDecl->stream;
	elem.Offset		= 0;
	elem.Stream		= 0;

	for (uint item = 0; item < s.items.size(); item++)
	{
		VertexDescription::Item& i = s.items[item];
		elem.Type		= s_MapTypeToD3D[i.type];
		elem.Usage		= s_MapUsageToD3D[i.usage];
		elem.UsageIndex	= i.usageIndex;

		vDecl.push_back(elem);

		elem.Offset += GeometryData::GetSizeOfType((GeometryData::VertexElemType)i.type);
	}

	D3DVERTEXELEMENT9 endElem = {0xFF, 0, D3DDECLTYPE_UNUSED, 0, 0, 0};
	vDecl.push_back(endElem);


	if (FAILED(s_pd3dDevice9->CreateVertexDeclaration(&vDecl[0], (HALVertexDecl*)(&pDecl->HALObj))))
	{
		Console::AppendText("Create Vertex Declaration failed!\n");
		return false;
	}

	return true;

}

void HALReleaseVertexDecl(VertexDescription* pDecl)
{
	assert(pDecl->HALObj);
	((HALVertexDecl)(pDecl->HALObj))->Release();
	pDecl->HALObj = NULL;
}


//////////////////////////////////////////////////////////////////////////


class VertexBufferD3D9 : public VertexBuffer
{
public:
	VertexBufferD3D9(const VertexDescription* pVertexDesc, GeometryData* pGeometry,
		const vector<Stream>& streams, uint nNumVertex);
	~VertexBufferD3D9();


	virtual void*			GetBindID() const;
	virtual void			OnReset();
	virtual void			OnLost();

	virtual void*			Lock(uint nStream);
	virtual void			Unlock(uint nStream);

	LPDIRECT3DVERTEXBUFFER9	GetBindPointer(uint nStream)	{	return m_pBuffers[nStream];	}


private:
	boost::shared_array<LPDIRECT3DVERTEXBUFFER9>	m_pBuffers;
	GeometryData*									m_pGeometry;
	const VertexDescription*						m_pVertexDesc;

};


VertexBufferD3D9::VertexBufferD3D9(const VertexDescription* pVertexDesc, GeometryData* pGeometry, 
								   const vector<VertexBuffer::Stream>& streams, uint nNumVertex):
VertexBuffer(streams, nNumVertex),m_pGeometry(pGeometry),m_pVertexDesc(pVertexDesc)
{
}

VertexBufferD3D9::~VertexBufferD3D9()
{
	assert(m_pBuffers == NULL);
}

void* VertexBufferD3D9::GetBindID() const
{
	assert(0);
	return NULL;
}


void* VertexBufferD3D9::Lock(uint nStream)
{
	VOID* pointer = NULL;
	m_pBuffers[nStream]->Lock(0, 0, &pointer, 0);
	return pointer;
}

void VertexBufferD3D9::Unlock(uint nStream)
{
	m_pBuffers[nStream]->Unlock();
}

void VertexBufferD3D9::OnReset()
{
	assert(m_pBuffers == NULL);
	m_pBuffers.reset(new LPDIRECT3DVERTEXBUFFER9[m_nNumStream]);

	for (uint i = 0; i < m_nNumStream; i++)
	{
		if (FAILED(s_pd3dDevice9->CreateVertexBuffer(m_pStreams[i].nSizeInByte, 0, 0, D3DPOOL_DEFAULT, &m_pBuffers[i], NULL)))
		{
			assert(0);
		}
	}

	Locker locker = GetLocker(0);
	m_pGeometry->AssembleVertexBuffer(locker.Ptr(), m_pVertexDesc->stream);
}

void VertexBufferD3D9::OnLost()
{
	assert(m_pBuffers != NULL);
	for (uint i = 0; i < m_nNumStream; i++)
	{
		m_pBuffers[i]->Release();
	}
	m_pBuffers.reset();
}



VertexBuffer* RendererDX9::CreateVertexBuffer(GeometryData* pGeometry, const VertexDescription* pVertexDesc, uint nVert)
{
	vector<VertexBuffer::Stream> streams;

	uint nStride = 0;

	for (uint j = 0; j < pVertexDesc->stream.items.size(); j++)
	{
		const VertexDescription::Item& item = pVertexDesc->stream.items[j];
		nStride += GeometryData::GetSizeOfType((GeometryData::VertexElemType)item.type);
	}

	VertexBuffer::Stream s;
	s.nStride		= nStride;
	s.nSizeInByte	= nStride * nVert;

	streams.push_back(s);

	return new VertexBufferD3D9(pVertexDesc, pGeometry, streams, nVert);
}


//////////////////////////////////////////////////////////////////////////


class IndexBufferD3D9 : public IndexBuffer
{
public:
	IndexBufferD3D9(GeometryData* pGeometry, IndexBuffer::Format fmt, uint nNumIndex);
	~IndexBufferD3D9();


	virtual void*			GetBindID() const;
	virtual void			OnReset();
	virtual void			OnLost();

	virtual void*			Lock();
	virtual void			Unlock();

private:
	LPDIRECT3DINDEXBUFFER9	m_pBuffer;
	GeometryData*			m_pGeometry;
};

IndexBufferD3D9::IndexBufferD3D9(GeometryData* pGeometry, IndexBuffer::Format fmt, uint nNumIndex):
IndexBuffer(fmt, nNumIndex),m_pBuffer(NULL),m_pGeometry(pGeometry)
{

}

IndexBufferD3D9::~IndexBufferD3D9()
{
	assert(m_pBuffer == NULL);
}


void* IndexBufferD3D9::GetBindID() const
{
	return m_pBuffer;
}

void* IndexBufferD3D9::Lock()
{
	VOID* pointer = NULL;
	m_pBuffer->Lock(0, 0, &pointer, 0);
	return pointer;
}

void IndexBufferD3D9::Unlock()
{
	m_pBuffer->Unlock();
}

void IndexBufferD3D9::OnReset()
{
	assert(m_pBuffer == NULL);
	D3DFORMAT d3dFmt = (m_nFormat == IndexBuffer::FMT_INDEX16) ? D3DFMT_INDEX16 : D3DFMT_INDEX32;
	uint stride = (m_nFormat == IndexBuffer::FMT_INDEX16) ? 2 : 4;
	UINT length = stride * m_nNumIndex;

	if (FAILED(s_pd3dDevice9->CreateIndexBuffer(length, 0, d3dFmt, D3DPOOL_DEFAULT, &m_pBuffer, NULL)))
	{
		assert(0);
	}

	ubyte* pMemBuffer = m_pGeometry->GetStreamPointer(GeometryData::USAGE_INDEX, 0);
	assert(pMemBuffer != NULL);

	Locker locker = GetLocker();
	Util::MemMap(pMemBuffer, stride, (ubyte*)locker.Ptr(), stride, stride, m_nNumIndex);
}

void IndexBufferD3D9::OnLost()
{
	assert(m_pBuffer != NULL);
	m_pBuffer->Release();
	m_pBuffer = NULL;
}

IndexBuffer* RendererDX9::CreateIndexBuffer(GeometryData* pGeometry, IndexBuffer::Format fmt, uint nNumIndex)
{
	return new IndexBufferD3D9(pGeometry, fmt, nNumIndex);
}

#endif


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


RendererPtr RendererDX9::CreateDevice(HWND hWnd, int nWidth, int nHeight)
{
	RendererPtr pRenderer(new RendererDX9(hWnd, nWidth, nHeight));

	return pRenderer;
}


RendererDX9::RendererDX9(HWND hWnd, int nWidth, int nHeight):Renderer()
{
	m_pd3dDevice9	= NULL;
	m_pd3dDevice9	= NULL;
	m_pFrameBuffer	= NULL;
	m_pDepthStencil	= NULL;

	m_hWnd		= hWnd;
	m_nWidth	= nWidth;
	m_nHeight	= nHeight;


	ZeroMemory( &m_d3dCaps, sizeof( m_d3dCaps ) );

    if(NULL == ( m_pDirect3D9 = Direct3DCreate9( D3D_SDK_VERSION ) ))
        return;


    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof( d3dpp ) );

    d3dpp.Windowed			= TRUE;
	d3dpp.BackBufferWidth	= nWidth;
	d3dpp.BackBufferHeight	= nHeight;
    d3dpp.SwapEffect		= D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat	= g_d3dFormat[Surface::FMT_A8R8G8B8];			//set as current display-mode format 
	d3dpp.BackBufferCount	= 1;
    d3dpp.EnableAutoDepthStencil	= TRUE;
	d3dpp.AutoDepthStencilFormat	= g_d3dFormat[Surface::FMT_D24S8];
	d3dpp.PresentationInterval		= D3DPRESENT_INTERVAL_IMMEDIATE;
	d3dpp.hDeviceWindow		= NULL;


	// Set default settings
	UINT AdapterToUse		= D3DADAPTER_DEFAULT;
	D3DDEVTYPE DeviceType	= D3DDEVTYPE_HAL;
	// Look for 'NVIDIA NVPerfHUD' adapter
	// If it is present, override default settings
	for (UINT Adapter = 0; Adapter < m_pDirect3D9->GetAdapterCount(); Adapter++)
	{
		D3DADAPTER_IDENTIFIER9 Identifier;
		HRESULT Res = m_pDirect3D9->GetAdapterIdentifier(Adapter, 0, &Identifier);
		if (strstr(Identifier.Description,"PerfHUD") != NULL)
		{
			AdapterToUse = Adapter;
			DeviceType = D3DDEVTYPE_REF;
			break;
		}
	}

    // Create the D3DDevice
    if( FAILED( m_pDirect3D9->CreateDevice( AdapterToUse, DeviceType, hWnd,
                                      D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                      &d3dpp, &m_pd3dDevice9 ) ) )
    {
		m_pDirect3D9->Release();
		m_pDirect3D9	= NULL;
		m_pd3dDevice9	= NULL;
        return;
    }
	s_pd3dDevice9 = m_pd3dDevice9;

	m_pSwapChain = NULL;
	m_pd3dDevice9->GetSwapChain(0, &m_pSwapChain);

	Global::SetRenderer(this);
}

RendererDX9::~RendererDX9(void)
{
	if (m_pd3dDevice9)
	{
		m_pd3dDevice9->Release();
		m_pd3dDevice9 = NULL;
	}

	if (m_pDirect3D9)
	{
		m_pDirect3D9->Release();
		m_pDirect3D9 = NULL;
	}
}


#ifdef DIRECTX_VERSION_DX9

void HALOnReset()
{	
	// store buffer
	m_pd3dDevice9->GetRenderTarget(0, &m_pFrameBuffer);

	/*D3DSURFACE_DESC surDesc;
	pSurface->GetDesc(&surDesc);

	RenderTarget::Desc desc;
	desc.nWidth = surDesc.Width;
	desc.nHeight = surDesc.Height;
	desc.nFormat = MapFromatFromD3D(surDesc.Format);
	desc.nUsage = Surface::USAGE_RENDERTARGET;
	m_pFrameBuffer.reset(new RenderTargetD3D9(pSurface, desc));*/


	m_pd3dDevice9->GetDepthStencilSurface(&m_pDepthStencil);

	/*pSurface->GetDesc(&surDesc);

	desc.nWidth = surDesc.Width;
	desc.nHeight = surDesc.Height;
	desc.nFormat = MapFromatFromD3D(surDesc.Format);
	desc.nUsage = Surface::USAGE_DEPTHSTENCIL;
	m_pDepthStencil.reset(new RenderTargetD3D9(pSurface, desc));*/


	m_pd3dDevice9->GetDeviceCaps(&m_d3dCaps);
	m_pd3dDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	m_pd3dDevice9->SetRenderState(D3DRS_ALPHAREF, 128);



	//LPDIRECT3DTEXTURE9 pTex = NULL;
	//D3DXCreateTextureFromFileA(m_pd3dDevice9, "../res/rockwall.jpg", &pTex);				//1
	//pTex = NULL;
	//D3DXCreateTextureFromFileA(m_pd3dDevice9, "../res/sky_cube_mipmap.dds", &pTex);			//1
	//pTex = NULL;
	//D3DXCreateTextureFromFileA(m_pd3dDevice9, "../res/test volume.dds", &pTex);				//1


	//LPDIRECT3DCUBETEXTURE9 pCubeTex = NULL;

	//D3DXCreateCubeTextureFromFileA(m_pd3dDevice9, "../res/rockwall.jpg", &pCubeTex);			//0
	//pCubeTex = NULL;
	//D3DXCreateCubeTextureFromFileA(m_pd3dDevice9, "../res/sky_cube_mipmap.dds", &pCubeTex);		//1
	//pCubeTex = NULL;
	//D3DXCreateCubeTextureFromFileA(m_pd3dDevice9, "../res/test volume.dds", &pCubeTex);			//0


	//LPDIRECT3DVOLUMETEXTURE9 pVolTex = NULL;
	//
	//D3DXCreateVolumeTextureFromFileA(m_pd3dDevice9, "../res/rockwall.jpg", &pVolTex);			//1
	//
	//pVolTex = NULL;
	//D3DXCreateVolumeTextureFromFileA(m_pd3dDevice9, "../res/sky_cube_mipmap.dds", &pVolTex);	//1
	//pVolTex = NULL;
	//D3DXCreateVolumeTextureFromFileA(m_pd3dDevice9, "../res/test volume.dds", &pVolTex);		//1

}

void HALOnLost()
{
	m_pFrameBuffer->Release();
	m_pFrameBuffer = NULL;

	m_pDepthStencil->Release();
	m_pDepthStencil = NULL;
}

void HALGetFrameBufferSize(uint& nWidth, uint& nHeight)
{
	nWidth = m_nWidth;
	nHeight = m_nHeight;
}

void HALClear(int clearMask, const Color& clearColor, float fDepth, ubyte nStencil)
{
	static DWORD dwFlagTable[] = {0x0, D3DCLEAR_ZBUFFER, D3DCLEAR_STENCIL, 0x0, D3DCLEAR_TARGET};
	DWORD dwFlag =  dwFlagTable[clearMask & Renderer::CLEAR_DEPTH_BUFFER] | \
					dwFlagTable[clearMask & Renderer::CLEAR_STENCIL_BUFFER] | \
					dwFlagTable[clearMask & Renderer::CLEAR_FRAME_BUFFER];
					
	s_pd3dDevice9->Clear(0, NULL, dwFlag, ToD3DColor(clearColor), fDepth, (DWORD)nStencil);

}


void HALReset(int nWidth, int nHeight)
{
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof( d3dpp ) );

	d3dpp.Windowed			= TRUE;
	d3dpp.BackBufferWidth	= nWidth;
	d3dpp.BackBufferHeight	= nHeight;
	d3dpp.SwapEffect		= D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat	= g_d3dFormat[Surface::FMT_A8R8G8B8];			//set as current display-mode format 
	d3dpp.BackBufferCount	= 1;
	d3dpp.EnableAutoDepthStencil	= TRUE;
	d3dpp.AutoDepthStencilFormat	= g_d3dFormat[Surface::FMT_D24X8];
	d3dpp.PresentationInterval		= D3DPRESENT_INTERVAL_IMMEDIATE;
	d3dpp.hDeviceWindow		= NULL;

	HRESULT hr = m_pd3dDevice9->Reset(&d3dpp);

	if (hr == D3DERR_DEVICELOST)
	{
		assert(0);
	}

	if (SUCCEEDED(hr))
	{
		m_nWidth = nWidth;
		m_nHeight = nHeight;
	}
}

void HALSetVertexDesc(VertexDescription* pVertDesc)
{
	s_pd3dDevice9->SetVertexDeclaration((IDirect3DVertexDeclaration9*)pVertDesc->GetBindID());
}

static D3DPRIMITIVETYPE sPrimitiveTypeMap[] = {
	D3DPT_POINTLIST,
	D3DPT_LINELIST,
	D3DPT_LINESTRIP,
	D3DPT_TRIANGLELIST,
	D3DPT_TRIANGLESTRIP,
	D3DPT_TRIANGLEFAN,

};

void HALBeginScene()
{
	HRESULT hr = s_pd3dDevice9->BeginScene();
}

void HALEndScene()
{
	HRESULT hr = s_pd3dDevice9->EndScene();
}

void HALPresent()
{
	s_pd3dDevice9->Present(NULL, NULL, m_hWnd, NULL);
}

void HALSetVertexShader(Shader* pShader)
{
	s_pd3dDevice9->SetVertexShader((IDirect3DVertexShader9*)pShader->GetBindID());
}

void HALSetPixelShader(Shader* pShader)
{
	s_pd3dDevice9->SetPixelShader((IDirect3DPixelShader9*)pShader->GetBindID());
}

void HALSetVertexShaderConstantF(const float* pBuffer, uint startReg, uint numReg)
{
	s_pd3dDevice9->SetVertexShaderConstantF(startReg, pBuffer, numReg);
}

void HALSetPixelShaderConstantF(const float* pBuffer, uint startReg, uint numReg)
{
	s_pd3dDevice9->SetPixelShaderConstantF(startReg, pBuffer, numReg);
}

void HALSetTexture(uint nStage, Texture* pTexture)
{
	s_pd3dDevice9->SetTexture(nStage, (IDirect3DBaseTexture9*)pTexture->GetBindID());
}

void HALSetRenderState(const StateValuePair& renderState)
{
	vector<pair<uint, uint>>::const_iterator it = renderState.states.begin();
	for (; it != renderState.states.end(); ++it)
	{
		s_pd3dDevice9->SetRenderState((D3DRENDERSTATETYPE)it->first, it->second);
	}
}

void HALSetSamplerState(uint nStage, const StateValuePair& samplerState)
{
	vector<pair<uint, uint>>::const_iterator it = samplerState.states.begin();
	for (; it != samplerState.states.end(); ++it)
	{
		s_pd3dDevice9->SetSamplerState(nStage, (D3DSAMPLERSTATETYPE)it->first, it->second);
	}
}

void HALSetRenderTarget(uint idx, RenderTarget* pRenderTarget)
{
	assert(pRenderTarget->GetDesc().nUsage == Surface::USAGE_RENDERTARGET);
	s_pd3dDevice9->SetRenderTarget(idx, (IDirect3DSurface9*)pRenderTarget->GetBindID());
	s_pd3dDevice9->SetSamplerState(idx, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	s_pd3dDevice9->SetSamplerState(idx, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

}

void HALSetDepthStencil(RenderTarget* pDepthStencil)
{
	assert(pDepthStencil->GetDesc().nUsage == Surface::USAGE_DEPTHSTENCIL);
	s_pd3dDevice9->SetDepthStencilSurface((IDirect3DSurface9*)pDepthStencil->GetBindID());
}

void HALRestoreFrameBuffer()
{
	m_pd3dDevice9->SetRenderTarget(0, m_pFrameBuffer);
	m_pd3dDevice9->SetRenderTarget(1, NULL);
	m_pd3dDevice9->SetRenderTarget(2, NULL);
	m_pd3dDevice9->SetRenderTarget(3, NULL);
	m_pd3dDevice9->SetDepthStencilSurface(m_pDepthStencil);
}


void HALExecuteOperator(const RenderOperator& op)
{
	op.pMaterial->ApplyInstance(op.pMtlInst);
	HALSetVertexDesc(op.pVertDesc);

	if (op.pSystemVertBuff)
	{
		if (op.pSystemIndexBuff)
		{
			// temporary only support 16 bit index buffer
			s_pd3dDevice9->DrawIndexedPrimitiveUP(sPrimitiveTypeMap[op.nPrimType],
				op.nBaseVertexIndex, op.nNumVertices, op.nPrimitiveCount, op.pSystemIndexBuff,
				D3DFMT_INDEX16, op.pSystemVertBuff, op.nVertexStride);
		}
		else
		{
			s_pd3dDevice9->DrawPrimitiveUP(sPrimitiveTypeMap[op.nPrimType], op.nPrimitiveCount, 
				op.pSystemVertBuff, op.nVertexStride);
		}
		return;
	}

	uint nStream = op.pVertexBuffer->GetNumStream();
	VertexBufferD3D9* pVB = (VertexBufferD3D9*)op.pVertexBuffer;

	for (uint i = 0; i < nStream; i++)
	{
		s_pd3dDevice9->SetStreamSource(i, (IDirect3DVertexBuffer9*)pVB->GetBindPointer(i),
			0, pVB->GetStride(i));
	}

	if (op.pIndexBuffer)
	{
		s_pd3dDevice9->SetIndices((IDirect3DIndexBuffer9*)op.pIndexBuffer->GetBindID());
		s_pd3dDevice9->DrawIndexedPrimitive(sPrimitiveTypeMap[op.nPrimType], op.nBaseVertexIndex,
			op.nMinVertexIndex, op.nNumVertices, op.nStartIndex, op.nPrimitiveCount);
	}
	else
	{
		s_pd3dDevice9->DrawPrimitive(sPrimitiveTypeMap[op.nPrimType], op.nMinVertexIndex, op.nPrimitiveCount);
	}
}



HALOcclusionQuery HALCreateOcclusionQuery()
{
	HALOcclusionQuery pQuery = NULL;

	HRESULT hr = s_pd3dDevice9->CreateQuery(D3DQUERYTYPE_OCCLUSION, &pQuery);
	assert(SUCCEEDED(hr));

	return pQuery;
}

void HALReleaseOcclusionQuery(HALOcclusionQuery& obj)
{
	obj->Release();
	obj = NULL;
}

void HALOcclusionQueryIssueBegin(HALOcclusionQuery obj)
{
	obj->Issue(D3DISSUE_BEGIN);
}

uint HALOcclusionQueryIssueEnd(HALOcclusionQuery obj)
{
	DWORD numberOfPixelsDrawn = 0;

	// Add an end marker to the command buffer queue.
	obj->Issue(D3DISSUE_END);

	// Force the driver to execute the commands from the command buffer.
	// Empty the command buffer and wait until the GPU is idle.
	while(S_FALSE == obj->GetData( &numberOfPixelsDrawn, 
		sizeof(DWORD), D3DGETDATA_FLUSH ))
		;

	return (uint)numberOfPixelsDrawn;
}

void HALSetScissorTest(bool enable, int left, int top, int right, int bottom)
{
	if (enable)
	{
		s_pd3dDevice9->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
		RECT rect;
		::SetRect(&rect, left, top, right, bottom);
		s_pd3dDevice9->SetScissorRect(&rect);
	}
	else
	{
		s_pd3dDevice9->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
	}
}



#endif











































//
//
//
//
//// 只动态创建，从不释放（内存），程序结束时由系统自动回收内存
//vector<DepthStencilStateOjbect*> DepthStencilStatePool;
//
//// 如果找不到匹配，返还NULL
//static DepthStencilStateOjbect* FindDepthStencilStateInPool(const DepthStencilStateOjbect::Desc& desc)
//{
//	for (vector<DepthStencilStateOjbect*>::iterator it = DepthStencilStatePool.begin();
//		it != DepthStencilStatePool.end(); ++it)
//	{
//		if (memcmp(&((*it)->desc), &desc, sizeof(desc)) == 0)
//		{
//			return *it;
//		}
//	}
//
//	return NULL;
//}
//
//static DepthStencilStateOjbect* CreateDepthStencilStateObject(const DepthStencilStateOjbect::Desc& desc)
//{
//	IDirect3DStateBlock9* pStateBlock = NULL;
//
//	s_pd3dDevice9->BeginStateBlock();
//
//	s_pd3dDevice9->SetRenderState(D3DRS_ZENABLE, desc.DepthEable ? D3DZB_TRUE : D3DZB_FALSE);
//	s_pd3dDevice9->SetRenderState(D3DRS_ZWRITEENABLE , desc.DepthWriteEnable ? TRUE : FALSE);
//	if (desc.DepthEable)
//	{
//		s_pd3dDevice9->SetRenderState(D3DRS_ZFUNC, D3D9Mapping::CmpFunc(desc.DepthFunc));
//	}
//	s_pd3dDevice9->SetRenderState(D3DRS_STENCILENABLE, desc.StencilEnable ? TRUE : FALSE);
//
//	// todo : stencil op
//	/*
//	if (desc.StencilEnable)
//	{
//		s_pd3dDevice9->SetRenderState(D3DRS_STENCILMASK, (DWORD)desc.s)
//
//	}*/
//
//
//	s_pd3dDevice9->EndStateBlock(&pStateBlock);
//
//	DepthStencilStateOjbect* pObject = new DepthStencilStateOjbect();
//	pObject->desc = desc;
//	pObject->pDeviceObject = pStateBlock;
//
//	return pObject;
//}
//
//
//DepthStencilStateOjbect* RendererDX9::CreateCachedDSStateOjbect(const DepthStencilStateOjbect::Desc& desc)
//{
//	DepthStencilStateOjbect* pObject = FindDepthStencilStateInPool(desc);
//
//	if (pObject == NULL)
//	{
//		pObject = CreateDepthStencilStateObject(desc);
//		DepthStencilStatePool.push_back(pObject);
//	}
//
//	return pObject;
//}
//
//
//
//
//
//vector<RasterStateObject*>	RasterStateObjectPool;
//
//// 如果找不到匹配，返还NULL
//static RasterStateObject* FindRasterStateObjectInPool(const RasterStateObject::Desc& desc)
//{
//	for (vector<RasterStateObject*>::iterator it = RasterStateObjectPool.begin();
//		it != RasterStateObjectPool.end(); ++it)
//	{
//		if (memcmp(&((*it)->desc), &desc, sizeof(desc)) == 0)
//		{
//			return *it;
//		}
//	}
//
//	return NULL;
//}
//
//static RasterStateObject* CreateRasterStateObject(const RasterStateObject::Desc& desc)
//{
//	IDirect3DStateBlock9* pStateBlock = NULL;
//
//	s_pd3dDevice9->BeginStateBlock();
//
//	//s_pd3dDevice9->SetRenderState(D3DRS_ZENABLE, desc.DepthEable ? D3DZB_TRUE : D3DZB_FALSE);
//	//s_pd3dDevice9->SetRenderState(D3DRS_ZWRITEENABLE , desc.DepthWriteEnable ? TRUE : FALSE);
//	//if (desc.DepthEable)
//	//{
//	//	s_pd3dDevice9->SetRenderState(D3DRS_ZFUNC, D3D9Mapping::CmpFunc(desc.DepthFunc));
//	//}
//	//s_pd3dDevice9->SetRenderState(D3DRS_STENCILENABLE, desc.StencilEnable ? TRUE : FALSE);
//
//	// todo : stencil op
//	/*
//	if (desc.StencilEnable)
//	{
//	s_pd3dDevice9->SetRenderState(D3DRS_STENCILMASK, (DWORD)desc.s)
//
//	}*/
//
//
//	s_pd3dDevice9->EndStateBlock(&pStateBlock);
//
//	RasterStateObject* pObject = new RasterStateObject();
//	pObject->desc = desc;
//	pObject->pDeviceObject = pStateBlock;
//
//	return pObject;
//}
//
//
//RasterStateObject* RendererDX9::CreateCachedRSStateOjbect(const RasterStateObject::Desc& desc)
//{
//	RasterStateObject* pObject = FindRasterStateObjectInPool(desc);
//
//	if (pObject == NULL)
//	{
//		pObject = CreateRasterStateObject(desc);
//		RasterStateObjectPool.push_back(pObject);
//	}
//
//	return pObject;
//}
//
//



}