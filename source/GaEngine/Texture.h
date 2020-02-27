#ifndef GATEXTURE_H
#define GATEXTURE_H

#include "Types.h"
#include "ResourceManager.h"
namespace Aurora
{

struct Surface
{
	enum Format
	{
		FMT_A32B32G32R32F = 0,
		FMT_A16B16G16R16F,
		FMT_A16B16G16R16,
		FMT_R8G8B8A8,
		FMT_R32F,
		FMT_R32_TYPELESS,
		FMT_R16F,
		FMT_D24S8,
		FMT_D16,
		FMT_NULL,
		FMT_INTZ,
	};

	enum Usage
	{
		USAGE_DEFAULT		= 0,
		USAGE_RENDERTARGET,
		USAGE_DEPTHSTENCIL,
	};

	static uint GetStride(Format fmt);

	static uint GetNumElement(Format fmt);
};


static const char* SurfaceUsageName[] = 
{
	"USAGE_DEFAULT",
	"USAGE_RENDERTARGET",
	"USAGE_DEPTHSTENCIL",
};



class Texture : public Resource
{
public:
	enum Type
	{
		TP_2D,
		TP_3D,
		TP_CUBE,
	};

	struct Desc
	{
		uint nWidth;

		uint nHeight;

		uint nDepth;

		uint nMipLevels;

		Surface::Format nFormat;

		Surface::Usage nUsage;

		uint bDynamic : 1;
		uint bMultiSample : 1;
	};

	Texture(const Desc& desc);
	virtual ~Texture();

	virtual Type GetType() const = 0;

	const Desc& GetDesc() const		{	return m_Desc;	}

	virtual void Update(int nMip, const void* pData)	{}

	virtual uint GetFootprint() const	{	return 0;	}

protected:
	Desc m_Desc;
};


class Texture2D : public Texture
{
public:
	Texture2D(const Texture::Desc& desc);

	virtual Texture::Type GetType() const;
};




class Texture3D : public Texture
{
public:
	Texture3D(const Texture::Desc& desc);

	virtual Texture::Type GetType() const;

	virtual uint GetFootprint() const;
};

class TextureCube : public Texture
{
public:
	TextureCube(const Texture::Desc& desc);

	virtual Texture::Type GetType() const;
};



class RenderTarget : public Texture2D
{
public:
	enum DefinedSize
	{
		DS_FRAMEBUFFER_WIDTH = 0x1000000,
		DS_FRAMEBUFFER_HEIGHT,
		DS_FRAMEBUFFER_WIDTH_2_1,
		DS_FRAMEBUFFER_HEIGHT_2_1,
		DS_FRAMEBUFFER_WIDTH_4_1,
		DS_FRAMEBUFFER_HEIGHT_4_1,
		DS_FRAMEBUFFER_WIDTH_8_1,
		DS_FRAMEBUFFER_HEIGHT_8_1,
		DS_FRAMEBUFFER_WIDTH_16_1,
		DS_FRAMEBUFFER_HEIGHT_16_1,
	};


	RenderTarget(const Texture::Desc& desc);
	~RenderTarget();

	uint GetRealWidth() const;

	uint GetRealHeight() const;


	void*		HALHandleRT;
};



class TextureManager
{
public:
	TextureManager();
	~TextureManager();

	void Initialize();
	void Finalize();
	static TextureManager*	Get();

	Texture*		GetTexture(const ResourceID& id);

private:
	Texture*		Find(const ResourceID& id);
	Texture*		Load(const ResourceID& id);

	map<ResourceID, Texture*>		m_ResourcePool;
};

extern TextureManager GTextureManager;

}

#endif