#include "stdHeader.h"
#include "LightNode.h"
#include "Util.h"
#include "Color.h"
#include "Model.h"

namespace Aurora
{


//
//	LightNode::LightNode()
//	{
//
//	}
//
//	LightNode::~LightNode()
//	{
//
//	}
//
//	void LightNode::EnumEditableProperty(vector<pair<string, string>>& properties)
//	{
//		super::EnumEditableProperty(properties);
//		properties.push_back(make_pair("Color", "color"));
//		properties.push_back(make_pair("Power", "float"));
//		properties.push_back(make_pair("CastShadow", "bool"));
//	}
//
//	const string LightNode::GetEditableProperty(const string& propertyName)
//	{
//		LightObject* pLight = (LightObject*)GetAttachable(0);
//		if (propertyName == "Color")
//		{
//			return pLight->GetBaseColor().ToStringRGB();
//		}
//		else if (propertyName == "Power")
//		{
//			char buffer[32];
//			sprintf_s(buffer, sizeof(buffer), "%.3f", pLight->GetMultiplier());
//			return buffer;
//		}
//		else if (propertyName == "CastShadow")
//		{
//			return pLight->IsCastShadow() ? "1" : "0";
//		}
//		else
//		{
//			return super::GetEditableProperty(propertyName);
//		}
//	}
//
//	void LightNode::SetEditableProperty(const string& propertyName, const string& value)
//	{
//
//		LightObject* pLight = (LightObject*)GetAttachable(0);
//		if (propertyName == "Color")
//		{
//			Color c;
//			c.FromStringRGB(value);
//			pLight->SetBaseColor(c);
//		}
//		else if (propertyName == "Power")
//		{
//
//		}
//		else if (propertyName == "CastShadow")
//		{
//			if (value == "1")
//			{
//				pLight->SetCastShadow(true);
//			}
//			else
//			{
//				pLight->SetCastShadow(false);
//			}
//		}
//		else
//		{
//			super::SetEditableProperty(propertyName, value);
//		}
//	}
//
//	void LightNode::Archive(TiXmlNode* pXmlLight, bool bRead)
//	{
//	}
//
//
//	//////////////////////////////////////////////////////////////////////////
//
//
//
//	PointLightNode::PointLightNode()
//	{
//		LightObjectPtr pObject;
//		pObject.reset(new PointLight(Color::WHITE, 1.0f, 0.0f, 100.0f));
//		pObject->Init();
//		Attach(pObject);
//	}
//
//	PointLightNode::~PointLightNode()
//	{
//
//	}
//
//	const string& PointLightNode::GetTypeName() const
//	{
//		static string typeName = "PointLight";
//		return typeName;
//	}
//
//	void PointLightNode::Archive(TiXmlNode* pXmlLightNode, bool bRead)
//	{
//		SceneNode::Archive(pXmlLightNode, bRead);
//
//		if (bRead)
//		{
//			PointLight* pObject = (PointLight*)GetAttachable(0);
//
//			float fMultiplayer = 1.0f;
//			Color color = Color::WHITE;
//			float attenStart = 20.0f;
//			float attenEnd = 100.0f;
//			bool bCastShadow = false;
//
//			TiXmlNode* pElem = pXmlLightNode->FirstChild("Light");
//			assert(pElem);
//
//			const char* pAttr = pElem->ToElement()->Attribute("CastShadow");
//			if (pAttr && atoi(pAttr) == 1)
//			{
//				bCastShadow = true;
//			}
//
//			pElem = pElem->FirstChild();
//
//			while (pElem)
//			{
//				if (strcmp(pElem->Value(), "Color") == 0)
//				{
//					string text = pElem->FirstChild()->ToText()->Value();
//
//					color = ParseColor(text);
//				}
//				else if (strcmp(pElem->Value(), "Multiplier") == 0)
//				{
//					string text = pElem->FirstChild()->ToText()->Value();
//					fMultiplayer = (float)atof(text.c_str());
//				}
//				else if (strcmp(pElem->Value(), "AttenStart") == 0)
//				{
//					string text = pElem->FirstChild()->ToText()->Value();
//					attenStart = (float)atof(text.c_str());
//				}
//				else if (strcmp(pElem->Value(), "AttenEnd") == 0)
//				{
//					string text = pElem->FirstChild()->ToText()->Value();
//					attenEnd = (float)atof(text.c_str());
//				}
//
//				pElem = pElem->NextSibling();
//			}
//
//			pObject->SetBaseColor(color);
//			pObject->SetMultiplier(fMultiplayer);
//			pObject->SetCastShadow(bCastShadow);
//			pObject->SetAttenStart(attenStart);
//			pObject->SetAttenEnd(attenEnd);
//		}
//		else
//		{
//			TiXmlElement* pElemNode = (TiXmlElement*)pXmlLightNode;
//			TiXmlElement* pXmlLight = LinkNewXmlElement(pElemNode, "Light");
//
//			PointLight* pLight = (PointLight*)GetAttachable(0);
//			pXmlLight->SetAttribute("CastShadow", pLight->IsCastShadow() ? "1" : "0");
//
//			char buff[128];
//			Color color = pLight->GetBaseColor();
//			sprintf_s(buff, sizeof(buff), "%.3f,%.3f,%.3f", color.r, color.g, color.b);
//			LinkNewXmlText(LinkNewXmlElement(pXmlLight, "Color"), buff);
//
//			float fMultiplayer = pLight->GetMultiplier();
//			sprintf_s(buff, sizeof(buff), "%.3f", fMultiplayer);
//			LinkNewXmlText(LinkNewXmlElement(pXmlLight, "Multiplier"), buff);
//
//			float fAttenStart = pLight->GetAttenStart();
//			sprintf_s(buff, sizeof(buff), "%.3f", fAttenStart);
//			LinkNewXmlText(LinkNewXmlElement(pXmlLight, "AttenStart"), buff);
//
//			float fAttenEnd = pLight->GetAttenEnd();
//			sprintf_s(buff, sizeof(buff), "%.3f", fAttenEnd);
//			LinkNewXmlText(LinkNewXmlElement(pXmlLight, "AttenEnd"), buff);
//		}
//
//	}
//
//
//	//////////////////////////////////////////////////////////////////////////
//
//	DirectionalLightNode::DirectionalLightNode()
//	{
//		LightObjectPtr pObject;
//		pObject.reset(new DirectionalLight(Color::WHITE, 1.0f));
//		pObject->Init();
//		Attach(pObject);
//	}
//
//	DirectionalLightNode::~DirectionalLightNode()
//	{
//
//	}
//
//	const string& DirectionalLightNode::GetTypeName() const
//	{
//		static string typeName = "DirectionalLight";
//		return typeName;
//	}
//
//	void DirectionalLightNode::Archive(TiXmlNode* pXmlLightNode, bool bRead)
//	{
//		SceneNode::Archive(pXmlLightNode, bRead);
//
//		if (bRead)
//		{
//			//LightObjectPtr pObject;
//
//			LightObject* pObject = (LightObject*)GetAttachable(0);
//
//			float fMultiplayer = 1.0f;
//			Color color = Color::WHITE;
//			bool bCastShadow = false;
//
//			TiXmlNode* pElem = pXmlLightNode->FirstChild("Light");
//			assert(pElem);
//
//			const char* pAttr = pElem->ToElement()->Attribute("CastShadow");
//			if (pAttr && atoi(pAttr) == 1)
//			{
//				bCastShadow = true;
//			}
//
//			pElem = pElem->FirstChild();
//
//			while (pElem)
//			{
//				if (strcmp(pElem->Value(), "Color") == 0)
//				{
//					string text = pElem->FirstChild()->ToText()->Value();
//
//					color = ParseColor(text);
//				}
//				else if (strcmp(pElem->Value(), "Multiplier") == 0)
//				{
//					string text = pElem->FirstChild()->ToText()->Value();
//					fMultiplayer = (float)atof(text.c_str());
//				}
//
//				pElem = pElem->NextSibling();
//			}
//
//			pObject->SetBaseColor(color);
//			pObject->SetMultiplier(fMultiplayer);
//			pObject->SetCastShadow(bCastShadow);
//
//			/*pAttr = pXmlLight->ToElement()->Attribute("ProjTex");
//			if (pAttr)
//			{
//			pObject->SetProjTexture(ConvertFileName(pAttr));
//			}
//
//			pElem = pXmlLight->FirstChild("Controller");
//			if (pElem)
//			{
//			LoadXmlLightController(pElem->ToElement(), pObject.get());
//			}*/
//		}
//		else
//		{
//			TiXmlElement* pElemNode = (TiXmlElement*)pXmlLightNode;
//			TiXmlElement* pXmlLight = LinkNewXmlElement(pElemNode, "Light");
//
//			LightObject* pLight = (LightObject*)GetAttachable(0);
//			pXmlLight->SetAttribute("CastShadow", pLight->IsCastShadow() ? "1" : "0");
//
//			char buff[128];
//			Color color = pLight->GetBaseColor();
//			sprintf_s(buff, sizeof(buff), "%.3f,%.3f,%.3f", color.r, color.g, color.b);
//			LinkNewXmlText(LinkNewXmlElement(pXmlLight, "Color"), buff);
//
//			float fMultiplayer = pLight->GetMultiplier();
//			sprintf_s(buff, sizeof(buff), "%.3f", fMultiplayer);
//			LinkNewXmlText(LinkNewXmlElement(pXmlLight, "Multiplier"), buff);
//		}
//	}
//
////////////////////////////////////////////////////////////////////////////
//
//	SpotLightNode::SpotLightNode()
//	{
//
//	}
//
//	SpotLightNode::~SpotLightNode()
//	{
//
//	}
//
//	const string& SpotLightNode::GetTypeName() const
//	{
//		static string typeName = "SpotLight";
//		return typeName;
//	}
//
//	void SpotLightNode::Archive(TiXmlNode* pXmlLightNode, bool bRead)
//	{
//
//	}

}