#include "Window Procedure.h"


namespace GameEngine
{
	// static declarations
	WindowProcedure::WndProcMap WindowProcedure::m_wndProcMap;
	WindowProcedure* WindowProcedure::wp_usedWndProc;

	


	MessageProcessorWrapper::MessageProcessorWrapper( const std::string& _str_processorName, UINT _ui_messageType, MessageProcessorWrapper* _mpw_nextSibling ) : 
		str_processorName( _str_processorName ), 
		ui_messageType( _ui_messageType ), 
		mpw_nextSibling( _mpw_nextSibling )
	{};

	MessageProcessorWrapper::~MessageProcessorWrapper()
	{
		if ( mpw_nextSibling )
			delete mpw_nextSibling;
	};


	MessageProcessorFunction::MessageProcessorFunction ( const std::string& _str_processorName, UINT _ui_messageType, bool ( *_msgProcFunc )( HWND, WPARAM, LPARAM ), MessageProcessorWrapper* _mpw_nextSibling ) : 
		MessageProcessorWrapper( _str_processorName, _ui_messageType, _mpw_nextSibling ), 
		msgProcFunc( _msgProcFunc )
	{};

	MessageProcessorWrapper* MessageProcessorFunction::clone()
	{
		MessageProcessorWrapper* tmp = new MessageProcessorFunction( str_processorName, ui_messageType, msgProcFunc, NULL );
		if ( mpw_nextSibling )
		{
			tmp->mpw_nextSibling = mpw_nextSibling->clone();
		}
		return tmp;
	};

	bool MessageProcessorFunction::operator()( HWND _hwnd, WPARAM _wParam, LPARAM _lParam )
	{
		return ( *msgProcFunc )( _hwnd, _wParam, _lParam );
	};

	bool MessageProcessorFunction::call( HWND _hwnd, WPARAM _wParam, LPARAM _lParam )
	{
		return ( *msgProcFunc )( _hwnd, _wParam, _lParam );
	};



	void WindowProcedure::addMessageProcessor( MessageProcessorWrapper* _wrappedMessageProcessor )
	{
		MessageProcMap::iterator it = mpm_actMsgProcessor.find( _wrappedMessageProcessor->ui_messageType );

		if ( it == mpm_actMsgProcessor.end() )
		{
			mpm_actMsgProcessor.insert( MessageProcMap::value_type( _wrappedMessageProcessor->ui_messageType, _wrappedMessageProcessor ) );
		}
		else
		{
			MessageProcessorWrapper* tmp = it->second;
			while ( tmp->mpw_nextSibling )
			{
				tmp = tmp->mpw_nextSibling;
			}

			tmp->mpw_nextSibling = _wrappedMessageProcessor;
		}
	};

	WindowProcedure::WindowProcedure() : str_name( std::string("") )
	{};

	WindowProcedure::WindowProcedure( const std::string& _str_name ) : str_name( _str_name )
	{};

	WindowProcedure::WindowProcedure( const WindowProcedure& _rhs, const std::string& _str_name ) : str_name( _str_name )
	{
		mpm_actMsgProcessor = _rhs.mpm_actMsgProcessor;
		for ( MessageProcMap::iterator it = mpm_actMsgProcessor.begin(); it != mpm_actMsgProcessor.end(); ++it )
		{
			it->second = it->second->clone();
		}
	};

	WindowProcedure::~WindowProcedure()
	{
		clear();
	};

	WindowProcedure& WindowProcedure::operator= ( const WindowProcedure& _rhs )
	{
		mpm_actMsgProcessor = _rhs.mpm_actMsgProcessor;
		for ( MessageProcMap::iterator it = mpm_actMsgProcessor.begin(); it != mpm_actMsgProcessor.end(); ++it )
		{
			it->second = it->second->clone();
		}

		return *this;
	};

	std::string WindowProcedure::getName() const
	{
		return str_name;
	};
	
	void WindowProcedure::clear()
	{
		for ( MessageProcMap::iterator it = mpm_actMsgProcessor.begin(); it != mpm_actMsgProcessor.end(); ++it )
		{
			delete it->second;
		}
		mpm_actMsgProcessor.clear();
	};

	WindowProcedure* WindowProcedure::getClone( const std::string& _str_name )
	{
		WindowProcedure* out = new WindowProcedure( *this, _str_name );
		return out;
	};

	void WindowProcedure::addMessageProcessor( const std::string& _str_processorName, UINT _ui_messageType, bool ( *_msgProcFunc )( HWND, WPARAM, LPARAM ) )
	{
		MessageProcessorWrapper* newMsgProc = new MessageProcessorFunction( _str_processorName, _ui_messageType, _msgProcFunc );
		addMessageProcessor( newMsgProc );
	};

	void WindowProcedure::removeMessageProcessor( const std::string& _str_processorName )
	{
		for ( MessageProcMap::iterator it = mpm_actMsgProcessor.begin(); it != mpm_actMsgProcessor.end(); ++it )
		{
			MessageProcessorWrapper* tmp = NULL;
			MessageProcessorWrapper* prev = NULL;
			MessageProcessorWrapper* test = it->second;

			while ( test )				
			{
				if ( test->str_processorName == _str_processorName )
				{
					tmp = test;
					if ( prev )
						prev->mpw_nextSibling = test->mpw_nextSibling;
					else
						it->second = test->mpw_nextSibling;

					test = test->mpw_nextSibling;
					tmp->mpw_nextSibling = NULL;
					delete tmp;
					tmp = NULL;
				}
				else
				{
					prev = test;
					test = test->mpw_nextSibling;
				}
			}

			if ( !( it->second ) )
			{
				MessageProcMap::iterator tmpIt = it;
				--it;
				mpm_actMsgProcessor.erase( tmpIt );
			}
		}
	};

	bool WindowProcedure::processMessage( HWND _hwnd, UINT _msg, WPARAM _wParam, LPARAM _lParam )
	{
		MessageProcMap::iterator it = mpm_actMsgProcessor.find( _msg );
		bool processedFlag = false;

		if ( it != mpm_actMsgProcessor.end() )
		{
			MessageProcessorWrapper* msgProc = it->second;

			while ( msgProc && !processedFlag )
			{
				processedFlag = ( *msgProc )( _hwnd, _wParam, _lParam );
				msgProc = msgProc->mpw_nextSibling;
			}
		}

		return processedFlag;
	};

	void WindowProcedure::setProcedure()
	{
		WindowProcedure::wp_usedWndProc = this;
	};


	LRESULT CALLBACK WindowProcedure::WndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
	{
		wp_usedWndProc->processMessage( hwnd, msg, wParam, lParam );
//		if ( wp_usedWndProc && wp_usedWndProc->processMessage( hwnd, msg, wParam, lParam ) )
//			return 0;

		switch( msg )
		{
			case WM_DESTROY:
			{
				PostQuitMessage(0);
				break;
			}

		// debug only
		#ifdef _DEBUG

			case WM_KEYDOWN:
			{
				if( wParam == VK_ESCAPE )
					DestroyWindow(hwnd);
				break;
			}

		#endif //_DEBUG


			default:
				return DefWindowProc(hwnd, msg, wParam, lParam);
		}

		return 0;
	};



	bool WindowProcedure::create( const std::string& _str_name, WindowProcedure** _wp_wndProc )
	{
		WndProcMap::iterator it = m_wndProcMap.find( _str_name );
		if ( it != m_wndProcMap.end() )
		{
			*_wp_wndProc = it->second;
			return false;
		}

		WindowProcedure* tmp = new WindowProcedure( _str_name );
		m_wndProcMap.insert( WndProcMap::value_type( _str_name, tmp ) );
		*_wp_wndProc = tmp;
		return true;
	};

	bool WindowProcedure::clone( const std::string& _str_name, const std::string& _str_existingName, WindowProcedure** _wp_wndProc )
	{
		WndProcMap::iterator itBase = m_wndProcMap.find( _str_existingName );
		if ( itBase == m_wndProcMap.end() )
		{
			*_wp_wndProc = NULL;
			return false;
		}

		std::pair<WndProcMap::iterator,bool> out = m_wndProcMap.insert( WndProcMap::value_type( _str_name, NULL ) );
		WndProcMap::iterator it = out.first;
		if ( ! out.second )
		{
			delete it->second;
			it->second = NULL;
		}

		it->second = itBase->second->getClone( _str_name );
		*_wp_wndProc = it->second;
		return true;
	};

	WindowProcedure* WindowProcedure::get( const std::string& _str_name )
	{
		WindowProcedure* tmp = NULL;
		create( _str_name, &tmp );
		return tmp;
	};

	bool WindowProcedure::setProcedure( const std::string& _str_name )
	{
		WndProcMap::iterator it = m_wndProcMap.find( _str_name );
		if ( it != m_wndProcMap.end() )
		{
			wp_usedWndProc = it->second;
			return true;
		}
		return false;
	};

	std::string WindowProcedure::getProcedureName()
	{
		return wp_usedWndProc->getName();
	};


}
