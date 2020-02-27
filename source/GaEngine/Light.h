#pragma once
#include "Types.h"
#include "ResourceManager.h"
#include "Entity.h"
namespace Aurora
{

	class Model;
	struct LightData;


Model* CreateLightObject(LightData* pLightData);



struct LightRenderingInfo
{
	RenderTarget*		pLightingRT;

	const vector<RenderStrategy*>* lights;
};


class LightRendering
{
public:
	LightRendering();
	~LightRendering();

	void			Initialize();
	void			Render(LightRenderingInfo* renderInfo);

private:

};

	enum ELightType
	{
		LT_DIRECTIONAL = 0,
		LT_POINT,
		LT_SPOT,
	};

//class Light
//{
//public:
//
//	ELightType m_Type;
//
//	Transform m_Transform;
//
//	Color m_cBaseColor;
//	float m_fMultiplier;
//
//	Color cShadowColor;
//	
//	float m_fAttenStart;
//	float m_fAttenEnd;
//	float m_fInnerCone;
//	float m_fOutterCone;
//
//	uint bCastShadow : 1;
//};



class Light
{
public:
	Light(ELightType lightType);

	ELightType Type;

	Transform m_Transform;

	Color cBaseColor;
	float fMultiplier;

	Color cShadowColor;

	float fRange;		// ((range - d) / range)^2
	float fInnerCone;
	float fOutterCone;

	uint bCastShadow : 1;
};





//
//
//
//class DirectionalLight : public Light
//{
//public:
//	DirectionalLight(const Color& color, float multiplier);
//	virtual ~DirectionalLight();
//
//	virtual bool				Init();
//
//	const Vector3f				GetDirection() const			{	return m_vDirection;	}
//
//	virtual bool				TestInside(const Vector3f& pos);
//	virtual void				UpdateFromParentNode();
//
//	virtual Matrix4f			GetViewProjMatrix() const;
//
//	void						SetShadowReceiveMatrix(const Matrix4f& mat)		{	m_matShadowReceive = mat;	}
//
//	virtual void				ApplyShadow();
//
//private:
//
//	Vector3f					m_vDirection;
//	Matrix4f					m_matViewProj;
//	Matrix4f					m_matShadowReceive;
//	ShaderConstParamPtr			m_pShadowMapParam;
//};

//
//class PointLight : public LightObject
//{
//public:
//	PointLight(const Color& color, float multiplier, float fAttenStart, float fAttenEnd);
//	virtual ~PointLight();
//	virtual bool				Init();
//
//	const Vector3f&				GetPosition() const;
//	void						SetAttenStart(float start);
//	void						SetAttenEnd(float end);
//	float						GetAttenStart() const;
//	float						GetAttenEnd() const;
//
//	virtual bool				TestInside(const Vector3f& pos);
//	virtual void				UpdateFromParentNode();
//
//private:
//	Vector3f					m_vPosition;
//
//	float						m_fAttenStart;
//	float						m_fAttenEnd;
//};
//
//inline const Vector3f& PointLight::GetPosition() const
//{
//	return m_vPosition;	
//}
//
//inline float PointLight::GetAttenStart() const
//{
//	return m_fAttenStart;
//}
//
//inline float PointLight::GetAttenEnd() const
//{
//	return m_fAttenEnd;
//}
//
//inline void PointLight::SetAttenStart(float start)
//{
//	m_fAttenStart = start;
//}
//
//inline void PointLight::SetAttenEnd(float end)
//{
//	m_fAttenEnd = end;
//}
//
//
//class SpotLight : public LightObject
//{
//public:
//	SpotLight(const Color& color, float multiplier, float fInnerCone, float fOutterCone);
//	virtual ~SpotLight();
//
//
//	virtual bool				Init();
//
//	const Vector3f&				GetPosition() const				{	return m_vPosition;	}
//	const Vector3f&				GetDirection() const			{	return m_vDirection;	}
//
//	virtual bool				TestInside(const Vector3f& pos);
//	virtual void				UpdateFromParentNode();
//
//	virtual Matrix4f			GetViewProjMatrix() const;
//
//private:
//	Vector3f					m_vPosition;
//	Vector3f					m_vDirection;
//
//	float						m_fInnerCone;
//	float						m_fOutterCone;
//
//	float						m_fCosHalfOutAngle;
//	Matrix4f					m_matViewProj;
//
//	ShaderConstParamPtr			m_pShadowMapParam;
//
//};


}
