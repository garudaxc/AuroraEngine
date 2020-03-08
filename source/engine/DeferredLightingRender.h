#pragma once
#include "ResourceManager.h"

namespace Aurora
{

class RenderingContext;
class GBufferRendering;


class GBufferRendering
{
public:
	GBufferRendering();
	~GBufferRendering();

	void					Init();
	void					Render(RenderingContext* pContex);

private:
	ResourcePtr<RenderTarget>			m_pRTNormal;
	ResourcePtr<RenderTarget>			m_pDepth;
};

//////////////////////////////////////////////////////////////////////////

class OcclusionQuery;
class ShadingRendering;


class ShadingRendering
{
public:
	ShadingRendering();
	~ShadingRendering();

	void					Init();
	void					Render(RenderingContext* pContex, OcclusionQuery* pQuery);
private:
	ResourcePtr<RenderTarget>			m_pRTColor;
};

//////////////////////////////////////////////////////////////////////////





class ShaderManager;
class ShaderVariable;
class LightRendering;


// pre-pass light rendering

class PPLRendering
{
public:
	PPLRendering();
	~PPLRendering();


	void		Initialize(ShaderManager* pSVManager);
	void		Finalize();

	void		Render(RenderingContext* pContex);

private:

	void		RenderGBufferPass(RenderingContext* pContex);
	void		RenderLightingPass(RenderingContext* pContex);
	void		RenderShadingPass(RenderingContext* pContex);

	RenderTarget*	m_pRTGbuffer0;
	RenderTarget*	m_pRTGbuffer1;
	RenderTarget*	m_pRTLighting;

	LightRendering*	m_pLightRendering;

};





}
