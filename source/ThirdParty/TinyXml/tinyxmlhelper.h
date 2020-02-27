/*
add by eva
*/

#ifndef TIXML_HELPER_INCLUDED
#define TIXML_HELPER_INCLUDED

#include "tinyxml.h"

// ���ֹ�����tinyutil.h�еĹ����ظ�����Ҫ����ɾ��һ�������๦��
class TinyXmlHelper
{
public:
	// ��ö�Ӧ�ڵ��ֵ����û�з���0
	static const char* get_value(TiXmlNode* node, const char* key);
	// ��ö�Ӧ�ڵ��attribute�����û�з���0
	static const char* get_attribute(TiXmlNode* node, const char* key);
};

// �õ���Ӧ�ڵ��ֵ
template<class T>
inline bool tiny_parse_xml_value(T& v, const char* key, TiXmlNode* node)
{
	if(const char* str = TinyXmlHelper::get_value(node, key))
	{
		v = str; // ����������������Է�ֹ��������ʹ��
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
