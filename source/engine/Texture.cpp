#include "stdHeader.h"
#include "Texture.h"
#include "Renderer.h"
#include "Console.h"
#include "Config.h"
#include "Util.h"
#include "FileSystem.h"

namespace Aurora
{

	struct FormatInfo
	{
		uint SizeInByte;

		uint NumElement;

		const char* Name;
	};

	//enum Format
	//{
	//	FMT_A32B32G32R32F,
	//	FMT_A16B16G16R16F,
	//	FMT_A16B16G16R16,
	//	FMT_R8G8B8A8,
	//	FMT_R32F,
	//	FMT_R32_TYPELESS,
	//	FMT_R16F,
	//	FMT_D24S8,
	//	FMT_D16,
	//	FMT_NULL,
	//	FMT_INTZ,
	//};

	static FormatInfo gSurfaceFormatInfo[] = 
	{
		{16, 4, "FMT_A32B32G32R32F"},
		{8, 4, "FMT_A16B16G16R16F"},
		{8, 4, "FMT_A16B16G16R16"},
		{4, 4, "FMT_R8G8B8A8"},
		{4, 1, "FMT_R32F"},
		{4, 1, "FMT_R32_TYPELESS"},
		{2, 1, "FMT_R16F"},
		{4, 2, "FMT_D24S8"},
		{2, 1, "FMT_D16"},
		{0, 1, "FMT_NULL"},
		{4, 1, "FMT_INTZ"},
	};


	uint Surface::GetStride(Format fmt)
	{
		return gSurfaceFormatInfo[fmt].SizeInByte;
	}

	uint Surface::GetNumElement(Format fmt)
	{
		return gSurfaceFormatInfo[fmt].NumElement;
	}


extern IRenderDevice* GRenderDevice;


Texture::Texture(const Texture::Desc& desc):m_Desc(desc)
{

}

Texture::~Texture()
{

}

//////////////////////////////////////////////////////////////////////////

RenderTarget::RenderTarget(const Texture::Desc& desc):
Texture(desc)
{

}

RenderTarget::~RenderTarget()
{
}

// convert pre-defined size to actual value
uint ConvertPredefinedSize(uint predefined)
{
	static float multiplier[] = {1.0f, 0.5f, 0.25f, 0.125f, 0.0625};

	uint size[2];
	GRenderDevice->GetFrameBufferSize(size[0], size[1]);
	
	uint idx = predefined & 0xff;

	return (uint)(multiplier[idx / 2] * size[idx % 2]);
}

uint RenderTarget::GetRealWidth() const
{
	if (m_Desc.nWidth & 0x1000000)
	{
		return ConvertPredefinedSize(m_Desc.nWidth);
	}
	else
	{
		return m_Desc.nWidth;
	}
}

uint RenderTarget::GetRealHeight() const
{
	if (m_Desc.nHeight & 0x1000000)
	{
		return ConvertPredefinedSize(m_Desc.nHeight);
	}
	else
	{
		return m_Desc.nHeight;
	}
	
}

//////////////////////////////////////////////////////////////////////////

TextureManager GTextureManager;
TextureManager::TextureManager()	{}
TextureManager::~TextureManager()	{}

void TextureManager::Initialize()
{
}

void TextureManager::Finalize()
{
	m_ResourcePool.clear();
}

Texture* TextureManager::GetTexture(const ResourceID& id)
{
	Texture* pTexture = Find(id);
	if (pTexture == NULL)
	{
		return Load(id);
	}
	return pTexture;
}

Texture* TextureManager::Find(const ResourceID& id)
{
	map<ResourceID, Texture*>::iterator it = m_ResourcePool.find(id);
	if (it != m_ResourcePool.end())
	{
		return it->second;
	}
	return NULL;
}

Texture* TextureManager::Load(const ResourceID& id)
{
	string pathName = Util::ProcessPathName(id);

	auto file = GFileSys->OpenFile(pathName);
	if (file == nullptr) {
		return nullptr;
	}

	Texture* pTexture = GRenderDevice->CreateTexture(file);

	file->Close();

	if (pTexture == NULL)
	{
		// log crate texture failed
		return NULL;
	}

	m_ResourcePool.insert(make_pair(id, pTexture));
	return pTexture;
}




//
//bool FrameBuffer::SaveRenderTargetToDXT1_R32F(const string& pathName)
//{
//	// 实现过程：
//	// 将RenderTarget拷贝到一张R32F格式的surface中
//	// 新建一张R8G8B8格式的surface将R32转换过来
//	// 将surface保存到内存中的纹理
//	// 将内存中的纹理打开为DXT1格式
//	// 将DXT1格式的纹理保存到文件
//
//
//	IDirect3DSurface9* pSrcRT = m_pFrameBuffer;
//	IDirect3DSurface9* pR32F = NULL;
//	IDirect3DSurface9* pRGB = NULL;
//	LPD3DXBUFFER pMemTex = NULL;
//	IDirect3DDevice9* pDevice = Global::GetRenderer()->GetDevice();
//
//	// 便于出错后跳出，执行清理代码
//	while (1)
//	{
//
//		D3DSURFACE_DESC desc;
//		memset(&desc, 0, sizeof(desc));
//		pSrcRT->GetDesc(&desc);
//		HRESULT hr = pDevice->CreateOffscreenPlainSurface(desc.Width, desc.Height, desc.Format, D3DPOOL_SYSTEMMEM, &pR32F, NULL);
//		if (FAILED(hr))
//		{
//			Console::AppendText("Create Off Screen Plain Surface failed!\n");
//			break;
//		}
//
//
//		hr = pDevice->CreateOffscreenPlainSurface(desc.Width, desc.Height, D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, &pRGB, NULL);
//		if (FAILED(hr))
//		{
//			Console::AppendText("Create Off Screen Plain Surface failed!\n");
//			break;
//		}
//
//		hr = pDevice->GetRenderTargetData(pSrcRT, pR32F);
//		if (FAILED(hr))
//		{
//			Console::AppendText("Get Render Target failed!\n");
//			break;
//		}
//
//		//D3DXSaveSurfaceToFileA(pathName.c_str(), D3DXIFF_DDS, pR32F, NULL, NULL);
//		//break;
//
//		D3DLOCKED_RECT lockedR32, lockedRGB;
//		memset(&lockedR32, 0, sizeof(lockedR32));
//		memset(&lockedRGB, 0, sizeof(lockedRGB));
//		pR32F->LockRect(&lockedR32, NULL, D3DLOCK_READONLY);
//		pRGB->LockRect(&lockedRGB, NULL, D3DLOCK_READONLY);
//		assert(lockedR32.Pitch == desc.Width * 4);
//		assert(lockedRGB.Pitch == desc.Width * 4);
//
//		unsigned char* pPixel = (unsigned char*)lockedRGB.pBits;
//		float* pFloat = (float*)lockedR32.pBits;
//		for (uint j = 0; j < desc.Height; j++)
//		{
//			for (uint i = 0; i < desc.Width; i++)
//			{
//				float value = *pFloat++ * 255;
//
//				// 将浮点数编码到三个通道中
//				// DXT1格式g通道拥有更多的信息
//				*pPixel++ = (unsigned char)(value);				// b
//				*pPixel++ = (unsigned char)(value + 0.5f);		// g
//				*pPixel++ = (unsigned char)(value + 0.75f);		// r
//				*pPixel++ = 0;									// x
//			}
//		}
//
//
//		pR32F->UnlockRect();
//		pRGB->UnlockRect();
//
//		pR32F->Release();
//		pR32F = NULL;
//
//
//		hr = D3DXSaveSurfaceToFileInMemory(&pMemTex, D3DXIFF_DDS, pRGB, NULL, NULL);
//		if (FAILED(hr))
//		{
//			Console::AppendText("Save Surface To File I nMemory failed!\n");
//			break;
//		}
//
//		pRGB->Release();
//		pRGB = NULL;
//
//		LPDIRECT3DTEXTURE9 pTexture = NULL;
//		hr = D3DXCreateTextureFromFileInMemoryEx(pDevice, pMemTex->GetBufferPointer(), pMemTex->GetBufferSize(), desc.Width, desc.Height,
//			D3DX_DEFAULT, 0, D3DFMT_DXT1, D3DPOOL_SYSTEMMEM, D3DX_DEFAULT ,D3DX_DEFAULT , 0, NULL, NULL, &pTexture);
//		if (FAILED(hr))
//		{
//			Console::AppendText("Create Texture From File In Memory failed!\n");
//			break;
//		}
//
//		pMemTex->Release();
//		pMemTex = NULL;
//
//		D3DSURFACE_DESC texDesc;
//		memset(&texDesc, 0, sizeof(texDesc));
//		pTexture->GetLevelDesc(0, &texDesc);
//		assert(texDesc.Format == D3DFMT_DXT1);
//
//		D3DXSaveTextureToFileA(pathName.c_str(), D3DXIFF_DDS, pTexture, NULL);
//
//		pTexture->Release();
//		return true;
//	}
//
//	if (pR32F != NULL)
//	{
//		pR32F->Release();
//	}
//	if (pRGB != NULL)
//	{
//		pRGB->Release();
//	}
//	if (pMemTex != NULL)
//	{
//		pMemTex->Release();
//	}
//
//	return false;
//}

}