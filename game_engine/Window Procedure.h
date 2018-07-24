#pragma once

#ifndef _WINDOW_PROCEDURE
#define _WINDOW_PROCEDURE


#include <windows.h>
#include "boost/container/flat_map.hpp"
#include <map>
#include <string>


namespace GameEngine
{
	class MessageProcessorWrapper;
	class MessageProcessorFunction;
	//class MessageProcessorFunctor; //templated
	class WindowProcedure;


	class MessageProcessorWrapper
	{
	public:
		const UINT ui_messageType;
		const std::string str_processorName;
		MessageProcessorWrapper* mpw_nextSibling;

		MessageProcessorWrapper( const std::string& _str_processorName, UINT _ui_messageType, MessageProcessorWrapper* _mpw_nextSibling = NULL );
		~MessageProcessorWrapper();

		virtual MessageProcessorWrapper* clone() = 0;

		// if return true stop processing
		virtual bool operator()( HWND _hwnd, WPARAM _wParam, LPARAM _lParam ) = 0;
		virtual bool call( HWND _hwnd, WPARAM _wParam, LPARAM _lParam ) = 0;

	};



	class MessageProcessorFunction : public MessageProcessorWrapper
	{
	private:
		bool ( *msgProcFunc )( HWND, WPARAM, LPARAM );

	public:

		MessageProcessorFunction ( const std::string& _str_processorName, UINT _ui_messageType, bool ( *_msgProcFunc )( HWND, WPARAM, LPARAM ), MessageProcessorWrapper* _mpw_nextSibling = NULL );

		virtual MessageProcessorWrapper* clone();

		bool operator()( HWND _hwnd, WPARAM _wParam, LPARAM _lParam );
		bool call( HWND _hwnd, WPARAM _wParam, LPARAM _lParam );
	};



	template <class T>
	class MessageProcessorFunctor : public MessageProcessorWrapper
	{
	private:
		bool ( T::*msgProcMthd )( HWND, WPARAM, LPARAM );
		T* msgProcObj;

	public:

		MessageProcessorFunctor (	const std::string& _str_processorName, 
									UINT _ui_messageType, 
									bool ( T::*_msgProcMthd )( HWND, WPARAM, LPARAM ), 
									T* _msgProcObj, 
									MessageProcessorWrapper* _mpw_nextSibling = NULL ) : 
			MessageProcessorWrapper( _str_processorName, _ui_messageType, _mpw_nextSibling ), 
			msgProcMthd( _msgProcMthd ), 
			msgProcObj( _msgProcObj )
		{};

		virtual MessageProcessorWrapper* clone()
		{
			MessageProcessorWrapper* tmp = new MessageProcessorFunctor( str_processorName, ui_messageType, msgProcMthd, msgProcObj, NULL );
			if ( mpw_nextSibling )
			{
				tmp->mpw_nextSibling = mpw_nextSibling->clone();
			}
			return tmp;
		};

		bool operator()( HWND _hwnd, WPARAM _wParam, LPARAM _lParam )
		{
			return ( *msgProcObj.*msgProcMthd )( _hwnd, _wParam, _lParam );
		};

		bool call( HWND _hwnd, WPARAM _wParam, LPARAM _lParam )
		{
			return ( *msgProcObj.*msgProcMthd )( _hwnd, _wParam, _lParam );
		};
	};




	class WindowProcedure
	{
	private:

		typedef boost::container::flat_map< UINT, MessageProcessorWrapper* > MessageProcMap;

		const std::string str_name;
		MessageProcMap mpm_actMsgProcessor;

		typedef std::map< std::string,WindowProcedure* > WndProcMap;
		static WndProcMap m_wndProcMap;
		static WindowProcedure* wp_usedWndProc;



		void addMessageProcessor( MessageProcessorWrapper* _wrappedMessageProcessor );


	protected:

		WindowProcedure();
		WindowProcedure( const std::string& _str_name );
		WindowProcedure( const WindowProcedure& _rhs, const std::string& _str_name = std::string("") );

		~WindowProcedure();

		WindowProcedure& operator= ( const WindowProcedure& _rhs );


	public:

		std::string getName() const;

		void clear();
		virtual WindowProcedure* getClone( const std::string& _str_name );

		void addMessageProcessor( const std::string& _str_processorName, UINT _ui_messageType, bool ( *_msgProcFunc )( HWND, WPARAM, LPARAM ) );

		template <class T>
		void addMessageProcessor( const std::string& _str_processorName, UINT _ui_messageType, bool ( T::*_msgProcMthd )( HWND, WPARAM, LPARAM ), T* _msgProcObj )
		{
			MessageProcessorWrapper* newMsgProc = new MessageProcessorFunctor<T>( _str_processorName, _ui_messageType, _msgProcMthd, _msgProcObj );
			addMessageProcessor( newMsgProc );
		};

		void removeMessageProcessor( const std::string& _str_processorName );


		// if message was processed and shouldn't be sent further, return true
		// else return false
		virtual bool processMessage( HWND _hwnd, UINT _msg, WPARAM _wParam, LPARAM _lParam );

		void setProcedure();


		static LRESULT CALLBACK WndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );

		static bool create( const std::string& _str_name, WindowProcedure** _wp_wndProc );
		static bool clone( const std::string& _str_name, const std::string& _str_existingName, WindowProcedure** _wp_wndProc );
		static WindowProcedure* get( const std::string& _str_name );

		static bool setProcedure( const std::string& _str_name );
		static std::string getProcedureName();

	};

}

#endif //_WINDOW_PROCEDURE
