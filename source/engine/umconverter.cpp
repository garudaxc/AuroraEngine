#include "umconverter.h"

std::string& toString(std::string& str, const char* formats, ...)
{
	size_t size = 1024;

	static char s_buffer[1024];
	char* p = s_buffer;

	va_list list;
	va_start(list, formats);

	size_t psize = vsnprintf(p, size - 1, formats, list);

	va_end( list);

	if(psize > size - 1)
	{
		p[size - 1] = 0;
	}
	else if(psize == size - 1)
		p[size - 1] = 0;

	str = p;

	return str;
}