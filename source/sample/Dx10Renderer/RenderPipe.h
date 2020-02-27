#pragma once
#include "GaResourceManager.h"
#include "GaRenderProcess.h"

namespace Ga
{

	class RenderTarget;
	class Renderable;
	class LightObject;
	class Material;
	class Camera;
	class MaterialInstance;
	class OcclusionQuery;
	class HelperRenderer;
	class SceneView;
	class Entity;
	class GlobalShaderVarManager;
	class RenderStrategy;
	class ShaderVariable;

	typedef shared_ptr<MaterialInstance>	MaterialInstancePtr;


}

using namespace Ga;


	class RenderPipe
	{
	public:

		RenderPipe(void);
		~RenderPipe(void);

		void			Init();

		void			RenderSceneView(SceneView* pView);

		void			BeginPipe();
		void			EndPipe();

		void			DrawLine();

	private:

		void Render(RenderingContext* pContex);

		void RenderEntity(RenderStrategy* pRenderStrategy, uint nTechnique);

		void			SetupShaderVariables(Camera* pCamera);

		GlobalShaderVarManager*	m_pShaderVarManager;

		ShaderVariable*		m_pSVMatrixViewProj;
		ShaderVariable*		m_pSVCameraParam;

	};

