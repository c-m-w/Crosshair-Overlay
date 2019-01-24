/// Filesystem.hpp

#pragma once

inline class CFileSystem: public IBaseInterface
{
private:
	std::wstring wstrCurrentDirectory;
	std::vector< std::wstring > vecFiles;

	bool OpenFile( std::wstring wstrFile, FILE*& pFile, bool bWriteMode, int* pSize = nullptr );

public:
	CFileSystem( ) = default;
	~CFileSystem( ) = default;

	bool __stdcall Initialize( ) override;
	void __stdcall Shutdown( ) override;
	void LoadFiles( );
	bool FileExists( const std::wstring& wstrFile );
	bool FileExists( const std::string& strFile );
	void WriteToFile( const std::wstring& wstrFile, const std::string& strContents );
	void AddToFile( const std::wstring& wstrFile, const std::string& strContents );
	bool ReadFile( const std::wstring& wstrFile, std::string& strContents );
	void DeleteFile( const std::wstring& wstrFile );
	std::wstring GetDirectory( );
	std::vector< std::wstring > GetAllFiles( const wchar_t* wszExtension = nullptr );

	template< typename _t > _t FileToDirectory( const std::string& strFilename );
	template< > std::wstring FileToDirectory( const std::string& strFilename );
	template< > std::string FileToDirectory( const std::string& strFilename );

	template< typename _t > _t FileToDirectory( const std::wstring& wstrFilename );
	template< > std::wstring FileToDirectory( const std::wstring& wstrFilename );
	template< > std::string FileToDirectory( const std::wstring& wstrFilename );
} filesystem;

#include "FileSystem.inl"
