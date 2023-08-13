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
	// �����������͵��ַ����Ĺ��캯��
	UMConverter( const wchar_t *str );
	UMConverter( const char *str);

	~UMConverter( void );

	// ���multibyte���ַ���
	const char *GetMBCS( void ) const { return( _MBCSString ); };
	// ���unicode���ַ���
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
	, _UnicodeString( nullptr )
	, _MBCSString(nullptr)
{
	if ( str == nullptr )
	{
		_MBCSString = nullptr;
		return;
	}
	int len = ::MultiByteToWideChar( GB18030, 0, str, strlen( str ) + 1,	nullptr, 0 );
	_UnicodeString = new wchar_t[ len ];
	::MultiByteToWideChar( GB18030, 0, str, strlen( str ) + 1, _UnicodeString, len );

	_MBCSString = const_cast< char * >( str );
}

inline UMConverter::UMConverter( const wchar_t *str )
	: _IsMBCS( false )
	, _MBCSString( nullptr )
{
	if ( str == nullptr )
	{
		_UnicodeString = nullptr;
		return;
	}
	int len = ::WideCharToMultiByte( GB18030, 0, str, wcslen( str ) + 1,	nullptr, 0, nullptr, nullptr );
	_MBCSString = new char[ len ];
	::WideCharToMultiByte( GB18030, 0, str, wcslen( str ) + 1, _MBCSString, len, nullptr, nullptr );

	_UnicodeString = const_cast< wchar_t * >( str );
}

inline UMConverter::~UMConverter( void )
{
	if ( _IsMBCS )
	{
		if ( _UnicodeString != nullptr )
		{
			delete []_UnicodeString;
			_UnicodeString = nullptr;
		}
	}
	else
	{
		if ( _MBCSString != nullptr )
		{
			delete []_MBCSString;
			_MBCSString = nullptr;
		}
	}
}

std::string& toString(std::string& str, const char* formats, ...);