#pragma once

#ifndef _VARIABLE_LIBRARY_V3
#define _VARIABLE_LIBRARY_V3


#include <typeinfo>
#include <string>
#include <fstream>

#include "strtrim.h"

#include "Base_Exception.h"
#include "boost/container/flat_map.hpp"

//#include "Assoc_Vector.h"


namespace GameEngine
{

	class DictionaryEntryValue;
	class VarLibrary;

/*
variable library can be populated from text file in following format:

; - marks starting of comment till the end of the line
# - marks dictionary name to which following entries are added, in case there is no name after, library goes back to base dictonary
normal values are in format 'entry name : entry value' where entry name is cleared from white signs from both sides and entry value can be one of those:

boolean value:
	true, TRUE, t, T - equals to true
	false, FALSE, f, F - equals to false

integer value:
	1234567890
	+12345
	-67890

floating point value:
	 12.34
	 3.141600E+000
	 +3e+000
	+12.34
	-12.34

character string value:
	any other value,
	if the value starts and ends with " both signs will be truncated and rest of string will be set as value
*/


	class DictionaryEntryValue
	{
	private:

		class Placeholder
		{
		public:
			Placeholder() {};
			virtual ~Placeholder() {};
			virtual Placeholder* clone( Placeholder** _ppPH_placeholder ) = 0;
			virtual const std::type_info& getTypeInfo() = 0;
			virtual const char* getTypeName() = 0;


			#ifndef VARLIB_VALUES_NO_THROW

				virtual bool getValueBool();
				virtual int getValueInt();
				virtual float getValueFloat();
				virtual double getValueDouble();
				virtual std::string getValueString();

			#else //VARLIB_VALUES_NO_THROW

				virtual bool getValueBool() noexcept;
				virtual int getValueInt() noexcept;
				virtual float getValueFloat() noexcept;
				virtual double getValueDouble() noexcept;
				virtual std::string getValueString() noexcept;

			#endif //VARLIB_VALUES_NO_THROW
		};


		class ValueBool : public Placeholder
		{
		private:
			bool b_value;

		public:
			ValueBool( bool _b_value = false );
			Placeholder* clone( Placeholder** _ppPH_placeholder );

			const std::type_info& getTypeInfo();
			const char* getTypeName();

			bool getValueBool();
		};


		class ValueInteger : public Placeholder
		{
		private:
			int i_value;

		public:
			ValueInteger( int _i_value = 0 );
			Placeholder* clone( Placeholder** _ppPH_placeholder );

			const std::type_info& getTypeInfo();
			const char* getTypeName();

			int getValueInt();
			float getValueFloat();
			double getValueDouble();
		};


		class ValueDouble : public Placeholder
		{
		private:
			double d_value;

		public:
			ValueDouble( double _d_value = 0.0 );
			Placeholder* clone( Placeholder** _ppPH_placeholder );

			const std::type_info& getTypeInfo();
			const char* getTypeName();

			float getValueFloat();
			double getValueDouble();
		};


		class ValueString : public Placeholder
		{
		private:
			std::string str_value;

		public:
			ValueString( const std::string& _str_value = "" );
			Placeholder* clone( Placeholder** _ppPH_placeholder );

			const std::type_info& getTypeInfo();
			const char* getTypeName();

			std::string getValueString();
		};


	private:
		Placeholder* pPH_value;


	public:

		DictionaryEntryValue();
		DictionaryEntryValue( bool _b_value );
		DictionaryEntryValue( int _i_value );
		DictionaryEntryValue( double _d_value );
		DictionaryEntryValue( const std::string& _str_value );
		DictionaryEntryValue( const DictionaryEntryValue& rhs );
		~DictionaryEntryValue();

		DictionaryEntryValue& operator=( const DictionaryEntryValue& rhs );

		const std::type_info& getTypeInfo();
		const char* getTypeName();

		inline bool getValueBool()          { return pPH_value->getValueBool(); };
		inline int getValueInt()            { return pPH_value->getValueInt(); };
		inline float getValueFloat()        { return pPH_value->getValueFloat(); };
		inline double getValueDouble()      { return pPH_value->getValueDouble(); };
		inline std::string getValueString() { return pPH_value->getValueString(); };


		void setValueBool( bool _b_value );
		void setValueInt( int _i_value );
		void setValueDouble( double _d_value );
		void setValueString( const std::string& _str_value );

		DictionaryEntryValue& operator=( bool _b_value );
		DictionaryEntryValue& operator=( int _i_value );
		DictionaryEntryValue& operator=( double _d_value );
		DictionaryEntryValue& operator=( const std::string& _str_value );
	};



	class VarLibrary
	{
		typedef boost::container::flat_map<std::string, DictionaryEntryValue>	Dictionary;
		typedef Dictionary::iterator	DictionaryEntry;
		typedef boost::container::flat_map<std::string, Dictionary>	Library;

	private:
		static Library lib_library;
		static std::string str_libraryFile;
		static bool b_libFileWriteFlag;

		static class _init
		{
		public:
			_init()
			{
				lib_library.insert( Library::value_type( std::string(""), Dictionary() ) );
				str_libraryFile = "";
				b_libFileWriteFlag = false;
			};
		} _initializer;


		VarLibrary() {};
		~VarLibrary() { shutdown(); };


		static Dictionary& pushDictionary( const std::string& _str_dictionary );
		static Dictionary& getDictionary( const std::string& _str_dictionary );


		inline static DictionaryEntryValue& getEntry( const std::string& _str_dictionary, const std::string& _str_entryKey )
		{
			return ( lib_library[ _str_dictionary ] )[ _str_entryKey ];
		};

	public:

		inline static void setLibraryFilename( const std::string& _str_libraryFile  ) { str_libraryFile = _str_libraryFile; };
		inline static std::string getLibraryFilename() { return str_libraryFile; };

		inline static void setModifyFileFlag( bool _b_libFileWriteFlag ) { b_libFileWriteFlag = _b_libFileWriteFlag; };
		inline static bool getModifyFileFlag() { return b_libFileWriteFlag; };


		static void initialize( const std::string& _str_libraryFile, bool _b_libFileWriteFlag = false );
		static void shutdown();

		static void writeLibraryFile( const std::string& _str_filename );
		static void readLibraryFile( const std::string& _str_filename );
		static void clear();


		static bool getBool( const std::string& _str_dictionary, const std::string& _str_entryKey );
		static int getInt( const std::string& _str_dictionary, const std::string& _str_entryKey );
		static float getFloat( const std::string& _str_dictionary, const std::string& _str_entryKey );
		static double getDouble( const std::string& _str_dictionary, const std::string& _str_entryKey );
		static std::string getString( const std::string& _str_dictionary, const std::string& _str_entryKey );

		static bool getBool( const std::string& _str_entryKey );
		static int getInt( const std::string& _str_entryKey );
		static float getFloat( const std::string& _str_entryKey );
		static double getDouble( const std::string& _str_entryKey );
		static std::string getString( const std::string& _str_entryKey );


		static void setBool( const std::string& _str_dictionary, const std::string& _str_entryKey, bool _b_value );
		static void setInt( const std::string& _str_dictionary, const std::string& _str_entryKey, int _i_value );
		static void setFloat( const std::string& _str_dictionary, const std::string& _str_entryKey, float _f_value );
		static void setDouble( const std::string& _str_dictionary, const std::string& _str_entryKey, double _d_value );
		static void setString( const std::string& _str_dictionary, const std::string& _str_entryKey, const std::string& _str_value );

		static void setBool( const std::string& _str_entryKey, bool _b_value );
		static void setInt( const std::string& _str_entryKey, int _i_value );
		static void setFloat( const std::string& _str_entryKey, float _f_value );
		static void setDouble( const std::string& _str_entryKey, double _d_value );
		static void setString( const std::string& _str_entryKey, const std::string& _str_value );

	};

}

#endif //_VARIABLE_LIBRARY_V3
