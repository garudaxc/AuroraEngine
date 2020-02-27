#include "tinyutil.h"

const char* TinyXmlUtility::get_value(const char* key, TiXmlElement *item)
{
	if(TiXmlElement* subitem = item->FirstChildElement(key))
	{
		return TinyXmlUtility::get_value(subitem);
	}
	else
		return 0;
}

const char* TinyXmlUtility::get_value(TiXmlElement *item)
{
	TiXmlNode *childNode = item->FirstChild();
	if ( childNode == NULL )
		return 0;
	TiXmlText *childText = childNode->ToText();
	if ( childText == NULL )
		return 0;

	return childText->Value();
}

const char* TinyXmlUtility::get_attribute(TiXmlNode* node, const char* key)
{
	TiXmlElement* ele = node->ToElement();
	if(ele)
	{
		return ele->Attribute(key);
	}
	else
		return 0;
}
