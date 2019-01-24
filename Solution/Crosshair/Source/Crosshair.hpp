/// Crosshair.hpp

#pragma once

#if not defined UNICODE or defined _MBCS
#error Unicode must be used for this project.
#endif

#if not defined WIN32
#error This project must be built in 32 bit mode.
#endif

#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <cstdio>
#include <cassert>
#include <string>
#include <fstream>
#include <sstream>
#include <ctime>
#include <chrono>
#include <iostream>
#include <thread>
#include <filesystem>

#include <d3d9.h>
#include <d3dx9core.h>
#pragma comment( lib, "d3d9.lib" )
#pragma comment( lib, "d3dx9.lib" )

#undef DeleteFile // theres a winapi function to delete files but filesystem has a function called deletefile

inline BOOL bShutdown = FALSE;
inline HINSTANCE hCurrentInstance = nullptr;

#include "Utilities.hpp"
#include "FileSystem.hpp"
#include "Configuration.hpp"
#include "Window.hpp"
#include "Logging.hpp"
#include "Drawing.hpp"
