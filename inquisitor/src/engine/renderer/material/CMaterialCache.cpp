#include "CMaterialCache.hpp"

#include "src/engine/logger/CLogger.hpp"

CMaterialCache::CMaterialCache( const CFileSystem &filesystem, CShaderManager &shaderManager ) :
	CResourceCache( "material", filesystem ),
	m_materialLoader( filesystem, shaderManager )
{
}

void CMaterialCache::LoadFromFile( const std::shared_ptr< CMaterial > &resource, const std::string &path )
{
	m_materialLoader.FromFile( resource, path );
}