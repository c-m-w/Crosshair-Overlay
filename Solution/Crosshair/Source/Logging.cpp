/// Logging.cpp

#include "Crosshair.hpp"

std::string CLogging::CurrentTimeString( )
{
	char szBuffer[ 32 ];
	auto tmNow = time_t( Utilities::GetMoment( ) );
	time( &tmNow );
	const auto tmTime = localtime( &tmNow );
	strftime( szBuffer, 80, "%B %e, 20%g | %H:%M:%S", tmTime );
	return szBuffer;
}

bool CLogging::CreateLogFile( )
{
	if ( filesystem.FileExists( wstrPath ) )
		return true;

	filesystem.WriteToFile( wstrPath, "[Log File begin]\n" );
	filesystem.LoadFiles( );
	return filesystem.FileExists( wstrPath );
}

void CLogging::LogToConsole( EColor clrColor, const std::string& strText )
{
#if defined _DEBUG
	static const auto hConsole = GetStdHandle( STD_OUTPUT_HANDLE );
	const auto uDateEnd = strText.find_first_of( ']' );

	SetConsoleTextAttribute( hConsole, uConsoleColors[ COLOR_GRAY ] );
	printf( strText.substr( 0, uDateEnd + 1 ).c_str( ) );
	SetConsoleTextAttribute( hConsole, uConsoleColors[ int( clrColor ) ] );
	printf( strText.substr( uDateEnd + 1 ).c_str( ) );
#endif
}

void CLogging::LogToConsole( EPrefix pPrefix, const std::string& strText )
{
	constexpr EColor clrCorrespondingColors[ PREFIX_MAX ]
	{
		COLOR_GRAY,		// PREFIX_INFORMATION
		COLOR_GREEN,	// PREFIX_SUCCESS
		COLOR_ORANGE,	// PREFIX_WARNING
		COLOR_RED		// PREFIX_ERROR
	};
	LogToConsole( clrCorrespondingColors[ pPrefix ], strText );
}

CLogging::~CLogging( )
{
	if ( Initialized( ) )
		CLogging::Shutdown( );
}

bool CLogging::Initialize( )
{
	wstrPath = filesystem.GetDirectory( ) + wszLogName;

	if ( !CreateLogFile( ) )
		return false;

	const auto strBuffer = ssLogBuffer.str( );
	ssLogBuffer.str( std::string( ) );
	ssLogBuffer << "----- Log Start [" << CurrentTimeString( ) << "] -----" << std::endl << strBuffer;
	LogToConsole( COLOR_GRAY, ssLogBuffer.str( ) );
	return bInitialized = true;
}

void CLogging::Shutdown( )
{
	std::stringstream ssCurrent;
	ssCurrent << "----- Log End [" << CurrentTimeString( ) << "] -----" << std::endl;
	ssLogBuffer << ssCurrent.str( );
	LogToConsole( COLOR_GRAY, ssCurrent.str( ) );
	WriteToFile( );
	bInitialized = false;
}

void __cdecl CLogging::Log( EPrefix pPrefix, ELocation lLocation, const char* szFormat, ... )
{
	constexpr auto MAX_LOG_LENGTH = std::size_t( 1024 );
	va_list vaArgs;
	char chBuffer[ MAX_LOG_LENGTH ];
	std::stringstream ssCurrent;

	va_start( vaArgs, szFormat );
	_vsnprintf_s( chBuffer, MAX_LOG_LENGTH, szFormat, vaArgs );
	va_end( vaArgs );
	ssCurrent << "[" << CurrentTimeString( ) << "]\t" << szPrefixText[ pPrefix ] << "\t" << szPlaceText[ lLocation ] << "\t" << chBuffer << std::endl;
	if( Initialized( ) )
		LogToConsole( pPrefix, ssCurrent.str( ) );
	ssLogBuffer << ssCurrent.str( );
	if ( pPrefix == PREFIX_ERROR )
		bShutdown = true;
}

void __stdcall CLogging::WriteToFile( )
{
	if( !Initialized( ) )
	{
		Log( PREFIX_WARNING, LOCATION_LOGGING, "WriteToFile() has been called without initializing!" );
		return;
	}
	filesystem.AddToFile( wstrPath, ssLogBuffer.str( ) );
	ssLogBuffer.str( std::string( ) );
}

CLogging logging;
