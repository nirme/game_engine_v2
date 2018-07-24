#include "strtrim.h"


_STD_BEGIN

string str_trim( const string& _str_text )
{
	size_t first = _str_text.find_first_not_of(" \t\n\v\f\r");
	if ( first == string::npos )
		return string("");

	size_t last = _str_text.find_last_not_of(" \t\n\v\f\r");

	return _str_text.substr( first, last - first + 1 );
};


wstring str_trim( const wstring& _str_text )
{
	size_t first = _str_text.find_first_not_of(L" \t\n\v\f\r");
	if ( first == wstring::npos )
		return wstring(L"");

	size_t last = _str_text.find_last_not_of(L" \t\n\v\f\r");

	return _str_text.substr( first, last - first + 1 );
};


_STD_END

