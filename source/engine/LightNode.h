#pragma once
#include "Entity.h"
#include "Light.h"


namespace Aurora
{


	//class RenderOperator;
	//class MaterialInstance;
	//class Renderable;
	//class RenderOperator;
	//class Model;


	//static char* LightTypeName[] = 
	//{
	//	"LT_DIRECTIONAL",
	//	"LT_POINT",
	//	"LT_SPOT",
	//};


	////void LoadXmlLight(TiXmlNode* pXmlLight, SceneNode* pSceneNode);


	//enum LightType
	//{
	//	LIGHTTYPE_DIRECTIONAL = 0,
	//	LIGHTTYPE_POINT,
	//	LIGHTTYPE_SPOT,
	//};

	//struct LightData
	//{
	//	int			lightType;
	//	Color		cBaseColor;
	//	float		fStrength;
	//	Color		cShadowColor;
	//	float		fAttenStart;
	//	float		fAttenEnd;
	//	uint		nPriority;

	//	Matrix4f	matTransform;
	//};



	//class Light : public RenderStrategy
	//{
	//public:
	//	Light();
	//	virtual ~Light();

	//	void			Update(LightData* pLightObj);

	//	virtual	void	Initialize()		{};

	//	virtual uint		GetNumRenderable();
	//	virtual Renderable*	GetRenderable(uint idx);

	//	virtual void		PushToPipe(RenderingContext* pContext);

	//protected:
	//	LightData	m_LightData;

	//	Model*		m_pLightObject;
	//};




	//class LightManager
	//{
	//public:

	//	static void			Initalize();
	//	static void			Finialize();
	//	static LightManager*	Get();

	//	LightManager();
	//	~LightManager();

	//	Light*		CreateLight(TiXmlElement* pXmlLight);

	//	Light*		CreateDirectionalLight();

	//};



	//class LightNode : public Entity
	//{
	//public:
	//	typedef SceneNode super;

	//	LightNode();
	//	~LightNode();

	//	virtual void			EnumEditableProperty(vector<pair<string, string>>& properties);
	//	virtual const string	GetEditableProperty(const string& propertyName);
	//	virtual void			SetEditableProperty(const string& propertyName, const string& value);

	//	virtual void			Archive(TiXmlNode* pXmlLightNode, bool bRead);

	//};

	//class PointLightNode : public LightNode
	//{
	//public:
	//	PointLightNode();
	//	~PointLightNode();
	//	virtual const string&		GetTypeName() const;


	//	virtual void				Archive(TiXmlNode* pXmlLightNode, bool bRead);
	//};


	//class DirectionalLightNode : public LightNode
	//{
	//public:
	//	DirectionalLightNode();
	//	~DirectionalLightNode();

	//	virtual const string&		GetTypeName() const;
	//	virtual void				Archive(TiXmlNode* pXmlLightNode, bool bRead);
	//};

	//class SpotLightNode : public LightNode
	//{
	//public:
	//	SpotLightNode();
	//	~SpotLightNode();
	//	virtual const string&		GetTypeName() const;
	//	virtual void				Archive(TiXmlNode* pXmlLightNode, bool bRead);
	//};



}
