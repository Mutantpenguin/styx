#include "CRenderer.hpp"

#include <algorithm>

#include <glm/gtx/norm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glbinding/Meta.h>

#include "src/engine/renderer/CGLState.hpp"

#include "src/engine/logger/CLogger.hpp"

CRenderer::CRenderer( const CSettings &settings, const CFileSystem &filesystem )
	try :
		m_settings { settings },
		m_samplerManager( settings ),
		m_materialmanager( settings, filesystem, m_samplerManager, m_rendererCapabilities )
{
	// TODO somehow put this into the OpenGlAdapter
	#ifdef INQ_DEBUG
		if( m_rendererCapabilities.isSupported( GLextension::GL_KHR_debug )
			||
			m_rendererCapabilities.isSupported( GLextension::GL_ARB_debug_output ) )
		{
			logINFO( "OpenGL debug output enabled" );

			glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );

			glDebugMessageCallback(	[]( GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei, const GLchar* message, const void* )
									{
										const e_loglevel loglvl = ( GL_DEBUG_SEVERITY_HIGH == severity ) ? e_loglevel::ERROR : e_loglevel::WARNING;

										LOG( loglvl, "OpenGL ERROR" );
										LOG( loglvl, "\tSource   : {0}", glbinding::Meta::getString( source ) );
										LOG( loglvl, "\tType     : {0}", glbinding::Meta::getString( type ) );
										LOG( loglvl, "\tID       : {0}", id );
										LOG( loglvl, "\tSeverity : {0}", glbinding::Meta::getString( severity ) );
										LOG( loglvl, "\tMessage  : {0}", message );
									}, nullptr );
		}
		else
		{
			logWARNING( "neither {0} nor {1} are available", glbinding::Meta::getString( GLextension::GL_KHR_debug ), glbinding::Meta::getString( GLextension::GL_ARB_debug_output ) );
		}
	#endif


	logINFO( "required OpenGL extensions:" );
	const auto requiredOpenGLExtensions = {	GLextension::GL_EXT_texture_filter_anisotropic,
											// TODO not needed anymore when we can switch to a 4.2 core context (or higher)
											GLextension::GL_ARB_shading_language_420pack,
											GLextension::GL_ARB_texture_storage,
											// TODO not needed anymore when we can switch to a 4.3 core context (or higher)
											GLextension::GL_ARB_program_interface_query,
											// TODO not needed anymore when we can switch to a 4.5 core context (or higher)
											GLextension::GL_ARB_direct_state_access };

	for( const auto &extension : requiredOpenGLExtensions )
	{
		if( !m_rendererCapabilities.isSupported( extension ) )
		{
			logERROR( "\t{0} is MISSING", glbinding::Meta::getString( extension ) );
			throw Exception();
		}
		else
		{
			logINFO( "\t{0} is available", glbinding::Meta::getString( extension ) );
		}
	}


	const bool supports_GL_NVX_gpu_memory_info = m_rendererCapabilities.isSupported( GLextension::GL_NVX_gpu_memory_info );
	const bool supports_GL_ATI_meminfo         = m_rendererCapabilities.isSupported( GLextension::GL_ATI_meminfo );

	logINFO( "video memory:" );
	if( !supports_GL_NVX_gpu_memory_info && !supports_GL_ATI_meminfo )
	{
		logINFO( "\tno information available" );
		logDEBUG( "\tneither {0} nor {1} are supported", glbinding::Meta::getString( GLextension::GL_NVX_gpu_memory_info ), glbinding::Meta::getString( GLextension::GL_ATI_meminfo ) );
	}
	else
	{
		if( supports_GL_NVX_gpu_memory_info )
		{
			logDEBUG( "\t{0} is supported", glbinding::Meta::getString( GLextension::GL_NVX_gpu_memory_info ) );

			GLint dedicatedMemKb = 0;
			glGetIntegerv( GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &dedicatedMemKb );

			GLint totalAvailableMemKb = 0;
			glGetIntegerv( GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &totalAvailableMemKb );

			GLint currentlyAvailableMemKb = 0;
			glGetIntegerv( GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &currentlyAvailableMemKb );

			logINFO( "\ttotal dedicated:     {0} MiB", dedicatedMemKb / 1024 );
			logINFO( "\ttotal available:     {0} MiB", totalAvailableMemKb / 1024 );
			logINFO( "\tcurrently available: {0} MiB", currentlyAvailableMemKb / 1024 );
		}

		if( supports_GL_ATI_meminfo )
		{
			logDEBUG( "\t{0} is supported", glbinding::Meta::getString( GLextension::GL_ATI_meminfo ) );

			GLint vboFreeMemKb = 0;
			glGetIntegerv( GL_VBO_FREE_MEMORY_ATI, &vboFreeMemKb );

			GLint textureFreeMemKb = 0;
			glGetIntegerv( GL_TEXTURE_FREE_MEMORY_ATI, &textureFreeMemKb );

			GLint renderbufferFreeMemKb = 0;
			glGetIntegerv( GL_RENDERBUFFER_FREE_MEMORY_ATI, &renderbufferFreeMemKb );

			logINFO( "\tfree for VBOs:           {0} MiB", vboFreeMemKb / 1024 );
			logINFO( "\tfree for textures:       {0} MiB", textureFreeMemKb / 1024 );
			logINFO( "\tfree for render buffers: {0} MiB", renderbufferFreeMemKb / 1024 );
		}
	}


	glDepthFunc( GL_LEQUAL );
	glEnable( GL_DEPTH_TEST );

	glEnable( GL_TEXTURE_CUBE_MAP_SEAMLESS );

	CreateUniformBuffers();
}
catch( CMaterialManager::Exception &e )
{
	logERROR( "unable to initialize MaterialManager" );
	throw Exception();
}
catch( COpenGlAdapter::Exception &e )
{
	logERROR( "unable to initialize OpenGlAdapter" );
	throw Exception();
}

void CRenderer::CreateUniformBuffers( void )
{
	const std::string cameraBody =	"vec3 position;" \
									"vec3 direction;" \
									"mat4 projectionMatrix;" \
									"mat4 viewMatrix;" \
									"mat4 viewProjectionMatrix;";

	m_uboCamera = std::make_shared< CUniformBuffer >( ( 2 * sizeof( glm::vec4 ) ) + ( 3 * sizeof( glm::mat4 ) ), GL_DYNAMIC_DRAW, EUniformBufferLocation::CAMERA, "Camera", cameraBody );
	m_materialmanager.ShaderManager().RegisterUniformBuffer( m_uboCamera );

	const std::string timerBody = "uint time;";

	m_uboTimer = std::make_shared< CUniformBuffer >( sizeof( glm::uint ), GL_DYNAMIC_DRAW, EUniformBufferLocation::TIME, "Timer", timerBody );
	m_materialmanager.ShaderManager().RegisterUniformBuffer( m_uboTimer );
}

void CRenderer::UpdateUniformBuffers( const std::shared_ptr< const CCamera > &camera, const std::uint64_t time ) const
{
	/*
	 * Update time into the uniform buffer
	 */

	const glm::uint timeMilliseconds = static_cast< glm::uint >( time / 1000 );
	m_uboTimer->SubData( 0,	sizeof( glm::uint ), &timeMilliseconds );

	/*
	 * Update calculated values into the uniform buffer
	 */

	const glm::vec3 &position = camera->Position();
	const glm::vec3 &direction = camera->Direction();
	const glm::mat4 &projectionMatrix = camera->CalculateProjectionMatrix();
	const glm::mat4 &viewMatrix = camera->CalculateViewMatrix();
	const glm::mat4 &viewProjectionMatrix = camera->CalculateViewProjectionMatrix();

	std::uint16_t offset = 0;
	m_uboCamera->SubData( offset,	sizeof( position ),				glm::value_ptr( position ) );
	offset += sizeof( glm::vec4 );
	m_uboCamera->SubData( offset,	sizeof( direction ),			glm::value_ptr( direction ) );
	offset += sizeof( glm::vec4 );
	m_uboCamera->SubData( offset,	sizeof( projectionMatrix ),		glm::value_ptr( projectionMatrix ) );
	offset += sizeof( glm::mat4 );
	m_uboCamera->SubData( offset,	sizeof( viewMatrix ),			glm::value_ptr( viewMatrix ) );
	offset += sizeof( glm::mat4 );
	m_uboCamera->SubData( offset,	sizeof( viewProjectionMatrix ),	glm::value_ptr( viewProjectionMatrix ) );
}

void CRenderer::Update( void )
{
	m_materialmanager.Update();
}

std::shared_ptr< CImage > CRenderer::GetScreenshot( void ) const
{
	const CSize &size			= m_settings.renderer.window.size;
	const std::uint32_t pitch	= size.width * 3;

	auto pixels = std::make_unique< CImage::PixelBuffer >( pitch * size.height );

	glReadPixels( 0, 0, size.width, size.height, GL_BGR, GL_UNSIGNED_BYTE, static_cast< void* >( pixels->data() ) );

	return( std::make_shared< CImage >( size, size, true, 24, pitch, std::move( pixels ) ) );
}

std::shared_ptr< CMaterial > CRenderer::LoadMaterial( const std::string &path )
{
	return( m_materialmanager.LoadMaterial( path ) );
}

void CRenderer::SetClearColor( const CColor &color )
{
	glClearColor( color.r(), color.g(), color.b(), color.a() );
}

void CRenderer::Clear( bool colorBuffer, bool depthBuffer ) const
{
	if( colorBuffer && depthBuffer )
	{
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	}
	else if( colorBuffer )
	{
		glClear( GL_COLOR_BUFFER_BIT );
	}
	else if( depthBuffer )
	{
		glClear( GL_DEPTH_BUFFER_BIT );
	}
}

void CRenderer::RenderScene( const CScene &scene, const std::uint64_t time ) const
{
	const auto camera = scene.Camera();

	if( !camera )
	{
		logWARNING( "scene has no camera" );
	}
	else
	{
		const CFrustum &frustum = camera->CalculateFrustum();

		UpdateUniformBuffers( camera, time );

		/*
		 * set up the renderqueues
		 */

		// Allocate enough memory to hold every object.
		// This wastes quite a bit of memory, but has the benefit that we only need to allocate memory once.
		TRenderQueue renderQueueOpaque;
		renderQueueOpaque.reserve( scene.Meshes().size() );
		TRenderQueue renderQueueTranslucent;
		renderQueueTranslucent.reserve( scene.Meshes().size() );

		for( const std::shared_ptr< const CMesh > &mesh : scene.Meshes() )
		{
			if( frustum.IsSphereInside( mesh->Position(), mesh->BoundingSphereRadius() ) )
			{
				if( mesh->Material()->Blending() )
				{
					renderQueueTranslucent.push_back( mesh );
				}
				else
				{
					renderQueueOpaque.push_back( mesh );
				}
			}
		}

		const glm::vec3 &cameraPosition = camera->Position();
		const glm::mat4 viewProjectionMatrix = camera->CalculateViewProjectionMatrix();

		// sort opaque front to back
		std::sort( renderQueueOpaque.begin(), renderQueueOpaque.end(),	[&]( const std::shared_ptr< const CMesh > &a, const std::shared_ptr< const CMesh > &b ) -> bool
																		{
																			return( glm::length2( a->Position() - cameraPosition ) < glm::length2( b->Position() - cameraPosition ) );
																		} );

		// sort translucent back to front
		std::sort( renderQueueTranslucent.begin(), renderQueueTranslucent.end(),	[&]( const std::shared_ptr< const CMesh > &a, const std::shared_ptr< const CMesh > &b ) -> bool
																					{
																						return( glm::length2( a->Position() - cameraPosition ) > glm::length2( b->Position() - cameraPosition ) );
																					} );

		for( const std::shared_ptr< const CMesh > &mesh : renderQueueOpaque )
		{
			RenderMesh( viewProjectionMatrix, mesh );
		}

		for( const std::shared_ptr< const CMesh > &mesh : renderQueueTranslucent )
		{
			RenderMesh( viewProjectionMatrix, mesh );
		}
	}
}

void CRenderer::RenderMesh( const glm::mat4 &viewProjectionMatrix, const std::shared_ptr< const CMesh > &mesh ) const
{
	const std::shared_ptr< const CMaterial > material = mesh->Material();

	material->Setup();

	const CVAO &vao = mesh->VAO();

	vao.Bind();

	const auto shader = material->Shader();

	shader->Use();

	for( const auto &requiredEngineUniform : shader->RequiredEngineUniforms() )
	{
		const auto location = requiredEngineUniform.first;

		switch( requiredEngineUniform.second )
		{
			case EEngineUniform::modelViewProjectionMatrix:
				glUniformMatrix4fv( location, 1, GL_FALSE, &( viewProjectionMatrix * mesh->ModelMatrix() )[ 0 ][ 0 ] );
				break;

			case EEngineUniform::modelMatrix:
				glUniformMatrix4fv( location, 1, GL_FALSE, &mesh->ModelMatrix()[ 0 ][ 0 ] );
				break;
		}
	}

	std::uint8_t textureUnit = 0;
	for( const auto &samplerData : material->SamplerData() )
	{
		const auto location = samplerData.first;

		glUniform1i( location, textureUnit );

		samplerData.second->BindToUnit( textureUnit );

		textureUnit++;
	}

	for( const auto &uniform : material->MaterialUniforms() )
	{
		const auto location = uniform.first;

		uniform.second->Set( location );
	}

	vao.Draw();
}
