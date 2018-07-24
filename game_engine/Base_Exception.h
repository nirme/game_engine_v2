#pragma once

#ifndef _GE_EXCEPTION_V3
#define _GE_EXCEPTION_V3

#include "Core_Defines.h"
#include <string>

#include "itoa.h"



namespace GameEngine
{

	class GEException : public std::exception
	{
	protected:
		int i_exceptionCode;

		std::string str_exceptionType;
		std::string str_description;
		std::string str_caller;
		std::string str_file;

		unsigned int i_line;

		std::string str_fullDescription;


	public:

		enum ExceptionCodes
		{
			EXCEPTION_FILE_NOT_FOUND,
			EXCEPTION_CANNOT_WRITE_TO_FILE,
			EXCEPTION_ITEM_NOT_FOUND,
			EXCEPTION_ILLEGAL_TYPE_CONVERTION,
			EXCEPTION_SYSTEM_ERROR,

			//ERR_INVALID_STATE,
			//ERR_INVALIDPARAMS,
			//ERR_RENDERINGAPI_ERROR,
			//ERR_DUPLICATE_ITEM,
			//ERR_INTERNAL_ERROR,
			//ERR_RT_ASSERTION_FAILED, 
			//ERR_NOT_IMPLEMENTED
		};


		GEException( int _i_exceptionCode, const std::string& _str_description, const std::string& _str_caller );
		GEException( int _i_exceptionCode, const std::string& _str_description, const std::string& _str_caller, const char* _c_pExceptionType, const char* _c_pFile, unsigned int _i_line );
        virtual GEException& operator = ( const GEException& _ge_rhs );

		int getExceptionCode();
		const std::string& getExceptionType();
		const std::string& getDescription();
		const std::string& getCaller();
		const std::string& getFile();
		unsigned int getLine();
		virtual const std::string& getFullDescription();

		virtual const char* what();
	};



	class GEExceptionFileNotFound : public GEException
	{
	public:
		GEExceptionFileNotFound( int _i_exceptionCode, const std::string& _str_description, const std::string& _str_caller, const char* _c_pExceptionType, const char* _c_pFile, unsigned int _i_line );
	};

	class GEExceptionCannotWriteToFile : public GEException
	{
	public:
		GEExceptionCannotWriteToFile( int _i_exceptionCode, const std::string& _str_description, const std::string& _str_caller, const char* _c_pExceptionType, const char* _c_pFile, unsigned int _i_line );
	};

	class GEExceptionItemNotFound : public GEException
	{
	public:
		GEExceptionItemNotFound( int _i_exceptionCode, const std::string& _str_description, const std::string& _str_caller, const char* _c_pExceptionType, const char* _c_pFile, unsigned int _i_line );
	};

	class GEExceptionIllegalTypeConvertion : public GEException
	{
	public:
		GEExceptionIllegalTypeConvertion( int _i_exceptionCode, const std::string& _str_description, const std::string& _str_caller, const char* _c_pExceptionType, const char* _c_pFile, unsigned int _i_line );
	};

	class GEExceptionSystemError : public GEException
	{
	public:
		GEExceptionSystemError( int _i_exceptionCode, const std::string& _str_description, const std::string& _str_caller, const char* _c_pExceptionType, const char* _c_pFile, unsigned int _i_line );
	};




	template < int v >
	struct Int2type
	{
		enum { value = v };
	};


	class GEExceptionFactory
	{
	private:
		GEExceptionFactory() {};

	public:
		static GEExceptionFileNotFound create(			Int2type< GEException::EXCEPTION_FILE_NOT_FOUND > _i2t_exceptionCode, 
														const std::string& _str_description, 
														const std::string& _str_caller, 
														const char* _c_pFile, 
														unsigned int _i_line );

		static GEExceptionCannotWriteToFile create(		Int2type< GEException::EXCEPTION_CANNOT_WRITE_TO_FILE > _i2t_exceptionCode, 
														const std::string& _str_description, 
														const std::string& _str_caller, 
														const char* _c_pFile, 
														unsigned int _i_line );

		static GEExceptionItemNotFound create(			Int2type< GEException::EXCEPTION_ITEM_NOT_FOUND > _i2t_exceptionCode, 
														const std::string& _str_description, 
														const std::string& _str_caller, 
														const char* _c_pFile, 
														unsigned int _i_line );

		static GEExceptionIllegalTypeConvertion create(	Int2type< GEException::EXCEPTION_ILLEGAL_TYPE_CONVERTION > _i2t_exceptionCode, 
														const std::string& _str_description, 
														const std::string& _str_caller, 
														const char* _c_pFile, 
														unsigned int _i_line );

		static GEExceptionSystemError create(			Int2type< GEException::EXCEPTION_SYSTEM_ERROR > _i2t_exceptionCode, 
														const std::string& _str_description, 
														const std::string& _str_caller, 
														const char* _c_pFile, 
														unsigned int _i_line );










	};



	#define		THROW_GE_EXCEPTION( _i2t_exceptionCode, desc )			throw GEExceptionFactory::create( Int2type<_i2t_exceptionCode>(), desc, __FUNCSIG__, __FILE__, __LINE__ )

	#define		THROW_GE_EXCEPTION_FILE_NOT_FOUND( desc )				THROW_GE_EXCEPTION( GEException::EXCEPTION_FILE_NOT_FOUND, desc )
	#define		THROW_FILE_NOT_FOUND( desc )							THROW_GE_EXCEPTION( GEException::EXCEPTION_FILE_NOT_FOUND, desc )

	#define		THROW_GE_EXCEPTION_CANNOT_WRITE_TO_FILE( desc )			THROW_GE_EXCEPTION( GEException::EXCEPTION_CANNOT_WRITE_TO_FILE, desc )
	#define		THROW_CANNOT_WRITE_TO_FILE( desc )						THROW_GE_EXCEPTION( GEException::EXCEPTION_CANNOT_WRITE_TO_FILE, desc )

	#define		THROW_GE_EXCEPTION_ITEM_NOT_FOUND( desc )				THROW_GE_EXCEPTION( GEException::EXCEPTION_ITEM_NOT_FOUND, desc )
	#define		THROW_ITEM_NOT_FOUND( desc )							THROW_GE_EXCEPTION( GEException::EXCEPTION_ITEM_NOT_FOUND, desc )

	#define		THROW_GE_EXCEPTION_ILLEGAL_TYPE_CONVERTION( desc )		THROW_GE_EXCEPTION( GEException::EXCEPTION_ILLEGAL_TYPE_CONVERTION, desc )
	#define		THROW_ILLEGAL_TYPE_CONV( desc )							THROW_GE_EXCEPTION( GEException::EXCEPTION_ILLEGAL_TYPE_CONVERTION, desc )

	#define		THROW_GE_EXCEPTION_SYSTEM_ERROR( desc )					THROW_GE_EXCEPTION( GEException::EXCEPTION_SYSTEM_ERROR, desc )
	#define		THROW_SYSTEM_ERROR( desc )								THROW_GE_EXCEPTION( GEException::EXCEPTION_SYSTEM_ERROR, desc )

}

#endif //_BASIC_EXCEPTIONS
