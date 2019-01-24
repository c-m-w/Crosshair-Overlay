/// Logging.hpp

#pragma once

/** \brief Prefixes for messages to log. */
enum EPrefix
{
	/** \brief Logged message is informative of something. */
	PREFIX_INFORMATION,
	/** \brief Logged message is a success. */
	PREFIX_SUCCESS,
	/** \brief Logged message is a warning. */
	PREFIX_WARNING,
	/** \brief Logged message is an error and program execution must not continue.\n
			   This is only to be used when something has happened and the program cannot continue execution. */
	PREFIX_ERROR,
	PREFIX_MAX
};

/** \brief Where the logged message is from. */
enum ELocation
{
	LOCATION_MAIN,
	LOCATION_CONFIGURATION,
	LOCATION_DRAWING,
	LOCATION_FILESYSTEM,
	LOCATION_LOGGING,
	LOCATION_WINDOW,
	LOCATION_MAX
};

/** \brief Colors for logged messages in console. */
enum EColor
{
	COLOR_GRAY,
	COLOR_GREEN,
	COLOR_ORANGE,
	COLOR_RED,
	COLOR_MAX
};

class CLogging: public IBaseInterface
{
	const wchar_t* wszLogName = L"crosshair.log";
	const char* szPrefixText[ PREFIX_MAX ]
	{
		"[INFO]\t",
		"[SUCCESS]",
		"[WARNING]",
		"[ERROR]\t"
	};
	const char* szPlaceText[ LOCATION_MAX ]
	{
		"[MAIN]\t",
		"[CONFIG]",
		"[DRAWING]",
		"[FILES]\t",
		"[LOGGING]",
		"[WINDOW]"
	};
	const unsigned uConsoleColors[ COLOR_MAX ]
	{
		FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED,
		FOREGROUND_INTENSITY | FOREGROUND_GREEN,
		FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_RED,
		FOREGROUND_INTENSITY | FOREGROUND_RED
	};
	std::wstring wstrPath;
	std::stringstream ssLogBuffer;

	std::string CurrentTimeString( );
	bool CreateLogFile( );
	void LogToConsole( EColor clrColor, const std::string& strText );
	void LogToConsole( EPrefix pPrefix, const std::string& strText ); 

public:
	CLogging( ) = default;
	~CLogging( );

	bool __stdcall Initialize( ) override;
	void __stdcall Shutdown( ) override;
	void __cdecl Log( EPrefix pPrefix, ELocation lLocation, const char* szFormat, ... );
	void __stdcall WriteToFile( );
} extern logging;
