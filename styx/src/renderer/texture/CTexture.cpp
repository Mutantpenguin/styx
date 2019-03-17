#include "CTexture.hpp"

#include "src/renderer/CGLState.hpp"

CTexture::~CTexture()
{
	if( glIsTexture( GLID ) )
	{
		glDeleteTextures( 1, &GLID );
	}
}

void CTexture::BindToUnit( const GLuint unit ) const
{
	CGLState::BindTextureToUnit( GLID, unit );
}

CTexture::EType CTexture::Type() const
{
	return( m_type );
}

void CTexture::Type( const EType type )
{
	m_type = type;
}

void CTexture::Reset()
{
	if( glIsTexture( GLID ) )
	{
		glDeleteTextures( 1, &GLID );
	}

	GLID = 0;
}