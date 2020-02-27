#ifndef TINYXML_UTIL
#define TINYXML_UTIL

#include "tinyxml.h"

/*
#t tinyxml

star_cl使用tinyxml进行xml文件的读取，可以使用TinyXmlUtility或TinyXmlHelper和tiny_parse_xml_value来减少文件读取的代码量．
tinyxml使用的是多字节编码方式，如果需要变为游戏内部能够接受的字符串，需要进行编码形式的转换．

*/

// tiny xml读取帮助类
class TinyXmlUtility
{
public:
	// 获得给定key的节点的值，如果没有返回0
	static const char* get_value(const char* key, TiXmlElement *item);
	// 获得节点的值，如果没有返回0
	static const char* get_value(TiXmlElement *item);
	// 获得节点的对应的key的attribute值，如果没有返回0
	static const char* get_attribute(TiXmlNode* node, const char* key);
};


#endif // TINYXML_UTIL

