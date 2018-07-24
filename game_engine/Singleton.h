#pragma once

#ifndef _SINGLETON
#define _SINGLETON


template<class _Ty> class DynamicDtor
{
private:
	_Ty* pTy_instance;

public:
	DynamicDtor( _Ty* _pTy_instance = NULL ) : pTy_instance( _pTy_instance ) {};
	~DynamicDtor() { if ( pTy_instance ) delete pTy_instance; };
	void set( _Ty* _pTy_instance ) { pTy_instance = _pTy_instance; };

private:
	DynamicDtor(const DynamicDtor &);
	void operator = (const DynamicDtor &);
};


template<class _Ty> class Singleton
{
private:
	static _Ty* pTy_instance;
	static DynamicDtor<_Ty> dd_instanceDtor;

private: 
	Singleton(){};
	~Singleton(){};
	Singleton(const Singleton &old);
	const Singleton &operator=(const Singleton &old);

public:
	static _Ty& get()
	{
		if ( !pTy_instance )
		{
			pTy_instance = new _Ty;
			dd_instanceDtor.set( pTy_instance );
		}
		return *pTy_instance;
	};
};

template<class _Ty> _Ty* Singleton<_Ty>::pTy_instance = NULL;
template<class _Ty> DynamicDtor<_Ty> Singleton<_Ty>::dd_instanceDtor; 




// SINGLETON template with DynamicDtor inside Singleton class
/*
template<class _Ty> class Singleton
{
private:
	class DynamicDtor
	{
	private:
		_Ty* pTy_instance;

	public:
		DynamicDtor( _Ty* _pTy_instance = NULL ) : pTy_instance( _pTy_instance ) {};
		~DynamicDtor() { if ( pTy_instance ) delete pTy_instance; };
		void set( _Ty* _pTy_instance ) { pTy_instance = _pTy_instance; };

	private:
		DynamicDtor(const DynamicDtor &);
		void operator = (const DynamicDtor &);
	};

private:
	static _Ty* pTy_instance;
	static DynamicDtor dd_instanceDtor;

private: 
	Singleton(){};
	~Singleton(){};
	Singleton(const Singleton &old);
	const Singleton &operator=(const Singleton &old);

public:
	static _Ty& get()
	{
		if ( !pTy_instance )
		{
			pTy_instance = new _Ty;
			dd_instanceDtor.set( pTy_instance );
		}
		return *pTy_instance;
	};
};

template<class _Ty> _Ty* Singleton<_Ty>::pTy_instance = NULL;
template<class _Ty> typename Singleton<_Ty>::DynamicDtor Singleton<_Ty>::dd_instanceDtor; 
*/


#endif //_SINGLETON
