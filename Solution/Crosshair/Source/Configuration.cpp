/// Configuration.cpp

#include "Crosshair.hpp"
#include <nlohmann/json.hpp>

bool CConfig::Initialize( )
{
	return LoadConfiguration( );
}

void CConfig::Shutdown( )
{
	SaveConfiguration( );
}

bool CConfig::LoadConfiguration( )
{
	std::string strFileContents { };
	nlohmann::json jsConfig;

	if ( !filesystem.ReadFile( filesystem.FileToDirectory< std::wstring >( wszConfigFilename ), strFileContents ) )
	{
		logging.Log( PREFIX_ERROR, LOCATION_CONFIGURATION, "Error reading configuration file." );
		return false;
	}

	try
	{
		jsConfig = nlohmann::json::parse( strFileContents );
	}
	catch( nlohmann::json::parse_error& )
	{
		logging.Log( PREFIX_ERROR, LOCATION_CONFIGURATION, "Error parsing configuration file." );
	}

	if ( jsConfig[ "Size" ].get< int >( ) != sizeof( config_t ) )
	{
		logging.Log( PREFIX_WARNING, LOCATION_CONFIGURATION, "Outdated configuration file, removing." );
		filesystem.DeleteFile( wszConfigFilename );
		return false;
	}

	const auto pConfig = reinterpret_cast< unsigned char* >( &cfg.Get( ) );
	for ( auto u = 0u; u < sizeof( config_t ); u++ )
		pConfig[ u ] = jsConfig[ "Bytes" ][ u ].get< unsigned char >( );
	return true;
}

void CConfig::SaveConfiguration( )
{
	nlohmann::json jsConfig;
	const auto pConfig = reinterpret_cast< unsigned char* >( &cfg.Get( ) );
	jsConfig[ "Size" ] = int( sizeof( config_t ) );
	for( auto u = 0u; u < sizeof( config_t ); u++ )
		jsConfig[ "Bytes" ][ u ] = pConfig[ u ];
	filesystem.WriteToFile( filesystem.FileToDirectory< std::wstring >( wszConfigFilename ), jsConfig.dump( 4 ) );
}

config_t& CConfig::Get( )
{
	return cfgCurrent;
}
