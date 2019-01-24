/// Drawing.hpp

#pragma once

/** \brief Vertex for rendering with D3D9. */
struct vertex_t
{
	/** \brief X, Y, Z and RHW dimensions of the vertex. */
	float flVectors[ 4 ] { };
	/** \brief Color of the vertex.\n
			   If verticies have different colors, D3D9 will automatically apply a gradient. */
	DWORD dwColor { };

	/** \brief Default constructor for the vertex. */
	vertex_t( ) = default;
	/** \brief Assigns flVectors and dwColor values. */
	/** \param x X coordinate of the vertex. */
	/** \param y Y coordinate of the vertex. */
	/** \param _dwColor Color of the vertex. */
	vertex_t( float x, float y, DWORD _dwColor );

	/** \brief Rotates the vertex two-dimensionally about a point. */
	/** \param flAngle Angle in degrees to rotate the vertex by. */
	/** \param vecRotationPoint Point to rotate the vertex about. */
	void Rotate( float flAngle, D3DXVECTOR2 vecRotationPoint );
};

inline class CDrawing: public IBaseInterface
{
	IDirect3D9* pD3DInstance = nullptr;
	D3DPRESENT_PARAMETERS dpOverlay { }, dpInterface { };
	IDirect3DDevice9* pOverlayDevice = nullptr, *pInterfaceDevice;
	ID3DXLine* pLine = nullptr;
	ID3DXSprite* pSprite = nullptr;
	IDirect3DTexture9* pImage = nullptr;

	bool bShouldRedraw = true, bShowInterface = true, bRecreateImage = true;
	unsigned char* pSpriteData;

	void Polygon( vertex_t* pVertices, std::size_t sVertexCount, std::size_t sPrimitiveCount );

	bool InitializeInterface( );
	void ShutdownInterface( );
	bool DrawInterface( );
	bool DrawOverlay( );
	void DrawMenuBar( );
	void DrawSettings( );

public:
	CDrawing( ) = default;
	~CDrawing( ) = default;

	bool __stdcall Initialize( ) override;
	void __stdcall Shutdown( ) override;
	bool __stdcall Draw( );
	void Redraw( );
	void CreateImageTextures( );
	bool HandleEvent( HWND hwWindowHandle, UINT uMessage, WPARAM wParam, LPARAM llParam );
} drawing;
