/// Utilities.hpp

#pragma once

namespace Utilities
{
	namespace Types
	{
		class IBaseInterface
		{
		protected:
			bool bInitialized = false;

		public:
			IBaseInterface( ) = default;
			virtual ~IBaseInterface( ) = default;

			virtual bool __stdcall Initialize( ) = 0;
			virtual void __stdcall Shutdown( ) = 0;
			bool Initialized( ) const
			{
				return bInitialized;
			}

			typedef bool( __stdcall* initialize_t )( void* pThis );
			typedef void( __stdcall* shutdown_t )( void* pThis );
		};

		typedef unsigned long long moment_t;
	}

	bool __stdcall CreateConsole( );
	bool __stdcall DestroyConsole( );
	template< typename _t = ::std::chrono::seconds > Types::moment_t GetMoment( );
	template< typename _t = ::std::chrono::milliseconds > void Wait( Types::moment_t mmtTime );
	void StartRecording( );
	unsigned StopRecording( );
}

using Utilities::Types::IBaseInterface;

#include "Utilities.inl"
