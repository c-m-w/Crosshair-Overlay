/// Drawing.cpp

#include "Crosshair.hpp"

// Immediate-Mode GUI
#include <imgui.h>
#include <examples/imgui_impl_dx9.h>
#include <examples/imgui_impl_win32.h>
#include <imgui_internal.h>

#define FLOAT_TO_D3D_COLOR( fl ) D3DCOLOR_ARGB( ( unsigned char )( fl[ 3 ] * 255.f ), ( unsigned char )( fl[ 0 ] * 255.f ), ( unsigned char )( fl[ 1 ] * 255.f ), ( unsigned char )( fl[ 2 ] * 255.f ) )
constexpr auto FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;

extern LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

vertex_t::vertex_t( float x, float y, DWORD _dwColor )
{
	flVectors[ 0 ] = x;
	flVectors[ 1 ] = y;
	flVectors[ 2 ] = 0.f;
	flVectors[ 3 ] = 1.f;
	dwColor = _dwColor;
}

void vertex_t::Rotate( float flAngle, D3DXVECTOR2 vecRotationPoint )
{
	const auto flRadians = D3DXToRadian( flAngle );
	const auto flSin = sin( flRadians );
	const auto flCos = cos( flRadians );
	const D3DXVECTOR2 vecRelative { flVectors[ 0 ] - vecRotationPoint.x, flVectors[ 1 ] - vecRotationPoint.y };

	flVectors[ 0 ] = ( vecRelative.x * flCos ) - ( vecRelative.y * flSin ) + vecRotationPoint.x;
	flVectors[ 1 ] = ( vecRelative.x * flSin ) + ( vecRelative.y * flCos ) + vecRotationPoint.y;
}

void CDrawing::Polygon( vertex_t* pVertices, std::size_t sVertexCount, std::size_t sPrimitiveCount )
{
	IDirect3DVertexBuffer9* pVertexBuffer;

	const auto sVertexSize = sizeof( vertex_t ) * sVertexCount;
	if ( D3D_OK != pOverlayDevice->CreateVertexBuffer( sVertexSize, NULL, FVF, D3DPOOL_DEFAULT, &pVertexBuffer, nullptr ) )
	{
		if ( pVertexBuffer )
		{
			pVertexBuffer->Release( );
			pVertexBuffer = nullptr;
		}
		return;
	}

	void* pVertexMemory;
	pVertexBuffer->Lock( 0, sVertexSize, &pVertexMemory, 0 );
	memcpy( pVertexMemory, pVertices, sVertexSize );
	pVertexBuffer->Unlock( );

	if ( D3D_OK != pOverlayDevice->SetStreamSource( NULL, pVertexBuffer, NULL, sizeof( vertex_t ) ) )
	{
		pVertexBuffer->Release( );
		pVertexBuffer = nullptr;
		logging.Log( PREFIX_WARNING, LOCATION_DRAWING, "Failed to change stream source to vertex buffer of polygon." );
		return;
	}

	pOverlayDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, sPrimitiveCount );
	pVertexBuffer->Release( );
	pVertexBuffer = nullptr;
}

bool CDrawing::InitializeInterface( )
{
	if ( nullptr == ImGui::CreateContext( )
		 || !ImGui_ImplWin32_Init( window.GetInterfaceWindow( ) )
		 || !ImGui_ImplDX9_Init( pInterfaceDevice ) )
	{
		logging.Log( PREFIX_ERROR, LOCATION_DRAWING, "Failed to initialize GUI library." );
		return false;
	}
	ImGui::StyleColorsDark( );
	logging.Log( PREFIX_SUCCESS, LOCATION_DRAWING, "Initialized GUI library." );
	return true;
}

void CDrawing::ShutdownInterface( )
{
	ImGui_ImplDX9_Shutdown( );
	ImGui_ImplWin32_Shutdown( );
	ImGui::DestroyContext( );
	logging.Log( PREFIX_SUCCESS, LOCATION_DRAWING, "GUI library has been shutdown." );
}

bool CDrawing::DrawInterface( )
{
	constexpr auto GUI_WINDOW_FLAGS = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar;
	MSG msgCurrent;

	if ( window.ReceiveInput( &msgCurrent ) )
		return msgCurrent.message != WM_QUIT;
	if ( window.IsInterfaceMinimized( ) )
		return true;

	ImGui_ImplDX9_NewFrame( );
	ImGui_ImplWin32_NewFrame( );
	ImGui::NewFrame( );

	ImGui::Begin( "Overlay Crosshair v0.1", nullptr, GUI_WINDOW_FLAGS );
	{
		const auto flWindowWidth = window.GetOverlayDimensions( );
		ImGui::SetWindowSize( { flWindowWidth[ 0 ], flWindowWidth[ 1 ] } );
		ImGui::SetWindowPos( { 0, 0 } );

		DrawMenuBar( );
		DrawSettings( );
	}
	ImGui::End( );

	ImGui::EndFrame( );

	pInterfaceDevice->SetRenderState( D3DRS_ZENABLE, NULL );
	pInterfaceDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, NULL );
	pInterfaceDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, NULL );
	pInterfaceDevice->Clear( NULL, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, NULL, NULL, NULL );
	if ( SUCCEEDED( pInterfaceDevice->BeginScene( ) ) )
	{
		ImGui::Render( );
		ImGui_ImplDX9_RenderDrawData( ImGui::GetDrawData( ) );
		pInterfaceDevice->EndScene( );
		const auto hrPresent = pInterfaceDevice->Present( nullptr, nullptr, nullptr, nullptr );
		if( hrPresent == D3DERR_DEVICELOST )
		{
			logging.Log( PREFIX_WARNING, LOCATION_DRAWING, "Interface D3D device has been lost!" );
			if ( pInterfaceDevice->TestCooperativeLevel( ) == D3DERR_DEVICENOTRESET )
			{
				ImGui_ImplDX9_InvalidateDeviceObjects( );
				pInterfaceDevice->Reset( &dpInterface );
				ImGui_ImplDX9_CreateDeviceObjects( );
				logging.Log( PREFIX_SUCCESS, LOCATION_DRAWING, "Interface D3D device has been reset." );
			}
			else
				return false;
		}
		return true;
	}
	return false;
}

bool CDrawing::DrawOverlay( )
{
	const auto fnRotateVector = [ ]( D3DXVECTOR2 vecRotatee, D3DXVECTOR2 vecRotationPoint, float flAngle )
	{
		const auto flRadians = D3DXToRadian( flAngle );
		const auto flSin = sin( flRadians );
		const auto flCos = cos( flRadians );
		const D3DXVECTOR2 vecRelative { vecRotatee.x - vecRotationPoint.x, vecRotatee.y - vecRotationPoint.y };

		vecRotatee.x = ( vecRelative.x * flCos ) - ( vecRelative.y * flSin ) + vecRotationPoint.x;
		vecRotatee.y = ( vecRelative.x * flSin ) + ( vecRelative.y * flCos ) + vecRotationPoint.y;
		return vecRotatee;
	};

	const auto& cfgCurrent = cfg.Get( );
	if ( !bShouldRedraw || !cfgCurrent.bDrawCrosshair )
		return true;

	if( cfgCurrent.iCrosshairStyle == STYLE_IMAGE )
	{
		if ( !filesystem.FileExists( cfgCurrent.szImageName ) )
		{
			filesystem.LoadFiles( );
			if ( !filesystem.FileExists( cfgCurrent.szImageName ) )
				return true;
		}

		const auto wstrImage = filesystem.FileToDirectory< std::wstring >( cfgCurrent.szImageName );
		const auto wszImage = wstrImage.c_str( );

		if ( bRecreateImage )
		{
			if( pImage )
			{
				pImage->Release( );
				pImage = nullptr;
			}
			const auto hrResult = D3DXCreateTextureFromFileEx( pOverlayDevice, wszImage, cfgCurrent.iImageDimensions[ 0 ],
															   cfgCurrent.iImageDimensions[ 1 ], D3DX_FROM_FILE, D3DUSAGE_DYNAMIC,
															   D3DFMT_FROM_FILE, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT,
															   NULL, nullptr, nullptr, const_cast< IDirect3DTexture9** >( &pImage ) );
			if( !SUCCEEDED( hrResult ) )
				return true;
			bRecreateImage = false;
		}
	}

	bShouldRedraw = false;
	pOverlayDevice->Clear( NULL, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, NULL, NULL, NULL );
	if ( !SUCCEEDED( pOverlayDevice->BeginScene( ) )
		 || D3D_OK != pOverlayDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE )
		 || D3D_OK != pOverlayDevice->SetRenderState( D3DRS_ANTIALIASEDLINEENABLE, cfgCurrent.bAntiAlias )
		 || D3D_OK != pOverlayDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA )
		 || D3D_OK != pOverlayDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE )
		 || D3D_OK != pOverlayDevice->SetPixelShader( nullptr )
		 || D3D_OK != pOverlayDevice->SetVertexShader( nullptr )
		 || D3D_OK != pOverlayDevice->SetTexture( NULL, nullptr )
		 || D3D_OK != pOverlayDevice->SetFVF( FVF ) )
		return false;

	const auto flWindowCenter = window.GetOverlayCenter( );
	const D3DXVECTOR2 vecWindowCenter { flWindowCenter[ 0 ] + floor( cfgCurrent.flOffset[ 0 ] ), flWindowCenter[ 1 ] + floor( cfgCurrent.flOffset[ 1 ] ) };
	switch( cfgCurrent.iCrosshairStyle )
	{
		case STYLE_DEFAULT_OUTLINED:
		{
			const auto flSize = ceil( cfgCurrent.flSize / 2.f ),
				flThickness = ceil( cfgCurrent.flThickness / 2.f ),
				flOutlineThickness = floor( cfgCurrent.flOutlineThickness );
			const auto clr = FLOAT_TO_D3D_COLOR( cfgCurrent.flOutlineColor );

			vertex_t vtxVertices[ 2 ][ 4 ]
			{
				{
					vertex_t( vecWindowCenter.x - flSize - flOutlineThickness, vecWindowCenter.y - flThickness - flOutlineThickness, clr ),
					vertex_t( vecWindowCenter.x + flSize + flOutlineThickness, vecWindowCenter.y - flThickness - flOutlineThickness, clr ),
					vertex_t( vecWindowCenter.x + flSize + flOutlineThickness, vecWindowCenter.y + flThickness + flOutlineThickness, clr ),
					vertex_t( vecWindowCenter.x - flSize - flOutlineThickness, vecWindowCenter.y + flThickness + flOutlineThickness, clr )
				},
				{
					vertex_t( vecWindowCenter.x - flThickness - flOutlineThickness, vecWindowCenter.y - flSize - flOutlineThickness, clr ),
					vertex_t( vecWindowCenter.x + flThickness + flOutlineThickness, vecWindowCenter.y - flSize - flOutlineThickness, clr ),
					vertex_t( vecWindowCenter.x + flThickness + flOutlineThickness, vecWindowCenter.y + flSize + flOutlineThickness, clr ),
					vertex_t( vecWindowCenter.x - flThickness - flOutlineThickness, vecWindowCenter.y + flSize + flOutlineThickness, clr )
				}
			};

			for ( auto& vl : vtxVertices )
				for ( auto& vertex : vl )
					vertex.Rotate( floor( cfgCurrent.flRotation ), vecWindowCenter );

			Polygon( vtxVertices[ 0 ], 4, 2 ); /// men
			Polygon( vtxVertices[ 1 ], 4, 2 );
		}

		case STYLE_DEFAULT:
		{
			const auto flSize = ceil( cfgCurrent.flSize / 2.f ),
				flThickness = ceil( cfgCurrent.flThickness / 2.f );
			const auto clr = FLOAT_TO_D3D_COLOR( cfgCurrent.flColor );

			vertex_t vtxVertices[ 2 ][ 4 ]
			{
				{
					vertex_t( vecWindowCenter.x - flSize, vecWindowCenter.y - flThickness, clr ),
					vertex_t( vecWindowCenter.x + flSize, vecWindowCenter.y - flThickness, clr ),
					vertex_t( vecWindowCenter.x + flSize, vecWindowCenter.y + flThickness, clr ),
					vertex_t( vecWindowCenter.x - flSize, vecWindowCenter.y + flThickness, clr )
				},
				{
					vertex_t( vecWindowCenter.x - flThickness, vecWindowCenter.y - flSize, clr ),
					vertex_t( vecWindowCenter.x + flThickness, vecWindowCenter.y - flSize, clr ),
					vertex_t( vecWindowCenter.x + flThickness, vecWindowCenter.y + flSize, clr ),
					vertex_t( vecWindowCenter.x - flThickness, vecWindowCenter.y + flSize, clr )
				}
			};

			for ( auto& vl : vtxVertices )
				for ( auto& vertex : vl )
					vertex.Rotate( floor( cfgCurrent.flRotation ), vecWindowCenter );

			Polygon( vtxVertices[ 0 ], 4, 2 ); /// men
			Polygon( vtxVertices[ 1 ], 4, 2 );
		}
		break;

		case STYLE_POLYGON_OUTLINED:
		{
			const auto clr = FLOAT_TO_D3D_COLOR( cfgCurrent.flOutlineColor );
			const auto flRotationAmount = round( 360.f / float( cfgCurrent.iPolygonResolution ) );
			vertex_t vtxInitialPoints[ ] { vertex_t( vecWindowCenter.x, vecWindowCenter.y - ceil( cfgCurrent.flSize / 2.f ) + ceil( cfgCurrent.flOutlineThickness ) + 1.5f, clr ),
												vertex_t( vecWindowCenter.x, vecWindowCenter.y - ceil( cfgCurrent.flSize / 2.f ) - ceil( cfgCurrent.flThickness ) - ceil( cfgCurrent.flOutlineThickness ) - 1.5f, clr ) };
			vertex_t vtxRotatedPoints[ 2 ] { };

			for ( auto& vertex : vtxInitialPoints )
				vertex.Rotate( floor( cfgCurrent.flRotation ), vecWindowCenter );

			for ( auto i = 0; i < cfgCurrent.iPolygonResolution; i++ )
			{
				memcpy( vtxRotatedPoints, vtxInitialPoints, sizeof( vertex_t ) * 2 );
				for ( auto& vertex : vtxRotatedPoints )
					vertex.Rotate( flRotationAmount, vecWindowCenter );

				vertex_t vtxAllVertices[ ] { vtxInitialPoints[ 0 ], vtxRotatedPoints[ 0 ], vtxRotatedPoints[ 1 ], vtxInitialPoints[ 1 ] };
				Polygon( vtxAllVertices, 4, 2 );

				for ( auto& vertex : vtxInitialPoints )
					vertex.Rotate( flRotationAmount, vecWindowCenter );
			}

		}

		case STYLE_POLYGON:
		{
			const auto clr = FLOAT_TO_D3D_COLOR( cfgCurrent.flColor );
			const auto flRotationAmount = 360.f / float( cfgCurrent.iPolygonResolution );
			vertex_t vtxInitialPoints[ ] { vertex_t( vecWindowCenter.x, vecWindowCenter.y - ceil( cfgCurrent.flSize / 2.f ) + 0.5f, clr ),
												vertex_t( vecWindowCenter.x, vecWindowCenter.y - ceil( cfgCurrent.flSize / 2.f ) - ceil( cfgCurrent.flThickness ) - 0.5f, clr ) };
			vertex_t vtxRotatedPoints[ 2 ] { };

			for ( auto& vertex : vtxInitialPoints )
				vertex.Rotate( floor( cfgCurrent.flRotation ), vecWindowCenter );

			for( auto i = 0; i < cfgCurrent.iPolygonResolution; i++ )
			{
				memcpy( vtxRotatedPoints, vtxInitialPoints, sizeof( vertex_t ) * 2 );
				for ( auto& vertex : vtxRotatedPoints )
					vertex.Rotate( flRotationAmount, vecWindowCenter );

				vertex_t vtxAllVertices[ ] { vtxInitialPoints[ 0 ], vtxRotatedPoints[ 0 ], vtxRotatedPoints[ 1 ], vtxInitialPoints[ 1 ] };
				Polygon( vtxAllVertices, 4, 2 );

				for ( auto& vertex : vtxInitialPoints )
					vertex.Rotate( flRotationAmount, vecWindowCenter );
			}
		}
		break;

		case STYLE_IMAGE:
		{
			if( pSprite->Begin( D3DXSPRITE_ALPHABLEND ) != D3D_OK )
			{
				bShouldRedraw = bRecreateImage = true;
				logging.Log( PREFIX_WARNING, LOCATION_DRAWING, "Error beginning sprite for image." );
				return true;
			}
			D3DXVECTOR3 vecLocation { vecWindowCenter.x - cfgCurrent.iImageDimensions[ 0 ] / 2.f, vecWindowCenter.y - cfgCurrent.iImageDimensions[ 1 ] / 2.f, 0.f };
			pSprite->Draw( pImage, nullptr, nullptr, &vecLocation, 0xFFFFFFFF );
			pSprite->End( );
		}
		break;

		default:
			break;
	}

	pOverlayDevice->EndScene( );
	return SUCCEEDED( pOverlayDevice->Present( nullptr, nullptr, nullptr, nullptr ) );
}

void CDrawing::DrawMenuBar( )
{
	if ( ImGui::BeginMenuBar( ) )
	{
		if ( ImGui::BeginMenu( "File" ) )
		{
			if ( ImGui::MenuItem( "Minimize" ) )
				window.MinimizeInterface( );
			if ( ImGui::MenuItem( "Exit" ) )
				bShutdown = true;
			ImGui::EndMenu( );
		}
		if ( ImGui::BeginMenu( "Settings" ) )
		{
			if ( ImGui::MenuItem( "Load" ) )
				cfg.LoadConfiguration( );
			if ( ImGui::MenuItem( "Save" ) )
				cfg.SaveConfiguration( );
			ImGui::EndMenu( );
		}
		ImGui::EndMenuBar( );
	}
}

void CDrawing::DrawSettings( )
{
	constexpr char CROSSHAIR_STYLES[ ] = "Default\0Default Outlined\0Polygon\0Polygon Outlined\0Image\0";
	auto& cfgCurrent = cfg.Get( );
	const auto cfgOld = cfgCurrent;

	ImGui::Checkbox( "Draw Crosshair", &cfgCurrent.bDrawCrosshair );
	if ( !cfgCurrent.bDrawCrosshair )
	{
		if ( cfgOld.bDrawCrosshair )
			bShouldRedraw = true;
		return;
	}

	const auto flFullDimensions = window.GetOverlayDimensions( );
	const float flDimensions[ ] { flFullDimensions[ 0 ] / 2.f, flFullDimensions[ 1 ] / 2.f };
	ImGui::PushItemWidth( 305.f );
	ImGui::Checkbox( "Anti-Alias", &cfgCurrent.bAntiAlias );
	ImGui::SliderFloat( "X Offset", &cfgCurrent.flOffset[ 0 ], -flDimensions[ 0 ], flDimensions[ 0 ] );
	ImGui::SameLine( );
	ImGui::SliderFloat( "Y Offset", &cfgCurrent.flOffset[ 1 ], -flDimensions[ 1 ], flDimensions[ 1 ] );
	ImGui::SameLine( );
	if ( ImGui::Button( "Reset" ) )
		cfgCurrent.flOffset[ 0 ] = cfgCurrent.flOffset[ 1 ] = 0.f;
	ImGui::PushItemWidth( 150.f );
	ImGui::Combo( "Style", &cfgCurrent.iCrosshairStyle, CROSSHAIR_STYLES );
	ImGui::PushItemWidth( 305.f );

	if ( cfgCurrent.iCrosshairStyle != STYLE_IMAGE )
	{
		ImGui::ColorPicker3( "Crosshair", cfgCurrent.flColor );
		ImGui::SameLine( );
		ImGui::ColorPicker3( "Outline", cfgCurrent.flOutlineColor );
		ImGui::SliderFloat( "Thickness", &cfgCurrent.flThickness, 1.f, 50.f );
		ImGui::SameLine( );
		ImGui::SliderFloat( "Outline Thickness", &cfgCurrent.flOutlineThickness, 1.f, 50.f );
		ImGui::SliderFloat( "Size", &cfgCurrent.flSize, 1.f, 100.f );
		ImGui::SameLine( );
		ImGui::Indent( 379.f );
		ImGui::SliderFloat( "Rotation", &cfgCurrent.flRotation, -180.f, 180.f );
		if ( cfgCurrent.iCrosshairStyle == STYLE_POLYGON || cfgCurrent.iCrosshairStyle == STYLE_POLYGON_OUTLINED )
		{
			ImGui::Indent( -379.f );
			ImGui::SliderInt( "Polygon Resolution", &cfgCurrent.iPolygonResolution, 3, 24 );
		}
		ImGui::Indent( 0.f );
	}
	else
	{
		static auto bCheckedFileExistance = false, bFileExists = false;

		ImGui::InputText( "Image Name", cfgCurrent.szImageName, 32u );
		ImGui::SameLine( );
		if ( ImGui::Button( "Check If File Exists" ) )
		{
			filesystem.LoadFiles( );
			bCheckedFileExistance = true;
			bFileExists = filesystem.FileExists( cfgCurrent.szImageName );
		}
		if ( bCheckedFileExistance )
		{
			ImGui::SameLine( );
			if ( bFileExists )
				ImGui::TextColored( { 0, 255, 0, 255 }, "File Exists." );
			else
				ImGui::TextColored( { 255, 0, 0, 255 }, "File Doesn't Exist." );
		}
		ImGui::InputInt2( "Size", cfgCurrent.iImageDimensions );
	}
	
	if( memcmp( &cfgOld, &cfgCurrent, sizeof( config_t ) ) != 0 )
		bShouldRedraw = true;
	if ( strcmp( cfgOld.szImageName, cfgCurrent.szImageName ) != 0 || cfgOld.iImageDimensions[ 0 ] != cfgCurrent.iImageDimensions[ 0 ] || cfgOld.iImageDimensions[ 1 ] != cfgCurrent.iImageDimensions[ 1 ] )
		bShouldRedraw = bRecreateImage = true;
	
}

bool CDrawing::Initialize( )
{
	if ( nullptr == ( pD3DInstance = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		return false;

	dpOverlay.Windowed = TRUE;
	dpOverlay.SwapEffect = D3DSWAPEFFECT_DISCARD;
	dpOverlay.BackBufferFormat = D3DFMT_UNKNOWN;
	dpOverlay.EnableAutoDepthStencil = TRUE;
	dpOverlay.AutoDepthStencilFormat = D3DFMT_D16;
	dpOverlay.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	dpInterface = dpOverlay;

	if ( FAILED( pD3DInstance->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window.GetOverlayWindow( ), D3DCREATE_HARDWARE_VERTEXPROCESSING, &dpOverlay, &pOverlayDevice ) )
		 || FAILED( pD3DInstance->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window.GetInterfaceWindow( ), D3DCREATE_HARDWARE_VERTEXPROCESSING, &dpInterface, &pInterfaceDevice ) ) )
	{
		logging.Log( PREFIX_ERROR, LOCATION_DRAWING, "Creating D3D devices has failed!" );
		pD3DInstance->Release( );
		return false;
	}

	return bInitialized = D3D_OK == D3DXCreateLine( pOverlayDevice, &pLine )
		&& D3D_OK == D3DXCreateSprite( pOverlayDevice, &pSprite )
		&& InitializeInterface( );
}

void CDrawing::Shutdown( )
{
	pD3DInstance->Release( );
	pOverlayDevice->Release( );
	pInterfaceDevice->Release( );
	ShutdownInterface( );
}

bool __stdcall CDrawing::Draw( )
{
	return DrawOverlay( ) && DrawInterface( );
}

void CDrawing::Redraw( )
{
	bShouldRedraw = true;
}

void CDrawing::CreateImageTextures( )
{
	bRecreateImage = true;
}

bool CDrawing::HandleEvent( HWND hwWindowHandle, UINT uMessage, WPARAM wParam, LPARAM llParam )
{
	return ImGui_ImplWin32_WndProcHandler( hwWindowHandle, uMessage, wParam, llParam );
}
