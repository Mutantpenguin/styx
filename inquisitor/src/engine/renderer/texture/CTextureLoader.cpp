#include "CTextureLoader.hpp"

#include <json/json.h>

#include <glbinding/Meta.h>

#include "src/engine/logger/CLogger.hpp"

#include "src/engine/helper/image/ImageHandler.hpp"

CTextureLoader::CTextureLoader( const CSettings &p_settings, const CFileSystem &p_filesystem, const CRendererCapabilities &rendererCapabilities ) :
	m_filesystem { p_filesystem },
	// clamp the value so we don't get too bad texture-quality
	m_iPicMip { std::min( p_settings.renderer.textures.picmip, MAX_PICMIP ) }
{
	// look out for the maximal texture size
	glGetIntegerv( GL_MAX_TEXTURE_SIZE, &m_iMaxTextureSize );
	logDEBUG( "{0} is '{1}'", glbinding::Meta::getString( GL_MAX_TEXTURE_SIZE ), m_iMaxTextureSize );

	// look out for the maximal cubemap texture size
	glGetIntegerv( GL_MAX_CUBE_MAP_TEXTURE_SIZE, &m_iMaxCubeMapTextureSize );
	logDEBUG( "{0} is '{1}'", glbinding::Meta::getString( GL_MAX_CUBE_MAP_TEXTURE_SIZE ), m_iMaxCubeMapTextureSize );

	if( rendererCapabilities.isSupported( GLextension::GL_ARB_internalformat_query2 ) )
	{
		logDEBUG( "using {0} for internal texture formats", glbinding::Meta::getString( GLextension::GL_ARB_internalformat_query2 ) );

		glGetInternalformativ( GL_TEXTURE_2D,		GL_RGBA8, GL_INTERNALFORMAT_PREFERRED, 1, &m_internalTextureFormat2D );
		glGetInternalformativ( GL_TEXTURE_CUBE_MAP,	GL_RGBA8, GL_INTERNALFORMAT_PREFERRED, 1, &m_internalTextureFormatCube );
		glGetInternalformativ( GL_TEXTURE_2D_ARRAY,	GL_RGBA8, GL_INTERNALFORMAT_PREFERRED, 1, &m_internalTextureFormat2DArray );
	}
}


std::shared_ptr< CTexture > CTextureLoader::CreateTextureFromFile( const std::string &path ) const
{
	// TODO implement loading of compressed images in our own format

	if( !m_filesystem.Exists( path ) )
	{
		logWARNING( "'{0}' does not exist", path );
		return( nullptr );
	}
	else
	{
		const std::string fileExtension = path.substr( path.find_last_of( "." ) + 1 );

		if( fileExtension == std::string( "cub" ) )
		{
			return( CreateCubeTextureFromFile( path ) );
		}
		else if( fileExtension == std::string( "arr" ) )
		{
			return( Create2DArrayTextureFromFile( path ) );
		}
		else
		{
			return( Create2DTextureFromFile( path ) );
		}
	}
}

std::shared_ptr< CTexture > CTextureLoader::Create2DTextureFromFile( const std::string &path ) const
{
	const std::shared_ptr< const CImage > image = ImageHandler::Load( m_filesystem, path, m_iMaxTextureSize, m_iPicMip, false );

	if( !image )
	{
		logWARNING( "image '{0}' couldn't be loaded", path );
		return( nullptr );
	}
	else
	{
		return( std::make_shared< CTexture >( image, m_internalTextureFormat2D ) );
	}
}

std::shared_ptr< CTexture > CTextureLoader::CreateCubeTextureFromFile( const std::string &path ) const
{
	Json::Value		root;
	Json::Reader	reader;
	if ( !reader.parse( m_filesystem.LoadTextFileToBuffer( path ), root ) )
	{
		logWARNING( "failed to parse '{0}' because of {1}", path, reader.getFormattedErrorMessages() );
		return( nullptr );
	}

	const Json::Value json_faces = root[ "faces" ];

	if(	json_faces.empty() )
	{
		logWARNING( "no faces defined in '{0}'", path );
		return( nullptr );
	}
	else if( json_faces.size() < CCubemapData::countCubemapFaces )
	{
		logWARNING( "there are only {0} faces defined in '{1}'", json_faces.size(), path );
		return( nullptr );
	}
	else if( json_faces.size() > CCubemapData::countCubemapFaces )
	{
		logWARNING( "there are too many ( {0} ) faces defined in '{1}'", json_faces.size(), path );
		return( nullptr );
	}

	const std::string path_to_faces = path.substr( 0, path.find_last_of( CFileSystem::GetDirSeparator() ) + 1 );

	auto cubemapData = std::make_unique< CCubemapData >();

	for( std::uint8_t faceNum = 0; faceNum < CCubemapData::countCubemapFaces; ++faceNum )
	{
		const std::string path_to_face = path_to_faces + json_faces[ faceNum ].asString();
		const std::shared_ptr< const CImage > image = ImageHandler::Load( m_filesystem, path_to_face, m_iMaxCubeMapTextureSize, m_iPicMip, true );
		if( nullptr == image )
		{
			logWARNING( "failed to load image '{0}' for cubemap '{1}'", path_to_face, path );
			return( nullptr );
		}
		else
		{
			if( !cubemapData->AddFace( faceNum, image ) )
			{
				logWARNING( "failed to add face '{0}' for cubemap '{1}'", json_faces[ faceNum ].asString(), path );
				return( nullptr );
			}
		}
	}

	if( cubemapData->isComplete() )
	{
		return( std::make_shared< CTexture >( std::move( cubemapData ), m_internalTextureFormatCube ) );
	}
	else
	{
		logWARNING( "incomplete cubemap for '{0}'", path );
		return( nullptr );
	}
}

std::shared_ptr< CTexture > CTextureLoader::Create2DArrayTextureFromFile( const std::string &path ) const
{
	Json::Value		root;
	Json::Reader	reader;
	if( !reader.parse( m_filesystem.LoadTextFileToBuffer( path ), root ) )
	{
		logWARNING( "failed to parse '{0}' because of {1}", path, reader.getFormattedErrorMessages() );
		return( nullptr );
	}

	const Json::Value json_layers = root[ "layers" ];

	if( json_layers.empty() )
	{
		logWARNING( "no layers defined in '{0}'", path );
		return( nullptr );
	}
	else if( json_layers.size() > UINT8_MAX )
	{
		logWARNING( "more than the maximum of {0} layers defined in '{1}'", UINT8_MAX, path );
		return( nullptr );
	}

	const std::string path_to_layers = path.substr( 0, path.find_last_of( CFileSystem::GetDirSeparator() ) + 1 );

	auto arrayData = std::make_unique< C2DArrayData >();

	for( const Json::Value &layer : json_layers )
	{
		const std::string path_to_layer = path_to_layers + layer.asString();
		const std::shared_ptr< const CImage > image = ImageHandler::Load( m_filesystem, path_to_layer, m_iMaxTextureSize, m_iPicMip, false );

		if( nullptr == image )
		{
			logWARNING( "failed to load image '{0}' for array texture '{1}'", path_to_layer, path );
			return( nullptr );
		}
		else
		{
			if( !arrayData->AddLayer( image ) )
			{
				logWARNING( "failed to add layer '{0}' for array texture '{1}'", layer.asString(), path );
				return( nullptr );
			}
		}
	}

	return( std::make_shared< CTexture >( std::move( arrayData ), m_internalTextureFormat2DArray ) );
}

std::shared_ptr< CTexture > CTextureLoader::CreateDummyTexture( void ) const
{
	// Creates a checkerboard-like dummy-texture

	const std::shared_ptr< const CImage > image = ImageHandler::GenerateCheckerImage( CSize( 64, 64 ) );

	if( !image )
	{
		logERROR( "checker-image for the dummy-texture couldn't be generated" );
		return( nullptr );
	}
	else
	{
		return( std::make_shared< CTexture >( image, m_internalTextureFormat2D ) );
	}
}
