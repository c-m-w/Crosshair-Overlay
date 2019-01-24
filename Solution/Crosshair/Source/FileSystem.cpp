/// FileSystem.cpp

#include "Crosshair.hpp"

bool CFileSystem::OpenFile( std::wstring wstrFile, FILE*& pFile, bool bWriteMode, int* pSize /*= nullptr*/ )
{
	std::string strFile;
	strFile.resize( wstrFile.length( ) );
	wcstombs( &strFile[ 0 ], &wstrFile[ 0 ], wstrFile.size( ) );
	if ( nullptr == ( pFile = fopen( strFile.c_str( ), bWriteMode ? "wb" : "rb" ) ) )
	{
		logging.Log( PREFIX_ERROR, LOCATION_FILESYSTEM, "Failed to open file %s.", strFile.c_str( ) );
		return false;
	}
	logging.Log( PREFIX_SUCCESS, LOCATION_FILESYSTEM, "Opened file %s in %s mode.", strFile.c_str( ), bWriteMode ? "write" : "read" );

	if( pSize != nullptr )
	{
		fseek( pFile, 0, SEEK_END );
		*pSize = ftell( pFile );
		rewind( pFile );
	}
	return true;
}

bool CFileSystem::Initialize( )
{
	wstrCurrentDirectory.resize( MAX_PATH );
	GetModuleFileName( nullptr, &wstrCurrentDirectory[ 0 ], MAX_PATH );
	wstrCurrentDirectory = wstrCurrentDirectory.substr( 0, wstrCurrentDirectory.find_last_of( '\\' ) + 1 );
	LoadFiles( );

	std::string strDirectory;
	strDirectory.resize( wstrCurrentDirectory.length( ) );
	wcstombs( &strDirectory[ 0 ], &wstrCurrentDirectory[ 0 ], wstrCurrentDirectory.length( ) );
	logging.Log( PREFIX_INFORMATION, LOCATION_FILESYSTEM, "Found %u files in directory %s.", vecFiles.size( ), strDirectory.c_str( ) );
	return ( bInitialized = !vecFiles.empty( ) );
}

void CFileSystem::Shutdown( )
{
	
}

void CFileSystem::LoadFiles( )
{
	vecFiles.clear( );
	for ( auto& file : std::filesystem::directory_iterator( wstrCurrentDirectory.c_str( ) ) )
	{
		if ( !file.is_regular_file( ) )
			continue;
		vecFiles.emplace_back( file.path( ).wstring( ).substr( wstrCurrentDirectory.length( ) ) );
	}
}

bool CFileSystem::FileExists( const std::wstring& wstrFile )
{
	if ( wstrFile.empty( ) )
		return false;
	const auto wstrFinalFile = wstrFile.find( '\\' ) != std::string::npos ? wstrFile.substr( wstrFile.find_last_of( '\\' ) + 1 ) : wstrFile;

	for ( auto& file : vecFiles )
		if ( file == wstrFinalFile )
			return true;

	return false;
}

bool CFileSystem::FileExists( const std::string& strFile )
{
	std::wstring wstrFile;
	wstrFile.resize( strFile.length( ) );
	mbstowcs( &wstrFile[ 0 ], &strFile[ 0 ], strFile.length( ) );
	return FileExists( wstrFile );
}

void CFileSystem::WriteToFile( const std::wstring& wstrFile, const std::string& strContents )
{
	FILE* pFile = nullptr;
	if ( !OpenFile( wstrFile, pFile, true ) )
		return;
	fwrite( strContents.c_str( ), sizeof( char ), strContents.size( ), pFile );
	fclose( pFile );
}

void CFileSystem::AddToFile( const std::wstring& wstrFile, const std::string& strContents )
{
	std::string strFileContents { };
	if ( !ReadFile( wstrFile, strFileContents ) )
		return;
	WriteToFile( wstrFile, strFileContents + strContents );
}

bool CFileSystem::ReadFile( const std::wstring& wstrFile, std::string& strContents )
{
	FILE* pFile = nullptr;
	auto iSize = 0;
	if ( !OpenFile( wstrFile, pFile, false, &iSize ) )
		return false;
	strContents.resize( iSize );
	fread( &strContents[ 0 ], sizeof( char ), iSize, pFile );
	fclose( pFile );
	return true;
}

void CFileSystem::DeleteFile( const std::wstring& wstrFile )
{
	std::string strFile;
	strFile.resize( wstrFile.length( ) );
	wcstombs( &strFile[ 0 ], &wstrFile[ 0 ], wstrFile.length( ) );

	if ( !FileExists( wstrFile ) )
	{
		LoadFiles( );
		if ( !FileExists( wstrFile ) )
		{
			logging.Log( PREFIX_WARNING, LOCATION_FILESYSTEM, "Attempting to remove file %s which doesn't exist!", strFile.c_str( ) );
			return;
		}
	}

	if( remove( filesystem.FileToDirectory< std::string >( strFile ).c_str( ) ) )
		logging.Log( PREFIX_WARNING, LOCATION_FILESYSTEM, "Removing file %s failed!", strFile.c_str( ) );
}

std::wstring CFileSystem::GetDirectory( )
{
	return wstrCurrentDirectory;
}

std::vector< std::wstring > CFileSystem::GetAllFiles( const wchar_t* wszExtension /*= nullptr*/ )
{
	if( wszExtension == nullptr )
		return vecFiles;

	std::vector< std::wstring > vecFoundFiles;
	for ( auto& file : vecFiles )
		if ( wszExtension == nullptr || file.find( wszExtension ) != std::string::npos )
			vecFoundFiles.emplace_back( file );
	return vecFoundFiles;
}
