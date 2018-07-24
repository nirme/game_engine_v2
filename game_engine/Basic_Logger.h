#pragma once

#ifndef _LOGGER_V3
#define _LOGGER_V3

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <set>
//#include <varargs.h>
#include <cstdarg>

//required for time reading
#include <sys/types.h>
#include <sys/timeb.h>


//#include "Assoc_Vector.h"
#include "Core_Defines.h"
#include "Base_Exception.h"
#include "Reference_Counting_Object.h"
#include "Unicode.h"
#include "itoa.h"

#include "boost/container/flat_map.hpp"


//#include <boost/detail/endian.hpp>
//normal//BOOST_BIG_ENDIAN
//backward//BOOST_LITTLE_ENDIAN


#ifdef THREADED_LOGGING

	#include <boost/thread/thread.hpp>
	#include <boost/thread/mutex.hpp>
	#include <boost/thread/tss.hpp>

#endif //THREADED_LOGGING



#ifdef ERROR
#undef ERROR
#endif //ERROR


namespace GameEngine
{

	class LogPriority;
	class TimeStamp;
	struct LogEvent;
	class LogFilter;
	class LogLayout;
		class DummyLayout;
		class BasicLayout;
	class LoggingInterface;
		class LayoutLoggingInterface;
			class FileLoggingInterface;
			class OstreamLoggingInterface;
	class Logger;
	class LoggerStream;



	inline std::string getThreadId()
	{
		#ifdef THREADED_LOGGING
			return boost::lexical_cast<std::string>( boost::this_thread::get_id() );
		#else
			return "";
		#endif
	};


	class LogPriority
	{
	private:
		static const std::string* LogPriority::names();

	public:

		static const int MESSAGE_SIZE;

		typedef enum PriorityLevel
		{
			EMERG  = 0, 
			FATAL  = 0,
			ALERT  = 100,
			CRIT   = 200,
			ERROR  = 300, 
			WARN   = 400,
			NOTICE = 500,
			INFO   = 600,
			DEBUG  = 700,
			NOTSET = 800
		};

		typedef int Value;

		static const std::string& getPriorityName( int _priority );
		static Value getPriorityValue( const std::string& _priorityName );

	};



	class TimeStamp
	{
	protected:
		__int64 i64_miliseconds;

	public:

		TimeStamp();
		TimeStamp( __int64 _i64_miliseconds );
		TimeStamp( int _seconds, int _i64_miliseconds );
		TimeStamp( __int64 _seconds, int _i64_miliseconds );

		inline int getSeconds() const { return (int) ( i64_miliseconds / 1000 ); };
		inline __int64 getSeconds64() const { return i64_miliseconds / 1000; };
		inline int getMiliseconds() const { return i64_miliseconds % 1000; };
	};



	struct LogEvent
	{
	public:
		const Unicode::UString ustr_categoryName;
		const Unicode::UString ustr_message;

		LogPriority::Value lpv_priority;

		const std::string str_threadName;

		//milisecond from (1/1/1970 00:00:00 UTC)
		TimeStamp ts_timeStamp;
		
		LogEvent( const Unicode::UString& _ustr_categoryName, const Unicode::UString& _ustr_message, LogPriority::Value _lpv_priority, bool _b_getThreadInfo = false );
		LogEvent( const std::string& _str_categoryName, const Unicode::UString& _ustr_message, LogPriority::Value _lpv_priority, bool _b_getThreadInfo = false );
		LogEvent( const Unicode::UString& _ustr_categoryName, const std::string& _str_message, LogPriority::Value _lpv_priority, bool _b_getThreadInfo = false );
		LogEvent( const std::string& _str_categoryName, const std::string& _str_message, LogPriority::Value _lpv_priority, bool _b_getThreadInfo = false );
	};



	class LogFilter
	{
	private:
		LogFilter* lf_chainedFilter;

	public:

		typedef enum Decision
		{
			DENY = -1,
			NEUTRAL = 0,
			ACCEPT = 1
		};


		LogFilter();
		virtual ~LogFilter();

		virtual void setChainedFilter( LogFilter* _lf_filter );
		virtual LogFilter* getChainedFilter();
		virtual LogFilter* getEndOfChain();
		virtual void appendChainedFilter( LogFilter* _lf_filter );

		virtual Decision decide( const LogEvent& _le_event );

	protected:
		virtual Decision _decide( const LogEvent& _le_event ) = 0;

	};


	class LogLayout
	{
	public:
		virtual ~LogLayout() {};
		virtual Unicode::UString format( const LogEvent& _le_event ) = 0;
	};

	class DummyLayout : public LogLayout
	{
	public:
		virtual Unicode::UString format( const LogEvent& _le_event );
	};

	class BasicLayout : public LogLayout
	{
	public:
		virtual Unicode::UString format( const LogEvent& _le_event );
	};


	class LoggingInterface : public ReferenceCountingObject
	{
	private:
		std::string str_name;
		LogPriority::Value pr_threshold;
		LogFilter* lf_filter;

	public:
		LoggingInterface( const std::string& _str_name );
		virtual ~LoggingInterface();

		virtual void write( const LogEvent& _le_event );

		virtual bool reopen();
		virtual void close() = 0;
		virtual bool requiresLayout() const = 0;
		virtual void setLayout( LogLayout* _ll_layout ) = 0;
		inline const std::string& getName() const { return str_name; };

		virtual void setThreshold( LogPriority::Value _pr_threshold );
		virtual LogPriority::Value getThreshold();

		virtual void setFilter( LogFilter* _lf_filter );
		virtual LogFilter* getFilter();


	protected:
		virtual void _write( const LogEvent& _le_event ) = 0;

	};



	class DummyLoggingInterface : public LoggingInterface
	{
	public:
		DummyLoggingInterface( const std::string& _str_name ) : LoggingInterface( _str_name ) {};
		void close() {};
		bool requiresLayout() const { return false; };
		void setLayout( LogLayout* _ll_layout ) {};

	protected:
		void _write( const LogEvent& _le_event ) {};
	};



	class LayoutLoggingInterface : public LoggingInterface
	{
	private:
		LogLayout* ll_layout;

	public:

		LayoutLoggingInterface( const std::string& _str_name );
		virtual ~LayoutLoggingInterface();

		bool requiresLayout() const;
		void setLayout( LogLayout* _ll_layout = NULL );

	protected:
		LogLayout& _getLayout();

	};



	class FileLoggingInterface : public LayoutLoggingInterface
	{
	protected:

		const std::string str_fileName;
		FILE* F_file;
		bool b_append;
		bool b_binary;

	//  +--------+-------+--------+
	//  | MODE   | write | append |
	//  +--------+-------+--------+
	//  | binary | w+bc  | a+bc   |
	//  +--------+-------+--------+
	//  | text   | w+tc  | a+tc   |
	//  +--------+-------+--------+

	public:


		FileLoggingInterface( const std::string& _str_name, const std::string& _str_fileName, bool _b_binary = false, bool _b_append = true );
		virtual ~FileLoggingInterface();

		virtual bool reopen();
		virtual void close();

		virtual void setAppend( bool _b_append );
		virtual bool getAppend() const;
		virtual void setBinary( bool _b_binary );
		virtual bool getBinary() const;


	protected:

		virtual void _write( const LogEvent& _le_event );
	};



	class OstreamLoggingInterface : public LayoutLoggingInterface
	{
	protected:
		std::ostream* ost_stream;

	public:
		OstreamLoggingInterface( const std::string& _str_name, std::ostream* _ost_stream );
		virtual ~OstreamLoggingInterface();

		virtual bool reopen();
		virtual void close();

	protected:
		virtual void _append( const LogEvent& _le_event );
	};



	LoggerStream& eol ( LoggerStream& _os );
	LoggerStream& left ( LoggerStream& _os );


	class LoggerStream
	{
		friend LoggerStream& eol (LoggerStream& _os);
		friend LoggerStream& left (LoggerStream& _os);

	private:
		Logger& l_logger;
		LogPriority::Value lpv_priority;
		std::ostringstream* oss_buffer;

		inline std::ostringstream& getBuffer()
		{
			if ( !oss_buffer )
			{
				if ( !( oss_buffer = new std::ostringstream ) )
				{
					// XXX help help help
				}
			}
			return *oss_buffer;
		};


	public:
		typedef LoggerStream& (*cspf) (LoggerStream&);


		LoggerStream( Logger& _l_logger, LogPriority::Value _lpv_priority );
		~LoggerStream();

		inline Logger& getLogger() const { return l_logger; };
		inline LogPriority::Value getPriority() const throw() { return lpv_priority; };

		void flush();

		template<typename T> 
		LoggerStream& operator<< ( const T& t )
		{
			if ( getPriority() != LogPriority::NOTSET )
			{
				getBuffer() << t;
			}
			return *this;
		};

		LoggerStream& operator<< ( const char* t );

		template<typename T> 
		LoggerStream& operator<< ( const std::string& t )
		{
			if ( getPriority() != LogPriority::NOTSET )
			{
				getBuffer() << t;
			}
			return *this;
		};

		template<typename T> 
		LoggerStream& operator<< ( const Unicode::Codepoint& t )
		{
			if ( getPriority() != LogPriority::NOTSET )
			{
				getBuffer() << t;
			}
			return *this;
		};

		template<typename T> 
		LoggerStream& operator<< ( const Unicode::UString& t )
		{
			if ( getPriority() != LogPriority::NOTSET )
			{
				getBuffer() << t;
			}
			return *this;
		};

		std::streamsize width(std::streamsize wide );

		LoggerStream& LoggerStream::operator<< ( cspf _pf );

	};



	// pair template specialization for reference to logger as std::pair in cpp before c11 cannot hold values by reference
	template<class _Ty1> 
	struct std::pair< _Ty1, GameEngine::Logger& >
	{
		_Ty1 first; 
		GameEngine::Logger& second;
		pair( const _Ty1& _Val1, GameEngine::Logger& _Val2 ) : first(_Val1), second(_Val2) {};

		template<class _Other1>
		pair( const pair<_Other1, GameEngine::Logger&> _Right ) : first( _Right.first ), second( _Right.second ) {};
	};



	class Logger
	{
	private:
		const std::string str_name;
		Logger* l_parent;
		LogPriority::Value lpv_priority;
		bool b_chained;

	public:
		inline const std::string& getName() const { return str_name; };

	private:
		template< class _Tval > struct NameCmp
		{
			bool operator() ( const _Tval* lhs, const _Tval* rhs ) const { return lhs->getName() < rhs->getName(); };
			bool operator() ( const _Tval& lhs, const _Tval& rhs ) const { return lhs.getName()  < rhs.getName();  };
		};

		typedef std::set< LoggingInterface*, NameCmp<LoggingInterface> > LoggingInterfaceSet;

	private:
		#ifdef THREADED_LOGGING
			typedef boost::mutex Mutex;
			typedef boost::mutex::scoped_lock ScopedLock;
			mutable Mutex m_loggingIntSetMutex;
		#endif

		LoggingInterfaceSet lis_loggingIntSet;

	public:
		inline virtual LogPriority::Value getPriority() const { return lpv_priority; };
		inline virtual void setPriority( LogPriority::Value _lpv_priority ) { _lpv_priority = lpv_priority; };

		inline virtual bool getChaining() const { return b_chained; };
		inline virtual void setChaining( bool _b_chained ) { b_chained = _b_chained; };

		inline virtual Logger* getParent() { return l_parent; };
		inline virtual const Logger* getParent() const { return l_parent; };


		virtual bool setLoggingInterface( const std::string& _str_name );
		virtual bool setLoggingInterface( LoggingInterface* _li_loginterface );
		virtual LoggingInterface* getLoggingInterface( const std::string& _str_name );
		virtual LoggingInterfaceSet getLoggingInterfaces();

		virtual void log( LogPriority::Value _lpv_priority, const Unicode::CHAR8* _c8_format, ... );
		virtual void log( LogPriority::Value _lpv_priority, const Unicode::UString& _ustr_message );
		virtual void logva( LogPriority::Value _lpv_priority, const Unicode::CHAR8* _c8_format, va_list _va_arguments );

	protected:
		virtual void _logUnconditionally( LogPriority::Value _lpv_priority, const Unicode::CHAR8* _c8_format, va_list _va_arguments );
		virtual void _logUnconditionally( LogPriority::Value _lpv_priority, const Unicode::UString& _ustr_message );

	public:
		virtual void callLoggingInterfaces( const LogEvent& _le_event );

		LogPriority::Value getChainedPriority() const;
		bool isPriorityEnabled( LogPriority::Value _lpv_priority ) const;
		virtual LoggerStream getStream( LogPriority::Value _lpv_priority );

		void emerg( const Unicode::CHAR8* _c8_format, ... );
		void emerg( const Unicode::UString& _ustr_message );
		inline LoggerStream emerg() { return getStream( LogPriority::EMERG ); };

		void fatal( const Unicode::CHAR8* _c8_format, ... );
		void fatal( const Unicode::UString& _ustr_message );
		inline LoggerStream fatal() { return getStream( LogPriority::FATAL ); };

		void alert( const Unicode::CHAR8* _c8_format, ... );
		void alert( const Unicode::UString& _ustr_message );
		inline LoggerStream alert() { return getStream( LogPriority::ALERT ); };

		void crit( const Unicode::CHAR8* _c8_format, ... );
		void crit( const Unicode::UString& _ustr_message );
		inline LoggerStream crit() { return getStream( LogPriority::CRIT ); };

		void error( const Unicode::CHAR8* _c8_format, ... );
		void error( const Unicode::UString& _ustr_message );
		inline LoggerStream error() { return getStream( LogPriority::ERROR ); };

		void warn( const Unicode::CHAR8* _c8_format, ... );
		void warn( const Unicode::UString& _ustr_message );
		inline LoggerStream warn() { return getStream( LogPriority::WARN ); };

		void notice( const Unicode::CHAR8* _c8_format, ... );
		void notice( const Unicode::UString& _ustr_message );
		inline LoggerStream notice() { return getStream( LogPriority::NOTICE ); };

		void info( const Unicode::CHAR8* _c8_format, ... );
		void info( const Unicode::UString& _ustr_message );
		inline LoggerStream info() { return getStream( LogPriority::INFO ); };

		void debug( const Unicode::CHAR8* _c8_format, ... );
		void debug( const Unicode::UString& _ustr_message );
		inline LoggerStream debug() { return getStream( LogPriority::DEBUG ); };

		inline LoggerStream operator<< ( LogPriority::Value _lpv_priority )
		{
			return getStream( _lpv_priority );
		};

		void removeAllAppenders();

	protected:
		Logger( const std::string& _str_name, LogPriority::Value _lpv_priority, Logger* _l_parent, bool _b_chained );

	private:
		virtual ~Logger();
        Logger(const Logger& other);
        Logger& operator=(const Logger& other);

	private:
		typedef boost::container::flat_map< std::string, LoggingInterface* >	LoggingInterfaceMap;
		typedef boost::container::flat_map< std::string, Logger* >				LoggerMap;


		#ifdef THREADED_LOGGING
			static mutable Mutex m_loggingIntMapMutex;
			static mutable Mutex m_loggersMapMutex;
		#endif

		static LoggingInterfaceMap lim_loggingInterfaces;
		static LoggerMap lm_loggers;


	public:


		static std::pair<bool,Logger&> addLogger( const std::string& _str_name, LogPriority::Value _lpv_priority, Logger* _l_parent = NULL, bool _b_chained = true );
		static std::pair<bool,Logger&> addLogger( Logger* _l_logger );
		static Logger& getLogger( const std::string& _str_name );
		static bool removeLogger( const std::string& _str_name );
		static bool addLoggingInterface( LoggingInterface* _li_loggingInterface );
		static LoggingInterface* retrieveLoggingInterface( const std::string& _str_name );
		static void removeLoggingInterface( const std::string& _str_name );

		static void shutdown();

	};

};

#endif //_BASIC_LOGGER_V3
