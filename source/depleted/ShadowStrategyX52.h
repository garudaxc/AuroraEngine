/********************************************************************
created:	2010/08/31
author:		Garuda

purpose:	阴影算法策略。 用于炫舞2
*********************************************************************/

#pragma once
#include "H3DSTLHeader.h"
#include "ShadowStrategy.h"
#include "ViewPort.h"
#include "ShaderDefine.h"
#include "MyPlane.h"
//#include "engine_interface.h"

namespace H3D
{
	class H3DMaterial;
	class SubEntity;
	class Entity;
	class TextureBase;
	class RenderTarget;
	class H3DShaderEffect;
	class PrePassLight;
	class FxGroup;
	class PrePassLightDirectional;
	class PrePassLightSpot;
	class NewRenderOperation;
	class MaterialBallGroup;

	///每个render group中存在若干fx group
	///key是fx的文件名
	typedef H3DMap<H3DString,FxGroup*> FxGroupMap;
	typedef FxGroupMap::iterator FxGroupMapItr;


	class ShadowParameter
	{
	public:
		enum SHADOW_QUALITY
		{
			SHADOW_QUALITY_HIGH = 0,
			SHADOW_QUALITY_MID,
			SHADOW_QUALITY_LOW,
			SHADOW_QUALITY_OFF,
		};

		int		nQualityLevel;
		uint32	bShadowEnable : 1;
		uint32	bUseSoftShadow : 1;
		uint32	bUseCharacterShadow : 1;
		int		nNumPass;
		int		nShadowMapWidth;
		int		nShadowMapHeight;
		int		nAtlasSize;
		float	fShadowStart;
		float	fShadowEnd;
		float	fFadeoutStart;
		float	fSplitPos[4];
		float	fDepthBias;
		float	fSlopDepthBias;

		void	SetQualityLevel(int nLevel);
	};




	class ShadowStrategyX52 : public ShadowStrategy
	{
	public:
		ShadowStrategyX52();
		virtual ~ShadowStrategyX52();

		virtual bool	Init();

		///放置Renderable
		virtual int		PushRenderable(Renderable* );
		virtual int		PushShadowLight(Renderable* );

		///渲染管线
		virtual int		Render();
		virtual void	Cleanup();


		void			PreSSShadow();
		void			PostSSShadow();

		void			SetQualityLevel(int nLevel);
		//设置软阴影级别　０为关，１为开
		void			SetSoftShadowLevel(int nLevel);

		RenderTarget*	GetShadowMap();
		RenderTarget*	GetShadowDepthBuffer();
		void			ReturnRenderTarget();
	private:
		void			AllocResources();

		void			PrepareRender();

		// 初始化光源参数，判断每个物体在哪个优先级最高的光源照射范围内
		void			PrepareLight();

		void			CullShadowCasters();

		void			SetupParameter();

		void			CollectSceneShadow(PrePassLight* pLight, const H3DMat4* pMatDirProj);

		void			CollectCharacterShadow(PrePassLight* pLight, const H3DMat4& matShadowViewProj);

		void			RenderCharacterShadow();

		// 画render target上的一个分块
		void			RenderOneAtlas(PrePassLight* pLight, const H3DVector<Plane>& cullingPlanes);


		// 设置某一atlas的view port
		void			SetupViewPort(int nAtlas);

		// 计算某一个atlas对应的投影矩阵(从世界空间到贴图空间)
		H3DMat4			CalcTexProjMatrix(const H3DMat4& matProj, int nAtlas);


		// 渲染shadow map的全局方向光实现
		int				RenderLightImplDirectional(int nBeginAtlas, PrePassLightDirectional* pLight);

		// 渲染shadow map的聚光灯实现
		int				RenderLightImplSpot(int nBeginAtlas, PrePassLightSpot* pLight);

		// 本帧是否需要投射阴影
		bool			NeedRenderShadowmap();

		// 管线渲染

		// 渲染一个atlas时，激活一个renderable
		void			ActiveRenderableToPass(Renderable* pRenderable);

		///判断这个SubEntity是否能够使用cache的render op 继续渲染
		bool			IsCached(Renderable* pSubEntity);

		///生成op，分配到fxgroup中
		void			AssignFxGroup(Renderable* pRenderable,H3DMaterial* pMat);

		bool			ActiveCacheOp(H3DString& fxPath,H3DString& matName,MAT_ID id,NewRenderOperation* op,FxGroup* pFxGroup,MaterialBallGroup* pMatBallGroup);

		///得到一个FxGroup，如果没有则创建
		FxGroup*		GetFxGroup(H3DString& fxName);

		void			RenderRenderable();



		ShadowParameter			m_ShadowParameter;

		ViewPort				m_SavedVP;				// 临时保存view port

		RenderTarget*			m_pShadowMapColor;
		RenderTarget*			m_pShadowMapDepth;

		RenderTarget*			m_pScreenSpaceShadowRT;
		Entity*					m_pEntitySSShadow;
		Entity*					m_pEntityCharacterCollecter;
		Entity*					m_pEntityShadowMask;

		H3DSet<Entity*>				m_CharacterSet;

		H3DVector<PrePassLight*>	m_Lights;				// 投射阴影的光源
		H3DVector<Renderable*>		m_CasterObjs;			// 投射阴影的物体

		H3DVector<PrePassLight*>	m_PrincipalLight;		// 每个投射物体接受的，阴影优先级最高的光源

		H3DMat4						m_dirProjMatrix[3];

		///fx group
		FxGroupMap				m_FxGroupMap;

		int						m_nSoftShadowLevel;
	};

}