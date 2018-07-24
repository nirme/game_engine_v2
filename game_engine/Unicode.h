//	UTF codes descryption
//	
//	UTF-8
//	first part of first byte describe number of bytes used for encoding codepoint
//	+-----------+--------------------+
//	| codepoint | codepoint encoding |
//	|   size    |    descryption     |
//	+-----------+--------------------+
//	| 1 byte    |  0xxxxxxx          |
//	| 2 bytes   |  110xxxxx          |
//	| 3 bytes   |  1110xxxx          |
//	| 4 bytes   |  11110xxx          |
//	+-----------+--------------------+
//	any additional bytes in codepoint are encoded in following manner 10xxxxxx
//	where x are bits of actual encoded value
//	encode:
//	CP - codepoint value
//	for CP less than 0x0080 ( 7 bits or shorter)
//		Character[1] = CP
//	for CP less than 0x0800 ( 11 bits or shorter)
//		Character[1] = 0xC0 | ( ( CP >> 6 ) & 0x1F )
//		Character[2] = 0x80 | (   CP & 0x3F )
//	for CP less than 0x010000 ( 16 bits or shorter)
//		Character[1] = 0xE0 | ( ( CP >> 12 ) & 0x0F )
//		Character[2] = 0x80 | ( ( CP >>  6 ) & 0x3F )
//		Character[3] = 0x80 | (   CP & 0x3F )
//	for CP less than 0x200000 ( 21 bits or shorter)
//		Character[1] = 0xF0 | ( ( CP >> 18 ) & 0x07 )
//		Character[2] = 0x80 | ( ( CP >> 12 ) & 0x3F )
//		Character[3] = 0x80 | ( ( CP >>  6 ) & 0x3F )
//		Character[4] = 0x80 | (   CP & 0x3F )
//
//	decode:
//	for Character[1] less than 0x80
//		CP = Character[1]
//	for Character[1] less than 0xE0
//		CP = ( ( Character[1] & 0x1F ) <<  6 ) | (   Character[2] & 0x3F )
//	for Character[1] less than 0xF0
//		CP = ( ( Character[1] & 0x0F ) << 12 ) | ( ( Character[2] & 0x3F ) <<  6 ) | (   Character[3] & 0x3F )
//	for Character[1] less than 0xF8
//		CP = ( ( Character[1] & 0x07 ) << 18 ) | ( ( Character[2] & 0x3F ) << 12 ) | ( ( Character[3] & 0x3F ) << 6 ) | ( Character[4] & 0x3F )
//
//
//	UTF-16
//	codepoints from 0x0000 up to 0xFFFF (Basic Multilingual Plane) are described on single character (2 bytes)
//	single character values from 0xD800 to 0xDFFF are reserved for surrogate pairs (values from 0xD800 to 0xF8FF are reserved in BMP Plane)
//	for codepoints over 0xFFFF (65535) where CP - codepoint value
//	encode:
//	Lead character = ( ( ( CP - 0x010000 ) >> 10 ) & 0x03FF ) + 0xD800
//	Trail character = ( ( CP - 0x010000 ) & 0x03FF ) + 0xDC00
//	
//	decode:
//	CP = ( ( ( Lead character - 0xD800 ) << 10 ) | ( Trail character - 0xDC00 ) + 0x010000
//
//
//	UTF-32
//	UTF-32 with its 32bit character have more than enough space to take in whole codepoint ( 21 bits at most )
//	leading 11 bits in each character may be used for other issues


#pragma once

#ifndef __UNICODE__
#define __UNICODE__

#define _SCL_SECURE_NO_WARNINGS
#pragma warning(disable:4996)

#include <algorithm>
#include <iterator>
#include <string>
#include <istream>


namespace Unicode
{

	#ifdef _MSC_VER

		#include <xutility>
		//if compiler is MSVC
		typedef		char		CHAR8;
		typedef		wchar_t		CHAR16;
		typedef		int			CHAR32;
		typedef		int			CODEPOINT;

		#ifdef _DEBUG
			#define		USTRING_ERROR( text )		exit(EXIT_FAILURE)
		#else
			#define		USTRING_ERROR( text )		exit(EXIT_FAILURE) /*_invalid_parameter( __STR2WSTR( #text ), __FUNCTIONW__, __FILEW__, __LINE__, 0)*/
		#endif

	#elif
		//any other compiler (GCC)
		typedef		boost::int8_t		CHAR8;
		typedef		boost::int16_t		CHAR16;
		typedef		boost::int32_t		CHAR32;
		typedef		boost::int32_t		CODEPOINT;

		#ifdef _DEBUG
			#define		USTRING_ERROR( text )		exit(EXIT_FAILURE) /*error( 1, 0, "")*/
		#else
			#define		USTRING_ERROR( text )		exit(EXIT_FAILURE) /*error_at_line( 1, 0, __FILEW__, __LINE__, text )*/
		#endif
	#endif

	typedef		char	UTFSize;

	struct SLpair { size_t size; size_t length; SLpair():size(0),length(0){}; SLpair(size_t _size,size_t _length):size(_size),length(_length){};};


	#define		CP_UNKNOWN_CONVERTION			'?'

	#define		UNICODE_MAX_CP_VALUE			0x10FFFF
	#define		UNICODE_MAX_UTF8_LENGTH			4
	#define		UNICODE_MAX_UTF16_LENGTH		2
	#define		UNICODE_MAX_UTF32_LENGTH		1

	//	size in bytes of encoded cp, -1 if it's not the first byte of encoded value or 0 if value is not recognisable as utf-8 encoded unicode codepoint
	#define		UTF8_CHR_SIZE( chr )			(UTFSize) ( chr & 0x80 ? ( chr & 0x40 ? ( chr & 0x20 ? ( chr & 0x10 ? ( chr & 0x08 ? 0 : 4 ) : 3 ) : 2 ) : -1 ) : 1 )
	#define		UTF8_TO_CODEPOINT_1( chr )		( chr[0] )
	#define		UTF8_TO_CODEPOINT_2( chr )		( ( ( chr[0] & 0x1F ) << 6 ) | ( chr[1] & 0x3F ) )
	#define		UTF8_TO_CODEPOINT_3( chr )		( ( ( chr[0] & 0x0F ) << 12 ) | ( ( chr[1] & 0x3F ) << 6 ) | ( chr[2] & 0x3F ) )
	#define		UTF8_TO_CODEPOINT_4( chr )		( ( ( chr[0] & 0x07 ) << 18 ) | ( ( chr[1] & 0x3F ) << 12 ) | ( ( chr[2] & 0x3F ) << 6 ) | ( chr[3] & 0x3F ) )

	#define		UTF16_CHR_SIZE( chr )			(UTFSize) ( ( chr & 0xF800 ) ^ 0xD800 ? 1 : 2 )
	#define		UTF16_TO_CODEPOINT_1( chr )		( chr[0] )
	#define		UTF16_TO_CODEPOINT_2( chr )		( ( ( chr[0] - 0xD800 ) << 10 ) | ( chr[1] - 0xDC00 ) + 0x010000 )

	#define		UTF32_CHR_SIZE( chr )			(UTFSize) 1
	#define		UTF32_TO_CODEPOINT( chr )		( chr[0] & 0x1FFFFF )



	#define		CP_UTF8_SIZE( cp )				(UTFSize) ( ( cp < 0x80 ) ? 1 : ( cp < 0x800 ? 2 : ( cp < 0x10000 ? 3 : ( cp < 0x200000 ? 4 : 0 ) ) ) )
	#define		CP_TO_UTF8_1( cp, chr )			chr[0] = ( CHAR8 ) ( cp )
	#define		CP_TO_UTF8_2( cp, chr )			chr[0] = ( CHAR8 ) ( 0xC0 | ( ( cp >> 6 ) & 0x1F ) ); chr[1] = ( CHAR8 ) ( 0x80 | ( cp & 0x3F ) )
	#define		CP_TO_UTF8_3( cp, chr )			chr[0] = ( CHAR8 ) ( 0xE0 | ( ( cp >> 12 ) & 0x0F ) ); chr[1] = ( CHAR8 ) ( 0x80 | ( ( cp >> 6 ) & 0x3F ) ); chr[2] = ( CHAR8 ) ( 0x80 | ( cp & 0x3F ) )
	#define		CP_TO_UTF8_4( cp, chr )			chr[0] = ( CHAR8 ) ( 0xF0 | ( ( cp >> 18 ) & 0x07 ) ); chr[1] = ( CHAR8 ) ( 0x80 | ( ( cp >> 12 ) & 0x3F ) ); chr[2] = ( CHAR8 ) ( 0x80 | ( ( cp >> 6 ) & 0x3F ) ); chr[3] = ( CHAR8 ) ( 0x80 | ( cp & 0x3F ) )

	#define		CP_UTF16_SIZE( cp )				(UTFSize) ( ( cp < 0x10000 ) ? 1 : 2 )
	#define		CP_TO_UTF16_1( cp, chr )		chr[0] = ( ~( ( cp & 0xF800 ) ^ 0x27FF ) ) ? ( CHAR16 ) ( cp ) : ( CHAR16 ) CP_UNKNOWN_CONVERTION
	#define		CP_TO_UTF16_2( cp, chr )		chr[0] = ( CHAR16 ) ( ( ( ( cp - 0x010000 ) >> 10 ) & 0x03FF ) + 0xD800 ); chr[1] = ( CHAR16 ) ( ( ( cp - 0x010000 ) & 0x03FF ) + 0xDC00 )

	#define		CP_UTF32_SIZE( cp )				(UTFSize) 1
	#define		CP_TO_UTF32( cp, chr )			chr[0] = (CHAR32) ( cp & 0x1FFFFF )



	#define		UTF8_FIRST_BYTE( chr )			( ( chr & 0x80 ) ? ( chr & 0x40 ) : 0x01 )
	#define		UTF8_IS_ASCII( chr )			( ~( chr & 0x80 ) )



	class Codepoint;
	class _UString_const_iterator;
	class _UString_iterator;
	class _UString_const_reverse_iterator;
	class _UString_reverse_iterator;
	class UString;



	template<typename _Elem>
	unsigned int strlen( const _Elem* _e_str )
	{
		unsigned int size(0);
		const _Elem* e_ptr = _e_str;
		while ( *e_ptr ) ++size, ++e_ptr;
		return size;
	};



	class Codepoint
	{
		friend class UString;

	public:

		inline static UTFSize size( CHAR8 _firstChr )
		{
			return UTF8_CHR_SIZE( _firstChr );
		};



	private:

		typedef CHAR32 (* Utf8ToCodepoint )( const CHAR8* /*_ptrToEnc*/ );
		inline static CHAR32 utf8ToCodepoint_ERROR( const CHAR8* _ptrToEnc )	{	return 0;	};
		inline static CHAR32 utf8ToCodepoint_1( const CHAR8* _ptrToEnc )		{	return UTF8_TO_CODEPOINT_1( _ptrToEnc );	};
		inline static CHAR32 utf8ToCodepoint_2( const CHAR8* _ptrToEnc )		{	return UTF8_TO_CODEPOINT_2( _ptrToEnc );	};
		inline static CHAR32 utf8ToCodepoint_3( const CHAR8* _ptrToEnc )		{	return UTF8_TO_CODEPOINT_3( _ptrToEnc );	};
		inline static CHAR32 utf8ToCodepoint_4( const CHAR8* _ptrToEnc )		{	return UTF8_TO_CODEPOINT_4( _ptrToEnc );	};

		static Utf8ToCodepoint utf8ToCodepoint[5];

		typedef CHAR8* (* CodepointToUtf8 )( const CODEPOINT /*_cp*/, CHAR8* /*_ptrToEnc*/ );
		inline static CHAR8* codepointToUtf8_ERROR( const CODEPOINT _cp, CHAR8* _ptrToEnc )	{	return _ptrToEnc;	};
		inline static CHAR8* codepointToUtf8_1( const CODEPOINT _cp, CHAR8* _ptrToEnc )		{	CP_TO_UTF8_1( _cp, _ptrToEnc );	return _ptrToEnc;	};
		inline static CHAR8* codepointToUtf8_2( const CODEPOINT _cp, CHAR8* _ptrToEnc )		{	CP_TO_UTF8_2( _cp, _ptrToEnc );	return _ptrToEnc;	};
		inline static CHAR8* codepointToUtf8_3( const CODEPOINT _cp, CHAR8* _ptrToEnc )		{	CP_TO_UTF8_3( _cp, _ptrToEnc );	return _ptrToEnc;	};
		inline static CHAR8* codepointToUtf8_4( const CODEPOINT _cp, CHAR8* _ptrToEnc )		{	CP_TO_UTF8_4( _cp, _ptrToEnc );	return _ptrToEnc;	};

		static CodepointToUtf8 codepointToUtf8[5];


	public:

		inline static bool validateEncodedCP( const CHAR8* _ptrToEnc, UTFSize _length = 0 )
		{
			if ( _length && _length != UTF8_CHR_SIZE( *_ptrToEnc ) )
				return false;

			while ( --_length )
			{
				++_ptrToEnc;
				if ( UTF8_FIRST_BYTE( *_ptrToEnc ) )
					return false;
			};
			
			return true;
		};


		inline static CHAR32 getValue( const CHAR8* _ptrToEnc )
		{
			int r = UTF8_CHR_SIZE( *_ptrToEnc );
			return ( utf8ToCodepoint[ r ] )( _ptrToEnc );
		};


		inline static std::pair<UTFSize,UTFSize> convertCH32toCH16( const CHAR32* _enc32, CHAR16* _enc16 )
		{
			if ( *_enc32 < 0x10000 )
			{
				CP_TO_UTF16_1( *_enc32, _enc16 );
				return std::pair<UTFSize,UTFSize>(1,1);
			}
			CP_TO_UTF16_2( *_enc32, _enc16 );
			return std::pair<UTFSize,UTFSize>(1,2);
		};

		inline static std::pair<UTFSize,UTFSize> convertCH32toCH8( const CHAR32* _enc32, CHAR8* _enc8 )
		{
			UTFSize s = CP_UTF8_SIZE( *_enc32 );
			codepointToUtf8[ s ]( *_enc32, _enc8 );
			return std::pair<UTFSize,UTFSize>(1,s);
		};

		inline static std::pair<UTFSize,UTFSize> convertCH16toCH32( const CHAR16* _enc16, CHAR32* _enc32 )
		{
			if ( ( *_enc16 & 0xF800 ) ^ 0xD800 )
			{
				*_enc32 = *_enc16;
				return std::pair<UTFSize,UTFSize>(1,1);
			}

			*_enc32 = UTF16_TO_CODEPOINT_2( _enc16 );
			return std::pair<UTFSize,UTFSize>(2,1);
		};

		inline static std::pair<UTFSize,UTFSize> convertCH16toCH8( const CHAR16* _enc16, CHAR8* _enc8 )
		{
			if ( ( *_enc16 & 0xF800 ) ^ 0xD800 )
			{
				UTFSize s = CP_UTF8_SIZE( *_enc16 );
				codepointToUtf8[ s ]( *_enc16, _enc8 );
				return std::pair<UTFSize,UTFSize>(1,s);
			}

			codepointToUtf8[ 4 ]( UTF16_TO_CODEPOINT_2( _enc16 ), _enc8 );
			return std::pair<UTFSize,UTFSize>(2,4);
		};

		inline static std::pair<UTFSize,UTFSize> convertCH8toCH32( const CHAR8* _enc8, CHAR32* _enc32 )
		{
			UTFSize s = UTF8_CHR_SIZE( *_enc8 );
			*_enc32 = utf8ToCodepoint[ s ]( _enc8 );
			return std::pair<UTFSize,UTFSize>(s,1);
		};

		inline static std::pair<UTFSize,UTFSize> convertCH8toCH16( const CHAR8* _enc8, CHAR16* _enc16 )
		{
			UTFSize s = UTF8_CHR_SIZE( *_enc8 );

			if ( s < 4 )
			{
				CP_TO_UTF16_1( utf8ToCodepoint[ s ]( _enc8 ), _enc16 );
				return std::pair<UTFSize,UTFSize>(s,1);
			}

			CP_TO_UTF16_2( utf8ToCodepoint[ 4 ]( _enc8 ), _enc16 );
			return std::pair<UTFSize,UTFSize>(s,2);
		};


	private:

		mutable CHAR32 value;
		mutable UTFSize encSize;
		mutable UString* pUstr;

		union
		{
			//pointer goes first, it has at least 4 bytes in 86x and 8 bytes in 64x and as such ensures that the union have size big enough
			//with this setting we can happily copy the pointer and be sure that utf8 cp encoding is copied too
			mutable CHAR8* ptrToEnc;
			mutable CHAR8 utf8Enc[ UNICODE_MAX_UTF8_LENGTH ];
			//CHAR16 utf16Enc[ UNICODE_MAX_UTF16_LENGTH ];
			//CHAR32 utf32Enc;
		};


		inline CHAR8* getPtr()
		{
			return pUstr ? ptrToEnc : &(utf8Enc[0]);
		};

		void _resizeEncArr( UTFSize _newEncSize );

		void setValue( CHAR32 _value );
		void setValue( const Codepoint& _cp );
		void setValue( const CHAR8* _ptrToEnc );


	public:

		Codepoint( CHAR32 _value = 0 );
		Codepoint( const CHAR8* _ptrToEnc );
		Codepoint( UString* _pUstr, CHAR8* _ptrToEnc );
		Codepoint( const UString* _pUstr, const CHAR8* _ptrToEnc );
		Codepoint( const Codepoint& _rhs );

		#if __cplusplus >= 201103L
			Codepoint( Codepoint&& _rhs );
		#endif

		~Codepoint();

		Codepoint& operator= ( Codepoint& _rhs );

		#if __cplusplus >= 201103L
			Codepoint& operator= ( Codepoint&& _rhs );
		#endif

		inline operator CHAR32 () const
		{
			return value;
		};

		inline operator char () const
		{
			return UTF8_IS_ASCII( value ) ? value : CP_UNKNOWN_CONVERTION;
		};

		inline CHAR32 getVal() const
		{
			return value;
		};

		inline UTFSize getValUtf16( CHAR16* _out ) const
		{
			if ( value < 0x10000 )
			{
				CP_TO_UTF16_1( value, _out );
				return 1;
			}

			CP_TO_UTF16_2( value, _out );
			return 2;
		};

		inline Codepoint& operator=( CHAR32 _cp )
		{
			setValue( (CHAR32) _cp );
		};

		inline Codepoint& operator=( const CHAR8* _ptrToEnc )
		{
			setValue( _ptrToEnc );
		};

		inline Codepoint& operator=( const CHAR16* _ptrToEnc )
		{
			if ( ( *_ptrToEnc & 0xF800 ) ^ 0xD800 )
				setValue( UTF16_TO_CODEPOINT_1( _ptrToEnc ) );
			else
				setValue( UTF16_TO_CODEPOINT_2( _ptrToEnc ) );
		};

		inline Codepoint& operator=( char _cp )
		{
			setValue( (CHAR32) _cp );
		};

		inline UTFSize size() const
		{
			return encSize;
		};

		inline UTFSize copy( CHAR8* _out )
		{
			if ( _out )
			{
				memcpy( _out, getPtr(), encSize );
				//std::copy( getPtr(), getPtr() + encSize, _out ); //generate warning on each use in MSVC
			}
			return encSize;
		};


		Codepoint& operator+=( int _off );
		Codepoint operator+( int _off );
		Codepoint& operator-=( int _off );
		Codepoint operator-( int _off );
		Codepoint& operator++();
		Codepoint operator++(int);
		Codepoint& operator--();
		Codepoint operator--(int);

		inline bool operator== ( const Codepoint& _rhs ) const
		{
			return value == _rhs.value;
		};

		inline bool operator!= ( const Codepoint& _rhs ) const
		{
			return value != _rhs.value;
		};

		inline bool operator<= ( const Codepoint& _rhs ) const
		{
			return value <= _rhs.value;
		};

		inline bool operator>= ( const Codepoint& _rhs ) const
		{
			return value >= _rhs.value;
		};

		inline bool operator< ( const Codepoint& _rhs ) const
		{
			return value < _rhs.value;
		};

		inline bool operator> ( const Codepoint& _rhs ) const
		{
			return value > _rhs.value;
		};

	};


	class _UString_const_iterator
	{	// Iterator for nonmutable string

		friend class UString;
		friend _UString_const_reverse_iterator;

	public:
		typedef _UString_const_iterator Myt;
		typedef const UString Mystring;
		typedef CHAR8 Valuetype;
		typedef const CHAR8* Pointer;
		typedef const Codepoint Reference;
		typedef int Differencetype;
		typedef size_t Indextype;

	protected:

		const Mystring* mycont;
		Pointer myptr;
		Indextype myindex;


		inline const Pointer getPtr()
		{
			return myptr;
		};

	public:

		_UString_const_iterator();

	#define _USTRING_CONST_ITERATOR( ptr, index )	_UString_const_iterator( ptr, index, this )
		_UString_const_iterator( Pointer _ptr, Indextype _index, Mystring* _string );

		Indextype getIndex() const;
		Reference operator*() const;

		Myt& operator++();
		Myt operator++(int);
		Myt& operator--();
		Myt operator--(int);

		Myt& operator+=( Differencetype _off );
		Myt operator+( Differencetype _off ) const;
		Myt& operator-=( Differencetype _off );
		Myt operator-( Differencetype _off ) const;

		Differencetype operator-( const Myt& _right ) const;
		Reference operator[]( Differencetype _off ) const;

		bool operator==( const Myt& _right ) const;
		bool operator!=( const Myt& _right ) const;
		bool operator<( const Myt& _right ) const;
		bool operator>( const Myt& _right ) const;
		bool operator<=( const Myt& _right ) const;
		bool operator>=( const Myt& _right ) const;

		static void _Xlen();
		static void _Xran();
		static void _Xinvarg();
	};


	class _UString_iterator : public _UString_const_iterator
	{	// Iterator for mutable string
		friend class UString;
		friend class _UString_reverse_iterator;

	public:
		typedef _UString_iterator Myt;
		typedef _UString_const_iterator Mybase;
		typedef UString Mystring;
		typedef CHAR8 Valuetype;
		typedef CHAR8* Pointer;
		typedef Codepoint Reference;
		typedef int Differencetype;
		typedef size_t Indextype;


		_UString_iterator();

	#define _USTRING_ITERATOR( ptr, index )	_UString_iterator( ptr, index, this )
		_UString_iterator( Pointer _ptr, Indextype _index, Mystring* _string );

		Reference operator*();

		Myt& operator++();
		Myt operator++(int);
		Myt& operator--();
		Myt operator--(int);

		Myt& operator+=( Differencetype _off );
		Myt operator+( Differencetype _off );
		Myt& operator-=( Differencetype _off );
		Myt operator-( Differencetype _off );

		Differencetype operator-( const Mybase& _right ) const;
		Reference operator[]( Differencetype _off );
	};


	class _UString_const_reverse_iterator
	{	// Iterator for nonmutable string
		friend  class UString;

	public:

		typedef _UString_const_reverse_iterator Myt;
		typedef _UString_const_iterator RanIt;
		typedef const UString Mystring;
		typedef CHAR8 Valuetype;
		typedef const CHAR8* Pointer;
		typedef const Codepoint Reference;
		typedef int Differencetype;
		typedef size_t Indextype;


	protected:

		RanIt current;// the wrapped Iterator


		inline Pointer getPtr()
		{
			return current.getPtr();
		};


		_UString_const_reverse_iterator( RanIt& _current );

	public:

		_UString_const_reverse_iterator();

	#define _USTRING_CONST_REVERSE_ITERATOR( ptr, index )	_UString_const_reverse_iterator( ptr, index, this )
		_UString_const_reverse_iterator( Pointer _ptr, Indextype _index, Mystring* _string );

		RanIt base() const;
		Indextype getIndex() const;
		Reference operator*();

		Myt& operator++();
		Myt operator++(int);
		Myt& operator--();
		Myt operator--(int);

		Myt& operator+=( Differencetype _off );
		Myt operator+( Differencetype _off );
		Myt& operator-=( Differencetype _off );
		Myt operator-( Differencetype _off );

		Differencetype operator-( const Myt& _right ) const;
		Reference operator[]( Differencetype _off );

		bool operator==( const Myt& _right ) const;
		bool operator!=( const Myt& _right ) const;
		bool operator<( const Myt& _right ) const;
		bool operator>( const Myt& _right ) const;
		bool operator<=( const Myt& _right ) const;
		bool operator>=( const Myt& _right ) const;

		static void _Xlen();
		static void _Xran();
		static void _Xinvarg();
	};


	class _UString_reverse_iterator
	{	// Iterator for nonmutable string
		friend  class UString;

	public:

		typedef _UString_reverse_iterator Myt;
		typedef _UString_iterator RanIt;
		typedef UString Mystring;
		typedef CHAR8 Valuetype;
		typedef CHAR8* Pointer;
		typedef Codepoint Reference;
		typedef int Differencetype;
		typedef size_t Indextype;


	protected:

		RanIt current;// the wrapped Iterator


		inline Pointer getPtr()
		{
			return const_cast<Pointer>(current.getPtr());
		};


		_UString_reverse_iterator( RanIt& _current );

	public:

		_UString_reverse_iterator();

	#define _USTRING_REVERSE_ITERATOR( ptr, index )	_UString_reverse_iterator( ptr, index, this )
		_UString_reverse_iterator( Pointer _ptr, Indextype _index, Mystring* _string );

		RanIt base() const;
		Indextype getIndex() const;
		Reference operator*();

		Myt& operator++();
		Myt operator++(int);
		Myt& operator--();
		Myt operator--(int);

		Myt& operator+=( Differencetype _off );
		Myt operator+( Differencetype _off );
		Myt& operator-=( Differencetype _off );
		Myt operator-( Differencetype _off );

		Differencetype operator-( const Myt& _right ) const;
		Reference operator[]( Differencetype _off );

		bool operator==( const Myt& _right ) const;
		bool operator!=( const Myt& _right ) const;
		bool operator<( const Myt& _right ) const;
		bool operator>( const Myt& _right ) const;
		bool operator<=( const Myt& _right ) const;
		bool operator>=( const Myt& _right ) const;

		static void _Xlen();
		static void _Xran();
		static void _Xinvarg();
	};
/*
	std::string str;
	typedef basic_string<char, char_traits<char>, allocator<char> >
		template<class _Elem,
	class _Traits,
	class _Alloc>
	class basic_string
		: public _String_alloc<!is_empty<allocator<char>>::value,
		_String_base_types<char, allocator<char>> >

template<bool _Al_has_storage,
	class _Alloc_types>
	class _String_alloc
		: public _String_val<typename _String_base_types<char, allocator<char>>::_Val_types>

*/

	class UString : public std::_String_val< std::_String_base_types<char, std::allocator<char>>::_Val_types >
	{	// null-terminated transparent array of elements
		friend class UString;

	public:

		friend class _UString_const_iterator;
		friend class _UString_iterator;
		friend class _UString_const_reverse_iterator;
		friend class _UString_reverse_iterator;
		friend class Codepoint;


		typedef UString Myt;
		typedef std::_String_base_types<char, std::allocator<char>> Mybase_types;
		typedef std::_String_val< std::_String_base_types<char, std::allocator<char>>::_Val_types > Mybase;
		typedef size_t Sizetype;
		typedef CHAR8 Valuetype;
		typedef const Valuetype * Pointer;
		typedef const Valuetype * ConstPointer;
		typedef Codepoint Reference;
		typedef const Codepoint ConstReference;

		typedef CHAR8 Elem;

		typedef _UString_const_iterator ConstIterator;
		typedef _UString_iterator Iterator;

		typedef _UString_const_reverse_iterator ConstReverseIterator;
		typedef _UString_reverse_iterator ReverseIterator;


		static const Sizetype npos = (Sizetype) -1;	// generic bad/missing length/position

	protected:
		Sizetype mylength;	// current length of string
		Sizetype mysize;	// current size of string
		Sizetype myres;	// current storage reserved for string


		static const Sizetype BUF_SIZE = 16;
		static const Sizetype ALLOC_MASK = 15;


		union Bxty
		{	// storage for small buffer or pointer to larger one
			Elem buf[ BUF_SIZE ];
			Elem* ptr;
		} bx;


		Elem* _myptr();
		const Elem* _myptr() const;

		void _eos( Sizetype _newsize, Sizetype _newlength );
		void _tidy( bool _built = false,  Sizetype _newsize = 0, Sizetype _newlength = 0 );
		void _copy( Sizetype _newsize, Sizetype _oldsize );
		void _copyChangeMidstr( Sizetype _midIndex, Sizetype _midOldSize, Sizetype _midNewSize );

		inline void _resizeCodepoint( Pointer _ptrToEncCP, UTFSize _encCPSize, UTFSize _encCPNewSize )
		{
			_copyChangeMidstr( _ptrToEncCP - _myptr(), _encCPSize, _encCPNewSize );
		};

		void _chassign( Sizetype _indexOff, Sizetype _lcount, Reference _chr );

		bool _inside( const Elem* _ptr );
		static Sizetype _pdif( ConstIterator _P1, ConstIterator _P2 );
		bool _grow( Sizetype _newsize, bool _trim = false );

		static bool _validate_UTF8( const Elem* _ptr );
		static bool _validateL_UTF8( const Elem* _ptr, Sizetype _length );
		static bool _validateS_UTF8( const Elem* _ptr, Sizetype _size );
		static bool _validateLS_UTF8( const Elem* _ptr, Sizetype _length, Sizetype _size );

		static bool _validateGetL_UTF8( const Elem* _ptr, Sizetype* _length );
		static bool _validateGetS_UTF8( const Elem* _ptr, Sizetype* _size );
		static bool _validateGetLS_UTF8( const Elem* _ptr, Sizetype* _length, Sizetype* _size );
		static bool _validateLGetS_UTF8( const Elem* _ptr, Sizetype _length, Sizetype* _size );
		static bool _validateSGetL_UTF8( const Elem* _ptr, Sizetype _size, Sizetype* _length );

		static void _getL_UTF8( const Elem* _ptr, Sizetype* _length, Sizetype* _size = NULL );
		static void _getS_UTF8( const Elem* _ptr, Sizetype* _size, Sizetype* _length = NULL );
		static void _getLS_UTF8( const Elem* _ptr, Sizetype* _length = NULL, Sizetype* _size = NULL );


	public:

		static Sizetype length( const Elem* _ptr );
		inline Sizetype length() const
		{
			return mylength;
		};

		static Sizetype size( const Elem* _ptr );
		inline Sizetype size() const
		{
			return mysize;
		};


		static void _Xlen();
		static void _Xran();
		static void _Xinvarg();


		UString();
		UString( const Myt& _right, Sizetype _roff, Sizetype _count = npos );
		UString( const std::string& _str_str );
		UString( const Elem* _ptr, Sizetype _count = npos );
		UString( Reference _chr, Sizetype _count );

		template<class _It>
		UString( _It _first, _It _last ) : Mybase(), mylength(0), mysize(0), myres(0)
		{	// construct from [_first, _last)
			_tidy();
			_construct( _first, _last, std::_Iter_cat(_first) );
		};

		UString( ConstPointer _first, ConstPointer _last );
		UString( ConstIterator _first, ConstIterator _last );
		UString( const Myt& _right );

	/*
		template<class _It>
		void _construct( _It _chr, _It _count, std::_Int_iterator_tag )
		{	// initialize from _count * _Ch
			assign( Reference( (CHAR32) _chr ), (Sizetype) _count );
		};
	*/

		template<class _It>
		void _construct( _It _first, _It _last, std::input_iterator_tag )
		{	// initialize from [_first, _last), input iterators
			try
			{
				for (; _first != _last; ++_first)
				{
					append( (Reference)*_first, (Sizetype) 1 );
				}
			}
			catch (...)
			{
				_tidy(true);
				throw;
			}
		};

		template<class _It>
		void _construct( _It _first, _It _last, std::forward_iterator_tag )
		{	// initialize from [_first, _last), forward iterators
			Sizetype count = 0;
			std::_Distance( _first, _last, count);
			reserve( count );

			try
			{
				for (; _first != _last; ++_first)
				{
					append( (Reference)*_first, (Sizetype) 1 );
				}
			}
			catch (...)
			{
				_tidy(true);
				throw;
			}
		};

		~UString();

		Myt& operator=( const Myt& _right );
		Myt& operator=( const Elem* _ptr );
		Myt& operator=( Reference _chr );
		Myt& operator+=( const Myt& _right );
		Myt& operator+=( const Elem* _ptr );
		Myt& operator+=( Reference _chr );

		Myt& append( const Myt& _right );
		Myt& append( const Myt& _right, Sizetype _roff, Sizetype _count );
		Myt& append( const Elem* _ptr, Sizetype _count );
		Myt& append( const Elem* _ptr );
		Myt& append( Reference _chr, Sizetype _count = 1 );

		template<class _It>
		Myt& append( _It _first, _It _last )
		{	// append [_First, _Last)
			return ( _append( _first, _last, std::_Iter_cat(_first) ) );
		};

	/*
		template<class _It>
		Myt& _append(_It _count, _It _chr, std::_Int_iterator_tag )
		{	// append _count * _Ch
			return ( append( (Sizetype) _count, Reference( _chr ) ) );
		};
	*/

		template<class _It>
		Myt& _append(_It _first, _It _last, std::input_iterator_tag )
		{	// append [_First, _Last), input iterators
			return ( replace( end(), end(), _first, _last ) );
		};

		Myt& append( ConstPointer _first, ConstPointer _last );
		Myt& append( ConstIterator _first, ConstIterator _last );
		Myt& assign( const Myt& _right );
		Myt& assign( const Myt& _right, Sizetype _roff, Sizetype _count );
		Myt& assign( const Elem* _ptr, Sizetype _count );
		Myt& assign( const Elem* _ptr );
		Myt& assign( Reference _chr, Sizetype _count );
		Myt& assign( Elem _chr, Sizetype _count );

		template<class _It>
		Myt& assign( _It _first, _It _last )
		{	// assign [First, _Last)
			return ( _assign( _First, _Last, std::_Iter_cat( _First ) ) );
		};

	/*
		template<class _It>
		Myt& _assign( _It _chr, _It _count, std::_Int_iterator_tag )
		{	// assign _count * _Ch
			return ( assign( Reference( _chr ), ( Sizetype ) _count ) );
		};
	*/

		template<class _It>
		Myt& _assign( _It _first, _It _last, std::input_iterator_tag )
		{	// assign [First, _Last), input iterators
			return ( replace( begin(), end(), _first, _last ) );
		};

		Myt& assign( ConstPointer _first, ConstPointer _last );
		Myt& assign( ConstIterator _first, ConstIterator _last );

		Myt& insert( Sizetype _off, const Myt& _right);
		Myt& insert( Sizetype _off, const Myt& _right, Sizetype _roff, Sizetype _count );
		Myt& insert( Sizetype _off, const Elem* _ptr, Sizetype _count );
		Myt& insert( Sizetype _off, const Elem* _ptr);
		Myt& insert( Sizetype _off, Sizetype _count, Reference _chr );
		Iterator insert( ConstIterator _where );
		Iterator insert( ConstIterator _where, Reference _chr );
		void insert( ConstIterator _where, Reference _chr, Sizetype _count );

		template<class _It>
		void insert( ConstIterator _where, _It _first, _It _last)
		{	// insert [_first, _last) at _where
			_insert( _where, _first, _last, std::_Iter_cat( _first ) );
		};

	/*
		template<class _It>
		void _insert( ConstIterator _where, _It _chr, _It _count, std::_Int_iterator_tag )
		{	// insert _count * _Ch at _Where
			insert( _where, Reference( _chr ), ( Sizetype ) _count );
		};
	*/

		template<class _It>
		void _insert( ConstIterator _where, _It _first, _It _last, std::input_iterator_tag )
		{	// insert [_First, _Last) at _Where, input iterators
			replace( _where, _where, _first, _last);
		};

		void insert( ConstIterator _where, ConstPointer _first, ConstPointer _last );
		void insert( ConstIterator _where, ConstIterator _first, ConstIterator _last);

		Myt& erase ( Sizetype _off = 0, Sizetype _count = npos );
		Iterator erase( ConstIterator _where );
		Iterator erase( ConstIterator _first, ConstIterator _last );
		void clear();

		Myt& replace( Sizetype _off, Sizetype _n0, const Myt& _right );
		Myt& replace( Sizetype _off, Sizetype _n0, const Myt& _right, Sizetype _roff, Sizetype _count );
		Myt& replace( Sizetype _off, Sizetype _n0, const Elem* _ptr, Sizetype _count );
		Myt& replace( Sizetype _off, Sizetype _n0, const Elem* _ptr );
		Myt& replace( Sizetype _off, Sizetype _n0, Sizetype _count, Reference _chr );

		Myt& replace( ConstIterator _first, ConstIterator _last, const Myt& _right );
		Myt& replace( ConstIterator _first, ConstIterator _last, const Elem* _ptr, Sizetype _count );
		Myt& replace( ConstIterator _first, ConstIterator _last, const Elem* _ptr );
		Myt& replace( ConstIterator _first, ConstIterator _last, Sizetype _count, Reference _chr );

		template<class _It>
		Myt& replace( ConstIterator _first, ConstIterator _last, _It _first2, _It _last2 )
		{	// replace [_First, _Last) with [_First2, _Last2)
			return ( _replace( _first, _last, _first2, _last2, std::_Iter_cat( _first2 ) ) );
		};

	/*
		template<class _It>
		Myt& _Replace( ConstIterator _first, ConstIterator _last, _It _chr, _It _count, std::_Int_iterator_tag )
		{	// replace [_First, _Last) with _count * _Ch
			return ( replace( _first, _last, Reference( _chr ), (Sizetype)_count ) );
		};
	*/

		template<class _It>
		Myt& _Replace( ConstIterator _first, ConstIterator _last, _It _first2, _It _last2, std::input_iterator_tag )
		{	// replace [_First, _Last) with [_First2, _Last2), input iterators
			Myt _right( _first2, _last2 );
			return replace( _first, _last, _right );
		};

		Myt& replace( ConstIterator _first, ConstIterator _last, ConstPointer _first2, ConstPointer _last2 );
		Myt& replace( ConstIterator _first, ConstIterator _last, ConstIterator _first2, ConstIterator _last2 );
		Iterator begin();
		ConstIterator begin() const;
		Iterator end();
		ConstIterator end() const;
		ReverseIterator rbegin();
		ConstReverseIterator rbegin() const;
		ReverseIterator rend();
		ConstReverseIterator rend() const;
		Reference at( Sizetype _off );
		ConstReference at( Sizetype _off ) const;
		Reference operator[]( Sizetype _off );
		ConstReference operator[]( Sizetype _off ) const;
		void push_back( Reference _chr );
		const Elem* c_str() const;
		const Elem* data() const;
		Sizetype maxSize() const;
		void resize( Sizetype _newlength );
		void resize( Sizetype _newlength, Reference _chr );
		Sizetype capacity() const;
		void reserve( Sizetype _newcap = 0 );
		bool empty() const;
		Sizetype copy( Elem* _dest, Sizetype _count, Sizetype _off = 0 ) const;

		Sizetype _copy_s( Elem* _dest, /*Sizetype _Dest_size,*/ Sizetype _count, Sizetype _off = 0 ) const;
		void swap( Myt& _right );

		Sizetype find( const Myt& _right, Sizetype _off = 0 ) const;
		Sizetype find( const Elem* _ptr, Sizetype _off, Sizetype _count ) const;
		Sizetype find( const Elem *_ptr, Sizetype _off = 0 ) const;
		Sizetype find( Reference _chr, Sizetype _off = 0 ) const;

		Sizetype rfind( const Myt& _right, Sizetype _off = npos ) const;
		Sizetype rfind( const Elem* _ptr, Sizetype _off, Sizetype _count ) const;
		Sizetype rfind( const Elem* _ptr, Sizetype _off = npos ) const;
		Sizetype rfind( Reference _chr, Sizetype _off = npos ) const;

		Sizetype find_first_of( const Myt& _right, Sizetype _off = 0 ) const;
		Sizetype find_first_of( const Elem* _ptr, Sizetype _off, Sizetype _count ) const;
		Sizetype find_first_of( const Elem* _ptr, Sizetype _off = 0 ) const;
		Sizetype find_first_of( Reference _chr, Sizetype _off = 0 ) const;

		Sizetype find_last_of( const Myt& _right, Sizetype _off = npos ) const;
		Sizetype find_last_of( const Elem* _Ptr, Sizetype _off, Sizetype _count ) const;
		Sizetype find_last_of( const Elem* _ptr, Sizetype _off = npos ) const;
		Sizetype find_last_of( Reference _chr, Sizetype _off = npos ) const;

		Sizetype find_first_not_of( const Myt& _right, Sizetype _off = 0 ) const;
		Sizetype find_first_not_of( const Elem* _ptr, Sizetype _off, Sizetype _count ) const;
		Sizetype find_first_not_of( const Elem *_ptr, Sizetype _off = 0 ) const;
		Sizetype find_first_not_of( Reference _chr, Sizetype _off = 0 ) const;

		Sizetype find_last_not_of( const Myt& _right, Sizetype _off = npos ) const;
		Sizetype find_last_not_of( const Elem* _ptr, Sizetype _off, Sizetype _count ) const;
		Sizetype find_last_not_of( const Elem* _ptr, Sizetype _off = npos ) const;
		Sizetype find_last_not_of( Reference _chr, Sizetype _off = npos ) const;

		Myt substr( Sizetype _off = 0, Sizetype _count = npos ) const;

		int compare( const Myt& _right ) const;
		int compare( Sizetype _off, Sizetype _n0, const Myt& _right ) const;
		int compare( Sizetype _off, Sizetype _n0, const Myt& _right, Sizetype _roff, Sizetype _count ) const;

		int compare( const Elem* _ptr ) const;
		int compare( Sizetype _off, Sizetype _n0, const Elem* _ptr ) const;
		int compare( Sizetype _off, Sizetype _n0, const Elem* _ptr, Sizetype _count ) const;

		static int compare( const Myt& _left, const Myt& _right );
		static int compare( const Myt& _left, Sizetype _loff, Sizetype _lcount, const Myt& _right, Sizetype _roff, Sizetype _rcount );
		static int _compare( const Elem* _ptrLeft, Sizetype _offLeft, const Elem* _ptrRight, Sizetype _offRight );



		std::string getString() const;
		operator std::string() const { return getString(); };


		SLpair getString8( CHAR8* _ptrOut );
		SLpair getString16( CHAR16* _ptrOut );
		SLpair getString32( CHAR32* _ptrOut );


	};



	UString str_trim( const UString& _str_text );

} //Unicode


_STD_BEGIN

// basic_ostream ostream and wostream operator<< definition for Unicode UString and Codepoint

ostream& operator<< ( ostream& _Ostr, const Unicode::UString& _Str );
ostream& operator<< ( ostream& _Ostr, Unicode::Codepoint _Ch );
wostream& operator<< ( wostream& _Ostr, const Unicode::UString& _Str );
wostream& operator<< ( wostream& _Ostr, Unicode::Codepoint _Ch );

_STD_END


//#pragma warning(default:4996)

#endif //__UNICODE__
