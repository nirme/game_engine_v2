#include "Unicode.h"

#pragma warning(disable:4996)
#pragma warning(disable:4018)

namespace Unicode
{

	Codepoint::Utf8ToCodepoint Codepoint::utf8ToCodepoint[] = 
	{
		Codepoint::utf8ToCodepoint_ERROR, 
		Codepoint::utf8ToCodepoint_1, 
		Codepoint::utf8ToCodepoint_2, 
		Codepoint::utf8ToCodepoint_3, 
		Codepoint::utf8ToCodepoint_4 
	};


	Codepoint::CodepointToUtf8 Codepoint::codepointToUtf8[] = 
	{
		Codepoint::codepointToUtf8_ERROR, 
		Codepoint::codepointToUtf8_1, 
		Codepoint::codepointToUtf8_2, 
		Codepoint::codepointToUtf8_3, 
		Codepoint::codepointToUtf8_4 
	};

	////////////////////////////////////////////////////////////////
	////////////                Codepoint               ////////////
	////////////////////////////////////////////////////////////////

	void Codepoint::_resizeEncArr( UTFSize _newEncSize )
	{
		if ( pUstr )
			pUstr->_resizeCodepoint( ptrToEnc, encSize, _newEncSize );
		encSize = _newEncSize;
	};

	void Codepoint::setValue( CHAR32 _value )
	{
		UTFSize newSize = CP_UTF8_SIZE( _value );

		if ( encSize != newSize )
			_resizeEncArr( newSize );

		codepointToUtf8[ newSize ]( _value, getPtr() );
		value = _value;
	};

	void Codepoint::setValue( const Codepoint& _cp )
	{
		setValue( ( CHAR32 ) _cp );
	};

	void Codepoint::setValue( const CHAR8* _ptrToEnc )
	{
		UTFSize newSize = UTF8_CHR_SIZE( *_ptrToEnc );

		if ( encSize != newSize )
			_resizeEncArr( newSize );

		std::copy( _ptrToEnc, _ptrToEnc + encSize, getPtr() );
		value = utf8ToCodepoint[ newSize ]( _ptrToEnc );
	};

	Codepoint::Codepoint( CHAR32 _value ) : value( 0 ), encSize( 0 ), pUstr( NULL ), ptrToEnc( NULL )
	{
		setValue( _value );
	};

	Codepoint::Codepoint( const CHAR8* _ptrToEnc ) : value( 0 ), encSize( 0 ), pUstr( NULL ), ptrToEnc( NULL )
	{
		setValue( _ptrToEnc );
	};

	Codepoint::Codepoint( UString* _pUstr, CHAR8* _ptrToEnc ) : pUstr( _pUstr ), ptrToEnc( _ptrToEnc ), encSize( 0 ), value( 0 )
	{
		if ( _pUstr )
		{
			encSize = UTF8_CHR_SIZE( *_ptrToEnc );
			value = getValue( _ptrToEnc );
		}
		else
			setValue( _ptrToEnc );
	};

	Codepoint::Codepoint( const UString* _pUstr, const CHAR8* _ptrToEnc ) : pUstr( NULL ), ptrToEnc( NULL ), encSize( 0 ), value( 0 )
	{
		pUstr = const_cast<UString*>( _pUstr );
		ptrToEnc = const_cast<CHAR8*>( _ptrToEnc );

		if ( _pUstr )
		{
			encSize = UTF8_CHR_SIZE( *_ptrToEnc );
			value = getValue( _ptrToEnc );
		}
		else
			setValue( _ptrToEnc );
	};

	Codepoint::Codepoint ( const Codepoint& _rhs ) : value( _rhs.value ), encSize( _rhs.encSize ), pUstr( _rhs.pUstr ), ptrToEnc( _rhs.ptrToEnc )
	{	//copy constructor
	};

	#if __cplusplus >= 201103L
		Codepoint::Codepoint ( Codepoint&& _rhs ) : value( _rhs.value ), encSize( _rhs.encSize ), pUstr( _rhs.pUstr ), ptrToEnc( _rhs.ptrToEnc )
		{	//move constructor
			//set tmp object pointers to NULL to ensure moved mem doesn't get freed by destructor
			if ( this != &_rhs )
			{
				_rhs.value = 0;
				_rhs.encSize = 0;
				_rhs.pUstr = NULL;
				_rhs.ptrToEnc = NULL;
			}
		};
	#endif

	Codepoint::~Codepoint ()
	{};

	Codepoint& Codepoint::operator= ( Codepoint& _rhs )
	{	//copy assignment
		if ( this == &_rhs )
			return *this;

		value = _rhs.value;
		encSize = _rhs.encSize;
		pUstr = _rhs.pUstr;
		ptrToEnc = _rhs.ptrToEnc;

		return *this;
	};

	#if __cplusplus >= 201103L
		Codepoint& Codepoint::operator= ( Codepoint&& _rhs )
		{	//move assignment
			if ( this == &_rhs )
				return *this;

			value = _rhs.value
			encSize = _rhs.encSize;
			pUstr = _rhs.pUstr;
			ptrToEnc = _rhs.ptrToEnc;

			//set tmp object pointers to NULL to ensure moved mem doesn't get freed by destructor
			_rhs.value = 0;
			_rhs.encSize = 0;
			_rhs.pUstr = NULL;
			_rhs.ptrToEnc = NULL;

			return *this;
		};
	#endif

	Codepoint& Codepoint::operator+=( int _off )
	{	// increment by int
		if ( _off > ( UNICODE_MAX_CP_VALUE - value ) )
			setValue( UNICODE_MAX_CP_VALUE );
		else if ( _off < ( -value ) )
			setValue( 0 );
		else
			setValue( value + _off );

		return *this;
	};

	Codepoint Codepoint::operator+( int _off )
	{	// return this + int
		Codepoint tmp = *this;
		return ( tmp += _off );
	};

	Codepoint& Codepoint::operator-=( int _off )
	{	// decrement by int
		operator+=( -(_off ) );
		return *this;
	};

	Codepoint Codepoint::operator-( int _off )
	{	// return this - int
		Codepoint tmp = *this;
		return ( tmp -= _off );
	};

	Codepoint& Codepoint::operator++()
	{
		if ( value < UNICODE_MAX_CP_VALUE )
			setValue( ++value );
		return *this;
	};

	Codepoint Codepoint::operator++(int)
	{
		Codepoint tmp(*this);
		operator++();
		return tmp;
	};

	Codepoint& Codepoint::operator--()
	{
		if ( value > 0 )
			setValue( --value );
		return *this;
	};

	Codepoint Codepoint::operator--(int)
	{
		Codepoint tmp(*this);
		operator--();
		return tmp;
	};



	////////////////////////////////////////////////////////////////
	////////////        _UString_const_iterator         ////////////
	////////////////////////////////////////////////////////////////

	_UString_const_iterator::_UString_const_iterator(): mycont( NULL ), myptr( NULL ), myindex( 0 )
	{	// construct with null pointer
	};

	_UString_const_iterator::_UString_const_iterator( Pointer _ptr, Indextype _index, Mystring* _string ) : mycont( _string ), myptr( _ptr ), myindex( _index )
	{	// construct with pointer _ptr
	};

	_UString_const_iterator::Indextype _UString_const_iterator::getIndex() const
	{
		return myindex;
	};

	_UString_const_iterator::Reference _UString_const_iterator::operator*() const
	{	// return designated object
		Reference rout = Reference( mycont, myptr );
		return rout;
	};

	_UString_const_iterator::Myt& _UString_const_iterator::operator++()
	{	// preincrement
		do
			++myptr;
		while ( ! UTF8_FIRST_BYTE( *myptr ) );
		++myindex;
		return (*this);
	};

	_UString_const_iterator::Myt _UString_const_iterator::operator++(int)
	{	// postincrement
		Myt tmp = *this;
		++*this;
		return (tmp);
	};

	_UString_const_iterator::Myt& _UString_const_iterator::operator--()
	{	// predecrement
		do
			--myptr;
		while ( ! UTF8_FIRST_BYTE( *myptr ) );
		--myindex;

		return (*this);
	};

	_UString_const_iterator::Myt _UString_const_iterator::operator--(int)
	{	// postdecrement
		Myt tmp = *this;
		--*this;
		return (tmp);
	};

	_UString_const_iterator::Myt& _UString_const_iterator::operator+=( Differencetype _off )
	{	// increment by integer
		if ( _off >= 0 )
		{
			while ( _off-- )
			{
				do
					++myptr;
				while ( ! UTF8_FIRST_BYTE( *myptr ) );
			}
		}
		else
		{
			while ( _off++ )
			{
				do
					--myptr;
				while ( ! UTF8_FIRST_BYTE( *myptr ) );
			}
		}

		myindex += _off;

		return (*this);
	};

	_UString_const_iterator::Myt _UString_const_iterator::operator+( Differencetype _off ) const
	{	// return this + integer
		Myt tmp = *this;
		return ( tmp += _off );
	};

	_UString_const_iterator::Myt& _UString_const_iterator::operator-=( Differencetype _off )
	{	// decrement by integer
		return ( *this += -_off );
	};

	_UString_const_iterator::Myt _UString_const_iterator::operator-( Differencetype _off ) const
	{	// return this - integer
		Myt tmp = *this;
		return ( tmp += -_off );
	};

	_UString_const_iterator::Differencetype _UString_const_iterator::operator-( const Myt& _right ) const
	{	// return difference of iterators
		if ( mycont != _right.mycont )
		{
			USTRING_ERROR( "Different string iterators difference invoked" );
		}

		return ( myindex - _right.myindex);
	};

	_UString_const_iterator::Reference _UString_const_iterator::operator[]( Differencetype _off ) const
	{	// subscript
		return ( *( (*this) + _off ) ) ;
	};

	bool _UString_const_iterator::operator==( const Myt& _right ) const
	{	// test for Iterator equality
		if ( mycont != _right.mycont )
		{
			USTRING_ERROR( "Different string iterators comparison invoked" );
		}

		return (myptr == _right.myptr);
	};

	bool _UString_const_iterator::operator!=( const Myt& _right ) const
	{	// test for Iterator inequality
		return (!(*this == _right));
	};

	bool _UString_const_iterator::operator<( const Myt& _right ) const
	{	// test if this < _right
		if ( mycont != _right.mycont )
		{
			USTRING_ERROR( "Different string iterators comparison invoked" );
		}

		return (myptr < _right.myptr);
	};

	bool _UString_const_iterator::operator>( const Myt& _right ) const
	{	// test if this > _right
		return (_right < *this);
	};

	bool _UString_const_iterator::operator<=( const Myt& _right ) const
	{	// test if this <= _right
		return (!(_right < *this));
	};

	bool _UString_const_iterator::operator>=( const Myt& _right ) const
	{	// test if this >= _right
		return (!(*this < _right));
	};


	void _UString_const_iterator::_Xlen()
	{
		Mystring::_Xlen();
	};

	void _UString_const_iterator::_Xran()
	{
		Mystring::_Xran();
	};

	void _UString_const_iterator::_Xinvarg()
	{
		Mystring::_Xinvarg();
	};



	////////////////////////////////////////////////////////////////
	////////////            _UString_iterator           ////////////
	////////////////////////////////////////////////////////////////

	_UString_iterator::_UString_iterator()
	{	// construct with null string pointer
	};

	_UString_iterator::_UString_iterator( Pointer _ptr, Indextype _index, Mystring* _string ) : Mybase( _ptr, _index, _string )
	{	// construct with pointer _ptr
	};

	_UString_iterator::Reference _UString_iterator::operator*()
	{	// return designated object
		return ((Reference)**(Mybase *)this);

	};

	_UString_iterator::Myt& _UString_iterator::operator++()
	{	// preincrement
		++(*(Mybase *)this);
		return (*this);
	};

	_UString_iterator::Myt _UString_iterator::operator++(int)
	{	// postincrement
		Myt tmp = *this;
		++*this;
		return (tmp);
	};

	_UString_iterator::Myt& _UString_iterator::operator--()
	{	// predecrement
		--(*(Mybase *)this);
		return (*this);
	};

	_UString_iterator::Myt _UString_iterator::operator--(int)
	{	// postdecrement
		Myt tmp = *this;
		--*this;
		return (tmp);
	};

	_UString_iterator::Myt& _UString_iterator::operator+=( Differencetype _off )
	{	// increment by integer
		(*(Mybase *)this) += _off;
		return (*this);
	};

	_UString_iterator::Myt _UString_iterator::operator+( Differencetype _off )
	{	// return this + integer
		Myt tmp = *this;
		return (tmp += _off);
	};

	_UString_iterator::Myt& _UString_iterator::operator-=( Differencetype _off )
	{	// decrement by integer
		return (*this += -_off);
	};

	_UString_iterator::Myt _UString_iterator::operator-( Differencetype _off )
	{	// return this - integer
		Myt tmp = *this;
		return (tmp -= _off);
	};

	_UString_iterator::Differencetype _UString_iterator::operator-( const Mybase& _right ) const
	{	// return difference of iterators
		return ((Mybase)*this - _right);
	};

	_UString_iterator::Reference _UString_iterator::operator[]( Differencetype _off )
	{	// subscript
		return (*(*this + _off));
	};



	////////////////////////////////////////////////////////////////
	////////////    _UString_const_reverse_iterator     ////////////
	////////////////////////////////////////////////////////////////

	_UString_const_reverse_iterator::_UString_const_reverse_iterator() : current()
	{	// construct with null pointer
	};

	_UString_const_reverse_iterator::_UString_const_reverse_iterator( RanIt& _current ) : current( _current )
	{
	};

	_UString_const_reverse_iterator::_UString_const_reverse_iterator( Pointer _ptr, Indextype _index, Mystring* _string ) : current( _ptr, _index, _string )
	{	// construct with pointer _ptr
	};

	_UString_const_reverse_iterator::RanIt _UString_const_reverse_iterator::base() const
	{	// return wrapped Iterator
		return ( current );
	};

	_UString_const_reverse_iterator::Indextype _UString_const_reverse_iterator::getIndex() const
	{
		return ( ( current.getIndex() ) - 1 );
	};

	_UString_const_reverse_iterator::Reference _UString_const_reverse_iterator::operator*()
	{	// return designated object
		RanIt tmp = current;
		return (*--tmp);
	};

	_UString_const_reverse_iterator::Myt& _UString_const_reverse_iterator::operator++()
	{	// preincrement
		--current;
		return (*this);
	};

	_UString_const_reverse_iterator::Myt _UString_const_reverse_iterator::operator++(int)
	{	// postincrement
		Myt tmp = *this;
		--current;
		return (tmp);
	};

	_UString_const_reverse_iterator::Myt& _UString_const_reverse_iterator::operator--()
	{	// predecrement
		++current;
		return (*this);
	};

	_UString_const_reverse_iterator::Myt _UString_const_reverse_iterator::operator--(int)
	{	// postdecrement
		Myt tmp = *this;
		++current;
		return (tmp);
	};

	_UString_const_reverse_iterator::Myt& _UString_const_reverse_iterator::operator+=( Differencetype _off )
	{	// increment by integer
		current -= _off;
		return (*this);
	};

	_UString_const_reverse_iterator::Myt _UString_const_reverse_iterator::operator+( Differencetype _off )
	{	// return this + integer
		return ( Myt(current - _off) );
	};

	_UString_const_reverse_iterator::Myt& _UString_const_reverse_iterator::operator-=( Differencetype _off )
	{	// decrement by integer
		current += _off;
		return (*this);
	};

	_UString_const_reverse_iterator::Myt _UString_const_reverse_iterator::operator-( Differencetype _off )
	{	// return this - integer
		return ( Myt(current + _off) );
	};

	_UString_const_reverse_iterator::Differencetype _UString_const_reverse_iterator::operator-( const Myt& _right ) const
	{	// return difference of iterators
		return ( _right.base() - current );
	};

	_UString_const_reverse_iterator::Reference _UString_const_reverse_iterator::operator[]( Differencetype _off )
	{	// subscript
		return (*(*this + _off));
	};

	bool _UString_const_reverse_iterator::operator==( const Myt& _right ) const
	{	// test for Iterator equality
		return (current == _right.base());
	};

	bool _UString_const_reverse_iterator::operator!=( const Myt& _right ) const
	{	// test for Iterator inequality
		return (current != _right.base());
	};

	bool _UString_const_reverse_iterator::operator<( const Myt& _right ) const
	{	// test if this < _right
		return (current > _right.base());
	};

	bool _UString_const_reverse_iterator::operator>( const Myt& _right ) const
	{	// test if this > _right
		return (current < _right.base());
	};

	bool _UString_const_reverse_iterator::operator<=( const Myt& _right ) const
	{	// test if this <= _right
		return (current >= _right.base());
	};

	bool _UString_const_reverse_iterator::operator>=( const Myt& _right ) const
	{	// test if this >= _right
		return (current <= _right.base());
	};

	void _UString_const_reverse_iterator::_Xlen()
	{
		Mystring::_Xlen();
	};

	void _UString_const_reverse_iterator::_Xran()
	{
		Mystring::_Xran();
	};

	void _UString_const_reverse_iterator::_Xinvarg()
	{
		Mystring::_Xinvarg();
	};




	////////////////////////////////////////////////////////////////
	////////////        _UString_reverse_iterator       ////////////
	////////////////////////////////////////////////////////////////

	_UString_reverse_iterator::_UString_reverse_iterator() : current()
	{	// construct with null pointer
	};

	_UString_reverse_iterator::_UString_reverse_iterator( RanIt& _current ) : current( _current )
	{
	};

	_UString_reverse_iterator::_UString_reverse_iterator( Pointer _ptr, Indextype _index, Mystring* _string ) : current( _ptr, _index, _string )
	{	// construct with pointer _ptr
	};

	_UString_reverse_iterator::RanIt _UString_reverse_iterator::base() const
	{	// return wrapped Iterator
		return ( current );
	};

	_UString_reverse_iterator::Indextype _UString_reverse_iterator::getIndex() const
	{
		return ( ( current.getIndex() ) - 1 );
	};

	_UString_reverse_iterator::Reference _UString_reverse_iterator::operator*()
	{	// return designated object
		RanIt tmp = current;
		return (*--tmp);
	};

	_UString_reverse_iterator::Myt& _UString_reverse_iterator::operator++()
	{	// preincrement
		--current;
		return (*this);
	};

	_UString_reverse_iterator::Myt _UString_reverse_iterator::operator++(int)
	{	// postincrement
		Myt tmp = *this;
		--current;
		return (tmp);
	};

	_UString_reverse_iterator::Myt& _UString_reverse_iterator::operator--()
	{	// predecrement
		++current;
		return (*this);
	};

	_UString_reverse_iterator::Myt _UString_reverse_iterator::operator--(int)
	{	// postdecrement
		Myt tmp = *this;
		++current;
		return (tmp);
	};

	_UString_reverse_iterator::Myt& _UString_reverse_iterator::operator+=( Differencetype _off )
	{	// increment by integer
		current -= _off;
		return (*this);
	};

	_UString_reverse_iterator::Myt _UString_reverse_iterator::operator+( Differencetype _off )
	{	// return this + integer
		return ( Myt(current - _off) );
	};

	_UString_reverse_iterator::Myt& _UString_reverse_iterator::operator-=( Differencetype _off )
	{	// decrement by integer
		current += _off;
		return (*this);
	};

	_UString_reverse_iterator::Myt _UString_reverse_iterator::operator-( Differencetype _off )
	{	// return this - integer
		return ( Myt(current + _off) );
	};

	_UString_reverse_iterator::Differencetype _UString_reverse_iterator::operator-( const Myt& _right ) const
	{	// return difference of iterators
		return ( _right.base() - current );
	};

	_UString_reverse_iterator::Reference _UString_reverse_iterator::operator[]( Differencetype _off )
	{	// subscript
		return (*(*this + _off));
	};

	bool _UString_reverse_iterator::operator==( const Myt& _right ) const
	{	// test for Iterator equality
		return (current == _right.base());
	};

	bool _UString_reverse_iterator::operator!=( const Myt& _right ) const
	{	// test for Iterator inequality
		return (current != _right.base());
	};

	bool _UString_reverse_iterator::operator<( const Myt& _right ) const
	{	// test if this < _right
		return (current > _right.base());
	};

	bool _UString_reverse_iterator::operator>( const Myt& _right ) const
	{	// test if this > _right
		return (current < _right.base());
	};

	bool _UString_reverse_iterator::operator<=( const Myt& _right ) const
	{	// test if this <= _right
		return (current >= _right.base());
	};

	bool _UString_reverse_iterator::operator>=( const Myt& _right ) const
	{	// test if this >= _right
		return (current <= _right.base());
	};

	void _UString_reverse_iterator::_Xlen()
	{
		Mystring::_Xlen();
	};

	void _UString_reverse_iterator::_Xran()
	{
		Mystring::_Xran();
	};

	void _UString_reverse_iterator::_Xinvarg()
	{
		Mystring::_Xinvarg();
	};




	////////////////////////////////////////////////////////////////
	////////////                UString                 ////////////
	////////////////////////////////////////////////////////////////

	UString::Elem* UString::_myptr()
	{	// determine current pointer to buffer for mutable string
		return ( myres >= BUF_SIZE ? bx.ptr : &( bx.buf[0] ) );
	};

	const UString::Elem* UString::_myptr() const
	{	// determine current pointer to buffer for nonmutable string
		return ( myres >= BUF_SIZE ? bx.ptr : &( bx.buf[0] ) );
	};


	void UString::_eos( Sizetype _newsize, Sizetype _newlength )
	{	// set new length and null terminator
		_myptr()[ mysize = _newsize ] = 0;
		mysize = _newsize;
		mylength = _newlength;
	};

	void UString::_tidy( bool _built,  Sizetype _newsize, Sizetype _newlength )
	{	// initialize buffer, deallocating any storage
		if ( !_built )
			;
		else if ( myres >= BUF_SIZE )
		{	// copy any leftovers to small buffer and deallocate
			Elem* ptr = bx.ptr;
			if ( 0 < _newlength )
			{
				std::copy( ptr, ptr + _newsize, bx.buf );
			}

			bx.buf[ mysize = _newsize ] = '\0';
			mylength = _newlength;

			delete [] ptr;
		}

		myres = BUF_SIZE - 1;
		_eos( _newsize, _newlength );
	};

	void UString::_copy( Sizetype _newsize, Sizetype _oldsize )
	{	// copy _oldsize elements to newly allocated buffer
		Sizetype newres = _newsize | ALLOC_MASK;

		if ( maxSize() < newres )
			newres = _newsize; // undo roundup if too big
		else if ( ( newres / 3 ) < ( myres / 2 ) && myres <= ( maxSize() - myres / 2 ) )
			newres = myres + myres / 2; // grow exponentially if possible

		Elem* ptr = 0;

		try
		{
			ptr = new Elem [ newres + 1 ];
		}
		catch (...)
		{
			newres = _newsize; // allocation failed, undo roundup and retry

			try
			{
				ptr = new Elem [ newres + 1 ];
			}
			catch (...)
			{
				_tidy( true ); // failed again, discard storage and reraise
				throw;
			}
		}

		if ( 0 < _oldsize )
		{
			std::copy( _myptr(), _myptr() + _oldsize, ptr ); // copy existing elements
		}

		Sizetype oldLength = mylength;

		_tidy( true );
		bx.ptr = ptr;
		myres = newres;
		_eos( _oldsize, oldLength );
	};

	void UString::_copyChangeMidstr( Sizetype _midIndex, Sizetype _midOldSize, Sizetype _midNewSize )
	{
		if ( _midOldSize == _midNewSize )
			return;

		Sizetype newSize = mysize + _midNewSize - _midOldSize;
		Sizetype oldMysize = mysize;
		Sizetype oldMylength = mylength;

		if ( newSize > myres )
		{
			newSize = newSize | ALLOC_MASK;

			if ( maxSize() < newSize )
				_Xlen(); // new string too big
			else if ( ( newSize / 3 ) < ( myres / 2 ) && myres <= ( maxSize() - myres / 2 ) )
				newSize = myres + myres / 2; // grow exponentially if possible

			Elem* ptr = 0;

			try
			{
				ptr = new Elem [ newSize + 1 ];
			}
			catch (...)
			{
				_tidy( true ); // failed, discard storage and reraise
				_Xlen();
			}

			std::copy( _myptr(), _myptr() + _midIndex, ptr );
			std::fill( ptr + _midIndex, ptr + _midIndex + _midNewSize, (Elem) 0 );
			std::copy( _myptr() + _midIndex + _midOldSize, _myptr() + mysize, ptr + _midIndex + _midNewSize );

			_tidy( true );
			bx.ptr = ptr;
			myres = newSize;
		}
		else
		{
			Elem* p1 = _myptr() + _midIndex + _midOldSize;
			Elem* p2 = _myptr() + mysize;
			Elem* p3 = _myptr() + _midIndex + _midNewSize;


			if ( _midOldSize > _midNewSize )
				std::copy( _myptr() + _midIndex + _midOldSize, _myptr() + mysize, _myptr() + _midIndex + _midNewSize );
			else // ( _midOldSize < _midNewSize )
				std::copy_backward( _myptr() + _midIndex + _midOldSize, _myptr() + mysize, _myptr() + mysize + _midNewSize - _midOldSize );

			std::fill( _myptr() + _midIndex, _myptr() + _midIndex + _midNewSize, (Elem) 0 );
		}

		_eos( oldMysize + ( _midNewSize - _midOldSize ), oldMylength );
};

	void UString::_chassign( Sizetype _indexOff, Sizetype _lcount, Reference _chr )
	{	// assign _lcount copies of _ch beginning at _soff
		Valuetype chr[ UNICODE_MAX_UTF8_LENGTH ];
		UTFSize chrLen = _chr.copy( chr );
		if ( _indexOff > mylength )
			_indexOff = mylength;

		Sizetype oldSize = ( begin() + _indexOff ).getPtr() - begin().getPtr();
		Sizetype newSize = oldSize + _lcount * chrLen;
		Sizetype lcount = _lcount;

		if ( newSize > myres )
			_copy( newSize, oldSize );

		if ( chrLen == 1 )
		{
			std::fill( _myptr() + oldSize, _myptr() + newSize, (CHAR8) _chr.getVal() );
		}
		else
		{
			Elem* ptr = _myptr() + oldSize;
			while ( _lcount-- )
			{
				std::copy( chr, chr + chrLen, ptr );
				ptr += chrLen;
			}
		}

		_eos( newSize, _indexOff + lcount );
	};

	bool UString::_inside( const Elem* _ptr )
	{	// test if _ptr points inside string
		if ( _ptr == 0 || _ptr < _myptr() || _myptr() + mysize <= _ptr )
			return (false);	// don't ask
		else
			return (true);
	};

	UString::Sizetype UString::_pdif( ConstIterator _P1, ConstIterator _P2 )
	{	// compute safe Iterator difference
		return ( _P2.getPtr() - _P1.getPtr() );
	};

	bool UString::_grow( Sizetype _newsize, bool _trim )
	{	// ensure buffer is big enough, trim to size if trim is true
		if ( maxSize() < _newsize )
			_Xlen(); // result too long

		if ( _newsize > myres )
			_copy(_newsize, mysize); // reallocate to grow
		else if ( _trim && _newsize < BUF_SIZE )
		{	//find last qualified char fully encased in remaining string
			if ( _newsize >= mysize )
				_tidy( true, mysize );
			else
			{
				while ( !UTF8_FIRST_BYTE( _newsize ) )
					--_newsize;
				_tidy( true, _newsize );
			}
		}
		else if ( _newsize == 0 )
			_eos( 0, 0 ); // new size is zero, just null terminate

		return ( _newsize > 0 ); // return true only if more work to do
	};

	bool UString::_validate_UTF8( const Elem* _ptr )
	{
		UTFSize tmpSize = 0;

		while ( *_ptr )
		{
			if ( ( tmpSize = UTF8_CHR_SIZE( *_ptr ) ) <= 0 )
				return false;

			++_ptr;
			while ( --tmpSize )
			{
				if ( UTF8_FIRST_BYTE( *_ptr ) )
					return false;
				++_ptr;
			}
		}

		return true;
	};

	bool UString::_validateL_UTF8( const Elem* _ptr, Sizetype _length )
	{
		UTFSize tmpSize = 0;

		while ( _length )
		{
			if ( ( tmpSize = UTF8_CHR_SIZE( *_ptr ) ) <= 0 || !_length )
				return false;

			++_ptr;
			while ( --tmpSize )
			{
				if ( UTF8_FIRST_BYTE( *_ptr ) )
					return false;
				++_ptr;
			}

			--_length;
		}

		if ( _length )
			return false;

		return true;
	};

	bool UString::_validateS_UTF8( const Elem* _ptr, Sizetype _size )
	{
		UTFSize tmpSize = 0;

		while ( _size )
		{
			if ( ( tmpSize = UTF8_CHR_SIZE( *_ptr ) ) <= 0 || tmpSize > _size )
				return false;

			_size -= tmpSize;

			++_ptr;
			while ( --tmpSize )
			{
				if ( UTF8_FIRST_BYTE( *_ptr ) )
					return false;
				++_ptr;
			}
		}

		if ( _size )
			return false;

		return true;
	};

	bool UString::_validateLS_UTF8( const Elem* _ptr, Sizetype _length, Sizetype _size )
	{
		UTFSize tmpSize = 0;

		while ( _length && _size && *_ptr )
		{
			if ( ( tmpSize = UTF8_CHR_SIZE( *_ptr ) ) <= 0 || !_length || tmpSize > _size )
				return false;

			_size -= tmpSize;

			++_ptr;
			while ( --tmpSize )
			{
				if ( UTF8_FIRST_BYTE( *_ptr ) )
					return false;
				++_ptr;
			}

			--_length;
		}

		if ( _size )
			return false;

		return true;
	};

	bool UString::_validateGetL_UTF8( const Elem* _ptr, Sizetype* _length )
	{
		UTFSize tmpSize = 0;
		Sizetype length = 0;

		while ( *_ptr )
		{
			if ( ( tmpSize = UTF8_CHR_SIZE( *_ptr ) ) <= 0 )
				return false;

			++_ptr;
			while ( --tmpSize )
			{
				if ( UTF8_FIRST_BYTE( *_ptr ) )
					return false;
				++_ptr;
			}

			++length;
		}

		if ( _length )
			*_length = length;

		return true;
	};

	bool UString::_validateGetS_UTF8( const Elem* _ptr, Sizetype* _size )
	{
		UTFSize tmpSize = 0;
		Sizetype size = 0;

		while ( *_ptr )
		{
			if ( ( tmpSize = UTF8_CHR_SIZE( *_ptr ) ) <= 0 )
				return false;

			size += tmpSize;

			++_ptr;
			while ( --tmpSize )
			{
				if ( UTF8_FIRST_BYTE( *_ptr ) )
					return false;
				++_ptr;
			}
		}

		if ( _size )
			*_size = size;

		return true;
	};

	bool UString::_validateGetLS_UTF8( const Elem* _ptr, Sizetype* _length, Sizetype* _size )
	{
		UTFSize tmpSize = 0;
		Sizetype length = 0;
		Sizetype size = 0;

		while ( *_ptr )
		{
			if ( ( tmpSize = UTF8_CHR_SIZE( *_ptr ) ) <= 0 )
				return false;

			size += tmpSize;

			++_ptr;
			while ( --tmpSize )
			{
				if ( UTF8_FIRST_BYTE( *_ptr ) )
					return false;
				++_ptr;
			}

			++length;
		}

		if ( _length )
			*_length = length;
		if ( _size )
			*_size = size;

		return true;
	};

	bool UString::_validateLGetS_UTF8( const Elem* _ptr, Sizetype _length, Sizetype* _size )
	{
		UTFSize tmpSize = 0;
		const Elem* ptr = _ptr;

		while ( _length )
		{
			if ( ( tmpSize = UTF8_CHR_SIZE( *ptr ) ) <= 0 || !_length )
				return false;

			++ptr;
			while ( --tmpSize )
			{
				if ( UTF8_FIRST_BYTE( *ptr ) )
					return false;
				++ptr;
			}

			--_length;
		}

		if ( _length )
			return false;

		if ( _size )
			*_size = (Sizetype) ( ptr - _ptr );

		return true;
	};

	bool UString::_validateSGetL_UTF8( const Elem* _ptr, Sizetype _size, Sizetype* _length )
	{
		UTFSize tmpSize = 0;
		Sizetype length = 0;

		while ( _size )
		{
			if ( ( tmpSize = UTF8_CHR_SIZE( *_ptr ) ) <= 0 || tmpSize > _size )
				return false;

			_size -= tmpSize;

			++_ptr;
			while ( --tmpSize )
			{
				if ( UTF8_FIRST_BYTE( *_ptr ) )
					return false;
				++_ptr;
			}

			++length;
		}

		if ( _size )
			return false;

		if ( _length )
			*_length = length;

		return true;
	};

	void UString::_getL_UTF8( const Elem* _ptr, Sizetype* _length, Sizetype* _size )
	{
		Sizetype length = 0;

		if ( _size )
		{
			Sizetype size = *_size;
			while ( size-- )
			{
				if ( UTF8_FIRST_BYTE( *_ptr ) )
					++length;
				++_ptr;
			}
		}
		else
		{
			while ( *_ptr )
			{
				if ( UTF8_FIRST_BYTE( *_ptr ) )
					++length;
				++_ptr;
			}
		}

		if ( _length )
			*_length = length;
	};

	void UString::_getS_UTF8( const Elem* _ptr, Sizetype* _size, Sizetype* _length )
	{
		UTFSize tmpSize = 0;
		const Elem* ptr = _ptr;

		if ( _length )
		{
			Sizetype length = *_length;
			while ( length )
			{
				if ( UTF8_FIRST_BYTE( *ptr ) )
					--length;
				++ptr;
			}
		}
		else
		{
			while ( *ptr )
			{
				++ptr;
			}
		}

		if ( _size )
			*_size = (Sizetype) ( ptr - _ptr );
	};

	void UString::_getLS_UTF8( const Elem* _ptr, Sizetype* _length, Sizetype* _size )
	{
		const Elem* ptr = _ptr;

		if ( _length )
		{
			Sizetype length = 0;

			while ( *ptr )
			{
				if ( UTF8_FIRST_BYTE( *ptr ) )
					++length;
				++ptr;
			}

			*_length = length;
			if ( _size )
				*_size = (Sizetype) ( ptr - _ptr );
		}
		else if ( _size )
		{
			while ( *ptr )
				++ptr;

			*_size = (Sizetype) ( ptr - _ptr );
		}
	};

	UString::Sizetype UString::length( const Elem* _ptr )
	{
		Sizetype length;
		_getL_UTF8( _ptr, &length );
		return length;
	};

	UString::Sizetype UString::size( const Elem* _ptr )
	{
		Sizetype size;
		_getS_UTF8( _ptr, &size );
		return size;
	};


	void UString::_Xlen()
	{
		std::_Xlength_error("string too long");
	};

	void UString::_Xran()
	{
		std::_Xout_of_range("invalid string position");
	};

	void UString::_Xinvarg()
	{
		std::_Xinvalid_argument("invalid argument");
	};


	UString::UString() : Mybase(), mylength(0), mysize(0), myres(0)
	{	// construct empty string
		_tidy();
	};

	UString::UString( const Myt& _right, Sizetype _roff, Sizetype _count ) : Mybase(), mylength(0), mysize(0), myres(0)
	{	// construct from _right [_roff, _roff + _count)
		_tidy();
		assign( _right, _roff, _count );
	};

	UString::UString( const std::string& _str_str )
	{
		_tidy();
		assign( _str_str.c_str(), _str_str.length() );
	};

	UString::UString( const Elem* _ptr, Sizetype _count ) : Mybase(), mylength(0), mysize(0), myres(0)
	{	// construct from [_ptr, _ptr + _count)
		_tidy();
		assign( _ptr, ( _count != npos ) ? _count : length( _ptr ) );
	};

	UString::UString( Reference _chr, Sizetype _count ) : Mybase(), mylength(0), mysize(0), myres(0)
	{	// construct from _count * _Ch
		_tidy();
		assign( _chr, _count );
	};

	UString::UString( ConstPointer _first, ConstPointer _last ) : Mybase(), mylength(0), mysize(0), myres(0)
	{	// construct from [_first, _last), const pointers
		_tidy();
		assign( _first, _last );
	};

	UString::UString( ConstIterator _first, ConstIterator _last ) : Mybase(), mylength(0), mysize(0), myres(0)
	{	// construct from [_first, _last), const_iterators
		_tidy();
		assign( _first.getPtr(), _last.getPtr() );
	};

	UString::UString( const Myt& _right ) : Mybase( _right ), mylength(0), mysize(0), myres(0)
	{	// construct by copying _right
		_tidy();
		assign( _right );
	};

	UString::~UString()
	{	// destroy the string
		_tidy(true);
	};

	UString::Myt& UString::operator=( const Myt& _right )
	{	// assign _right
		return ( assign( _right ) );
	};

	UString::Myt& UString::operator=( const Elem* _ptr )
	{	// assign [_Ptr, <null>)
		return ( assign( _ptr ) );
	};

	UString::Myt& UString::operator=( Reference _chr )
	{	// assign 1 * _Ch
		return ( assign( _chr, 1 ) );
	};

	UString::Myt& UString::operator+=( const Myt& _right )
	{	// append _right
		return ( append( _right, 0, npos ) );
	};

	UString::Myt& UString::operator+=( const Elem* _ptr )
	{	// append [_Ptr, <null>)
		return ( append( _ptr ) );
	};

	UString::Myt& UString::operator+=( Reference _chr )
	{	// append 1 * _Ch
		return ( append( _chr, (Sizetype) 1 ) );
	};

	UString::Myt& UString::append( const Myt& _right )
	{	// append _right
		return ( append( _right, 0, npos ) );
	};

	UString::Myt& UString::append( const Myt& _right, Sizetype _roff, Sizetype _count )
	{	// append _right [_roff, _roff + _count)
		if ( _roff >= _right.length() )
			_Xran();

		if ( !_count ) return (*this);

		ConstIterator it = _right.begin() + _roff;
		const Elem* beginPtr = it.getPtr();
		const Elem* endPtr = NULL;

		if ( _roff + _count >= _right.length() )
		{
			_count = _right.length() - _roff;
			endPtr = _right.end().getPtr();
		}
		else
			endPtr = ( it + _count ).getPtr();

		Sizetype size = endPtr - beginPtr;
		if ( !size ) return (*this);

		if ( npos - mysize <= size || ( mysize + size ) < mysize )
			_Xlen();	// result too long

		if ( _grow( mysize + size ) )
		{
			std::copy( beginPtr, endPtr, _myptr() + mysize );
			_eos( mysize + size, mylength + _count );
		}

		return (*this);
	};

	UString::Myt& UString::append( const Elem* _ptr, Sizetype _count )
	{	// append [_ptr, _ptr + _count)
		if ( !_count ) return (*this);

		Sizetype size = 0;
		if ( !_validateLGetS_UTF8( _ptr, _count, &size ) )
			USTRING_ERROR( "Specified string is not a valid UTF-8 encoded string" );

		if ( ( npos - mysize ) <= size || ( mysize + size ) < mysize )
			_Xlen();	// result too long

		if ( _grow( mysize + size ) )
		{	// make room and append new stuff
			std::copy( _ptr, _ptr + size, _myptr() + mysize );
			_eos( mysize + size, mylength + _count);
		}

		return (*this);
	};

	UString::Myt& UString::append( const Elem* _ptr )
	{	// append [_Ptr, <null>)
		Sizetype length = 0;

		if ( !_validateGetL_UTF8( _ptr, &length ) )
			USTRING_ERROR( "Specified string is not a valid UTF-8 encoded string" );

		return ( append( _ptr, length ) );
	};

	UString::Myt& UString::append( Reference _chr, Sizetype _count )
	{	// append _count * _chr
		if ( _count >= npos - mylength )
			_Xlen();	// result too long

		Sizetype num = mysize + ( _count * _chr.size() );
		if ( _count && _grow( num ))
			_chassign( mysize, _count, _chr );

		return (*this);
	};

	UString::Myt& UString::append( ConstPointer _first, ConstPointer _last )
	{	// append [_First, _Last), const pointers
		return ( replace( end(), end(), _first, _last ) );
	};

	UString::Myt& UString::append( ConstIterator _first, ConstIterator _last )
	{	// append [_First, _Last), const_iterators
		return ( replace( end(), end(), _first, _last ) );
	};

	UString::Myt& UString::assign( const Myt& _right )
	{	// assign _right
		return ( assign( _right, 0, npos ) );
	};

	UString::Myt& UString::assign( const Myt& _right, Sizetype _roff, Sizetype _count )
	{	// assign _right [_roff, _roff + _count)
		if ( _roff > _right.length() )
			_Xran();	// _roff off end
		Sizetype num = _right.length() - _roff;

		if ( num > _count )
			num = _count;	// trim num to size

		if (this == &_right)
		{
			erase((Sizetype)(_roff + num));
			erase(0, _roff);
		}
		else if ( _grow( num ) )
		{	// make room and assign new stuff
			ConstIterator it = _right.begin() + _roff;
			const Elem* beginPtr = it.getPtr();
			const Elem* endPtr = ( it + num ).getPtr();
			std::copy( beginPtr, endPtr, _myptr() );
			_eos( endPtr - beginPtr, num );
		}

		return (*this);
	};

	UString::Myt& UString::assign( const Elem* _ptr, Sizetype _count )
	{	// assign [_ptr, _ptr + _count)
		if ( _inside( _ptr ) )
			return ( assign( *this, _ptr - _myptr(), _count ) );

		Sizetype size;
		if ( _validateLGetS_UTF8( _ptr, _count, &size ) && _grow( size ) )
		{
			std::copy( _ptr, _ptr + size, _myptr() );
			_eos( size, _count );
		}

		return (*this);
	};

	UString::Myt& UString::assign( const Elem* _ptr )
	{	// assign [_Ptr, <null>)
		return ( assign( _ptr, length( _ptr ) ) );
	};

	UString::Myt& UString::assign( Reference _chr, Sizetype _count )
	{	// assign _count * _Ch
		if ( _count == npos )
			_Xlen();

		if ( _grow( _count * _chr.size() ) )
		{
			_chassign( 0, _count, _chr );
			_eos( _count * _chr.size(), _count );
		}

		return (*this);
	};

	UString::Myt& UString::assign( Elem _chr, Sizetype _count )
	{
		return assign( Reference( _chr ), _count );
	};

	UString::Myt& UString::assign( ConstPointer _first, ConstPointer _last )
	{	// assign [First, _Last), const pointers
		return ( replace( begin(), end(), _first, _last ) );
	};

	UString::Myt& UString::assign( ConstIterator _first, ConstIterator _last )
	{	// assign [First, _Last), const_iterators
		return ( replace( begin(), end(), _first, _last ) );
	};

	UString::Myt& UString::insert( Sizetype _off, const Myt& _right)
	{	// insert _right at _off
		return ( insert( _off, _right, 0, npos ) );
	};

	UString::Myt& UString::insert( Sizetype _off, const Myt& _right, Sizetype _roff, Sizetype _count )
	{	// insert _right [_roff, _roff + _count) at _off
		if ( _off > mylength || _roff > _right.length() )
			_Xran();

		Sizetype num = _right.length() - _roff;
		if ( _count > num )
			_count = num;

		if ( _count >= npos - mylength )
			_Xlen();

		if ( _count )
		{	// make room and insert new stuff
			ConstIterator it = _right.begin() + _roff;
			Sizetype rsbegin = it.getPtr() - _right.begin().getPtr();
			Sizetype rssize = ( it + _count ).getPtr() - _right.begin().getPtr() - rsbegin;
			Sizetype sbegin = ( begin() + _off ).getPtr() - begin().getPtr();

			_copyChangeMidstr( sbegin, 0, rssize );

			if ( this == &_right && rsbegin + rssize >= sbegin )
			{
				if ( rsbegin < sbegin )
				{
					std::copy( _myptr() + rsbegin, _myptr() + sbegin, _myptr() + sbegin );
					std::copy( _myptr() + sbegin + rssize, _myptr() + sbegin + rssize + sbegin - rsbegin, _myptr() + sbegin + sbegin - rsbegin );
				}
				else
					std::copy( _myptr() + rsbegin + rssize, _myptr() + rsbegin + rssize + rssize, _myptr() + sbegin );
			}
			else
				std::copy( _right._myptr() + rsbegin, _right._myptr() + rsbegin + rssize, _myptr() + sbegin );

			_eos( mysize, mylength + _count );
		}

		return (*this);
	};

	UString::Myt& UString::insert( Sizetype _off, const Elem* _ptr, Sizetype _count )
	{	// insert [_ptr, _ptr + _count) at _off

		if ( _inside( _ptr ) )
			return ( insert( _off, *this, _ptr - _myptr(), _count ) );

		if ( _off > mylength )
			_Xran();
		if ( _count >= npos - mylength )
			_Xlen();	// result too long

		if ( _count )
		{
			Sizetype soff = ( begin() + _off ).getPtr() - begin().getPtr();
			const Elem* ptrEnd = _ptr;
			Sizetype c = _count;
			while ( c )
			{
				++ptrEnd;
				if ( UTF8_FIRST_BYTE( *ptrEnd ) )
					--c;
			}

			_copyChangeMidstr( soff, 0, ptrEnd - _ptr );
			std::copy( _ptr, ptrEnd, _myptr() + soff );

			_eos( mysize, mylength + _count );
		}

		return (*this);
	};

	UString::Myt& UString::insert( Sizetype _off, const Elem* _ptr)
	{	// insert [_Ptr, <null>) at _off
		return ( insert( _off, _ptr, length( _ptr ) ) );
	};

	UString::Myt& UString::insert( Sizetype _off, Sizetype _count, Reference _chr )
	{	// insert _count * _chr at _off
		if ( _off > mylength )
			_Xran();	// _off off end

		if ( _count >= npos - mylength )
			_Xlen();	// result too long

		if ( _count )
		{	// make room and insert new stuff
			Sizetype soff = ( begin() + _off ).getPtr() - begin().getPtr();
			UTFSize chrLen = _chr.size();
			_copyChangeMidstr( soff, 0, chrLen * _count );
			if ( chrLen == 1 )
			{
				std::fill( _myptr() + soff, _myptr() + soff + chrLen * _count, (CHAR8) _chr.getVal() );
			}
			else
			{
				Elem* ptr = _myptr() + soff;
				while ( _count-- )
				{
					ptr += _chr.copy( ptr );
				}
			}

			_eos( mysize, mylength + _count );
		}

		return (*this);
	};

	UString::Iterator UString::insert( ConstIterator _where )
	{	// insert <null> at _Where
		return ( insert( _where, Reference() ) );
	};

	UString::Iterator UString::insert( ConstIterator _where, Reference _chr )
	{	// insert _chr at _where
		Sizetype _off = _where.getIndex();
		insert( _off, 1, _chr );
		return ( begin() + _off );
	};

	void UString::insert( ConstIterator _where, Reference _chr, Sizetype _count )
	{	// insert _count * Elem at _Where
		Sizetype _off = _where.getIndex();
		insert(_off, _count, _chr );
	};

	void UString::insert( ConstIterator _where, ConstPointer _first, ConstPointer _last )
	{	// insert [_First, _Last) at _Where, const pointers
		replace( _where, _where, _first, _last);
	};

	void UString::insert( ConstIterator _where, ConstIterator _first, ConstIterator _last)
	{	// insert [_First, _Last) at _Where, const_iterators
		replace( _where, _where, _first, _last);
	};

	UString::Myt& UString::erase ( Sizetype _off, Sizetype _count )
	{	// erase elements [_off, _off + _count)
		if ( _off > mylength )
			_Xran();	// _off off end

		if ( _count > mylength - _off )
			_count = mylength - _off;	// trim _count

		if ( _count )
		{	// move elements down
			Sizetype soff = ( begin() + _off ).getPtr() - begin().getPtr();
			Sizetype scount = ( begin() + ( _off + _count ) ).getPtr() - ( begin() + _off ).getPtr();
			_copyChangeMidstr( soff, scount, 0 );

			_eos( mysize, mylength - _count);
		}

		return (*this);
	};

	UString::Iterator UString::erase( ConstIterator _where )
	{	// erase element at _Where
		Sizetype count = _where.getIndex();
		erase( count, 1 );
		return ( _USTRING_ITERATOR( _myptr(), count ) );
	};

	UString::Iterator UString::erase( ConstIterator _first, ConstIterator _last )
	{	// erase substring [_First, _Last)
		Sizetype count = _first.getIndex();
		erase( count, _last.getIndex() - count );
		return ( _USTRING_ITERATOR( _myptr(), count ) );
	};

	void UString::clear()
	{	// erase all
		erase();
	};

	UString::Myt& UString::replace( Sizetype _off, Sizetype _n0, const Myt& _right )
	{	// replace [_off, _off + _n0) with _right
		return ( replace( _off, _n0, _right, 0, npos ) );
	};

	UString::Myt& UString::replace( Sizetype _off, Sizetype _n0, const Myt& _right, Sizetype _roff, Sizetype _count )
	{	// replace [_off, _off + _n0) with _right [_roff, _roff + _count)
		if ( !_n0 )
			return insert( _off, _right, _roff, _count );

		if ( _off > mylength || _roff > _right.size() )
			_Xran();	// _off or _roff off end

		if ( _n0 > mylength - _off )
			_n0 = mylength - _off;	// trim _n0 to size

		Sizetype num = _right.length() - _roff;
		if ( _count > num )
			_count = num;	// trim _count to size

		if (npos - _count <= mylength - _n0)
			_Xlen();	// result too long

		const Elem* ptrOff = ( begin() + _off ).getPtr();
		const Elem* ptrOffN0 = ( begin() + _off + _n0 ).getPtr();
		const Elem* ptrROff = ( _right.begin() + _roff ).getPtr();
		const Elem* ptrROffN0 = ( _right.begin() + _roff + _count ).getPtr();

		Sizetype sOff = ptrOff - _myptr();
		Sizetype sCount = ptrOffN0 - ptrOff;
		Sizetype srOff = ptrROff - _right._myptr();
		Sizetype srCount = ptrROffN0 - ptrROff;
		Sizetype oldMysize = mysize;


		if ( this != &_right )
		{// no overlap
			_copyChangeMidstr( sOff, sCount, srCount );
			std::copy( ptrROff, ptrROffN0, _myptr() + sOff );
		}
		else if ( srOff + srCount < sOff )
		{// left side of the gap
			_copyChangeMidstr( sOff, sCount, srCount );
			std::copy( _myptr() + srOff, _myptr() + srOff + srCount, _myptr() + sOff );
		}
		else if ( srOff > sOff + sCount )
		{// right side of the gap
			_copyChangeMidstr( sOff, sCount, srCount );
			std::copy( _myptr() + srOff - sCount + srCount, _myptr() + srOff + srCount - sCount + srCount, _myptr() + sOff );
		}
		else if ( srOff >= sOff && srOff + srCount <= sOff + sCount )
		{// totally inside the gap
			if ( srOff != sOff )
			{
				std::copy( _myptr() + srOff, _myptr() + srOff + srCount, _myptr() + sOff );
				_copyChangeMidstr( sOff + srCount, sCount - srCount, 0 );
			}
			else if ( sCount != srCount )
				_copyChangeMidstr( sOff + srCount, sCount - srCount, 0 );
		}
		else if ( srOff >= sOff )
		{// right side, partially in gap
			_copyChangeMidstr( sOff, 0, srCount - sCount );
			std::copy( _myptr() + srOff + srCount - sCount, _myptr() + srOff + srCount - sCount + srCount, _myptr() + sOff );
		}
		else if ( srOff + srCount <= sOff + sCount )
		{// left side, partially in gap
			_copyChangeMidstr( sOff + sCount, 0, srCount - sCount );
			std::copy_backward( _myptr() + srOff, _myptr() + srOff + srCount, _myptr() + sOff );
		}
		else
		{//covers gap and overflow on left and right side
			_copyChangeMidstr( sOff + sCount, 0, srCount - sCount );
			std::copy( _myptr() + srOff, _myptr() + sOff + sCount, _myptr() + sOff );
			std::copy( _myptr() + sOff + srCount, _myptr() + srCount - sCount + srCount + srOff, _myptr() + sOff - srOff + sOff + sCount );
		}

		_eos( mysize, mylength - _n0 + _count );
		return (*this);
	};

	UString::Myt& UString::replace( Sizetype _off, Sizetype _n0, const Elem* _ptr, Sizetype _count )
	{	// replace [_off, _off + _n0) with [_Ptr, _Ptr + _count)
		if ( _inside( _ptr ) )
			return ( replace( _off, _n0, *this, _ptr - _myptr(), _count ) );

		if ( _off > mylength )
			_Xran();	// _off off end
 
		if ( _n0 > mylength - _off )
			_n0 = mylength - _off;	// trim _n0 to size

		if ( mylength - _n0 >= npos - _count )
			_Xlen();	// result too long

		const Elem* ptrBegin = ( begin() + _off ).getPtr();
		const Elem* ptrEnd = ( begin() + _off + _n0 ).getPtr();

		Sizetype start = ptrBegin - _myptr();
		Sizetype oldSize = ptrEnd - ptrBegin;
		Sizetype newSize;
		if ( !_validateLGetS_UTF8( _ptr, _count, &newSize ) )
			_Xinvarg();		//pointed string incorrect

		Sizetype oldMysize = mysize;

		_copyChangeMidstr( ptrBegin - _myptr(), oldSize, newSize );
		std::copy( _ptr, _ptr + newSize, _myptr() + start );

		_eos( mysize, mylength - _n0 + _count );
		return (*this);
	};

	UString::Myt& UString::replace( Sizetype _off, Sizetype _n0, const Elem* _ptr )
	{	// replace [_off, _off + _n0) with [_Ptr, <null>)
		return ( replace( _off, _n0, _ptr, length( _ptr ) ) );
	};

	UString::Myt& UString::replace( Sizetype _off, Sizetype _n0, Sizetype _count, Reference _chr )
	{	// replace [_off, _off + _n0) with _count * _Ch
		if ( _off > mylength )
			_Xran();	// _off off end

		if ( _n0 > mylength - _off )
			_n0 = mylength - _off;	// trim _n0 to size

		if ( mylength - _n0 >= npos - _count )
			_Xlen();	// result too long

		if ( _count )
		{
			Sizetype ptrBegin = ( begin() + _off ).getPtr() - _myptr();
			Sizetype ptrEnd = ( begin() + _off + _n0 ).getPtr() - _myptr();

			UTFSize chrLen = _chr.size();
			_copyChangeMidstr( ptrBegin, ptrEnd - ptrBegin, _count * chrLen );

			if ( chrLen == 1 )
			{
				std::fill( _myptr() + ptrBegin, _myptr() + ptrBegin + _count * chrLen, (CHAR8) _chr.getVal() );
			}
			else
			{
				Elem* ptr = _myptr() + ptrBegin;
				while ( _count-- )
				{
					ptr += _chr.copy( ptr );
				}
			}

			_eos( mysize, mylength - _n0 + _count );
		}

		return (*this);
	};

	UString::Myt& UString::replace( ConstIterator _first, ConstIterator _last, const Myt& _right )
	{	// replace [_First, _Last) with _right
		return ( replace( _pdif( begin(), _first ), _pdif( _first, _last ), _right ) );
	};

	UString::Myt& UString::replace( ConstIterator _first, ConstIterator _last, const Elem* _ptr, Sizetype _count )
	{	// replace [_First, _Last) with [_Ptr, _Ptr + _count)
		return ( replace( _pdif( begin(), _first ), _pdif( _first, _last ), _ptr, _count ) );
	};

	UString::Myt& UString::replace( ConstIterator _first, ConstIterator _last, const Elem* _ptr )
	{	// replace [_First, _Last) with [_Ptr, <null>)
		return ( replace( _pdif( begin(), _first ), _pdif( _first, _last ), _ptr ) );
	};

	UString::Myt& UString::replace( ConstIterator _first, ConstIterator _last, Sizetype _count, Reference _chr )
	{	// replace [_First, _Last) with _count * _chr
		return ( replace( _pdif( begin(), _first ), _pdif( _first, _last ), _count, _chr ) );
	};

	UString::Myt& UString::replace( ConstIterator _first, ConstIterator _last, ConstPointer _first2, ConstPointer _last2 )
	{	// replace [_First, _Last) with [_First2, _Last2), const pointers
		if ( _first2 == _last2 )
			erase( _pdif( begin(), _first ), _pdif( _first, _last ) );
		else
			replace( _pdif( begin(), _first ), _pdif( _first, _last ), _first2, _last2 - _first2 );
		return (*this);
	};

	UString::Myt& UString::replace( ConstIterator _first, ConstIterator _last, ConstIterator _first2, ConstIterator _last2 )
	{	// replace [_First, _Last) with [_First2, _Last2), const_iterators
		if ( _first2 == _last2 )
			erase( _pdif( begin(), _first ), _pdif( _first, _last ) );
		else
			replace( _pdif( begin(), _first ), _pdif( _first, _last ), _first2.getPtr(), _last2 - _first2 );
		return (*this);
	};

	UString::Iterator UString::begin()
	{	// return Iterator for beginning of mutable sequence
		return ( _USTRING_ITERATOR( _myptr(), 0 ) );
	};

	UString::ConstIterator UString::begin() const
	{	// return Iterator for beginning of nonmutable sequence
		return ( _USTRING_CONST_ITERATOR( _myptr(), 0 ) );
	};

	UString::Iterator UString::end()
	{	// return Iterator for end of mutable sequence
		return ( _USTRING_ITERATOR( _myptr() + mysize, mylength ) );
	};

	UString::ConstIterator UString::end() const
	{	// return Iterator for end of nonmutable sequence
		return ( _USTRING_CONST_ITERATOR( _myptr() + mysize, mylength ) );
	};

	UString::ReverseIterator UString::rbegin()
	{	// return Iterator for beginning of reversed mutable sequence
		return ( ReverseIterator( end() ) );
	};

	UString::ConstReverseIterator UString::rbegin() const
	{	// return Iterator for beginning of reversed nonmutable sequence
		return ( ConstReverseIterator( end() ) );
	};

	UString::ReverseIterator UString::rend()
	{	// return Iterator for end of reversed mutable sequence
		return ( ReverseIterator( begin() ) );
	};

	UString::ConstReverseIterator UString::rend() const
	{	// return Iterator for end of reversed nonmutable sequence
		return ( ConstReverseIterator( begin() ) );
	};

	UString::Reference UString::at( Sizetype _off )
	{	// subscript mutable sequence with checking
		if ( _off >= mylength )
			_Xran();	// _off off end
		return ( begin()[_off] );
	};

	UString::ConstReference UString::at( Sizetype _off ) const
	{	// subscript nonmutable sequence with checking
		if ( _off >= mylength )
			_Xran();	// _off off end
		return ( begin()[_off] );
	};

	UString::Reference UString::operator[]( Sizetype _off )
	{	// subscript mutable sequence
		if ( _off > mylength )
		{
			USTRING_ERROR( "out of range" );
		}

		return ( begin()[ _off ] );
	};

	UString::ConstReference UString::operator[]( Sizetype _off ) const
	{	// subscript nonmutable sequence
		if ( _off > mylength )
		{
			USTRING_ERROR( "out of range" );
		}

		return ( begin()[ _off ] );
	};

	void UString::push_back( Reference _chr )
	{	// insert element at end
		insert( end(), _chr );
	};

	const UString::Elem* UString::c_str() const
	{	// return pointer to null-terminated nonmutable array
		return ( _myptr() );
	};

	const UString::Elem* UString::data() const
	{	// return pointer to nonmutable array
		return ( c_str() );
	};

	UString::Sizetype UString::maxSize() const
	{	// return maximum possible length of sequence
		return 1073741824;
	/*
		Sizetype num = Mybase_types::_Alty::max_size();
		return (num <= 1 ? 1 : num - 1);
	*/
	};

	void UString::resize( Sizetype _newlength )
	{	// determine new length, padding with null elements as needed
		resize( _newlength, Reference() );
	};

	void UString::resize( Sizetype _newlength, Reference _chr )
	{	// determine new length, padding with _Ch elements as needed
		if ( _newlength <= mylength )
			erase( _newlength );
		else
			append( _chr, _newlength - mylength );
	};

	UString::Sizetype UString::capacity() const
	{	// return current length of allocated storage
		return ( myres );
	};

	void UString::reserve( Sizetype _newcap )
	{	// determine new minimum length of allocated storage
		if ( mysize <= _newcap && myres != _newcap )
		{	// change reservation
			Sizetype size = mysize;
			Sizetype length = mylength;
			if ( _grow( _newcap, true ) )
				_eos( size, length );
		}
	};

	bool UString::empty() const
	{	// test if sequence is empty
		return ( mylength == 0 );
	};

	UString::Sizetype UString::copy( Elem* _dest, Sizetype _count, Sizetype _off ) const
	{	// copy [_off, _off + _count) to [_Dest, _Dest + _count)
		// assume there is enough space in _Ptr
		return _copy_s( _dest, /*_count,*/ _count, _off);
	};

	UString::Sizetype UString::_copy_s( Elem* _dest, /*Sizetype _Dest_size,*/ Sizetype _count, Sizetype _off ) const
	{	// copy [_off, _off + _count) to [_Dest, _Dest + _count)
		if ( _off > mylength )
			_Xran();	// _off off end
		if ( _count > mylength - _off )
			_count = mylength - _off;

		ConstPointer ptrBegin = ( begin() + _off ).getPtr();
		ConstPointer ptrEnd = ( begin() + _off + _count ).getPtr();
		std::copy( ptrBegin, ptrEnd, _dest );

		return ( ptrEnd - ptrBegin );
	};

	void UString::swap( Myt& _right )
	{	// exchange contents with _right
		if ( this == &_right )
			;	// same object, do nothing
		else// if (Mybase_types::_Alty == _right._Alval)
		{	// same allocator, swap control information
			Bxty tBx = bx;
			bx = _right.bx, _right.bx = tBx;

			Sizetype tLength = mylength;
			mylength = _right.mylength, _right.mylength = tLength;

			Sizetype tSize = mysize;
			mysize = _right.mysize, _right.mysize = tSize;

			Sizetype tRes = myres;
			myres = _right.myres, _right.myres = tRes;
		}
	/*
		else
		{	// different allocator, do multiple assigns
			Myt tmp = *this;

			*this = _right;
			_right = tmp;
		}
	*/
	};

	UString::Sizetype UString::find( const Myt& _right, Sizetype _off ) const
	{	// look for _right beginnng at or after _off
		return ( find( _right._myptr(), _off, _right.size() ) );
	};

	UString::Sizetype UString::find( const Elem* _ptr, Sizetype _off, Sizetype _count ) const
	{	// look for [_Ptr, _Ptr + _count) beginnng at or after _off
		if ( !_count && mylength >= _off )
			return ( _off );	// null string always matches (if inside string)

		Sizetype oSize;
		_getS_UTF8( _ptr, &oSize, &_count );

		const Elem* localPtr = ( begin() +_off ).getPtr();
		Sizetype localSize = mysize - ( localPtr - _myptr() );
		Sizetype localLength = mylength - _off;
		UTFSize tmpSize = 0;

		while ( localLength >= _count && localSize >= oSize )
		{
			tmpSize = UTF8_CHR_SIZE( *localPtr );

			if ( *_ptr == *localPtr && !memcmp( localPtr, _ptr, oSize ) )
				return mylength - localLength;

			if ( localSize < tmpSize )
				return NULL;

			localSize -= tmpSize;
			--localLength;

			localPtr += tmpSize;
		}

		return NULL;
	};

	UString::Sizetype UString::find( const Elem *_ptr, Sizetype _off ) const
	{	// look for [_Ptr, <null>) beginnng at or after _off
		return ( find( _ptr, _off, length( _ptr ) ) );
	};

	UString::Sizetype UString::find( Reference _chr, Sizetype _off ) const
	{	// look for _Ch at or after _off
		return ( find( _chr.getPtr(), _off, 1 ) );
	};

	UString::Sizetype UString::rfind( const Myt& _right, Sizetype _off ) const
	{	// look for _right beginning before _off
		return ( rfind( _right._myptr(), _off, _right.size() ) );
	};

	UString::Sizetype UString::rfind( const Elem* _ptr, Sizetype _off, Sizetype _count ) const
	{	// look for [_Ptr, _Ptr + _count) beginning before _off
		if ( !_count && mylength >= _off )
			return ( _off );	// null string always matches (if inside string)

		Sizetype oSize;
		_getS_UTF8( _ptr, &oSize, &_count );

		if (  _off > mylength )
			_off = mylength;

		if ( mylength - _off < _count )
			_off = mylength - _count;

		const Elem* localPtr = ( begin() + ( _off - 1 ) ).getPtr();
		bool firstChrFlag = false;
		while ( mysize - ( localPtr - _myptr() ) < oSize || ( firstChrFlag = ( UTF8_FIRST_BYTE( *localPtr ) != 0 ) ) )
		{
			if ( firstChrFlag )
				--_off;
			--localPtr;
		}

		Sizetype localSize = mysize - ( localPtr - _myptr() );
		Sizetype localLength = mylength - _off;
		UTFSize tmpSize = 0;

		while ( localPtr < _myptr() )
		{
			if ( UTF8_FIRST_BYTE( *localPtr ) )
			{
				--_off;
				if ( *_ptr == *localPtr && !memcmp( localPtr, _ptr, oSize ) )
					return mylength - localLength;
			}

			--localPtr;
		}

		return NULL;
	};

	UString::Sizetype UString::rfind( const Elem* _ptr, Sizetype _off ) const
	{	// look for [_Ptr, <null>) beginning before _off
		//_DEBUG_POINTER(_Ptr);
		return ( rfind( _ptr, _off, length( _ptr ) ) );
	};

	UString::Sizetype UString::rfind( Reference _chr, Sizetype _off ) const
	{	// look for _Ch before _off
		return ( rfind( _chr.getPtr(), _off, 1 ) );
	};

	UString::Sizetype UString::find_first_of( const Myt& _right, Sizetype _off ) const
	{	// look for one of _right at or after _off
		return ( find_first_of( _right._myptr(), _off, _right.size() ) );
	};

	UString::Sizetype UString::find_first_of( const Elem* _ptr, Sizetype _off, Sizetype _count ) const
	{	// look for one of [_Ptr, _Ptr + _count) at or after _off
		if ( _count > 0 && _off < mylength )
		{	// room for match, look for it
			Sizetype count = _count;
			const Elem* ptr = _ptr;
			bool asciiOnly = true;
			while ( count && asciiOnly )
			{
				if ( UTF8_FIRST_BYTE( *ptr ) )
				{
					asciiOnly = !UTF8_IS_ASCII( *ptr );
					--count;
				}
				++ptr;
			}

			if ( asciiOnly )
			{
				ConstIterator it = begin() + _off;
				const Elem* ptrLast = _ptr + _count;

				while ( it != end() )
				{
					ptr = _ptr;
					while ( ptr < ptrLast )
					{
						if ( *( it.getPtr() ) == *ptr )
							return it.getIndex();
						++ptr;
					}
					++it;
				}
			}
			else //asciiOnly
			{
				Codepoint* cpToFind = new Codepoint [ _count ];
				count = _count;
				ptr = _ptr;
				while ( count && asciiOnly )
				{
					if ( UTF8_FIRST_BYTE( *ptr ) )
					{
						cpToFind[ _count - count ].setValue( ptr );
						--count;
					}
					++ptr;
				}

				ConstIterator it = begin() + _off;
				
				while ( it != end() )
				{
					count = 0;
					while ( count < _count )
					{
						if ( *it == cpToFind[ count ] )
						{
							delete [] cpToFind;
							return it.getIndex();
						}
						++count;
					}
					++it;
				}

				delete [] cpToFind;
			}
		}

		return (npos);	// no match
	};

	UString::Sizetype UString::find_first_of( const Elem* _ptr, Sizetype _off ) const
	{	// look for one of [_Ptr, <null>) at or after _off
		return ( find_first_of( _ptr, _off, length( _ptr ) ) );
	};

	UString::Sizetype UString::find_first_of( Reference _chr, Sizetype _off ) const
	{	// look for _Ch at or after _off
		return ( find( _chr.getPtr(), _off, 1 ) );
	};

	UString::Sizetype UString::find_last_of( const Myt& _right, Sizetype _off ) const
	{	// look for one of _right before _off
		return (find_last_of(_right._myptr(), _off, _right.size()));
	};

	UString::Sizetype UString::find_last_of( const Elem* _ptr, Sizetype _off, Sizetype _count ) const
	{	// look for one of [_Ptr, _Ptr + _count) before _off
		if ( _count > 0 && mylength > 0 )
		{	// room for match, look for it
			if ( _off > mylength )
				_off = mylength;

			Sizetype count = _count;
			const Elem* ptr = _ptr;
			bool asciiOnly = true;
			while ( count && asciiOnly )
			{
				if ( UTF8_FIRST_BYTE( *ptr ) )
				{
					asciiOnly = !UTF8_IS_ASCII( *ptr );
					--count;
				}
				++ptr;
			}

			if ( asciiOnly )
			{
				ConstIterator it = begin() + _off;
				const Elem* ptrLast = _ptr + _count;

				while ( it != begin() )
				{
					--it;
					ptr = _ptr;
					while ( ptr < ptrLast )
					{
						if ( *( it.getPtr() ) == *ptr )
							return it.getIndex();
						++ptr;
					}
				}
			}
			else //asciiOnly
			{
				Codepoint* cpToFind = new Codepoint [ _count ];

				count = _count;
				ptr = _ptr;
				while ( count && asciiOnly )
				{
					if ( UTF8_FIRST_BYTE( *ptr ) )
					{
						cpToFind[ _count - count ].setValue( ptr );
						--count;
					}
					++ptr;
				}

				ConstIterator it = begin() + _off;
				while ( it != begin() )
				{
					--it;
					count = 0;
					while ( count < _count )
					{
						if ( *it == cpToFind[ count ] )
						{
							delete [] cpToFind;
							return it.getIndex();
						}
						++count;
					}
				}

				delete [] cpToFind;
			}
		}

		return (npos);	// no match
	};

	UString::Sizetype UString::find_last_of( const Elem* _ptr, Sizetype _off ) const
	{	// look for one of [_Ptr, <null>) before _off
		return ( find_last_of( _ptr, _off, length( _ptr ) ) );
	};

	UString::Sizetype UString::find_last_of( Reference _chr, Sizetype _off ) const
	{	// look for _Ch before _off
		return ( rfind( _chr.getPtr(), _off, 1 ) );
	};

	UString::Sizetype UString::find_first_not_of( const Myt& _right, Sizetype _off ) const
	{	// look for none of _right at or after _off
		return ( find_first_not_of( _right._myptr(), _off, _right.size() ) );
	};

	UString::Sizetype UString::find_first_not_of( const Elem* _ptr, Sizetype _off, Sizetype _count ) const
	{	// look for none of [_Ptr, _Ptr + _count) at or after _off
		if ( _count > 0 && _off < mylength )
		{	// room for match, look for it
			Sizetype count = _count;
			const Elem* ptr = _ptr;
			bool asciiOnly = true;
			while ( count && asciiOnly )
			{
				if ( UTF8_FIRST_BYTE( *ptr ) )
				{
					asciiOnly = !UTF8_IS_ASCII( *ptr );
					--count;
				}
				++ptr;
			}

			bool notFoundFlag = false;
			
			if ( asciiOnly )
			{
				ConstIterator it = begin() + _off;
				const Elem* ptrLast = _ptr + _count;

				while ( it != end() )
				{
					ptr = _ptr;
					notFoundFlag = false;
					while ( ptr < ptrLast )
					{
						notFoundFlag |= ( *( it.getPtr() ) == *ptr );
						++ptr;
					}

					if ( !notFoundFlag )
						return it.getIndex();
					++it;
				}
			}
			else //asciiOnly
			{
				Codepoint* cpToFind = new Codepoint [ _count ];
				count = _count;
				ptr = _ptr;
				while ( count && asciiOnly )
				{
					if ( UTF8_FIRST_BYTE( *ptr ) )
					{
						cpToFind[ _count - count ].setValue( ptr );
						--count;
					}
					++ptr;
				}

				ConstIterator it = begin() + _off;
				
				while ( it != end() )
				{
					count = 0;
					notFoundFlag = false;
					while ( count < _count )
					{
						notFoundFlag |= ( *it != cpToFind[ count ] );
						++count;
					}

					if ( !notFoundFlag )
					{
						delete [] cpToFind;
						return it.getIndex();
					}

					++it;
				}

				delete [] cpToFind;
			}
		}

		return (npos);	// no match
	};

	UString::Sizetype UString::find_first_not_of( const Elem *_ptr, Sizetype _off ) const
	{	// look for one of [_Ptr, <null>) at or after _off
		return ( find_first_not_of( _ptr, _off, length( _ptr ) ) );
	};

	UString::Sizetype UString::find_first_not_of( Reference _chr, Sizetype _off ) const
	{	// look for non _Ch at or after _off
		return ( find_first_not_of( _chr.getPtr(), _off, 1 ) );
	};

	UString::Sizetype UString::find_last_not_of( const Myt& _right, Sizetype _off ) const
	{	// look for none of _right before _off
		return ( find_last_not_of( _right._myptr(), _off, _right.size() ) );
	};

	UString::Sizetype UString::find_last_not_of( const Elem* _ptr, Sizetype _off, Sizetype _count ) const
	{	// look for none of [_Ptr, _Ptr + _count) before _off
		if ( _count > 0 && mylength > 0 )
		{	// room for match, look for it
			if ( _off > mylength )
				_off = mylength;

			Sizetype count = _count;
			const Elem* ptr = _ptr;
			bool asciiOnly = true;
			while ( count && asciiOnly )
			{
				if ( UTF8_FIRST_BYTE( *ptr ) )
				{
					asciiOnly = !UTF8_IS_ASCII( *ptr );
					--count;
				}
				++ptr;
			}

			bool notFoundFlag = false;

			if ( asciiOnly )
			{
				ConstIterator it = begin() + _off;
				const Elem* ptrLast = _ptr + _count;

				while ( it != begin() )
				{
					--it;
					ptr = _ptr;
					notFoundFlag = false;
					while ( ptr < ptrLast )
					{
						notFoundFlag |= ( *( it.getPtr() ) == *ptr );
						++ptr;
					}

					if ( !notFoundFlag )
						return it.getIndex();
				}
			}
			else //asciiOnly
			{
				Codepoint* cpToFind = new Codepoint [ _count ];
				count = _count;
				ptr = _ptr;
				while ( count && asciiOnly )
				{
					if ( UTF8_FIRST_BYTE( *ptr ) )
					{
						cpToFind[ _count - count ].setValue( ptr );
						--count;
					}
					++ptr;
				}

				ConstIterator it = begin() + _off;
				while ( it != begin() )
				{
					--it;
					count = 0;
					while ( count < _count )
					{
						notFoundFlag |= ( *it == cpToFind[ count ] );
						++count;
					}

					if ( !notFoundFlag )
					{
						delete [] cpToFind;
						return it.getIndex();
					}
				}

				delete [] cpToFind;
			}
		}

		return (npos);	// no match
	};

	UString::Sizetype UString::find_last_not_of( const Elem* _ptr, Sizetype _off ) const
	{	// look for none of [_Ptr, <null>) before _off
		return ( find_last_not_of( _ptr, _off, length( _ptr ) ) );
	};

	UString::Sizetype UString::find_last_not_of( Reference _chr, Sizetype _off ) const
	{	// look for non _Ch before _off
		return ( find_last_not_of( _chr.getPtr(), _off, 1 ) );
	};

	UString::Myt UString::substr( Sizetype _off, Sizetype _count ) const
	{	// return [_off, _off + _count) as new string
		return ( Myt( *this, _off, _count ) );
	};

	int UString::compare( const Myt& _right ) const
	{	// compare [0, _Mysize) with _right
		return _compare( _myptr(), size(), _right._myptr(), _right.size() );
	};

	int UString::compare( Sizetype _off, Sizetype _n0, const Myt& _right ) const
	{	// compare [_off, _off + _n0) with _right
		if ( _off > mylength )
			_Xran();	// _off off end

		ConstIterator it = begin() + _off;
		ConstIterator itEnd = ( _n0 >= ( mylength - _off ) ) ? end() : it + _n0;
		return _compare( it.getPtr(), itEnd.getPtr() - it.getPtr(), _right._myptr(), _right.size() );
	};

	int UString::compare( Sizetype _off, Sizetype _n0, const Myt& _right, Sizetype _roff, Sizetype _count ) const
	{	// compare [_off, _off + _n0) with _right [_roff, _roff + _count)
		if ( _roff > _right.mylength || _off > mylength )
			_Xran();	// _off off end

		ConstIterator itL = begin() + _off;
		ConstIterator itLEnd = ( _n0 >= ( mylength - _off ) ) ? end() : itL + _n0;
		ConstIterator itR = _right.begin() + _roff;
		ConstIterator itREnd = ( _count >= ( _right.length() - _roff ) ) ? _right.end() : itR + _count;

		return ( _compare( itL.getPtr(), itLEnd.getPtr() - itL.getPtr(), itR.getPtr(), itREnd.getPtr() - itR.getPtr() ) );
	};

	int UString::compare( const Elem* _ptr ) const
	{	// compare [0, _Mysize) with [_Ptr, <null>)
		return ( _compare( _myptr(), mysize, _ptr, size( _ptr ) ) );
	};

	int UString::compare( Sizetype _off, Sizetype _n0, const Elem* _ptr ) const
	{	// compare [_off, _off + _n0) with [_Ptr, <null>)
		if ( _off > mylength )
			_Xran();	// _off off end

		ConstIterator itL = begin() + _off;
		ConstIterator itLEnd = ( _n0 >= ( mylength - _off ) ) ? end() : itL + _n0;

		return ( _compare( itL.getPtr(), itLEnd.getPtr() - itL.getPtr(), _ptr, size( _ptr ) ) );
	};

	int UString::compare( Sizetype _off, Sizetype _n0, const Elem* _ptr, Sizetype _count ) const
	{	// compare [_off, _off + _n0) with [_Ptr, _Ptr + _count)
		if ( _off > mylength )
			_Xran();	// _off off end

		ConstIterator itL = begin() + _off;
		ConstIterator itLEnd = ( _n0 >= ( mylength - _off ) ) ? end() : itL + _n0;
		Sizetype rsize;
		_getS_UTF8( _ptr, &rsize, &_count );

		return ( _compare( itL.getPtr(), itLEnd.getPtr() - itL.getPtr(), _ptr, rsize ) );
	};

	int UString::compare( const Myt& _left, const Myt& _right )
	{
		return _compare( _left._myptr(), _left.size(), _right._myptr(), _right.size() );
	};

	int UString::compare( const Myt& _left, Sizetype _loff, Sizetype _lcount, const Myt& _right, Sizetype _roff, Sizetype _rcount )
	{
		if ( _loff > _left.mylength || _roff > _right.mylength )
			_Xran();	// _off off end

		ConstIterator itL = _left.begin() + _loff;
		ConstIterator itLEnd = ( _lcount >= ( _left.length() - _loff ) ) ? _left.end() : itL + _lcount;
		ConstIterator itR = _right.begin() + _roff;
		ConstIterator itREnd = ( _rcount >= ( _right.length() - _roff ) ) ? _right.end() : itR + _rcount;

		return ( _compare( itL.getPtr(), itLEnd.getPtr() - itL.getPtr(), itR.getPtr(), itREnd.getPtr() - itR.getPtr() ) );
	};

	int UString::_compare( const Elem* _ptrLeft, Sizetype _offLeft, const Elem* _ptrRight, Sizetype _offRight )
	{
		int ans = std::memcmp( _ptrLeft, _ptrRight, _offLeft < _offRight ? _offLeft : _offRight );
		if ( ans )
			return ans;
		if ( _offLeft < _offRight )
			return -1;
		if ( _offLeft > _offRight )
			return 1;
		return 0;
	};


	std::string UString::getString() const
	{
		std::string strOut( mylength, CP_UNKNOWN_CONVERTION );
		for ( ConstIterator it = begin(); it != end(); ++it )
		{
			if ( UTF8_IS_ASCII( *( it.getPtr() ) ) )
				strOut[ it.getIndex() ] = *( it.getPtr() );
		}
		return strOut;
	};

	SLpair UString::getString8( CHAR8* _ptrOut )
	{
		if ( _ptrOut )
		{
			std::copy( _myptr(), _myptr() + mysize, _ptrOut );
		}

		return SLpair( mysize, mylength );
	};

	SLpair UString::getString16( CHAR16* _ptrOut )
	{
		if ( _ptrOut )
		{
			CHAR16* ptr = _ptrOut;
			SLpair slpOut( 0, mylength );
			for ( ConstIterator it = begin(); it != end(); ++it )
			{
				CHAR32 value = Codepoint::getValue( it.getPtr() );

				if ( value < 0x010000 )
				{
					slpOut.size += 1;

					CP_TO_UTF16_1( value, ptr );
					ptr += 1;
				}
				else
				{
					slpOut.size += 2;
					CP_TO_UTF16_2( value, ptr );
					ptr += 2;
				}
			}
			return slpOut;
		}

		if ( mysize == mylength )
			return SLpair( mylength, mylength );

		SLpair slpOut( 0, mylength );
		for ( ConstIterator it = begin(); it != end(); ++it )
		{
			slpOut.size += CP_UTF16_SIZE( Codepoint::getValue( it.getPtr() ) );
		}
		return slpOut;
	};

	SLpair UString::getString32( CHAR32* _ptrOut )
	{
		if ( _ptrOut )
		{
			CHAR32* ptr = _ptrOut;
			for ( ConstIterator it = begin(); it != end(); ++it )
			{
				*ptr = Codepoint::getValue( it.getPtr() );
				++ptr;
			}
		}

		return SLpair( mylength, mylength );
	};



	UString str_trim( const UString& _str_text )
	{
		size_t first = _str_text.find_first_not_of(" \t\n\v\f\r");
		if ( first == UString::npos )
			return UString("");

		size_t last = _str_text.find_last_not_of(" \t\n\v\f\r");

		return _str_text.substr( first, last - first + 1 );
	};

}


_STD_BEGIN

ostream& operator<< ( ostream& _Ostr, const Unicode::UString& _Str )
{	// insert a string
	typedef char_traits<char> _Traits;
	typedef ostream _Myos;
	typedef Unicode::UString _Mystr;
	typedef _Mystr::Sizetype _Mysizt;

	ios_base::iostate _State = ios_base::goodbit;
	_Mysizt _Size = _Str.size();
	_Mysizt _Length = _Str.length();
	_Mysizt _Pad = _Ostr.width() <= 0 || (_Mysizt)_Ostr.width() <= _Length ? 0 : (_Mysizt)_Ostr.width() - _Length;
	const _Myos::sentry _Ok( _Ostr );

	if (!_Ok)
	{
		_State |= ios_base::badbit;
	}
	else
	{	// state okay, insert characters
		_TRY_IO_BEGIN

		if ( ( _Ostr.flags() & ios_base::adjustfield ) != ios_base::left )
		{
			for ( ; 0 < _Pad; --_Pad )	// pad on left
			{
				if ( _Traits::eq_int_type( _Traits::eof(), _Ostr.rdbuf()->sputc( _Ostr.fill() ) ) )
				{	// insertion failed, quit
					_State |= ios_base::badbit;
					break;
				}
			}
		}

		if ( _State == ios_base::goodbit )
		{
			const Unicode::CHAR8* _String = _Str.c_str();
			for ( _Mysizt _Count = 0; _Count < _Size; ++_Count )
			{
				if ( _Traits::eq_int_type( _Traits::eof(), _Ostr.rdbuf()->sputc( _String[_Count] ) ) )
				{	// insertion failed, quit
					_State |= ios_base::badbit;
					break;
				}
			}
		}

		if ( _State == ios_base::goodbit )
		{
			for ( ; 0 < _Pad; --_Pad )	// pad on right
			{
				if ( _Traits::eq_int_type( _Traits::eof(), _Ostr.rdbuf()->sputc( _Ostr.fill() ) ) )
				{	// insertion failed, quit
					_State |= ios_base::badbit;
					break;
				}
			}
		}

		_Ostr.width(0);

		//_CATCH_IO_(_Ostr)
		_CATCH_ALL
		(_Ostr).setstate(ios_base::badbit, true);
		_CATCH_END
	}

	_Ostr.setstate(_State);
	return (_Ostr);
};


ostream& operator<< ( ostream& _Ostr, Unicode::Codepoint _Ch )
{	// insert a character
	typedef char_traits<char> _Traits;
	typedef ostream _Myos;

	ios_base::iostate _State = ios_base::goodbit;
	const _Myos::sentry _Ok(_Ostr);

	if (_Ok)
	{	// state okay, insert
		streamsize _Pad = _Ostr.width() <= 1 ? 0 : _Ostr.width() - 1;

		_TRY_IO_BEGIN

		if ( ( _Ostr.flags() & ios_base::adjustfield ) != ios_base::left )
		{
			for ( ; _State == ios_base::goodbit && 0 < _Pad; --_Pad )	// pad on left
			{
				if ( _Traits::eq_int_type( _Traits::eof(), _Ostr.rdbuf()->sputc( _Ostr.fill() ) ) )
					_State |= ios_base::badbit;
			}
		}

		if ( _State == ios_base::goodbit )
		{
			Unicode::CHAR8 _String[ UNICODE_MAX_UTF8_LENGTH ];
			Unicode::UTFSize _Size = _Ch.copy( _String );
			for ( Unicode::UTFSize _Count = 0; _Count < _Size; ++_Count )
			{
				if ( _Traits::eq_int_type( _Traits::eof(), _Ostr.rdbuf()->sputc( _String[_Count] ) ) )
				{	// insertion failed, quit
					_State |= ios_base::badbit;
					break;
				}
			}
		}

		for ( ; _State == ios_base::goodbit && 0 < _Pad; --_Pad )	// pad on right
		{
			if ( _Traits::eq_int_type( _Traits::eof(), _Ostr.rdbuf()->sputc( _Ostr.fill() ) ) )
				_State |= ios_base::badbit;
		}

		//_CATCH_IO_(_Ostr)
		_CATCH_ALL
		(_Ostr).setstate(ios_base::badbit, true);
		_CATCH_END
	}

	_Ostr.width(0);
	_Ostr.setstate(_State);
	return (_Ostr);
};



wostream& operator<< ( wostream& _Ostr, const Unicode::UString& _Str )
{	// insert a string
	typedef char_traits<wchar_t> _Traits;
	typedef wostream _Myos;
	typedef Unicode::UString _Mystr;
	typedef _Mystr::Sizetype _Mysizt;

	ios_base::iostate _State = ios_base::goodbit;
	_Mysizt _Length = _Str.length();
	_Mysizt _Pad = _Ostr.width() <= 0 || (_Mysizt)_Ostr.width() <= _Length ? 0 : (_Mysizt)_Ostr.width() - _Length;
	const _Myos::sentry _Ok( _Ostr );

	if (!_Ok)
	{
		_State |= ios_base::badbit;
	}
	else
	{	// state okay, insert characters
		_TRY_IO_BEGIN

		if ( ( _Ostr.flags() & ios_base::adjustfield ) != ios_base::left )
		{
			for ( ; 0 < _Pad; --_Pad )	// pad on left
			{
				if ( _Traits::eq_int_type( _Traits::eof(), _Ostr.rdbuf()->sputc( _Ostr.fill() ) ) )
				{	// insertion failed, quit
					_State |= ios_base::badbit;
					break;
				}
			}
		}


		if ( _State == ios_base::goodbit )
		{
			Unicode::CHAR16 _Chr[ UNICODE_MAX_UTF16_LENGTH ];
			Unicode::UTFSize _Size = 0;
			for ( Unicode::UString::ConstIterator it = _Str.begin(); it != _Str.end(); ++it )
			{
				_Size = (*it).getValUtf16( _Chr );
				if ( ( _Traits::eq_int_type( _Traits::eof(), _Ostr.rdbuf()->sputc( _Chr[0] ) ) ) || ( _Size > 1 && ( _Traits::eq_int_type( _Traits::eof(), _Ostr.rdbuf()->sputc( _Chr[1] ) ) ) ) )
				{	// insertion failed, quit
					_State |= ios_base::badbit;
					break;
				}
			}
		}

		if ( _State == ios_base::goodbit )
		{
			for ( ; 0 < _Pad; --_Pad )	// pad on right
			{
				if ( _Traits::eq_int_type( _Traits::eof(), _Ostr.rdbuf()->sputc( _Ostr.fill() ) ) )
				{	// insertion failed, quit
					_State |= ios_base::badbit;
					break;
				}
			}
		}

		_Ostr.width(0);

		//_CATCH_IO_(_Ostr)
		_CATCH_ALL
		(_Ostr).setstate(ios_base::badbit, true);
		_CATCH_END
	}

	_Ostr.setstate(_State);
	return (_Ostr);
};


wostream& operator<< ( wostream& _Ostr, Unicode::Codepoint _Ch )
{	// insert a character
	typedef char_traits<wchar_t> _Traits;
	typedef wostream _Myos;

	ios_base::iostate _State = ios_base::goodbit;
	const _Myos::sentry _Ok(_Ostr);

	if (_Ok)
	{	// state okay, insert
		streamsize _Pad = _Ostr.width() <= 1 ? 0 : _Ostr.width() - 1;

		_TRY_IO_BEGIN

		if ( ( _Ostr.flags() & ios_base::adjustfield ) != ios_base::left )
		{
			for ( ; _State == ios_base::goodbit && 0 < _Pad; --_Pad )	// pad on left
			{
				if ( _Traits::eq_int_type( _Traits::eof(), _Ostr.rdbuf()->sputc( _Ostr.fill() ) ) )
					_State |= ios_base::badbit;
			}
		}

		if ( _State == ios_base::goodbit )
		{
			Unicode::CHAR16 _Chr[ UNICODE_MAX_UTF16_LENGTH ];
			Unicode::UTFSize _Size = _Ch.getValUtf16( _Chr );
			if ( ( _Traits::eq_int_type( _Traits::eof(), _Ostr.rdbuf()->sputc( _Chr[0] ) ) ) || ( _Size > 1 && ( _Traits::eq_int_type( _Traits::eof(), _Ostr.rdbuf()->sputc( _Chr[1] ) ) ) ) )
			{	// insertion failed, quit
				_State |= ios_base::badbit;
			}
		}

		for ( ; _State == ios_base::goodbit && 0 < _Pad; --_Pad )	// pad on right
		{
			if ( _Traits::eq_int_type( _Traits::eof(), _Ostr.rdbuf()->sputc( _Ostr.fill() ) ) )
				_State |= ios_base::badbit;
		}

		//_CATCH_IO_(_Ostr)
		_CATCH_ALL
		(_Ostr).setstate(ios_base::badbit, true);
		_CATCH_END
	}

	_Ostr.width(0);
	_Ostr.setstate(_State);
	return (_Ostr);
};


_STD_END


#pragma warning(default:4018)
//#pragma warning(default:4996)
