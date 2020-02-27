#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "tinyxmlhelper.h"

const char* TinyXmlHelper::get_value(TiXmlNode* node, const char* key)
{
	TiXmlNode *item = node->FirstChild(key);
	if(item)
	{
		TiXmlNode* subnode = item->FirstChild();
		if(!subnode)
			return 0;
		else
			return subnode->Value();
	}
	else
		return 0;
}

const char* TinyXmlHelper::get_attribute(TiXmlNode* node, const char* key)
{
	TiXmlElement* ele = node->ToElement();
	if(ele)
	{
		return ele->Attribute(key);
	}
	else
		return 0;
}
