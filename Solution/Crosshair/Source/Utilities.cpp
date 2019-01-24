/// Utilities.cpp

#include "Crosshair.hpp"

namespace Utilities
{
	FILE* pConsole;
	Types::moment_t mmtRecordStart;

	bool __stdcall CreateConsole( )
	{
#if defined _DEBUG
		return TRUE == AllocConsole( ) &&
			!freopen_s( &pConsole, "CONOUT$", "w", stdout )
			&& pConsole != nullptr;
#endif
		return true;
	}

	bool __stdcall DestroyConsole( )
	{
#if defined _DEBUG
		return NULL == fclose( pConsole ) &&
				NULL != FreeConsole( );
#endif
		return true;
	}

	void StartRecording( )
	{
		mmtRecordStart = GetMoment( );
	}

	unsigned StopRecording( )
	{
		return unsigned( GetMoment( ) - mmtRecordStart );
	}
}
