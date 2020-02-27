/********************************************************************
created:	2010/08/31
author:		Garuda

purpose:	��Ӱ�㷨���ԡ� ��������2
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

	///ÿ��render group�д�������fx group
	///key��fx���ļ���
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

		///����Renderable
		virtual int		PushRenderable(Renderable* );
		virtual int		PushShadowLight(Renderable* );

		///��Ⱦ����
		virtual int		Render();
		virtual void	Cleanup();


		void			PreSSShadow();
		void			PostSSShadow();

		void			SetQualityLevel(int nLevel);
		//��������Ӱ���𡡣�Ϊ�أ���Ϊ��
		void			SetSoftShadowLevel(int nLevel);

		RenderTarget*	GetShadowMap();
		RenderTarget*	GetShadowDepthBuffer();
		void			ReturnRenderTarget();
	private:
		void			AllocResources();

		void			PrepareRender();

		// ��ʼ����Դ�������ж�ÿ���������ĸ����ȼ���ߵĹ�Դ���䷶Χ��
		void			PrepareLight();

		void			CullShadowCasters();

		void			SetupParameter();

		void			CollectSceneShadow(PrePassLight* pLight, const H3DMat4* pMatDirProj);

		void			CollectCharacterShadow(PrePassLight* pLight, const H3DMat4& matShadowViewProj);

		void			RenderCharacterShadow();

		// ��render target�ϵ�һ���ֿ�
		void			RenderOneAtlas(PrePassLight* pLight, const H3DVector<Plane>& cullingPlanes);


		// ����ĳһatlas��view port
		void			SetupViewPort(int nAtlas);

		// ����ĳһ��atlas��Ӧ��ͶӰ����(������ռ䵽��ͼ�ռ�)
		H3DMat4			CalcTexProjMatrix(const H3DMat4& matProj, int nAtlas);


		// ��Ⱦshadow map��ȫ�ַ����ʵ��
		int				RenderLightImplDirectional(int nBeginAtlas, PrePassLightDirectional* pLight);

		// ��Ⱦshadow map�ľ۹��ʵ��
		int				RenderLightImplSpot(int nBeginAtlas, PrePassLightSpot* pLight);

		// ��֡�Ƿ���ҪͶ����Ӱ
		bool			NeedRenderShadowmap();

		// ������Ⱦ

		// ��Ⱦһ��atlasʱ������һ��renderable
		void			ActiveRenderableToPass(Renderable* pRenderable);

		///�ж����SubEntity�Ƿ��ܹ�ʹ��cache��render op ������Ⱦ
		bool			IsCached(Renderable* pSubEntity);

		///����op�����䵽fxgroup��
		void			AssignFxGroup(Renderable* pRenderable,H3DMaterial* pMat);

		bool			ActiveCacheOp(H3DString& fxPath,H3DString& matName,MAT_ID id,NewRenderOperation* op,FxGroup* pFxGroup,MaterialBallGroup* pMatBallGroup);

		///�õ�һ��FxGroup�����û���򴴽�
		FxGroup*		GetFxGroup(H3DString& fxName);

		void			RenderRenderable();



		ShadowParameter			m_ShadowParameter;

		ViewPort				m_SavedVP;				// ��ʱ����view port

		RenderTarget*			m_pShadowMapColor;
		RenderTarget*			m_pShadowMapDepth;

		RenderTarget*			m_pScreenSpaceShadowRT;
		Entity*					m_pEntitySSShadow;
		Entity*					m_pEntityCharacterCollecter;
		Entity*					m_pEntityShadowMask;

		H3DSet<Entity*>				m_CharacterSet;

		H3DVector<PrePassLight*>	m_Lights;				// Ͷ����Ӱ�Ĺ�Դ
		H3DVector<Renderable*>		m_CasterObjs;			// Ͷ����Ӱ������

		H3DVector<PrePassLight*>	m_PrincipalLight;		// ÿ��Ͷ��������ܵģ���Ӱ���ȼ���ߵĹ�Դ

		H3DMat4						m_dirProjMatrix[3];

		///fx group
		FxGroupMap				m_FxGroupMap;

		int						m_nSoftShadowLevel;
	};

}