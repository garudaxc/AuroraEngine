#ifndef GARENDERERDX9_H
#define GARENDERERDX9_H

#include <d3d9.h>
#include <d3dx9.h>
#include "GaTypes.h"
#include "GaGeometryData.h"
#include "GaTexture.h"
#include "GaRenderer.h"

#pragma comment (lib, "d3d9.lib") 
#pragma comment (lib, "d3dx9.lib") 

namespace Ga
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

typedef shared_ptr<Shader>				ShaderPtr;
typedef shared_ptr<ShaderConstTable>	ShaderConstTablePtr;



class RendererDX9 : public Renderer
{
public:

	// static constrator
	static RendererPtr CreateDevice(HWND hWnd, int nWidth, int nHeight);


	RendererDX9(HWND hWnd, int nWidth, int nHeight);
	virtual ~RendererDX9(void);

#ifdef DIRECTX_VERSION_DX9

	// resourec creation
	bool									CreateVertexShader(const string& pathname, ShaderPtr& pShader, ShaderConstTablePtr& pShaderConstTable, const vector<pair<string, string>>& defines);
	bool									CreatePixelShader(const string& pathname, ShaderPtr& pShader, ShaderConstTablePtr& pShaderConstTable, const vector<pair<string, string>>& defines);

	Texture*								CreateTexture(const string& name);

	Texture*								CreateTexture(Texture::Type type, const Texture::Desc& desc);
	RenderTarget*							CreateRenderTarget(const RenderTarget::Desc& desc);

	VertexDescription*						CreateEmptyVertexDesc();
	LPDIRECT3DVERTEXDECLARATION9			CreateVertexDesc(CONST D3DVERTEXELEMENT9* pElem);

	VertexBuffer*							CreateVertexBuffer(GeometryData* pGeometry, const VertexDescription* pVertexDesc, uint nVert);
	IndexBuffer*							CreateIndexBuffer(GeometryData* pGeometry, IndexBuffer::Format fmt, uint nNumIndex);

#endif

	//virtual DepthStencilStateOjbect*		CreateCachedDSStateOjbect(const DepthStencilStateOjbect::Desc& desc);
	//virtual RasterStateObject*			CreateCachedRSStateOjbect(const RasterStateObject::Desc& desc);


private:

};




}


#endif