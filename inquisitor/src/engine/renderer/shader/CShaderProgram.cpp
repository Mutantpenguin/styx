#include "CShaderProgram.hpp"

#include "src/engine/renderer/CGLState.hpp"

CShaderProgram::CShaderProgram( GLuint program ) :
	m_program { program }
{
}

CShaderProgram::~CShaderProgram()
{
	if( glIsProgram( m_program ) == GL_TRUE )
	{
		glDeleteProgram( m_program );
	}
}

void CShaderProgram::Use( void ) const
{
	CGLState::UseProgram( m_program );
}

const std::vector< std::pair< GLint, const SShaderInterface > > &CShaderProgram::RequiredSamplers( void ) const
{
	return( m_requiredSamplers );
}

const std::vector< std::pair< GLint, const EEngineUniform > > &CShaderProgram::RequiredEngineUniforms( void ) const
{
	return( m_requiredEngineUniforms );
}

const std::vector< std::pair< GLint, const SShaderInterface > > &CShaderProgram::RequiredMaterialUniforms( void ) const
{
	return( m_requiredMaterialUniforms );
}
