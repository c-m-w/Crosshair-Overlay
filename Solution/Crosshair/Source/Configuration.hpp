/// Configuration.hpp

#pragma once

/** \brief Style of crosshair to draw. */
enum ECrosshairStyle
{
	/** \brief Default crosshair - draws two lines which intersect eachother. */
	STYLE_DEFAULT,
	/** \brief Default outlined crosshair - draws an outline behind the default style. */
	STYLE_DEFAULT_OUTLINED,
	/** \brief Polygon crosshair - draws a polygon with a specified amount of sides. */
	STYLE_POLYGON,
	/** \brief Polygon outlined crosshair - draws an outline behind the polygon style. */
	STYLE_POLYGON_OUTLINED,
	/** \brief Image crosshair - draws an image to the window. */
	STYLE_IMAGE,
	STYLE_MAX
};

/** \brief Holds information about the crosshair. */
struct config_t
{
	/** \brief Whether or not the crosshair should be drawn. */
	bool bDrawCrosshair = false;
	/** \brief Whether or not anti aliasing should be applied when drawing. */
	bool bAntiAlias = false;
	/** \brief Offset of the crosshair relative to the center of the window. */
	float flOffset[ 2 ] { 0.f, 0.f };
	/** \brief Style of the crosshair. */
	int iCrosshairStyle = STYLE_DEFAULT;
	/** \brief Thickness of the lines drawn for the crosshair. */
	float flThickness = 1.f;
	/** \brief Thickness of the outline drawn for the crosshair. */
	float flOutlineThickness = 1.f;
	/** \brief How large the crosshair is. */
	float flSize = 5.f;
	/** \brief Rotation of each of the vertices when drawing the crosshair. */
	float flRotation = 0.f;
	/** \brief Color of the crosshair. */
	float flColor[ 4 ] { 1.f, 0.f, 0.f, 1.f };
	/** \brief Color of the outline of the crosshair, if an outlined style is selected. */
	float flOutlineColor[ 4 ] { 1.f, 1.f, 1.f, 1.f };
	/** \brief How many sides the polygon should have, if the polygon style is selected. */
	int iPolygonResolution = 10;
	/** \brief Filename of the image to use as a crosshair in the directory of the executable,\n
			   if the image style is selected. */
	char szImageName[ 32 ] { };
	/** \brief Dimensions of the image. */
	int iImageDimensions[ 2 ] { 0, 0 };
};

/** \brief Manages the configuration of the crosshair. */
inline class CConfig: public IBaseInterface
{
	/** \brief Name of the file that the configuration should be stored to. */
	const wchar_t* wszConfigFilename = L"config.json";
	/** \brief Current configuration. */
	config_t cfgCurrent;

public:
	CConfig( ) = default;
	~CConfig( ) = default;

	bool __stdcall Initialize( ) override;
	void __stdcall Shutdown( ) override;
	/** \brief Loads the configuration from file. */
	/** \return Whether or not reading and parsing the file was successful. */
	bool LoadConfiguration( );
	/** \brief Writes the current configuration to file for the ability to be loaded later. */
	void SaveConfiguration( );
	/** \brief Get a reference to the current configuration. */
	/** \return Reference of the current configuration. */
	config_t& Get( );
} cfg;
