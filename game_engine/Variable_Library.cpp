#include "Variable_Library.h"

#pragma warning(disable:4996)

namespace GameEngine
{

	VarLibrary::Library VarLibrary::lib_library;
	std::string VarLibrary::str_libraryFile;
	bool VarLibrary::b_libFileWriteFlag;
	VarLibrary::_init VarLibrary::_initializer;


	#ifndef VARLIB_VALUES_NO_THROW

		bool DictionaryEntryValue::Placeholder::getValueBool()
		{
			THROW_ILLEGAL_TYPE_CONV( std::string( "Type conversion from " ) + this->getTypeName() + " to bool not defined" );
		};


		int DictionaryEntryValue::Placeholder::getValueInt()
		{
			THROW_ILLEGAL_TYPE_CONV( std::string( "Type conversion from " ) + this->getTypeName() + " to int not defined" );
		};


		float DictionaryEntryValue::Placeholder::getValueFloat()
		{
			THROW_ILLEGAL_TYPE_CONV( std::string( "Type conversion from " ) + this->getTypeName() + " to float not defined" );
		};


		double DictionaryEntryValue::Placeholder::getValueDouble()
		{
			THROW_ILLEGAL_TYPE_CONV( std::string( "Type conversion from " ) + this->getTypeName() + " to double not defined" );
		};


		std::string DictionaryEntryValue::Placeholder::getValueString()
		{
			THROW_ILLEGAL_TYPE_CONV( std::string( "Type conversion from " ) + this->getTypeName() + " to std::string not defined" );
		};


	#else //VARLIB_VALUES_NO_THROW

		bool DictionaryEntryValue::Placeholder::getValueBool() noexcept
		{
			return false;
		};


		int DictionaryEntryValue::Placeholder::getValueInt() noexcept
		{
			return 0;
		};


		float DictionaryEntryValue::Placeholder::getValueFloat() noexcept
		{
			return 0.0f;
		};


		double DictionaryEntryValue::Placeholder::getValueDouble() noexcept
		{
			return 0.0;
		};


		std::string DictionaryEntryValue::Placeholder::getValueString() noexcept
		{
			return std::string();
		};

	#endif //VARLIB_VALUES_NO_THROW


	DictionaryEntryValue::ValueBool::ValueBool( bool _b_value ) : DictionaryEntryValue::Placeholder(), b_value( _b_value )
	{};


	DictionaryEntryValue::Placeholder* DictionaryEntryValue::ValueBool::clone( DictionaryEntryValue::Placeholder** _ppPH_placeholder )
	{
		*_ppPH_placeholder = new DictionaryEntryValue::ValueBool( b_value );
		return *_ppPH_placeholder;
	};


	const std::type_info& DictionaryEntryValue::ValueBool::getTypeInfo()
	{
		return typeid( b_value );
	};


	const char* DictionaryEntryValue::ValueBool::getTypeName()
	{
		return "bool";
	};


	bool DictionaryEntryValue::ValueBool::getValueBool()
	{
		return b_value;
	};



	DictionaryEntryValue::ValueInteger::ValueInteger( int _i_value ) : DictionaryEntryValue::Placeholder(), i_value( _i_value )
	{};


	DictionaryEntryValue::Placeholder* DictionaryEntryValue::ValueInteger::clone( DictionaryEntryValue::Placeholder** _ppPH_placeholder )
	{
		*_ppPH_placeholder = new DictionaryEntryValue::ValueInteger( i_value );
		return *_ppPH_placeholder;
	};


	const std::type_info& DictionaryEntryValue::ValueInteger::getTypeInfo()
	{
		return typeid( i_value );
	};


	const char* DictionaryEntryValue::ValueInteger::getTypeName()
	{
		return "int";
	};


	int DictionaryEntryValue::ValueInteger::getValueInt()
	{
		return i_value;
	};


	float DictionaryEntryValue::ValueInteger::getValueFloat()
	{
		return (float) i_value;
	};


	double DictionaryEntryValue::ValueInteger::getValueDouble()
	{
		return (double) i_value;
	};



	DictionaryEntryValue::ValueDouble::ValueDouble( double _d_value ) : DictionaryEntryValue::Placeholder(), d_value( _d_value )
	{};


	DictionaryEntryValue::Placeholder* DictionaryEntryValue::ValueDouble::clone( DictionaryEntryValue::Placeholder** _ppPH_placeholder )
	{
		*_ppPH_placeholder = new DictionaryEntryValue::ValueDouble( d_value );
		return *_ppPH_placeholder;
	};


	const std::type_info& DictionaryEntryValue::ValueDouble::getTypeInfo()
	{
		return typeid( d_value );
	};


	const char* DictionaryEntryValue::ValueDouble::getTypeName()
	{
		return "double";
	};


	float DictionaryEntryValue::ValueDouble::getValueFloat()
	{
		return (float) d_value;
	};


	double DictionaryEntryValue::ValueDouble::getValueDouble()
	{
		return d_value;
	};



	DictionaryEntryValue::ValueString::ValueString( const std::string& _str_value ) : DictionaryEntryValue::Placeholder(), str_value( _str_value )
	{};


	DictionaryEntryValue::Placeholder* DictionaryEntryValue::ValueString::clone( DictionaryEntryValue::Placeholder** _ppPH_placeholder )
	{
		*_ppPH_placeholder = new DictionaryEntryValue::ValueString( str_value );
		return *_ppPH_placeholder;
	};


	const std::type_info& DictionaryEntryValue::ValueString::getTypeInfo()
	{
		return typeid( str_value );
	};


	const char* DictionaryEntryValue::ValueString::getTypeName()
	{
		return "std::string";
	};


	std::string DictionaryEntryValue::ValueString::getValueString()
	{
		return str_value;
	};



	DictionaryEntryValue::DictionaryEntryValue() : pPH_value( NULL )
	{};


	DictionaryEntryValue::DictionaryEntryValue( bool _b_value ) : pPH_value( new ValueBool( _b_value ) )
	{};


	DictionaryEntryValue::DictionaryEntryValue( int _i_value ) : pPH_value( new ValueInteger( _i_value ) )
	{};


	DictionaryEntryValue::DictionaryEntryValue( double _d_value ) : pPH_value( new ValueDouble( _d_value ) )
	{};


	DictionaryEntryValue::DictionaryEntryValue( const std::string& _str_value ) : pPH_value( new ValueString( _str_value ) )
	{};


	DictionaryEntryValue::DictionaryEntryValue( const DictionaryEntryValue& rhs ) : pPH_value(NULL)
	{
		if ( rhs.pPH_value ) rhs.pPH_value->clone( &pPH_value );
	};


	DictionaryEntryValue::~DictionaryEntryValue()
	{
		if ( pPH_value ) delete pPH_value;
		pPH_value = NULL;
	};


	DictionaryEntryValue& DictionaryEntryValue::operator=( const DictionaryEntryValue& rhs )
	{
		if ( pPH_value ) delete pPH_value;
		pPH_value = NULL;
		if ( rhs.pPH_value ) rhs.pPH_value->clone( &pPH_value );
		return *this;
	};


	const std::type_info& DictionaryEntryValue::getTypeInfo()
	{
		return pPH_value->getTypeInfo();
	};


	const char* DictionaryEntryValue::getTypeName()
	{
		return pPH_value->getTypeName();
	};


	void DictionaryEntryValue::setValueBool( bool _b_value )
	{
		if ( pPH_value ) delete pPH_value;
		pPH_value = NULL;
		pPH_value = new DictionaryEntryValue::ValueBool( _b_value );
	};


	void DictionaryEntryValue::setValueInt( int _i_value )
	{
		if ( pPH_value ) delete pPH_value;
		pPH_value = NULL;
		pPH_value = new DictionaryEntryValue::ValueInteger( _i_value );
	};


	void DictionaryEntryValue::setValueDouble( double _d_value )
	{
		if ( pPH_value ) delete pPH_value;
		pPH_value = NULL;
		pPH_value = new DictionaryEntryValue::ValueDouble( _d_value );
	};


	void DictionaryEntryValue::setValueString( const std::string& _str_value )
	{
		if ( pPH_value ) delete pPH_value;
		pPH_value = NULL;
		pPH_value = new DictionaryEntryValue::ValueString( _str_value );
	};


	DictionaryEntryValue& DictionaryEntryValue::operator=( bool _b_value )
	{
		setValueBool( _b_value );
		return *this;
	};


	DictionaryEntryValue& DictionaryEntryValue::operator=( int _i_value )
	{
		setValueInt( _i_value );
		return *this;
	};


	DictionaryEntryValue& DictionaryEntryValue::operator=( double _d_value )
	{
		setValueDouble( _d_value );
		return *this;
	};


	DictionaryEntryValue& DictionaryEntryValue::operator=( const std::string& _str_value )
	{
		setValueString( _str_value );
		return *this;
	};



	VarLibrary::Dictionary& VarLibrary::pushDictionary( const std::string& _str_dictionary )
	{
		return ( *( ( lib_library.insert( Library::value_type( _str_dictionary, Dictionary() ) ) ).first ) ).second;
	};


	VarLibrary::Dictionary& VarLibrary::getDictionary( const std::string& _str_dictionary )
	{
		return pushDictionary( _str_dictionary );
	};



	void VarLibrary::initialize( const std::string& _str_libraryFile, bool _b_libFileWriteFlag )
	{
		b_libFileWriteFlag = _b_libFileWriteFlag;
		if ( _str_libraryFile.size() )
			readLibraryFile( _str_libraryFile );
	};


	void VarLibrary::shutdown()
	{
		if ( b_libFileWriteFlag && str_libraryFile.size() )
		{
			writeLibraryFile( str_libraryFile );
		}

		clear();
		str_libraryFile = "";
		b_libFileWriteFlag = false;
	};


	void VarLibrary::writeLibraryFile( const std::string& _str_filename )
	{
		std::ofstream stream( _str_filename.c_str(), std::ofstream::out | std::ofstream::trunc );
		if ( stream.fail() )
			THROW_CANNOT_WRITE_TO_FILE( std::string("Cannot write variable library into file '") + _str_filename + "'" );


		for ( Library::iterator it = lib_library.begin(); it != lib_library.end(); ++it )
		{
			stream.write( "# ", 2 );
			stream.write( (*it).first.c_str(), (*it).first.size() );
			stream.put( '\n' );

			Dictionary& dictionary = (*it).second;

			for ( DictionaryEntry itd = dictionary.begin(); itd != dictionary.end(); ++itd )
			{
				stream.put( '\t' );
				stream.write( (*itd).first.c_str(), (*itd).first.size() );
				stream.write( " : ", 3 );

				if ( (*itd).second.getTypeInfo() == typeid( bool ) )
				{
					bool b = (*itd).second.getValueBool();
					stream.write( b ? "TRUE" : "FALSE", b ? 4 : 5 );
				}
				else if ( (*itd).second.getTypeInfo() == typeid( int ) )
				{
					char out[12];
					__itoa( (*itd).second.getValueInt(), out, 10 );
					stream.write( out, strlen( out ) );
				}
				else if ( (*itd).second.getTypeInfo() == typeid( double ) )
				{
					char out[32];
					int len = sprintf( out, "%#f", (*itd).second.getValueDouble() );
					stream.write( out, len );
				}
				else //if ( (*itd).second.getTypeInfo() == typeid( std::string ) )
				{
					stream.put( '"' );
					stream.write( (*itd).second.getValueString().c_str(), (*itd).second.getValueString().size() );
					stream.put( '"' );
				}

				stream.put( '\n' );
			}

			stream.put( '\n' );
		}

		stream.close();
	};


	void VarLibrary::readLibraryFile( const std::string& _str_filename )
	{
		std::ifstream stream( _str_filename.c_str(), std::ifstream::in );
		if ( stream.fail() )
			THROW_FILE_NOT_FOUND( std::string("Cannot open variable library file '") + _str_filename + "'" );

		char c_tmp[2048] = {'\0'};
		std::string str_line;
		std::string str_dictionary = std::string("");
		std::string str_entryKey;
		DictionaryEntryValue* dev_entryValue = NULL;

		

		while ( stream.getline( c_tmp, 2048 /*, '\n'*/ ) )
		{
			str_line = str_trim( std::string(c_tmp) );

			if ( str_line.empty() || str_line.c_str()[0] == ';' ) continue;

			if ( str_line.c_str()[0] == '#' )
			{
				str_dictionary = str_trim( str_line.substr(1) );
				continue;
			}


			size_t pos = str_line.find_first_of(':');
			if ( pos == std::string::npos ) continue;


			str_entryKey = str_trim( str_line.substr( 0, pos ) );
			str_line = str_trim( str_line.substr( pos + 1 ) );

			if ( !str_line.compare( "true" ) || !str_line.compare( "TRUE" ) || !str_line.compare( "t" ) || !str_line.compare( "T" ) )
			{
				getEntry( str_dictionary, str_entryKey ) = true;
			}
			else if ( !str_line.compare( "false" ) || !str_line.compare( "FALSE" ) || !str_line.compare( "f" ) || !str_line.compare( "F" ) )
			{
				getEntry( str_dictionary, str_entryKey ) = false;
			}
			else if ( str_line.find_first_not_of( "+-1234567890" ) == std::string::npos )
			{
				int value = 0;
				sscanf( str_line.c_str(), "%i", &value );
				getEntry( str_dictionary, str_entryKey ) = value;
			}
			else if ( str_line.find_first_not_of( "+-.eE1234567890" ) == std::string::npos )
			{
				double value = 0;
				sscanf( str_line.c_str(), "%lf", &value );
				getEntry( str_dictionary, str_entryKey ) = value;
			}
			else
			{
				if ( str_line[ 1 ] == '"' && str_line[ str_line.size() - 1 ] == '"' )
					getEntry( str_dictionary, str_entryKey ) = str_line.substr( 1 , str_line.size() - 1 );
				else
					getEntry( str_dictionary, str_entryKey ) = str_line;
			}
		}

		stream.close();

		str_libraryFile = _str_filename;
	};


	void VarLibrary::clear()
	{
		lib_library.clear();
		str_libraryFile = "";
		b_libFileWriteFlag = false;
	};


	bool VarLibrary::getBool( const std::string& _str_dictionary, const std::string& _str_entryKey )
	{
		return getEntry( _str_dictionary, _str_entryKey ).getValueBool();
	};


	int VarLibrary::getInt( const std::string& _str_dictionary, const std::string& _str_entryKey )
	{
		return getEntry( _str_dictionary, _str_entryKey ).getValueInt();
	};


	float VarLibrary::getFloat( const std::string& _str_dictionary, const std::string& _str_entryKey )
	{
		return getEntry( _str_dictionary, _str_entryKey ).getValueFloat();
	};


	double VarLibrary::getDouble( const std::string& _str_dictionary, const std::string& _str_entryKey )
	{
		return getEntry( _str_dictionary, _str_entryKey ).getValueDouble();
	};


	std::string VarLibrary::getString( const std::string& _str_dictionary, const std::string& _str_entryKey )
	{
		return getEntry( _str_dictionary, _str_entryKey ).getValueString();
	};


	bool VarLibrary::getBool( const std::string& _str_entryKey )
	{
		return getEntry( "", _str_entryKey ).getValueBool();
	};


	int VarLibrary::getInt( const std::string& _str_entryKey )
	{
		return getEntry( "", _str_entryKey ).getValueInt();
	};


	float VarLibrary::getFloat( const std::string& _str_entryKey )
	{
		return getEntry( "", _str_entryKey ).getValueFloat();
	};


	double VarLibrary::getDouble( const std::string& _str_entryKey )
	{
		return getEntry( "", _str_entryKey ).getValueDouble();
	};


	std::string VarLibrary::getString( const std::string& _str_entryKey )
	{
		return getEntry( "", _str_entryKey ).getValueString();
	};


	void VarLibrary::setBool( const std::string& _str_dictionary, const std::string& _str_entryKey, bool _b_value )
	{
		getEntry( _str_dictionary, _str_entryKey ) = _b_value;
	};


	void VarLibrary::setInt( const std::string& _str_dictionary, const std::string& _str_entryKey, int _i_value )
	{
		getEntry( _str_dictionary, _str_entryKey ) = _i_value;
	};


	void VarLibrary::setFloat( const std::string& _str_dictionary, const std::string& _str_entryKey, float _f_value )
	{
		getEntry( _str_dictionary, _str_entryKey ) = _f_value;
	};


	void VarLibrary::setDouble( const std::string& _str_dictionary, const std::string& _str_entryKey, double _d_value )
	{
		getEntry( _str_dictionary, _str_entryKey ) = _d_value;
	};


	void VarLibrary::setString( const std::string& _str_dictionary, const std::string& _str_entryKey, const std::string& _str_value )
	{
		getEntry( _str_dictionary, _str_entryKey ) = _str_value;
	};


	void VarLibrary::setBool( const std::string& _str_entryKey, bool _b_value )
	{
		getEntry( "", _str_entryKey ) = _b_value;
	};


	void VarLibrary::setInt( const std::string& _str_entryKey, int _i_value )
	{
		getEntry( "", _str_entryKey ) = _i_value;
	};


	void VarLibrary::setFloat( const std::string& _str_entryKey, float _f_value )
	{
		getEntry( "", _str_entryKey ) = _f_value;
	};


	void VarLibrary::setDouble( const std::string& _str_entryKey, double _d_value )
	{
		getEntry( "", _str_entryKey ) = _d_value;
	};


	void VarLibrary::setString( const std::string& _str_entryKey, const std::string& _str_value )
	{
		getEntry( "", _str_entryKey ) = _str_value;
	};


#pragma warning(default:4996)

}
