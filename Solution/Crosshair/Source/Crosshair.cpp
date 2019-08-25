/// Crosshair.cpp

#include "Crosshair.hpp"

namespace Main
{
	using namespace Utilities;
	using namespace Types;

	moment_t mmtLaunchTime, mmtShutdownTime;
	BOOL bReturn;
	const char* szFileSystemSuccessfulMessage = "Initialized file system.",
		*szFileSystemUnsuccessfulMessage = "Unable to initialize file system.";
	const char* szLoggingSuccessfulMessage = "Initialized logging system.",
		*szLoggingUnsuccessfulMessage = "Unable to initialize logging system.";
	const char* szWindowSuccessfulMessage = "Created windows.",
		*szWindowUnsuccessfulMessage = "Unable to create windows.";
	const char* szDrawingSuccessfulmessage = "Initialized D3D9 and ImGUI.",
		*szDrawingUnsuccessfulMessage = "Unable to initialize D3D9 or ImGUI";
	const char* szShutdownMessage = "Shutting down after running for %u seconds.";

	/** \brief Uninitializes an object by calling the shutdown function of it. */
	/** \param pObject Object to uninitialize. */
	__declspec( naked ) void UninitializeObject( IBaseInterface* pObject )
	{
		DWORD dwReturnAddress;

		__asm
		{
			pop		dwReturnAddress
			pop		pObject
			mov		ecx, pObject
			push	ecx
			mov		ecx, [ ecx ]
			add		ecx, 8
			mov		ecx, [ ecx ]
			call	ecx
			push	dwReturnAddress
			retn
		}
	}

	/** \brief Initializes an object which inherits the IBaseInterface interface. */
	/** \param pObject Object to initialize. */
	/** \param szSuccessMessage Message to log if initialization was successful. */
	/** \param szErrorMessage Message to log if initialization was unsuccessful. */
	/** \return Whether or not initialization was successful. */
	__declspec( naked ) bool __stdcall InitializeObject( IBaseInterface* pObject, const char* szSuccessMessage, const char* szErrorMessage )
	{
		DWORD dwReturnAddress;
		__asm
		{
			pop		dwReturnAddress
			pop		pObject

			mov		ecx, pObject
			push	pObject
			push	dwReturnAddress

			push	ecx
			mov		ecx, [ ecx ]
			add		ecx, 4
			mov		ecx, [ ecx ]
			call	ecx

			pop		dwReturnAddress
			pop		pObject
			pop		szSuccessMessage
			pop		szErrorMessage

			cmp		eax, 0
			jnz		LogSuccessful
			jmp		Shutdown

			LogSuccessful:
				
				push	dwReturnAddress
				push	szSuccessMessage
				push	LOCATION_MAIN
				push	PREFIX_SUCCESS
				push	offset logging
				call	offset CLogging::Log
				pop		ecx
				pop		ecx
				pop		ecx
				pop		ecx
				ret

			Shutdown:
				mov		ecx, offset logging
				push	ecx
				call	offset CLogging::Initialized

				cmp		eax, 1
				je		LogError

			TestShutdownWindow:
				mov		ecx, offset window
				push	ecx
				call	CWindow::Initialized
				cmp		eax, 1
				je		ShutdownWindow

			TestShutdownDrawing:
				mov		ecx, offset drawing
				push	ecx
				call	CDrawing::Initialized
				cmp		eax, 1
				je		ShutdownDrawing

			TestShutdownFileSystem:
				mov		ecx, offset filesystem
				push	ecx
				call	CFileSystem::Initialized
				cmp		eax, 1
				je		ShutdownFileSystem

			EndShutdown:
				mov		eax, 0
				push	dwReturnAddress
				retn

			LogError:
				push	szErrorMessage
				push	LOCATION_MAIN
				push	PREFIX_ERROR
				push	offset logging
				call	CLogging::Log
				push	offset logging
				call	CLogging::Shutdown
				jmp		TestShutdownWindow

			ShutdownWindow :
				push	offset window
				call	UninitializeObject
				jmp		TestShutdownDrawing

			ShutdownDrawing :
				push	offset drawing
				call	UninitializeObject
				jmp		TestShutdownFileSystem

			ShutdownFileSystem :
				push	offset filesystem
				call	UninitializeObject
				jmp		EndShutdown
		}
	}

	moment_t __stdcall GetMomentHelper( )
	{
		return GetMoment( );
	}

	void __stdcall WaitHelper( )
	{
		return Wait( 1ull );
	}

	__declspec( naked ) __declspec( noreturn ) void Setup( HINSTANCE hInstance )
	{
		DWORD dwReturnAddress;
		__asm
		{
			pop dwReturnAddress
			pop hInstance

			push hInstance
			pop hCurrentInstance
			mov bReturn, 0

			call StartRecording

			call CreateConsole
			cmp eax, 0
			je Shutdown

			push szFileSystemUnsuccessfulMessage
			push szFileSystemSuccessfulMessage
			push offset filesystem
			call InitializeObject
			cmp eax, 0
			je Shutdown

			push szLoggingUnsuccessfulMessage
			push szLoggingSuccessfulMessage
			push offset logging
			call InitializeObject
			cmp eax, 0
			je Shutdown

			push szWindowUnsuccessfulMessage
			push szWindowSuccessfulMessage
			push offset window
			call InitializeObject
			cmp eax, 0
			je Shutdown

			push szDrawingUnsuccessfulMessage
			push szDrawingSuccessfulmessage
			push offset drawing
			call InitializeObject
			cmp eax, 0
			je Shutdown

			mov bReturn, 1
			push offset logging
			call CLogging::WriteToFile

			Draw:
				call WaitHelper
				push offset drawing
				call CDrawing::Draw
				xor eax, bShutdown
				cmp eax, 1
				je Draw

			call StopRecording
			push eax
			push szShutdownMessage
			push LOCATION_MAIN
			push PREFIX_INFORMATION
			push offset logging
			call CLogging::Log

			push offset filesystem
			call UninitializeObject
			push offset logging
			call UninitializeObject
			push offset window
			call UninitializeObject
			push offset drawing
			call UninitializeObject

			Shutdown:
				call DestroyConsole
				push bReturn
				call ExitProcess
		}
	}
}

BOOL WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{
	Main::Setup( hInstance );
}