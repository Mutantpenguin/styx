#ifndef CINPUT_HPP
#define CINPUT_HPP

#ifdef __linux__
	#include <SDL2/SDL.h>
#elif _WIN32
	#include <SDL.h>
#else
	#error "unsupported platform"
#endif

#include "CFileSystem.hpp"
#include "CSettings.hpp"
#include "CTimer.hpp"

class CInput final
{
friend class CEngine;
friend class CEngineInterface;

private:
	CInput( const CInput &rhs ) = delete;
	CInput& operator = ( const CInput &rhs ) = delete;

public:
	std::int32_t inline MouseDeltaX( void ) const { return( m_dX ); }
	std::int32_t inline MouseDeltaY( void ) const { return( m_dY ); }

	bool inline KeyDown( const std::uint32_t index ) const 			{ return( CurrentKey( index ) && !OldKey( index ) ); }
	bool inline KeyStillDown( const std::uint32_t index ) const 	{ return( CurrentKey( index ) && OldKey( index ) ); }
	bool inline KeyUp( const std::uint32_t index ) const 			{ return( !CurrentKey( index ) && OldKey( index ) ); }
	bool inline KeyStillUp( const std::uint32_t index ) const 		{ return( !CurrentKey( index ) && !OldKey( index ) ); }

	bool inline MouseDown( const std::uint32_t button ) const 		{ return( CurrentMouse( button ) && !OldMouse( button ) ); }
	bool inline MouseStillDown( const std::uint32_t button ) const 	{ return( CurrentMouse( button ) && OldMouse( button ) ); }
	bool inline MouseUp( const std::uint32_t button ) const 		{ return( !CurrentMouse( button ) && OldMouse( button ) ); }
	bool inline MouseStillUp( const std::uint32_t button ) const 	{ return( !CurrentMouse( button ) && !OldMouse( button ) ); }

private:
	CInput( const CSettings &settings, const CFileSystem &filesystem );
	~CInput();

	void Update( void );

private:
	std::int32_t m_keyCount	{ 0 };

	const Uint8	* const m_keys;
	Uint8		*m_oldKeys;

	std::int32_t m_dX { 0 };
	std::int32_t m_dY { 0 };

	std::uint32_t m_buttons		{ 0 };
	std::uint32_t m_oldButtons	{ 0 };

private:
	bool inline CurrentKey( const std::uint32_t index ) const	{ return( m_keys[ index ] !=0 ); }
	bool inline OldKey( const std::uint32_t index ) const 		{ return( m_oldKeys[ index ] !=0 ); }

	bool inline CurrentMouse( const std::uint32_t button ) const	{ return( ( m_buttons & SDL_BUTTON( button ) ) != 0 ); }
	bool inline OldMouse( const std::uint32_t button ) const		{ return( ( m_oldButtons & SDL_BUTTON( button ) ) != 0 ); }
};

#endif // CINPUT_HPP