#include "Base_Exception.h"

#pragma warning(disable:4996)


namespace GameEngine
{

	GEException::GEException( int _i_exceptionCode, const std::string& _str_description, const std::string& _str_caller ) : i_exceptionCode( _i_exceptionCode ), str_description( _str_description ), str_caller( _str_caller ), i_line( 0 )
	{};


	GEException::GEException( int _i_exceptionCode, const std::string& _str_description, const std::string& _str_caller, const char* _c_pExceptionType, const char* _c_pFile, unsigned int _i_line ) : i_exceptionCode( _i_exceptionCode ), str_description( _str_description ), str_caller( _str_caller ), str_exceptionType( _c_pExceptionType ), str_file( _c_pFile ), i_line( _i_line )
	{};


	GEException& GEException::operator = ( const GEException& _ge_rhs )
	{
		i_exceptionCode = _ge_rhs.i_exceptionCode;
		str_exceptionType = _ge_rhs.str_exceptionType;
		str_description = _ge_rhs.str_description;
		str_caller = _ge_rhs.str_caller;
		str_file = _ge_rhs.str_file;
		i_line = _ge_rhs.i_line;

		return *this;
	};


	int GEException::getExceptionCode()
	{
		return i_exceptionCode;
	};


	const std::string& GEException::getExceptionType()
	{
		return str_exceptionType;
	};


	const std::string& GEException::getDescription()
	{
		return str_description;
	};


	const std::string& GEException::getCaller()
	{
		return str_caller;
	};


	const std::string& GEException::getFile()
	{
		return str_file;
	};


	unsigned int GEException::getLine()
	{
		return i_line;
	};


	const std::string& GEException::getFullDescription()
	{
		if ( !str_fullDescription.size() )
		{
			char c_num[10];
			str_fullDescription = "Exception of type ";
			str_fullDescription += __itoa( i_exceptionCode, c_num, 10 );
			str_fullDescription += " ocurred in file: ";
			str_fullDescription += str_file;
			str_fullDescription += " function: ";
			str_fullDescription += str_caller;
			str_fullDescription += " line: ";
			str_fullDescription += __itoa( i_line, c_num, 10 );
			str_fullDescription += ", full description: \"";
			str_fullDescription += str_description;
			str_fullDescription += "\"";
		}

		return str_fullDescription;
	};


	const char* GEException::what()
	{
		return getFullDescription().c_str();
	};



	GEExceptionFileNotFound::GEExceptionFileNotFound( int _i_exceptionCode, const std::string& _str_description, const std::string& _str_caller, const char* _c_pExceptionType, const char* _c_pFile, unsigned int _i_line ) : GEException( _i_exceptionCode, _str_description, _str_caller, _c_pExceptionType, _c_pFile, _i_line )
	{};

	GEExceptionCannotWriteToFile::GEExceptionCannotWriteToFile( int _i_exceptionCode, const std::string& _str_description, const std::string& _str_caller, const char* _c_pExceptionType, const char* _c_pFile, unsigned int _i_line ) : GEException( _i_exceptionCode, _str_description, _str_caller, _c_pExceptionType, _c_pFile, _i_line )
	{};

	GEExceptionItemNotFound::GEExceptionItemNotFound( int _i_exceptionCode, const std::string& _str_description, const std::string& _str_caller, const char* _c_pExceptionType, const char* _c_pFile, unsigned int _i_line ) : GEException( _i_exceptionCode, _str_description, _str_caller, _c_pExceptionType, _c_pFile, _i_line )
	{};

	GEExceptionIllegalTypeConvertion::GEExceptionIllegalTypeConvertion( int _i_exceptionCode, const std::string& _str_description, const std::string& _str_caller, const char* _c_pExceptionType, const char* _c_pFile, unsigned int _i_line ) : GEException( _i_exceptionCode, _str_description, _str_caller, _c_pExceptionType, _c_pFile, _i_line )
	{};

	GEExceptionSystemError::GEExceptionSystemError( int _i_exceptionCode, const std::string& _str_description, const std::string& _str_caller, const char* _c_pExceptionType, const char* _c_pFile, unsigned int _i_line ) : GEException( _i_exceptionCode, _str_description, _str_caller, _c_pExceptionType, _c_pFile, _i_line )
	{};



	GEExceptionFileNotFound GEExceptionFactory::create( Int2type< GEException::EXCEPTION_FILE_NOT_FOUND > _i2t_exceptionCode, const std::string& _str_description, const std::string& _str_caller, const char* _c_pFile, unsigned int _i_line )
	{
		return GEExceptionFileNotFound( GEException::EXCEPTION_FILE_NOT_FOUND, _str_description, _str_caller, "GEExceptionFileNotFound", _c_pFile, _i_line );
	};

	GEExceptionCannotWriteToFile GEExceptionFactory::create( Int2type< GEException::EXCEPTION_CANNOT_WRITE_TO_FILE > _i2t_exceptionCode, const std::string& _str_description, const std::string& _str_caller, const char* _c_pFile, unsigned int _i_line )
	{
		return GEExceptionCannotWriteToFile( GEException::EXCEPTION_CANNOT_WRITE_TO_FILE, _str_description, _str_caller, "GEExceptionCannotWriteToFile", _c_pFile, _i_line );
	};

	GEExceptionItemNotFound GEExceptionFactory::create( Int2type< GEException::EXCEPTION_ITEM_NOT_FOUND > _i2t_exceptionCode, const std::string& _str_description, const std::string& _str_caller, const char* _c_pFile, unsigned int _i_line )
	{
		return GEExceptionItemNotFound( GEException::EXCEPTION_ITEM_NOT_FOUND, _str_description, _str_caller, "GEExceptionItemNotFound", _c_pFile, _i_line );
	};

	GEExceptionIllegalTypeConvertion GEExceptionFactory::create( Int2type< GEException::EXCEPTION_ILLEGAL_TYPE_CONVERTION > _i2t_exceptionCode, const std::string& _str_description, const std::string& _str_caller, const char* _c_pFile, unsigned int _i_line )
	{
		return GEExceptionIllegalTypeConvertion( GEException::EXCEPTION_ILLEGAL_TYPE_CONVERTION, _str_description, _str_caller, "GEExceptionItemNotFound", _c_pFile, _i_line );
	};

	GEExceptionSystemError GEExceptionFactory::create( Int2type< GEException::EXCEPTION_SYSTEM_ERROR > _i2t_exceptionCode, const std::string& _str_description, const std::string& _str_caller, const char* _c_pFile, unsigned int _i_line )
	{
		return GEExceptionSystemError( GEException::EXCEPTION_SYSTEM_ERROR, _str_description, _str_caller, "GEExceptionItemNotFound", _c_pFile, _i_line );
	};


}


#pragma warning(default:4996)
