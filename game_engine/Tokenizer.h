/**
* just another tokenizer
*/


#pragma once

#ifndef _TOKENIZER
#define _TOKENIZER


#include <vector>
#include <string>

#define		TKN_STRING_NOT_SPECIFIED		-1
#define		TKN_STRING_IS_EMPTY				-2
#define		TKN_TOKENS_NOT_SPECIFIED		-4



int tokenize( const char* _c_string, unsigned int _i_stringLength, const char* _c_tokens, unsigned int _i_tokensNumber, std::vector<std::string>& _v_tokenizedOutput );

int tokenize( std::string _str_string, std::string _str_tokens, std::vector<std::string>& _v_tokenizedOutput );
int tokenize( std::string _str_string, const char* _c_tokens, unsigned int _i_tokensNumber, std::vector<std::string>& _v_tokenizedOutput );
int tokenize( std::string _str_string, char _c_token, std::vector<std::string>& _v_tokenizedOutput );
int tokenize( const char* _c_string, unsigned int _i_stringLength, char _c_token, std::vector<std::string>& _v_tokenizedOutput );
int tokenize( const char* _c_string, unsigned int _i_stringLength, std::string _str_tokens, std::vector<std::string>& _v_tokenizedOutput );



#endif //_TOKENIZER