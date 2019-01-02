#include "src/engine/logger/CLogger.hpp"

#include "src/ext/fmt/chrono.h"

CLogger::TLogBuffer CLogger::m_logBuffer;

std::list< std::unique_ptr< CLogger::CLogTarget > > CLogger::m_logTargets;

void CLogger::Log( e_loglevel logLevel, const std::string &message )
{
	static const std::chrono::high_resolution_clock::time_point first = std::chrono::high_resolution_clock::now();

	const std::chrono::milliseconds	diff = std::chrono::duration_cast< std::chrono::milliseconds >( std::chrono::high_resolution_clock::now() - first );

	m_logBuffer.emplace_back( std::make_unique< logEntry >( diff, logLevel, message ) );

	for( const std::unique_ptr< CLogTarget > &target : m_logTargets )
	{
		target->Log( m_logBuffer.back() );
	}
}

const std::string CLogger::LogLevelToString( e_loglevel level )
{
	switch( level )
	{
		case e_loglevel::eERROR :
			return( "ERROR" );

		case e_loglevel::eWARNING :
			return( "WARNING" );

		case e_loglevel::eINFO :
			return( "INFO" );

		case e_loglevel::eDEBUG :
			return( "DEBUG" );

		default:
			return( "UNKNOWN" );
	}
}

void CLogger::Destroy()
{
	m_logBuffer.clear();
}

const std::string CLogger::logEntry::FormattedTime() const
{
	return( fmt::format( "{:%H:%M:%S}", m_time ) );
}
