#pragma once
#include "stdheader.h"
#include "ResourceManager.h"

namespace Aurora
{

class Pipeline;
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
class ShaderManager;
class RenderStrategy;
class ShaderVariable;


class Pipeline
{
public:

	Pipeline(void);
	~Pipeline(void);

	void	Initialize(int width, int height);
	void	Finalize();

	
	void	RenderSceneView(SceneView* pView);

	void	OnReset();
	void	OnLost();

	void	BeginPipe();
	void	Render();
	void	EndPipe();

	void	Clear();

	void	BeginOcclusionQuery(int x0, int y0, int x1, int y1);
	int		EndOcclusionQuery();

	void	DrawLine(const Vector3f& a, const Vector3f& b, const Color& color, int pickupId);
	void	DrawLineList(const Vector3f* pPos, uint nNumPoint, const Color& color, int pickupId);
	void	DrawCicle(const Vector3f& center, float radius, const Vector3f& dir, const Color& color, int pickupId);
	void	DrawAABB(const Vector3f& a, const Vector3f& b, const Color& color, int pickupId);
		
private:
	void	SetupShaderVariables(SceneView* pView);

	bool		m_bOcclusionQuerying;

	unique_ptr<OcclusionQuery>	m_pOcclusionQuery;
	unique_ptr<HelperRenderer>	m_pHelperRenderer;

};




extern Pipeline GPipeline;

}