#include "CEngineInterface.hpp"

CEngineInterface::CEngineInterface( const CSettings &settings, const CFileSystem &filesystem ) :
	Input( settings, filesystem ),
	Renderer( settings, filesystem ),
	SoundManager( settings, filesystem )
{
}

CEngineInterface::~CEngineInterface()
{
}
