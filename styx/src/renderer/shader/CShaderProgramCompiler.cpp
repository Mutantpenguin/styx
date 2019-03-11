#include "CShaderProgramCompiler.hpp"

#include <glbinding-aux/Meta.h>

#include "src/logger/CLogger.hpp"

#include "src/renderer/shader/CShaderCompiler.hpp"

constexpr const GLint CShaderProgramCompiler::requiredCombinedTextureImageUnits;

bool CShaderProgramCompiler::Compile( const std::shared_ptr<CShaderProgram> &shaderProgram, const std::shared_ptr<const CShader> &vertexShader, const std::shared_ptr<const CShader> &fragmentShader ) const
{
	shaderProgram->GLID = glCreateProgram();
	if( shaderProgram->GLID )
	{
		logWARNING( "Error creating program object" );
		return( false );
	}

	glAttachShader( shaderProgram->GLID, vertexShader->GLID );
	glAttachShader( shaderProgram->GLID, fragmentShader->GLID );

	glLinkProgram( shaderProgram->GLID );

	GLint compileResult;
	glGetProgramiv( shaderProgram->GLID, GL_LINK_STATUS, &compileResult );

	if( compileResult != static_cast<GLint>( GL_TRUE ) )
	{
		int infoLogLength;
		glGetProgramiv( shaderProgram->GLID, GL_INFO_LOG_LENGTH, &infoLogLength );
		std::vector<char> errorMessage( infoLogLength );
		glGetProgramInfoLog( shaderProgram->GLID, infoLogLength, nullptr, errorMessage.data() );

		logWARNING( "Error linking program: {0}", errorMessage.data() );

		shaderProgram->Reset();

		return( false );
	}

	if( !SetupInterface( shaderProgram ) )
	{
		logWARNING( "unable to setup the interface" );

		shaderProgram->Reset();
		
		return( false );
	}

	return( true );
}

bool CShaderProgramCompiler::SetupInterface( const std::shared_ptr<CShaderProgram> &shaderProgram ) const
{
	/*
	 * active attributes
	 */
	GLint numActiveAttributes = 0;
	glGetProgramInterfaceiv( shaderProgram->GLID, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numActiveAttributes );
	const std::array<GLenum, 3> attributeProperties{ { GL_TYPE, GL_NAME_LENGTH, GL_LOCATION } };

	for( GLint attribIndex = 0; attribIndex < numActiveAttributes; ++attribIndex )
	{
		GLint values[ attributeProperties.size() ];
		glGetProgramResourceiv( shaderProgram->GLID, GL_PROGRAM_INPUT, attribIndex, attributeProperties.size(), attributeProperties.data(), attributeProperties.size(), NULL, values );

		const GLint attributeLocation = values[ 2 ];

		const auto attributeIt = CShaderCompiler::AllowedAttributes.find( static_cast<CVAO::EAttributeLocation>( attributeLocation ) );
		if( std::end( CShaderCompiler::AllowedAttributes ) == attributeIt )
		{
			logERROR( "attribute location '{0}' is not allowed", attributeLocation );
			return( false );
		}
		else
		{
			std::vector<char> nameData( values[ 1 ] );
			glGetProgramResourceName( shaderProgram->GLID, GL_PROGRAM_INPUT, attribIndex, nameData.size(), nullptr, &nameData[ 0 ] );
			const std::string attributeName( nameData.data() );

			const GLenum attributeType = static_cast<GLenum>( values[ 0 ] );
			const SShaderInterface &attributeInterface = attributeIt->second;
			if( ( attributeInterface.name != attributeName )
				||
				( attributeInterface.type != attributeType ) )
			{
				logERROR( "attribute '{0}' with type {1} is not allowed at location '{2}'", attributeName, glbinding::aux::Meta::getString( attributeType ), attributeLocation );
				return( false );
			}
		}
	}

	/*
	 * active non-block uniforms
	 */
	GLint numActiveUniforms = 0;
	glGetProgramInterfaceiv( shaderProgram->GLID, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numActiveUniforms );
	static const std::array<GLenum, 4> uniformProperties{ { GL_BLOCK_INDEX, GL_TYPE, GL_NAME_LENGTH, GL_LOCATION } };

	for( GLint uniformIndex = 0; uniformIndex < numActiveUniforms; ++uniformIndex )
	{
		GLint values[ uniformProperties.size() ];
		glGetProgramResourceiv( shaderProgram->GLID, GL_UNIFORM, uniformIndex, uniformProperties.size(), uniformProperties.data(), uniformProperties.size(), nullptr, &values[ 0 ] );

		// skip any uniforms that are in a block.
		if( values[ 0 ] != -1 )
		{
			continue;
		}

		std::vector<char> nameData( values[ 2 ] );
		glGetProgramResourceName( shaderProgram->GLID, GL_UNIFORM, uniformIndex, nameData.size(), NULL, &nameData[ 0 ] );
		const std::string uniformName( nameData.data() );

		const GLint  uniformLocation = values[ 3 ];
		const GLenum uniformType = static_cast<GLenum>( values[ 1 ] );

		switch( uniformType )
		{
		case GL_SAMPLER_2D:
		case GL_SAMPLER_CUBE:
		case GL_SAMPLER_2D_ARRAY:
			shaderProgram->RequiredSamplers().emplace_back( std::make_pair( uniformLocation, SShaderInterface{ uniformName, uniformType } ) );
			if( shaderProgram->RequiredSamplers().size() > CShaderProgramCompiler::requiredCombinedTextureImageUnits )
			{
				logERROR( "uses {0} samplers but max {1} are allowed", shaderProgram->RequiredSamplers().size(), CShaderProgramCompiler::requiredCombinedTextureImageUnits );
				return( false );
			}
			break;

		case GL_FLOAT_MAT3:
		case GL_FLOAT_MAT4:
		case GL_FLOAT_VEC2:
		case GL_FLOAT_VEC3:
		case GL_FLOAT_VEC4:
		case GL_UNSIGNED_INT:
		case GL_INT:
		case GL_FLOAT:
		{
			const auto engineUniformIt = std::find_if( std::cbegin( CShaderCompiler::EngineUniforms ),
				std::cend( CShaderCompiler::EngineUniforms ),
				[ & ]( const auto &uniform )
			{
				return( uniform.second.name == uniformName );
			} );

			if( std::cend( CShaderCompiler::EngineUniforms ) != engineUniformIt )
			{
				// uniform gets provided by the engine

				const SShaderInterface &uniformInterface = engineUniformIt->second;
				if( uniformInterface.type != uniformType )
				{
					logERROR( "uniform '{0}' needs to be of type '{1}' but was declared as '{2}'", uniformName, glbinding::aux::Meta::getString( uniformInterface.type ), glbinding::aux::Meta::getString( uniformType ) );
					return( false );
				}
				else
				{
					shaderProgram->RequiredEngineUniforms().emplace_back( std::make_pair( uniformLocation, engineUniformIt->first ) );
				}
			}
			else
			{
				// uniform gets provided by the material

				shaderProgram->RequiredMaterialUniforms().emplace_back( std::make_pair( uniformLocation, SShaderInterface{ uniformName, uniformType } ) );
			}
			break;
		}

		default:
			logERROR( "unsupported uniform type {0} for uniform '{1}'", glbinding::aux::Meta::getString( uniformType ), uniformName );
			return( false );
		}
	}

	return( true );
}