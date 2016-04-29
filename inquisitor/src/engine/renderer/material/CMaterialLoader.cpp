#include "CMaterialLoader.hpp"

#include <json/json.h>

#include "../../logger/CLogger.hpp"

#include "../GLHelper.hpp"

std::shared_ptr< CMaterial > CMaterialLoader::CreateMaterial( CTextureManager &textureManager, CShaderManager &shaderManager, const CSamplerManager &samplerManager, const std::string &identifier, const std::string &definition )
{
	Json::Reader	reader;
	Json::Value		mat_root;

	if( !reader.parse( definition, mat_root ) )
	{
		LOG( logWARNING ) << "failed to parse '" << identifier << "' because of " << reader.getFormattedErrorMessages();
		return( nullptr );
	}

	const Json::Value mat_name = mat_root[ "name" ];

	std::shared_ptr< CMaterial > newMaterial = std::make_shared< CMaterial >( mat_name.asString() );

	const Json::Value mat_cullmode = mat_root[ "cullmode" ];
	if( !mat_cullmode.isNull() )
	{
		if( ( !mat_cullmode.asString().empty() ) && GLHelper::FaceModeFromString( mat_cullmode.asString(), newMaterial->m_cullfacemode ) )
		{
			newMaterial->m_bCullFace = true;
		}
	}

	const Json::Value mat_polygonmode = mat_root[ "polygonmode" ];
	if( !mat_polygonmode.isNull() )
	{
		if( !GLHelper::PolygonModeFromString( mat_polygonmode.asString(), newMaterial->m_polygonmode ) )
		{
			newMaterial->m_polygonmode = GL_FILL;
		}
	}

	const Json::Value mat_blending = mat_root[ "blending" ];
	if( !mat_blending.isNull() )
	{
		const Json::Value mat_blendingsrc = mat_blending[ "src" ];
		const Json::Value mat_blendingdst = mat_blending[ "dst" ];
		if( !mat_blendingsrc.isNull() && GLHelper::SrcBlendFuncFromString( mat_blendingsrc.asString(), newMaterial->m_blendSrc ) && !mat_blendingdst.isNull() && GLHelper::DstBlendFuncFromString( mat_blendingdst.asString(), newMaterial->m_blendDst ) )
		{
			newMaterial->m_blending = true;
		}
		else
		{
			newMaterial->m_blending = false;
		}
	}
	else
	{
		newMaterial->m_blending = false;
	}

	const Json::Value mat_layers = mat_root[ "layers" ];
	if(	mat_layers.isNull()
		||
		( mat_layers.size() == 0 ) )
	{
		LOG( logWARNING ) << "no layers specified in '" << identifier << "'";
		return( nullptr );
	}
	else
	{
		newMaterial->m_layers.reserve( mat_layers.size() );

		unsigned int layer_index = 0;
		for( const Json::Value &mat_layer : mat_layers )
		{
			++layer_index;

			std::shared_ptr< CMaterialLayer > newLayer = newMaterial->CreateLayer();

			// TODO shaders
			//newLayer->m_animFreq = mat_layer.get( "animfreq", 1.0f ).asDouble();

			const Json::Value mat_shaders = mat_layer[ "shaders" ];
			if( mat_shaders.isNull() )
			{
				LOG( logWARNING ) << "no shader specified in layer '" << layer_index << "' of '" << identifier << "'";
				return( nullptr );
			}
			else
			{
				std::string shader_vs_path;
				std::string shader_fs_path;

				const Json::Value mat_shader_vs = mat_shaders[ "vs" ];
				if( mat_shader_vs.isNull() )
				{
					LOG( logWARNING ) << "no vertex shader specified in layer '" << layer_index << "' of '" << identifier << "'";
					return( nullptr );
				}
				else
				{
					shader_vs_path = mat_shader_vs.asString();
				}

				const Json::Value mat_shader_fs = mat_shaders[ "fs" ];
				if( mat_shader_fs.isNull() )
				{
					LOG( logWARNING ) << "no fragment shader specified in layer '" << layer_index << "' of '" << identifier << "'";
					return( nullptr );
				}
				else
				{
					shader_fs_path = mat_shader_fs.asString();
				}

				newLayer->m_shader = shaderManager.LoadProgram( shader_vs_path, shader_fs_path );
			}

			if( !newLayer->m_shader->m_requiredTextures.empty() )
			{
				const Json::Value mat_textures = mat_layer[ "textures" ];
				if(	mat_textures.isNull()
					||
					( mat_textures.size() == 0 ) )
				{
					LOG( logWARNING ) << "no textures specified in layer '" << layer_index << "' of '" << identifier << "'";
					return( nullptr );
				}
				else
				{
					for( const auto &sampler : newLayer->m_shader->m_requiredTextures )
					{
						const Json::Value mat_texture = mat_textures[ sampler.second.name ];
						if( mat_texture.isNull() )
						{
							LOG( logWARNING ) << "required texture for sampler '" << sampler.second.name << "' not specified in layer '" << layer_index << "' of '" << identifier << "'";
							return( nullptr );
						}
						else
						{
							// check that sampler-type and type of texture match

							const std::shared_ptr< CTexture > texture = textureManager.LoadTexture( mat_texture.asString() );

							switch( sampler.second.type )
							{
								case GL_SAMPLER_2D:
									if( texture->Type() != CTexture::type::TEX_2D )
									{
										LOG( logWARNING ) << "required texture for sampler '" << sampler.second.name << "' has to be of type 2D in layer '" << layer_index << "' of '" << identifier << "'";
										return( nullptr );
									}
									break;

								case GL_SAMPLER_CUBE:
									if( texture->Type() != CTexture::type::TEX_CUBE_MAP )
									{
										LOG( logWARNING ) << "required texture for sampler '" << sampler.second.name << "' has to be of type CUBEMAP in layer '" << layer_index << "' of '" << identifier << "'";
										return( nullptr );
									}
									break;

								case GL_SAMPLER_2D_ARRAY:
									if( texture->Type() != CTexture::type::TEX_2D_ARRAY )
									{
										LOG( logWARNING ) << "required texture for sampler '" << sampler.second.name << "' has to be of type 2D_ARRAY in layer '" << layer_index << "' of '" << identifier << "'";
										return( nullptr );
									}
									break;
							}

							newLayer->m_textures[ sampler.first ].first = texture;
						}
					}
				}

				const Json::Value mat_samplers = mat_layer[ "samplers" ];
				if(	mat_samplers.isNull()
					||
					( mat_samplers.size() == 0 ) )
				{
					LOG( logDEBUG ) << "no samplers specified in layer '" << layer_index << "' of '" << identifier << "'";
				}
				else
				{
					for( const auto &sampler : newLayer->m_shader->m_requiredTextures )
					{
						const Json::Value mat_sampler = mat_samplers[ sampler.second.name ];
						if( !mat_sampler.isNull() )
						{
							// TODO check here, if specified sampler fits to the type of the texture
							if( !samplerManager.SamplerFromString( mat_sampler.asString(), newLayer->m_textures[ sampler.first ].second ) )
							{
								LOG( logDEBUG ) << "invalid sampler specified for texture '" << sampler.second.name << "' in layer '" << layer_index << "' of '" << identifier << "'";
							}
						}
					}

					#ifndef INQ_DEBUG
					for( const Json::Value &mat_sampler : mat_samplers )
					{
						// TODO check for samplers here, which are not used
					}
					#endif
				}

				// set default samplers where not already set
				for( const auto &sampler : newLayer->m_shader->m_requiredTextures )
				{
					auto &tex = newLayer->m_textures[ sampler.first ];

					if( !tex.second )
					{
						switch( tex.first->Type() )
						{
							case CTexture::type::TEX_2D:
								tex.second = samplerManager.SamplerFromType( CSampler::Type::REPEAT_2D );
								break;

							case CTexture::type::TEX_CUBE_MAP:
								tex.second = samplerManager.SamplerFromType( CSampler::Type::REPEAT_CUBE );
								break;

							case CTexture::type::TEX_2D_ARRAY:
								tex.second = samplerManager.SamplerFromType( CSampler::Type::REPEAT_2D );
								break;
						}
					}
				}
			}

			const Json::Value mat_tcmods = mat_layer[ "tcmods" ];
			if( !mat_tcmods.isNull() )
			{
				newLayer->m_texcoordmods.reserve( mat_tcmods.size() );

				unsigned int tcmod_index = 0;
				for( const Json::Value &mat_tcmod : mat_tcmods )
				{
					++tcmod_index;

					if( !mat_tcmod.isNull() )
					{
						std::string mode = mat_tcmod[ "mode" ].asString();

						if( std::string( "SCALE" ) == mode )
						{
							float uscale = mat_tcmod.get( "uscale", 0.0f ).asDouble();
							float vscale = mat_tcmod.get( "vscale", 0.0f ).asDouble();
							newLayer->m_texcoordmods.emplace_back( std::make_shared< CTexCoordModScale >( uscale, vscale ) );
						}
						else if( std::string( "SCROLL" ) == mode )
						{
							float uspeed = mat_tcmod.get( "uspeed", 0.0f ).asDouble();
							float vspeed = mat_tcmod.get( "vspeed", 0.0f ).asDouble();
							newLayer->m_texcoordmods.emplace_back( std::make_shared< CTexCoordModScroll >( uspeed, vspeed ) );
						}
						else if( std::string( "ROTATE" ) == mode )
						{
							float deg = mat_tcmod.get( "deg", 0.0f ).asDouble();
							newLayer->m_texcoordmods.emplace_back( std::make_shared< CTexCoordModRotate >( deg ) );
						}
						else if( std::string( "MOVE" ) == mode )
						{
							float umove = mat_tcmod.get( "umove", 0.0f ).asDouble();
							float vmove = mat_tcmod.get( "vmove", 0.0f ).asDouble();
							newLayer->m_texcoordmods.emplace_back( std::make_shared< CTexCoordModMove >( umove, vmove ) );
						}
						else if( std::string( "TURB" ) == mode )
						{
							LOG( logWARNING ) << "tcmod '" << mode << "' in layer '" << layer_index << "' of '" << identifier << "' not implemented yet";
						}
						else
						{
							LOG( logWARNING ) << "unknown tcmod '" << mode << "' in layer '" << layer_index << "' of '" << identifier << "'";
						}
					}
				}
			}
		}
    }

	return( newMaterial );
}
