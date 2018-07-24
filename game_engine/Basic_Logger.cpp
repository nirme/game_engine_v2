#include "Basic_Logger.h"

#pragma warning(disable:4996)


namespace GameEngine
{

	const int LogPriority::MESSAGE_SIZE = 8;
	Logger::LoggingInterfaceMap Logger::lim_loggingInterfaces;
	Logger::LoggerMap Logger::lm_loggers;

	#ifdef THREADED_LOGGING
		mutable Logger::Mutex Logger::m_loggingIntMapMutex;
		mutable Logger::Mutex Logger::m_loggersMapMutex;
	#endif


	const std::string* LogPriority::names()
	{
		static const std::string priority_names[10] =
		{
			"FATAL",
			"ALERT",
			"CRIT",
			"ERROR",
			"WARN",
			"NOTICE",
			"INFO",
			"DEBUG",
			"NOTSET",
			"UNKNOWN" 
		};
		return priority_names;
	};


	const std::string& LogPriority::getPriorityName( int _priority )
	{
		_priority /= 100;
		return names()[ ( ( _priority < 0 ) || ( _priority > 8 ) ) ? 8 : _priority ];
	};


	LogPriority::Value LogPriority::getPriorityValue( const std::string& _priorityName )
	{
		Value value = -1;

		for ( unsigned int i = 0; i < 10; i++ )
		{
			if ( _priorityName == names()[i] )
			{
				value = i * 100;
				break;
			}
		}

		if ( value == -1 )
		{
			if ( _priorityName == "EMERG" )
			{
				value = 0;
			}
			else
			{
				throw std::invalid_argument( std::string("unknown priority name: '") + _priorityName + "'" );
			}
		}

		return value;
	};



	TimeStamp::TimeStamp()
	{
		struct __timeb64 tmpTime;
		_ftime64( &tmpTime );

		i64_miliseconds = tmpTime.time * 1000 + tmpTime.millitm;
	};


	TimeStamp::TimeStamp( __int64 _i64_miliseconds ) : i64_miliseconds( _i64_miliseconds )
	{};


	TimeStamp::TimeStamp( int _seconds, int _i64_miliseconds ) : i64_miliseconds( _seconds * 1000 + _i64_miliseconds )
	{};


	TimeStamp::TimeStamp( __int64 _seconds, int _i64_miliseconds ) : i64_miliseconds( _seconds * 1000 + _i64_miliseconds )
	{};



	LogEvent::LogEvent( const Unicode::UString& _ustr_categoryName, const Unicode::UString& _ustr_message, LogPriority::Value _lpv_priority, bool _b_getThreadInfo ) : ustr_categoryName(_ustr_categoryName), ustr_message(_ustr_message), lpv_priority(_lpv_priority), str_threadName( _b_getThreadInfo ? getThreadId() : "" )
	{};

	LogEvent::LogEvent( const std::string& _str_categoryName, const Unicode::UString& _ustr_message, LogPriority::Value _lpv_priority, bool _b_getThreadInfo ) : ustr_categoryName(_str_categoryName), ustr_message(_ustr_message), lpv_priority(_lpv_priority), str_threadName( _b_getThreadInfo ? getThreadId() : "" )
	{};

	LogEvent::LogEvent( const Unicode::UString& _ustr_categoryName, const std::string& _str_message, LogPriority::Value _lpv_priority, bool _b_getThreadInfo ) : ustr_categoryName(_ustr_categoryName), ustr_message(_str_message), lpv_priority(_lpv_priority), str_threadName( _b_getThreadInfo ? getThreadId() : "" )
	{};

	LogEvent::LogEvent( const std::string& _str_categoryName, const std::string& _str_message, LogPriority::Value _lpv_priority, bool _b_getThreadInfo ) : ustr_categoryName(_str_categoryName), ustr_message(_str_message), lpv_priority(_lpv_priority), str_threadName( _b_getThreadInfo ? getThreadId() : "" )
	{};



	LogFilter::LogFilter() : lf_chainedFilter( NULL )
	{};


	LogFilter::~LogFilter()
	{
		if ( lf_chainedFilter )
		{
			delete lf_chainedFilter;
		}
	};


	void LogFilter::setChainedFilter( LogFilter* _lf_filter )
	{
		if ( lf_chainedFilter != _lf_filter )
		{
			if ( lf_chainedFilter )
			{
				delete lf_chainedFilter;
			}

			lf_chainedFilter = _lf_filter;
		}
	};


	LogFilter* LogFilter::getChainedFilter()
	{
		return lf_chainedFilter;
	};


	LogFilter* LogFilter::getEndOfChain()
	{
		LogFilter* end = this;
		while ( end->getChainedFilter() )
		{
			end = end->getChainedFilter();
		}
		return end;
	};


	void LogFilter::appendChainedFilter( LogFilter* _lf_filter )
	{
		LogFilter* end = getEndOfChain();
		end->setChainedFilter( _lf_filter );
	};


	LogFilter::Decision LogFilter::decide( const LogEvent& _le_event )
	{
		Decision decision = _decide( _le_event );

		if ( ( NEUTRAL == decision ) && getChainedFilter() )
		{
			decision = getChainedFilter()->decide( _le_event );
		}

		return decision;
	};



	Unicode::UString DummyLayout::format( const LogEvent& _le_event )
	{
		return _le_event.ustr_message;
	};



	Unicode::UString BasicLayout::format( const LogEvent& _le_event )
	{
		char tmp[12];
		Unicode::UString us_msg = "";
		us_msg.reserve( 26 + LogPriority::MESSAGE_SIZE + _le_event.ustr_categoryName.size() + _le_event.ustr_message.size() );
		us_msg += __itoa( _le_event.ts_timeStamp.getSeconds64(), &( tmp[0] ), 10 );
		us_msg += " ";
		us_msg += ( LogPriority::getPriorityName( _le_event.lpv_priority ) ).c_str();
		us_msg += " ";
		us_msg += _le_event.ustr_categoryName;
		us_msg += " ";
		us_msg += ": ";
		us_msg += _le_event.ustr_message;
		us_msg += "\n";

        return us_msg.c_str();
	};



	LoggingInterface::LoggingInterface( const std::string& _str_name ) : ReferenceCountingObject(), str_name( _str_name ), lf_filter( NULL )
	{};


	LoggingInterface::~LoggingInterface()
	{
		if ( lf_filter )
			delete lf_filter;
	};


	void LoggingInterface::write( const LogEvent& _le_event )
	{
		if ( ( LogPriority::NOTSET == pr_threshold ) || ( _le_event.lpv_priority <= pr_threshold ) )
		{
			if ( !lf_filter || ( lf_filter->decide( _le_event ) != LogFilter::DENY ) )
			{
				_write( _le_event );
			}
		}
	};


	
	bool LoggingInterface::reopen()
	{
		return true;
	};


	void LoggingInterface::setThreshold( LogPriority::Value _pr_threshold )
	{
		pr_threshold = _pr_threshold;
	};


	LogPriority::Value LoggingInterface::getThreshold()
	{
		return pr_threshold;
	};


	void LoggingInterface::setFilter( LogFilter* _lf_filter )
	{
		if ( lf_filter != _lf_filter )
		{
			if ( lf_filter )
			{
				delete lf_filter;
			}

			lf_filter = _lf_filter;
		}
	};


	LogFilter* LoggingInterface::getFilter()
	{
		return lf_filter;
	};



	LayoutLoggingInterface::LayoutLoggingInterface( const std::string& _str_name ) : LoggingInterface( _str_name ), ll_layout( new BasicLayout() )
	{};


	LayoutLoggingInterface::~LayoutLoggingInterface()
	{
		delete ll_layout;
	};


	bool LayoutLoggingInterface::requiresLayout() const
	{
		return true;
	};


	void LayoutLoggingInterface::setLayout( LogLayout* _ll_layout )
	{
		if ( ll_layout != _ll_layout )
		{
			LogLayout *oldLayout = ll_layout;
			ll_layout = (_ll_layout == NULL) ? new BasicLayout() : _ll_layout;
			delete oldLayout;
		}
	};


	LogLayout& LayoutLoggingInterface::_getLayout()
	{
		return *ll_layout;
	};



	FileLoggingInterface::FileLoggingInterface( const std::string& _str_name, const std::string& _str_fileName, bool _b_binary, bool _b_append ) : LayoutLoggingInterface( _str_name ), str_fileName( _str_fileName ), b_append( _b_append ), b_binary( _b_binary )
	{
		char c_mode[] = "a+tc\0";
		if (!b_append) c_mode[0] = 'w';
		if (b_binary) c_mode[2] = 'b';
		F_file = fopen( str_fileName.c_str(), c_mode );
	};


	FileLoggingInterface::~FileLoggingInterface()
	{
		fclose( F_file );
	};


	bool FileLoggingInterface::reopen()
	{
		char c_mode[] = "a+tc\0";
		if (!b_append) c_mode[0] = 'w';
		if (b_binary) c_mode[2] = 'b';
		if ( F_file )
		{
			F_file = freopen( ( str_fileName.size() ? str_fileName.c_str() : NULL ), c_mode, F_file );
		}
		else
		{
			F_file = fopen( str_fileName.c_str(), c_mode );
		}

		return F_file ? true : false;
	};


	void FileLoggingInterface::close()
	{
		if ( F_file )
		{
			fclose( F_file );
			F_file = NULL;
		}
	};


	void FileLoggingInterface::setAppend( bool _b_append )
	{
		b_append = _b_append;
	};


	bool FileLoggingInterface::getAppend() const
	{
		return b_append;
	};


	void FileLoggingInterface::setBinary( bool _b_binary )
	{
		b_binary = _b_binary;
	};


	bool FileLoggingInterface::getBinary() const
	{
		return b_binary;
	};


	void FileLoggingInterface::_write( const LogEvent& _le_event )
	{
	   Unicode::UString ustr_message( _getLayout().format( _le_event ) );
	   fwrite( reinterpret_cast <const char*> ( ustr_message.c_str() ), ustr_message.size(), 1, F_file );
	   fflush( F_file );
	};



	OstreamLoggingInterface::OstreamLoggingInterface( const std::string& _str_name, std::ostream* _ost_stream ) : LayoutLoggingInterface( _str_name ), ost_stream( _ost_stream )
	{};


	OstreamLoggingInterface::~OstreamLoggingInterface()
	{
		close();
	};


	bool OstreamLoggingInterface::reopen()
	{
		return true;
	};


	void OstreamLoggingInterface::close()
	{};


	void OstreamLoggingInterface::_append( const LogEvent& _le_event )
	{
		Unicode::UString ustr_message( _getLayout().format( _le_event ) );
		ost_stream->write( reinterpret_cast <const char*> ( ustr_message.c_str() ), ustr_message.size() );
		//(*ost_stream) << _getLayout().format( _le_event );
		//if (!_stream->good()) {}
	};



	LoggerStream& eol ( LoggerStream& _os )
	{
		if ( _os.oss_buffer )
		{
			_os.flush();
		}
		return _os;
	};


	LoggerStream& left ( LoggerStream& _os )
	{
		if ( _os.oss_buffer )
		{
			_os.oss_buffer->setf( std::ios::left );
		}
		return _os;
	};



	LoggerStream::LoggerStream( Logger& _l_logger, LogPriority::Value _lpv_priority ) : l_logger(_l_logger), lpv_priority(_lpv_priority), oss_buffer(NULL)
	{};


	LoggerStream::~LoggerStream()
	{ 
		flush();
	};


	void LoggerStream::flush()
	{
		if (oss_buffer)
		{
			getLogger().log(getPriority(), reinterpret_cast<const Unicode::CHAR8*> ( oss_buffer->str().c_str() ));
			delete oss_buffer;
			oss_buffer = NULL;
		}
	};


	LoggerStream& LoggerStream::operator<< ( const char* t )
	{
		if ( getPriority() != LogPriority::NOTSET )
		{
			getBuffer() << t;
		}
		return *this;
	};


	std::streamsize LoggerStream::width(std::streamsize wide )
	{
		return getBuffer().width(wide);
	};


	LoggerStream& LoggerStream::operator<< ( cspf _pf )
	{
		return (*_pf)(*this);
	};



	bool Logger::setLoggingInterface( const std::string& _str_name )
	{
		#ifdef THREADED_LOGGING
			ScopedLock lock( m_loggingIntSetMutex );
		#endif

		LoggingInterfaceSet::iterator its = lis_loggingIntSet.find( &DummyLoggingInterface( _str_name ) );
		if ( its != lis_loggingIntSet.end() )
			return true;

		LoggingInterfaceMap::iterator itm = lim_loggingInterfaces.find( _str_name );
		if ( itm == lim_loggingInterfaces.end() )
			return false;

		lis_loggingIntSet.insert( (*itm).second );
		(*itm).second->addRef();
		return true;
	};


	bool Logger::setLoggingInterface( LoggingInterface* _li_loginterface )
	{
		#ifdef THREADED_LOGGING
			ScopedLock lock( m_loggingIntSetMutex );
		#endif

		LoggingInterfaceMap::iterator itm = lim_loggingInterfaces.find( _li_loginterface->getName() );
		if ( itm != lim_loggingInterfaces.end() )
		{
			if ( (*itm).second == _li_loginterface )
			{
				if ( lis_loggingIntSet.insert( (*itm).second ).second )
					(*itm).second->addRef();
				return true;
			}

			return false;
		}

		addLoggingInterface( _li_loginterface );
		lis_loggingIntSet.insert( _li_loginterface );
		_li_loginterface->addRef();
		return true;
	};


	LoggingInterface* Logger::getLoggingInterface( const std::string& _str_name )
	{
		#ifdef THREADED_LOGGING
			ScopedLock lock( m_loggingIntSetMutex );
		#endif

		LoggingInterfaceMap::iterator it = lim_loggingInterfaces.find( _str_name );
		if ( it != lim_loggingInterfaces.end() )
		{
			(*it).second->addRef();
			return ( (*it).second );
		}
		return NULL;
	};


	Logger::LoggingInterfaceSet Logger::getLoggingInterfaces()
	{
		#ifdef THREADED_LOGGING
			ScopedLock lock( m_loggingIntSetMutex );
		#endif

		return lis_loggingIntSet;
	};


	void Logger::log( LogPriority::Value _lpv_priority, const Unicode::CHAR8* _c8_format, ... )
	{
		if ( isPriorityEnabled( _lpv_priority ) )
		{
			va_list va;
			va_start( va, _c8_format );
			_logUnconditionally( _lpv_priority, _c8_format, va );
			va_end(va);
		}
	};


	void Logger::log( LogPriority::Value _lpv_priority, const Unicode::UString& _ustr_message )
	{
		if ( isPriorityEnabled( _lpv_priority ) )
		{
			_logUnconditionally( _lpv_priority, _ustr_message );
		}
	};


	void Logger::logva( LogPriority::Value _lpv_priority, const Unicode::CHAR8* _c8_format, va_list _va_arguments )
	{
		if ( isPriorityEnabled( _lpv_priority ) )
		{
			_logUnconditionally( _lpv_priority, _c8_format, _va_arguments );
		}
	};


	void Logger::_logUnconditionally( LogPriority::Value _lpv_priority, const Unicode::CHAR8* _c8_format, va_list _va_arguments )
	{
		size_t i_size = 1024;
		char tmpstr[ 1024 ];
		int length = vsnprintf( tmpstr, i_size, reinterpret_cast <const char*> ( _c8_format ), _va_arguments );

		if ( length < 0 )
		{
			i_size = 2048;
			char tmpstr2[ 2048 ];
			length = vsnprintf( tmpstr2, i_size, reinterpret_cast <const char*> ( _c8_format ), _va_arguments );

			if ( length < 0 )
			{
				i_size = Unicode::strlen( _c8_format ) * 2;
				char* tmpstr3 = new char [ i_size ];
				length = vsnprintf( tmpstr2, i_size, reinterpret_cast <const char*> ( _c8_format ), _va_arguments );

				//!!!vsnprintf breaks down with fancy strings - needs some more work here!!!

				_logUnconditionally( _lpv_priority, length < 0 ? "Logged string too long for the system :(" : tmpstr3 );
				delete [] tmpstr3;
			}
			else
				_logUnconditionally( _lpv_priority, tmpstr2 );
		}
		else
			_logUnconditionally( _lpv_priority, tmpstr );
	};


	void Logger::_logUnconditionally( LogPriority::Value _lpv_priority, const Unicode::UString& _ustr_message )
	{
		LogEvent le_event( getName(), _ustr_message, _lpv_priority );
		callLoggingInterfaces( le_event );
	};


	void Logger::callLoggingInterfaces( const LogEvent& _le_event )
	{
		#ifdef THREADED_LOGGING
			ScopedLock lock( m_loggingIntSetMutex );
		#endif

		if ( !lis_loggingIntSet.empty() )
		{
			for ( LoggingInterfaceSet::const_iterator it = lis_loggingIntSet.begin(); it != lis_loggingIntSet.end(); ++it )
			{
				(*it)->write( _le_event );
			}
		}

		if ( getChaining() && getParent() )
		{
			getParent()->callLoggingInterfaces( _le_event );
		}
	};


	LogPriority::Value Logger::getChainedPriority() const
	{
		const Logger* c = this;
		while( c->getPriority() >= LogPriority::NOTSET )
		{
			if ( !c ) return LogPriority::NOTSET;
			c = c->getParent();
		}
		return c->getPriority();
	};


	bool Logger::isPriorityEnabled( LogPriority::Value _lpv_priority ) const
	{
		return( getChainedPriority() >= _lpv_priority );
	};


	LoggerStream Logger::getStream( LogPriority::Value _lpv_priority )
	{
		return LoggerStream( *this, isPriorityEnabled( _lpv_priority ) ? _lpv_priority : LogPriority::NOTSET );
	};


	void Logger::emerg( const Unicode::CHAR8* _c8_format, ... )
	{
		if ( isPriorityEnabled( LogPriority::EMERG ) )
		{
			va_list va_arguments;
			va_start( va_arguments, _c8_format );
			_logUnconditionally( LogPriority::EMERG, _c8_format, va_arguments );
			va_end( va_arguments );
		}
	};


	void Logger::emerg( const Unicode::UString& _ustr_message )
	{
		if ( isPriorityEnabled( LogPriority::EMERG ) )
		{
			_logUnconditionally( LogPriority::EMERG, _ustr_message );
		}
	};


	void Logger::fatal( const Unicode::CHAR8* _c8_format, ... )
	{
		if ( isPriorityEnabled( LogPriority::FATAL ) )
		{
			va_list va_arguments;
			va_start( va_arguments, _c8_format );
			_logUnconditionally( LogPriority::FATAL, _c8_format, va_arguments );
			va_end( va_arguments );
		}
	};


	void Logger::fatal( const Unicode::UString& _ustr_message )
	{
		if ( isPriorityEnabled( LogPriority::FATAL ) )
		{
			_logUnconditionally( LogPriority::FATAL, _ustr_message );
		}
	};


	void Logger::alert( const Unicode::CHAR8* _c8_format, ... )
	{
		if ( isPriorityEnabled( LogPriority::ALERT ) )
		{
			va_list va_arguments;
			va_start( va_arguments, _c8_format );
			_logUnconditionally( LogPriority::ALERT, _c8_format, va_arguments );
			va_end( va_arguments );
		}
	};


	void Logger::alert( const Unicode::UString& _ustr_message )
	{
		if ( isPriorityEnabled( LogPriority::ALERT ) )
		{
			_logUnconditionally( LogPriority::ALERT, _ustr_message );
		}
	};


	void Logger::crit( const Unicode::CHAR8* _c8_format, ... )
	{
		if ( isPriorityEnabled( LogPriority::CRIT ) )
		{
			va_list va_arguments;
			va_start( va_arguments, _c8_format );
			_logUnconditionally( LogPriority::CRIT, _c8_format, va_arguments );
			va_end( va_arguments );
		}
	};


	void Logger::crit( const Unicode::UString& _ustr_message )
	{
		if ( isPriorityEnabled( LogPriority::CRIT ) )
		{
			_logUnconditionally( LogPriority::CRIT, _ustr_message );
		}
	};


	void Logger::error( const Unicode::CHAR8* _c8_format, ... )
	{
		if ( isPriorityEnabled( LogPriority::ERROR ) )
		{
			va_list va_arguments;
			va_start( va_arguments, _c8_format );
			_logUnconditionally( LogPriority::ERROR, _c8_format, va_arguments );
			va_end( va_arguments );
		}
	};


	void Logger::error( const Unicode::UString& _ustr_message )
	{
		if ( isPriorityEnabled( LogPriority::ERROR ) )
		{
			_logUnconditionally( LogPriority::ERROR, _ustr_message );
		}
	};


	void Logger::warn( const Unicode::CHAR8* _c8_format, ... )
	{
		if ( isPriorityEnabled( LogPriority::WARN ) )
		{
			va_list va_arguments;
			va_start( va_arguments, _c8_format );
			_logUnconditionally( LogPriority::WARN, _c8_format, va_arguments );
			va_end( va_arguments );
		}
	};


	void Logger::warn( const Unicode::UString& _ustr_message )
	{
		if ( isPriorityEnabled( LogPriority::WARN ) )
		{
			_logUnconditionally( LogPriority::WARN, _ustr_message );
		}
	};


	void Logger::notice( const Unicode::CHAR8* _c8_format, ... )
	{
		if ( isPriorityEnabled( LogPriority::NOTICE ) )
		{
			va_list va_arguments;
			va_start( va_arguments, _c8_format );
			_logUnconditionally( LogPriority::NOTICE, _c8_format, va_arguments );
			va_end( va_arguments );
		}
	};


	void Logger::notice( const Unicode::UString& _ustr_message )
	{
		if ( isPriorityEnabled( LogPriority::NOTICE ) )
		{
			_logUnconditionally( LogPriority::NOTICE, _ustr_message );
		}
	};


	void Logger::info( const Unicode::CHAR8* _c8_format, ... )
	{
		if ( isPriorityEnabled( LogPriority::INFO ) )
		{
			va_list va_arguments;
			va_start( va_arguments, _c8_format );
			_logUnconditionally( LogPriority::INFO, _c8_format, va_arguments );
			va_end( va_arguments );
		}
	};


	void Logger::info( const Unicode::UString& _ustr_message )
	{
		if ( isPriorityEnabled( LogPriority::INFO ) )
		{
			_logUnconditionally( LogPriority::INFO, _ustr_message );
		}
	};


	void Logger::debug( const Unicode::CHAR8* _c8_format, ... )
	{
		if ( isPriorityEnabled( LogPriority::DEBUG ) )
		{
			va_list va_arguments;
			va_start( va_arguments, _c8_format );
			_logUnconditionally( LogPriority::DEBUG, _c8_format, va_arguments );
			va_end( va_arguments );
		}
	};


	void Logger::debug( const Unicode::UString& _ustr_message )
	{
		if ( isPriorityEnabled( LogPriority::DEBUG ) )
		{
			_logUnconditionally( LogPriority::DEBUG, _ustr_message );
		}
	};


	void Logger::removeAllAppenders()
	{
		#ifdef THREADED_LOGGING
			ScopedLock lock( m_loggingIntSetMutex );
		#endif

		for ( LoggingInterfaceSet::iterator it = lis_loggingIntSet.begin(); it != lis_loggingIntSet.end(); ++it )
		{
			(*it)->release();
		}
		lis_loggingIntSet.clear();
	};



	Logger::Logger( const std::string& _str_name, LogPriority::Value _lpv_priority, Logger* _l_parent, bool _b_chained ) : 
		str_name( _str_name ), 
		l_parent( _l_parent ), 
		lpv_priority( _lpv_priority ), 
		b_chained( _b_chained )
	{};


	Logger::~Logger()
	{
		removeAllAppenders();
	};



	std::pair<bool,Logger&> Logger::addLogger( const std::string& _str_name, LogPriority::Value _lpv_priority, Logger* _l_parent, bool _b_chained )
	{
		#ifdef THREADED_LOGGING
			ScopedLock lock( m_loggersMapMutex );
		#endif

		LoggerMap::iterator it = lm_loggers.find( _str_name );
		if ( it != lm_loggers.end() )
			return std::pair<bool,Logger&>( false, *((*it).second) );

		Logger* l_new = new Logger( _str_name, _lpv_priority, _l_parent, _b_chained );
		lm_loggers.insert( LoggerMap::value_type( _str_name, l_new ) );
		return std::pair<bool,Logger&>( true, *l_new );
	};


	std::pair<bool,Logger&> Logger::addLogger( Logger* _l_logger )
	{
		#ifdef THREADED_LOGGING
			ScopedLock lock( m_loggersMapMutex );
		#endif

		LoggerMap::iterator it = lm_loggers.find( _l_logger->getName() );
		if ( it != lm_loggers.end() )
			return std::pair<bool,Logger&>( false, *((*it).second) );

		lm_loggers.insert( LoggerMap::value_type( _l_logger->getName(), _l_logger ) );

		return std::pair<bool,Logger&>( true, *_l_logger );
	};


	Logger& Logger::getLogger( const std::string& _str_name )
	{
		#ifdef THREADED_LOGGING
			ScopedLock lock( m_loggersMapMutex );
		#endif

		LoggerMap::iterator it = lm_loggers.find( _str_name );
		if ( it != lm_loggers.end() )
			return ( *((*it).second) );

		return addLogger( _str_name, LogPriority::NOTSET, NULL, true ).second;
	};


	bool Logger::removeLogger( const std::string& _str_name )
	{
		#ifdef THREADED_LOGGING
			ScopedLock lock( m_loggersMapMutex );
		#endif

		LoggerMap::iterator it = lm_loggers.find( _str_name );
		if ( it != lm_loggers.end() )
		{
			if ( (*it).second ) delete (*it).second;
			lm_loggers.erase( it );
			return true;
		}
		return false;
	};


	bool Logger::addLoggingInterface( LoggingInterface* _li_loggingInterface )
	{
		#ifdef THREADED_LOGGING
			ScopedLock lock( m_loggingIntMapMutex );
		#endif

		LoggingInterfaceMap::iterator it = lim_loggingInterfaces.find( _li_loggingInterface->getName() );
		if ( it != lim_loggingInterfaces.end() )
			return false;

		_li_loggingInterface->addRef();
		lim_loggingInterfaces.insert( LoggingInterfaceMap::value_type( _li_loggingInterface->getName(), _li_loggingInterface ) );
		return true;
	};


	LoggingInterface* Logger::retrieveLoggingInterface( const std::string& _str_name )
	{
		#ifdef THREADED_LOGGING
			ScopedLock lock( m_loggingIntMapMutex );
		#endif

		LoggingInterfaceMap::iterator it = lim_loggingInterfaces.find( _str_name );
		if ( it != lim_loggingInterfaces.end() )
		{
			(*it).second->addRef();
			return ( (*it).second );
		}
		return NULL;
	};


	void Logger::removeLoggingInterface( const std::string& _str_name )
	{
		#ifdef THREADED_LOGGING
			ScopedLock lock( m_loggingIntMapMutex );
		#endif

		LoggingInterfaceMap::iterator it = lim_loggingInterfaces.find( _str_name );
		if ( it != lim_loggingInterfaces.end() )
		{
			lim_loggingInterfaces.erase( it );
			(*it).second->release();
		}
	};


	void Logger::shutdown()
	{
		#ifdef THREADED_LOGGING
			ScopedLock lockLoggers( m_loggersMapMutex );
		#endif

		for ( LoggerMap::iterator it = lm_loggers.begin(); it != lm_loggers.end(); ++it )
		{
			if ( (*it).second ) delete (*it).second;
			(*it).second = NULL;
		}
		lm_loggers.clear();

		{
			#ifdef THREADED_LOGGING
				ScopedLock lockLogInts( m_loggingIntMapMutex );
			#endif

			for ( LoggingInterfaceMap::iterator it = lim_loggingInterfaces.begin(); it != lim_loggingInterfaces.end(); ++it )
			{
				(*it).second->release();
				(*it).second = NULL;
			}
			lim_loggingInterfaces.clear();
		}
	};

}

#pragma warning(default:4996)
