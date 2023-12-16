#pragma once
#include "ResourceManager.h"
#include "Primitive.h"
#include "MaterialInstance.h"
#include "CGeometry.h"
#include "Renderer.h"

namespace Aurora
{

class RenderTarget;
class PostProcessRendering;
class RenderingContext;


class PostProcessRendering
{
public:
	PostProcessRendering();
	~PostProcessRendering();

	void					Init();
	void					Render(RenderingContext* pContex);

private:
	MaterialInstance*				m_pMatInst;
	MaterialInstance*				m_pBlurHInst;
	MaterialInstance*				m_pBlurVInst;

	vector<VertexPT>				m_VertBuff;
	vector<uint16>					m_Index;
	ResourcePtr<RenderTarget>		m_pOutputRT;
};


}