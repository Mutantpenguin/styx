#include "CSDL.hpp"

#include <SDL2/SDL.h>

#include "../logger/CLogger.hpp"

CSDL::CSDL()
{
	// get some version-infos so we can check if the right version of SDL is beeing used
	SDL_version version_linked;
	SDL_version version_compiled;

	SDL_GetVersion( &version_linked );
	SDL_VERSION( &version_compiled );

	if( SDL_COMPILEDVERSION != SDL_VERSIONNUM( version_linked.major, version_linked.minor, version_linked.patch ) )
	{
		logERROR( "SDL has version '{0}.{1}.{2}' but expected was version '{3}.{4}.{5}'", static_cast< unsigned short >( version_linked.major ), static_cast< unsigned short >( version_linked.minor ), static_cast< unsigned short >( version_linked.patch ), SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL );
		throw std::exception();
	}

	logDEBUG( "SDL has version '{0}.{1}.{2}'", static_cast< unsigned short >( version_linked.major ), static_cast< unsigned short >( version_linked.minor ), static_cast< unsigned short >( version_linked.patch ) );

	if( SDL_Init( 0 ) )
	{
		logERROR( "initialising SDL failed: {0}", SDL_GetError() );
		throw std::exception();
	}

	#ifdef WIN32
		// redirect the output to the console because SDL would redirect it to the files stdout.txt and stderr.txt
		if( std::freopen( "CON", "w", stdout ) == nullptr )
		{
			logERROR( "couldn't redirect stdout" );
			throw std::exception();
		}
		if( std::freopen( "CON", "w", stderr ) == nullptr )
		{
			logERROR( "couldn't redirect stdout" );
			throw std::exception();
		}
	#endif
}

CSDL::~CSDL()
{
	SDL_Quit();
}
