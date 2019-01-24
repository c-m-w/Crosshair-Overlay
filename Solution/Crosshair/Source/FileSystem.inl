/// FileSystem.inl

#pragma once

template< > inline std::wstring CFileSystem::FileToDirectory< std::wstring >( const std::string& strFilename )
{
	std::wstring wstrFilename;
	wstrFilename.resize( strFilename.length( ) );
	mbstowcs( &wstrFilename[ 0 ], &strFilename[ 0 ], strFilename.length( ) );
	return GetDirectory( ) + wstrFilename;
}

template< > inline std::string CFileSystem::FileToDirectory< std::string >( const std::string& strFilename )
{
	std::string strDirectory;
	strDirectory.resize( GetDirectory( ).length( ) );
	wcstombs( &strDirectory[ 0 ], &GetDirectory( )[ 0 ], GetDirectory( ).length( ) );
	return strDirectory + strFilename;
}

template< > inline std::wstring CFileSystem::FileToDirectory< std::wstring >( const std::wstring& wstrFilename )
{
	return GetDirectory( ) + wstrFilename;
}

template< > inline std::string CFileSystem::FileToDirectory< std::string >( const std::wstring& wstrFilename )
{
	const auto wstrDirectory = GetDirectory( ) + wstrFilename;
	std::string strDirectory;
	strDirectory.resize( wstrDirectory.length( ) );
	wcstombs( &strDirectory[ 0 ], &wstrDirectory[ 0 ], wstrDirectory.length( ) );
	return strDirectory;
}
