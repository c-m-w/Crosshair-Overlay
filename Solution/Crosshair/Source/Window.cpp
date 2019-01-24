/// Window.cpp

#include "Crosshair.hpp"

LRESULT WINAPI OverlayWindowProc( HWND hwWindowHandle, UINT uMessage, WPARAM wParam, LPARAM llParam )
{
	switch ( uMessage )
	{
		case WM_CREATE:
		{
			logging.Log( PREFIX_INFORMATION, LOCATION_WINDOW, "Overlay window has been sent create message." );
		}
		return TRUE;

		case WM_DESTROY:
		{
			logging.Log( PREFIX_INFORMATION, LOCATION_WINDOW, "Overlay window has been sent destroy message, shutting down." );
			bShutdown = true;
		}
		return TRUE;

		default:
			return DefWindowProc( hwWindowHandle, uMessage, wParam, llParam );
	}
}

LRESULT WINAPI InterfaceWindowProc( HWND hwWindowHandle, UINT uMessage, WPARAM wParam, LPARAM llParam )
{
	if ( uMessage == WM_CREATE )
	{
		logging.Log( PREFIX_INFORMATION, LOCATION_WINDOW, "Interface window has been sent create message." );
		return TRUE;
	}
	if( uMessage == WM_IME_SETCONTEXT && window.bInterfaceMinimized )
	{
		logging.Log( PREFIX_INFORMATION, LOCATION_WINDOW, "Interface window has been sent set context message, unminimizing..." );
		window.bInterfaceMinimized = false;
		return TRUE;
	}
	if( uMessage == WM_DESTROY )
	{
		logging.Log( PREFIX_INFORMATION, LOCATION_WINDOW, "Interface window has been sent destroy message, shutting down..." );
		bShutdown = true;
		return TRUE;
	}
	return drawing.HandleEvent( hwWindowHandle, uMessage, wParam, llParam ) ? TRUE
		: DefWindowProc( hwWindowHandle, uMessage, wParam, llParam );
}

void CWindow::ResizeConsole( )
{
#if defined _DEBUG
	const auto hwConsole = GetConsoleWindow( );
	RECT recConsole;

	GetWindowRect( hwConsole, &recConsole );
	MoveWindow( hwConsole, recConsole.left, recConsole.top, flConsoleDimensions[ 0 ], flConsoleDimensions[ 1 ], TRUE );
#endif
}

bool CWindow::Initialize( )
{
	RECT recDesktop;
	wndOverlay.hInstance = wndInterface.hInstance = hCurrentInstance;

	if ( FALSE == GetWindowRect( GetDesktopWindow( ), &recDesktop )
		 || NULL == RegisterClassEx( &wndOverlay )
		 || NULL == RegisterClassEx( &wndInterface ) )
		return false;

	flOverlayDimensions[ 0 ] = float( recDesktop.right - recDesktop.left );
	flOverlayDimensions[ 1 ] = float( recDesktop.bottom - recDesktop.top );

	logging.Log( PREFIX_INFORMATION, LOCATION_WINDOW, "Retrieved screen dimensions, X:%f, Y:%f", flOverlayDimensions[ 0 ], flOverlayDimensions[ 1 ] );

	hwOverlay = CreateWindowEx( uUninteractableExtendedStyle, wszOverlayWindowTitle, wszOverlayWindowTitle, uUninteractableStyle,
							   recDesktop.left, recDesktop.top, int( flOverlayDimensions[ 0 ] ), int( flOverlayDimensions[ 1 ] ),
							   nullptr, nullptr, wndOverlay.hInstance, nullptr );
	logging.Log( PREFIX_INFORMATION, LOCATION_WINDOW, "Overlay window creation attempted, resulting handle: %08X", hwOverlay );
	SetLayeredWindowAttributes( hwOverlay, NULL, NULL, LWA_COLORKEY );

	hwInterface = CreateWindowEx( uInteractableExtendedStyle, wszInterfaceWindowTitle, wszInterfaceWindowTitle, uInteractableStyle,
								  int( recDesktop.left + flOverlayDimensions[ 0 ] / 2.f - flInterfaceDimensions[ 0 ] / 2.f ),
								  int( recDesktop.top + flOverlayDimensions[ 1 ] / 2.f - flInterfaceDimensions[ 1 ] / 2.f ),
								  int( flInterfaceDimensions[ 0 ] ), int( flInterfaceDimensions[ 1 ] ), nullptr, nullptr, wndInterface.hInstance, nullptr );
	logging.Log( PREFIX_INFORMATION, LOCATION_WINDOW, "Interface window creation attempted, resulting handle: %08X", hwInterface );

	SetForegroundWindow( hwOverlay );
	SetForegroundWindow( hwInterface );
	ResizeConsole( );
	return bInitialized = hwOverlay != nullptr && hwInterface != nullptr;
}

void CWindow::Shutdown( )
{
	UnregisterClass( wszOverlayWindowTitle, wndOverlay.hInstance );
	UnregisterClass( wszInterfaceWindowTitle, wndInterface.hInstance );
	DestroyWindow( hwOverlay );
	DestroyWindow( hwInterface );
}

HWND& CWindow::GetOverlayWindow( )
{
	return hwOverlay;
}

HWND& CWindow::GetInterfaceWindow( )
{
	return hwInterface;
}

const float* CWindow::GetOverlayDimensions( )
{
	return flOverlayDimensions;
}

bool CWindow::ReceiveInput( MSG* pMessage )
{
	if ( PeekMessage( pMessage, nullptr, NULL, NULL, PM_REMOVE ) )
	{
		TranslateMessage( pMessage );
		DispatchMessage( pMessage );
		return true;
	}
	return false;
}

void CWindow::MinimizeInterface( )
{
	bInterfaceMinimized = true;
	logging.Log( PREFIX_INFORMATION, LOCATION_WINDOW, "Minimizing interface window." );
	ShowWindow( hwInterface, SW_MINIMIZE );
}

bool CWindow::IsInterfaceMinimized( )
{
	return bInterfaceMinimized;
}

const float* CWindow::GetOverlayCenter( )
{
	static float flCenter[ 2 ];
	RECT recOverlay;

	GetWindowRect( hwOverlay, &recOverlay );
	flCenter[ 0 ] = round( recOverlay.left + ( recOverlay.right - recOverlay.left ) / 2.f );
	flCenter[ 1 ] = round( recOverlay.top + ( recOverlay.bottom - recOverlay.top ) / 2.f );
	return flCenter;
}
