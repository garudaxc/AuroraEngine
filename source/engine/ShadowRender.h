#pragma once
#include "ResourceManager.h"

namespace Aurora
{

class ShadowRendering;
class RenderingContext;


class ShadowRendering
{
public:
	ShadowRendering();
	~ShadowRendering();

	void Init();
	void Render(RenderingContext* pContex);

private:
	ResourcePtr<RenderTarget>			m_pRTShadowMap;
	ResourcePtr<RenderTarget>			m_pRTDummy;

	//MaterialInstance*					m_pShadowMaterial;
	//ShaderConstParamPtr					m_pMatShadowCastParam;
};





}