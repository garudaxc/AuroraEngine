/*
add by eva
*/

#ifndef TIXML_HELPER_INCLUDED
#define TIXML_HELPER_INCLUDED

#include "tinyxml.h"

// 部分功能与tinyutil.h中的功能重复，需要尽快删除一部分冗余功能
class TinyXmlHelper
{
public:
	// 获得对应节点的值，如没有返回0
	static const char* get_value(TiXmlNode* node, const char* key);
	// 获得对应节点的attribute，如果没有返回0
	static const char* get_attribute(TiXmlNode* node, const char* key);
};

// 得到对应节点的值
template<class T>
inline bool tiny_parse_xml_value(T& v, const char* key, TiXmlNode* node)
{
	if(const char* str = TinyXmlHelper::get_value(node, key))
	{
		v = str; // 这里会产生编译错误，以防止函数的误使用
		return true;
	}
	else
		return false;
}

template<>
inline bool tiny_parse_xml_value(int& v, const char* key, TiXmlNode* node)
{
	if(const char* str = TinyXmlHelper::get_value(node, key))
	{
		v = atoi(str);
		return true;
	}
	else
		return false;
}

template<>
inline bool tiny_parse_xml_value(unsigned int& v, const char* key, TiXmlNode* node)
{
	if(const char* str = TinyXmlHelper::get_value(node, key))
	{		
		v = atoi(str);
		return true;
	}
	else
		return false;
}

template<>
inline bool tiny_parse_xml_value(double& v, const char* key, TiXmlNode* node)
{
	if(const char* str = TinyXmlHelper::get_value(node, key))
	{
		v = atof(str);
		return true;
	}
	else
		return false;
}

template<>
inline bool tiny_parse_xml_value(float& v, const char* key, TiXmlNode* node)
{
	if(const char* str = TinyXmlHelper::get_value(node, key))
	{
		v = static_cast<float>(atof(str));
		return true;
	}
	else
		return false;
}


#endif // TIXML_HELPER_INCLUDED
