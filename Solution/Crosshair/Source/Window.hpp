/// Window.hpp

#pragma once

LRESULT WINAPI OverlayWindowProc( HWND hwWindowHandle, UINT uMessage, WPARAM wParam, LPARAM llParam );
LRESULT WINAPI InterfaceWindowProc( HWND hwWindowHandle, UINT uMessage, WPARAM wParam, LPARAM llParam );

inline class CWindow: public IBaseInterface
{
	const unsigned uInteractableStyle = WS_VISIBLE, uUninteractableStyle = WS_POPUP | WS_VISIBLE,
					uInteractableExtendedStyle = WS_EX_APPWINDOW, uUninteractableExtendedStyle = WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_NOACTIVATE;
	const wchar_t* wszOverlayWindowTitle = L"Crosshair Overlay - v0.1";
	const wchar_t* wszInterfaceWindowTitle = L"Crosshair GUI - v0.1";
	float flOverlayDimensions[ 2 ] { };
	const float flInterfaceDimensions[ 2 ] { 830.f, 600.f };
	const float flConsoleDimensions[ 2 ] { 1500.f, 500.f };
	WNDCLASSEX wndOverlay { sizeof( WNDCLASSEX ), NULL, OverlayWindowProc, NULL, NULL, nullptr, nullptr, nullptr, nullptr, nullptr, wszOverlayWindowTitle, nullptr };
	WNDCLASSEX wndInterface { sizeof( WNDCLASSEX ), NULL, InterfaceWindowProc, NULL, NULL, nullptr, nullptr, nullptr, nullptr, nullptr, wszInterfaceWindowTitle, nullptr };
	HWND hwOverlay = nullptr, hwInterface = nullptr;
	bool bInterfaceMinimized = false;

	void ResizeConsole( );

public:
	CWindow( ) = default;
	~CWindow( ) = default;

	bool __stdcall Initialize( ) override;
	void __stdcall Shutdown( ) override;
	HWND& GetOverlayWindow( );
	HWND& GetInterfaceWindow( );
	const float* GetOverlayDimensions( );
	bool ReceiveInput( MSG* pMessage );
	void MinimizeInterface( );
	bool IsInterfaceMinimized( );
	const float* GetOverlayCenter( );
	friend LRESULT WINAPI InterfaceWindowProc( HWND hwWindowHandle, UINT uMessage, WPARAM wParam, LPARAM llParam );
} window;
