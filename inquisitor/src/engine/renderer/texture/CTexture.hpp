#ifndef CTEXTURE_HPP
#define CTEXTURE_HPP

#include "src/engine/renderer/GL.h"

class CTexture final
{
	friend class CTextureLoader;
	friend class CTextureManager;
	friend class CFrameBuffer;

private:
	CTexture( const CTexture& rhs );
	CTexture & operator = ( const CTexture& rhs );

public:
	enum struct type : std::uint8_t
	{
		TEX_2D = 0,
		TEX_CUBE_MAP,
		TEX_2D_ARRAY
	};

	CTexture( void );
	~CTexture( void );

	void BindToUnit( const GLuint unit ) const;

	type Type( void ) const;

private:
	void Reset( void );

	type m_type;

	GLuint	m_textureID;
};

#endif // CTEXTURE_HPP
