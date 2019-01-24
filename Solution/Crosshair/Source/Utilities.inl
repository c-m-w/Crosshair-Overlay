/// Utilities.inl

#pragma once

namespace Utilities
{
	template< typename _t > extern __forceinline Types::moment_t GetMoment( )
	{
		return ::std::chrono::duration_cast< _t >( ::std::chrono::system_clock::now( ).time_since_epoch( ) ).count( );
	}

	template< typename _t > extern __forceinline void Wait( Types::moment_t mmtTime )
	{
		::std::this_thread::sleep_for( _t( mmtTime ) );
	}
}
