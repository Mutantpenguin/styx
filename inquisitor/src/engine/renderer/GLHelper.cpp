#include "GLHelper.hpp"

#include <glbinding/Meta.h>

#include "../logger/CLogger.hpp"

namespace GLHelper
{
	bool DstBlendFuncFromString( const std::string &string, GLenum &dstblend )
	{
		if( string == "ZERO" )
		{
			dstblend = GL_ZERO;
		}
		else if( string == "ONE" )
		{
			dstblend = GL_ONE;
		}
		else if( string == "SRC_COLOR" )
		{
			dstblend = GL_DST_COLOR;
		}
		else if( string == "ONE_MINUS_SRC_COLOR" )
		{
			dstblend = GL_ONE_MINUS_DST_COLOR;
		}
		else if( string == "SRC_ALPHA" )
		{
			dstblend = GL_SRC_ALPHA;
		}
		else if( string == "ONE_MINUS_SRC_ALPHA" )
		{
			dstblend = GL_ONE_MINUS_SRC_ALPHA;
		}
		else if( string == "DST_ALPHA" )
		{
			dstblend = GL_DST_ALPHA;
		}
		else if( string == "ONE_MINUS_DST_ALPHA" )
		{
			dstblend = GL_ONE_MINUS_DST_ALPHA;
		}
		else if( string == "CONSTANT_COLOR" )
		{
			dstblend = GL_CONSTANT_COLOR;
		}
		else if( string == "ONE_MINUS_CONSTANT_COLOR" )
		{
			dstblend = GL_ONE_MINUS_CONSTANT_COLOR;
		}
		else if( string == "CONSTANT_ALPHA" )
		{
			dstblend = GL_CONSTANT_ALPHA;
		}
		else if( string == "ONE_MINUS_CONSTANT_ALPHA" )
		{
			dstblend = GL_ONE_MINUS_CONSTANT_ALPHA;
		}
		else
		{
			LOG( logWARNING ) << "unknown blend-function '" << string << "'";
			return( false );
		}

		return( true );
	}

	bool SrcBlendFuncFromString( const std::string &string, GLenum &srcblend )
	{
		if( string == "ZERO" )
		{
			srcblend = GL_ZERO;
		}
		else if( string == "ONE" )
		{
			srcblend = GL_ONE;
		}
		else if( string == "DST_COLOR" )
		{
			srcblend = GL_DST_COLOR;
		}
		else if( string == "ONE_MINUS_DST_COLOR" )
		{
			srcblend = GL_ONE_MINUS_DST_COLOR;
		}
		else if( string == "SRC_ALPHA" )
		{
			srcblend = GL_SRC_ALPHA;
		}
		else if( string == "ONE_MINUS_SRC_ALPHA" )
		{
			srcblend = GL_ONE_MINUS_SRC_ALPHA;
		}
		else if( string == "DST_ALPHA" )
		{
			srcblend = GL_DST_ALPHA;
		}
		else if( string == "ONE_MINUS_DST_ALPHA" )
		{
			srcblend = GL_ONE_MINUS_DST_ALPHA;
		}
		else if( string == "SRC_ALPHA_SATURATE" )
		{
			srcblend = GL_SRC_ALPHA_SATURATE;
		}
		else if( string == "CONSTANT_COLOR" )
		{
			srcblend = GL_CONSTANT_COLOR;
		}
		else if( string == "ONE_MINUS_CONSTANT_COLOR" )
		{
			srcblend = GL_ONE_MINUS_CONSTANT_COLOR;
		}
		else if( string == "CONSTANT_ALPHA" )
		{
			srcblend = GL_CONSTANT_ALPHA;
		}
		else if( string == "ONE_MINUS_CONSTANT_ALPHA" )
		{
			srcblend = GL_ONE_MINUS_CONSTANT_ALPHA;
		}
		else
		{
			LOG( logWARNING ) << "unknown blend-function '" << string << "'";
			return( false );
		}

		return( true );
	}

	bool FaceModeFromString( const std::string &string, GLenum &facemode )
	{
		if( string == "FRONT" )
		{
			facemode = GL_FRONT;
		}
		else if( string == "BACK" )
		{
			facemode = GL_BACK;
		}
		else if( string == "FRONT_AND_BACK" )
		{
			facemode = GL_FRONT_AND_BACK;
		}
		else
		{
			LOG( logWARNING ) << "unknown cull-mode '" << string << "'";
			return( false );
		}

		return( true );
	}

	bool PolygonModeFromString( const std::string &string, GLenum &polygonmode )
	{
		if( string == "POINT" )
		{
			polygonmode = GL_POINT;
		}
		else if( string == "LINE" )
		{
			polygonmode = GL_LINE;
		}
		else if( string == "FILL" )
		{
			polygonmode = GL_FILL;
		}
		else
		{
			LOG( logWARNING ) << "unknown cull-mode '" << string << "'";
			return( false );
		}

		return( true );
	}

	bool isFramebufferComplete( void )
	{
		const GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );

		if( GL_FRAMEBUFFER_COMPLETE == status )
		{
			return( true );
		}
		else
		{
			LOG( logERROR ) << glbinding::Meta::getString( status );
			return( false );
		}
	}

	GLenum GLFormatFromImage( const std::shared_ptr< const CImage > &image )
	{
		if( image->HasAlpha() )
		{
			return( GL_BGRA );
		}
		else
		{
			return( GL_BGR );
		}
	}
}
