#ifndef TINYXML_UTIL
#define TINYXML_UTIL

#include "tinyxml.h"

/*
#t tinyxml

star_clʹ��tinyxml����xml�ļ��Ķ�ȡ������ʹ��TinyXmlUtility��TinyXmlHelper��tiny_parse_xml_value�������ļ���ȡ�Ĵ�������
tinyxmlʹ�õ��Ƕ��ֽڱ��뷽ʽ�������Ҫ��Ϊ��Ϸ�ڲ��ܹ����ܵ��ַ�������Ҫ���б�����ʽ��ת����

*/

// tiny xml��ȡ������
class TinyXmlUtility
{
public:
	// ��ø���key�Ľڵ��ֵ�����û�з���0
	static const char* get_value(const char* key, TiXmlElement *item);
	// ��ýڵ��ֵ�����û�з���0
	static const char* get_value(TiXmlElement *item);
	// ��ýڵ�Ķ�Ӧ��key��attributeֵ�����û�з���0
	static const char* get_attribute(TiXmlNode* node, const char* key);
};


#endif // TINYXML_UTIL

