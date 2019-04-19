#pragma once

#include <set>
#include <string>

#include "src/renderer/GL.h"

class COpenGlAdapter final
{
public:
	COpenGlAdapter();

	GLint MaxTextureSize() const;
	GLint MaxCubeMapTextureSize() const;

	GLint PreferredInternalTextureFormat2D() const;
	GLint PreferredInternalTextureFormatCube() const;
	GLint PreferredInternalTextureFormat2DArray() const;
	
private:
	bool isSupported( const std::set< GLextension > &extensions, const GLextension extension ) const;

	GLint m_maxTextureSize;
	GLint m_maxCubeMapTextureSize;

	GLint m_preferredInternalTextureFormat2D;
	GLint m_preferredInternalTextureFormatCube;
	GLint m_preferredInternalTextureFormat2DArray;
};
