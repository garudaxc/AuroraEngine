#pragma once
#include <string>
#include <windows.h>

#define  GB18030 54936
class UMConverter
{
protected:
	bool		_IsMBCS;
	char*		_MBCSString;
	wchar_t*	_UnicodeString;

public:
	// 接收两种类型的字符串的构造函数
	UMConverter( const wchar_t *str );
	UMConverter( const char *str);

	~UMConverter( void );

	// 获得multibyte的字符串
	const char *GetMBCS( void ) const { return( _MBCSString ); };
	// 获得unicode的字符串
	const wchar_t *GetUnicode( void ) const { return( _UnicodeString ); };

	static const char* GetMBFromUTF8(const char* utf8){ return utf8; };

protected:
	UMConverter( void );
private:
	// disable
	UMConverter(const UMConverter&);
	const UMConverter& operator=(const UMConverter&);
};


inline UMConverter::UMConverter( const char *str )
	: _IsMBCS( true )
	, _UnicodeString( NULL )
	, _MBCSString(NULL)
{
	if ( str == NULL )
	{
		_MBCSString = NULL;
		return;
	}
	int len = ::MultiByteToWideChar( GB18030, 0, str, strlen( str ) + 1,	NULL, 0 );
	_UnicodeString = new wchar_t[ len ];
	::MultiByteToWideChar( GB18030, 0, str, strlen( str ) + 1, _UnicodeString, len );

	_MBCSString = const_cast< char * >( str );
}

inline UMConverter::UMConverter( const wchar_t *str )
	: _IsMBCS( false )
	, _MBCSString( NULL )
{
	if ( str == NULL )
	{
		_UnicodeString = NULL;
		return;
	}
	int len = ::WideCharToMultiByte( GB18030, 0, str, wcslen( str ) + 1,	NULL, 0, NULL, NULL );
	_MBCSString = new char[ len ];
	::WideCharToMultiByte( GB18030, 0, str, wcslen( str ) + 1, _MBCSString, len, NULL, NULL );

	_UnicodeString = const_cast< wchar_t * >( str );
}

inline UMConverter::~UMConverter( void )
{
	if ( _IsMBCS )
	{
		if ( _UnicodeString != NULL )
		{
			delete []_UnicodeString;
			_UnicodeString = NULL;
		}
	}
	else
	{
		if ( _MBCSString != NULL )
		{
			delete []_MBCSString;
			_MBCSString = NULL;
		}
	}
}

std::string& toString(std::string& str, const char* formats, ...);