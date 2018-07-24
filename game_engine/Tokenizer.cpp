#include "Tokenizer.h"



int tokenize( const char* _c_string, unsigned int _i_stringLength, const char* _c_tokens, unsigned int _i_tokensNumber, std::vector<std::string>& _v_tokenizedOutput )
{
	if ( !_c_string ) return TKN_STRING_NOT_SPECIFIED;
	if ( !_i_stringLength ) return TKN_STRING_IS_EMPTY;
	if ( !_c_tokens || !_i_tokensNumber ) return TKN_TOKENS_NOT_SPECIFIED;

	int i_tokensFound(0);

	unsigned int i_lastAfterTokenIndex(0);
	unsigned int i_index(0);
	char c_workset(0);
	unsigned int i_tokInd(0);
	while ( i_index < _i_stringLength )
	{
		c_workset = _c_string[ i_index ];
		i_tokInd = 0;
		while ( i_tokInd < _i_tokensNumber && _c_tokens[ i_tokInd ] != c_workset ) ++i_tokInd;

		if ( i_tokInd < _i_tokensNumber )
		{ // found a token!!

			if ( i_index != i_lastAfterTokenIndex )
				_v_tokenizedOutput.push_back( std::string( &( _c_string[ i_lastAfterTokenIndex ] ), i_index - i_lastAfterTokenIndex ) );
			else
				_v_tokenizedOutput.push_back( std::string() );

			i_lastAfterTokenIndex = i_index + 1;

			++i_tokensFound;
		}

		++i_index;
	}

	//add what's left after last token
	if ( i_lastAfterTokenIndex != i_index )
		_v_tokenizedOutput.push_back( std::string( &( _c_string[ i_lastAfterTokenIndex ] ), i_index - i_lastAfterTokenIndex ) );
	else
		_v_tokenizedOutput.push_back( std::string() );

	return i_tokensFound + 1; //add 1 for the last part after the main loop
};



int tokenize( std::string _str_string, std::string _str_tokens, std::vector<std::string>& _v_tokenizedOutput )
{
	return tokenize( _str_string.c_str(), _str_string.length(), _str_tokens.c_str(), _str_tokens.length(), _v_tokenizedOutput );
};


int tokenize( std::string _str_string, const char* _c_tokens, unsigned int _i_tokensNumber, std::vector<std::string>& _v_tokenizedOutput )
{
	return tokenize( _str_string.c_str(), _str_string.length(), _c_tokens, _i_tokensNumber, _v_tokenizedOutput );
};


int tokenize( std::string _str_string, char _c_token, std::vector<std::string>& _v_tokenizedOutput )
{
	return tokenize( _str_string.c_str(), _str_string.length(), &_c_token, 1, _v_tokenizedOutput );
};


int tokenize( const char* _c_string, unsigned int _i_stringLength, char _c_token, std::vector<std::string>& _v_tokenizedOutput )
{
	return tokenize( _c_string, _i_stringLength, &_c_token, 1, _v_tokenizedOutput );
};


int tokenize( const char* _c_string, unsigned int _i_stringLength, std::string _str_tokens, std::vector<std::string>& _v_tokenizedOutput )
{
	return tokenize( _c_string, _i_stringLength, _str_tokens.c_str(), _str_tokens.length(), _v_tokenizedOutput );
};

