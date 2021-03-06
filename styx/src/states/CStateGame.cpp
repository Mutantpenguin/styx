﻿#include "CStateGame.hpp"

#include <algorithm>

#include "external/fmt/chrono.h"

#include "external/effolkronium/random.hpp"

#include "src/scene/components/camera/CCameraFreeComponent.hpp"
#include "src/renderer/components/CModelComponent.hpp"
#include "src/renderer/components/CGuiModelComponent.hpp"

#include "src/states/CStatePause.hpp"

#include "src/helper/Date.hpp"
#include "src/helper/image/ImageHandler.hpp"

#include "src/logger/CLogger.hpp"

#include "src/renderer/geometry/prefabs/Cube.hpp"
#include "src/renderer/geometry/prefabs/Cuboid.hpp"
#include "src/renderer/geometry/prefabs/Quad.hpp"
#include "src/renderer/geometry/prefabs/Rectangle.hpp"
#include "src/renderer/geometry/prefabs/Sphere.hpp"

CStateGame::CStateGame( const CFileSystem &filesystem, const CSettings &settings, CEngineInterface &engineInterface ) :
	CState( "game", filesystem, settings, engineInterface )
{
	m_scene.ClearColor( CColor( 0.0f, 0.0f, 4.0f, 0.0f ) );

	m_cameraEntity = m_scene.CreateEntity( "free camera" );
	m_cameraEntity->Transform.Position = { 43.0f, 76.0f, -99.0f };
	m_cameraEntity->Transform.Direction( { 0.0f, 0.0f, -10.0f } );
	m_cameraEntity->Add<CCameraFreeComponent>( m_settings.renderer.window.aspect_ratio, 72.0f, 0.1f, 1000.0f );

	m_scene.Camera( m_cameraEntity );

	auto &audio = m_engineInterface.Audio;

	{
		const auto &position = m_cameraEntity->Transform.Position;
		const auto direction = m_cameraEntity->Transform.Direction();
		const auto up = m_cameraEntity->Transform.Up();

		audio.SetListener( position, direction, up );
	}

	auto &resources = m_engineInterface.Resources;
	auto &samplerManager = m_engineInterface.SamplerManager;

	using Random = effolkronium::random_static;

	// create floor
	{
		const auto material = resources.Get<CMaterial>( "materials/standard.mat" );

		auto floorGeometry = GeometryPrefabs::QuadPNU0( 400.0f );

		for( auto &vertex : floorGeometry.Vertices )
		{
			vertex.UV0 *= 20;
		}

		const CMesh::TMeshTextureSlots floorMeshTextureSlots = { { "diffuseTexture", std::make_shared<CMeshTextureSlot>( resources.Get<CTexture>( "textures/texpack_2/stone_floor.png" ), samplerManager.GetFromType( CSampler::SamplerType::REPEAT_2D ) ) } };

		const auto floorMesh = std::make_shared<CMesh>( floorGeometry, material, floorMeshTextureSlots );

		const auto floorEntity = m_scene.CreateEntity( "floor" );
		floorEntity->Transform.Rotate( -90.0f, 0.0f, 0.0f );
		floorEntity->Add<CModelComponent>( floorMesh );
	}

	//auto material2 = resources.Get<CMaterial>( "materials/flames.mat" );
	//auto material2 = resources.Get<CMaterial>( "materials/texture_from_zip.mat" );
	//auto material2 = resources.Get<CMaterial>( "materials/newscast.mat" );
	//auto material2 = resources.Get<CMaterial>( "textures/texpack_1/killblockgeomtrn.png" );

	{
		const auto materialWaitCursor = resources.Get<CMaterial>( "materials/wait_cursor.mat" );

		const CMesh::TMeshTextureSlots movableMeshTextureSlots = {	{ "skullTexture", std::make_shared<CMeshTextureSlot>( resources.Get<CTexture>( "textures/cursor/skull.png" ), samplerManager.GetFromType( CSampler::SamplerType::EDGE_2D ) ) },
																	{ "waitTexture", std::make_shared<CMeshTextureSlot>( resources.Get<CTexture>( "textures/cursor/wait.png" ), samplerManager.GetFromType( CSampler::SamplerType::EDGE_2D ) ) } };

		const auto movableMesh = std::make_shared<CMesh>( GeometryPrefabs::QuadPNU0( 6.0f ), materialWaitCursor, movableMeshTextureSlots );

		m_movableEntity = m_scene.CreateEntity( "wait_cursor" );
		m_movableEntity->Transform.Position = { 0.0f, 10.0f, 20.0f };
		m_movableEntity->Add<CModelComponent>( movableMesh );
	}

	// create small cube of cubes
	{
		const auto material1 = resources.Get<CMaterial>( "materials/schnarf.mat" );

		const CMesh::TMeshTextureSlots schnarfMeshTextureSlots = { { "diffuseTexture", std::make_shared<CMeshTextureSlot>( resources.Get<CTexture>( "textures/texpack_2/pattern_07.png" ), samplerManager.GetFromType( CSampler::SamplerType::REPEAT_2D ) ) } };

		const auto cubeMesh = std::make_shared<CMesh>( GeometryPrefabs::CubePU0( 4.0f ), material1, schnarfMeshTextureSlots );

		const u16 cubeSize { 4 };

		for( u16 i = 0; i < cubeSize; i++ )
		{
			for( u16 j = 0; j < cubeSize; j++ )
			{
				for( u16 k = 0; k < cubeSize; k++ )
				{
					const auto cubeEntity = m_scene.CreateEntity( "cube" );
					cubeEntity->Transform.Position = { 20.0f + i * 4.0f, ( 0.0f + j * 4.0f ) + 2, -10.0f + k * 4.0f };
					cubeEntity->Add<CModelComponent>( cubeMesh );
				}
			}
		}
	}

	{
		const auto materialSuperBox = resources.Get<CMaterial>( "materials/superBox.mat" );

		const CMesh::TMeshTextureSlots superBoxMeshTextureSlots = {	{ "bgTexture", std::make_shared<CMeshTextureSlot>( resources.Get<CTexture>( "textures/texpack_1/mybitmap.bmp" ), samplerManager.GetFromType( CSampler::SamplerType::REPEAT_2D ) ) },
																	{ "fgTexture", std::make_shared<CMeshTextureSlot>( resources.Get<CTexture>( "textures/texpack_1/senn_icyfangrate.tga" ), samplerManager.GetFromType( CSampler::SamplerType::REPEAT_2D ) ) },
																	{ "skyBoxTexture", std::make_shared<CMeshTextureSlot>( resources.Get<CTexture>( "textures/cube/sixtine/sixtine.cub" ), samplerManager.GetFromType( CSampler::SamplerType::EDGE_CUBE ) ) } };

		{
			const auto superBoxMesh = std::make_shared<CMesh>( GeometryPrefabs::CubePNU0( 20.0f ), materialSuperBox, superBoxMeshTextureSlots );

			const auto superBoxEntity = m_scene.CreateEntity( "superBox" );
			superBoxEntity->Transform.Position = { 0.0f, 10.0f, -10.0f };
			superBoxEntity->Add<CModelComponent>( superBoxMesh );
		}

		// create big cube of cubes
		{
			const auto superBoxMesh = std::make_shared<CMesh>( GeometryPrefabs::CubePNU0( 4.0f ), materialSuperBox, superBoxMeshTextureSlots );

			const u16 cubeSize { 14 };

			for( u16 i = 0; i < cubeSize; i++ )
			{
				for( u16 j = 0; j < cubeSize; j++ )
				{
					for( u16 k = 0; k < cubeSize; k++ )
					{
						const auto superBoxEntity = m_scene.CreateEntity( "cube" );
						superBoxEntity->Transform.Position = { 20.0f + i * 4.0f, ( 0.0f + j * 4.0f ) + 2, 50.0f + k * 4.0f };
						superBoxEntity->Add<CModelComponent>( superBoxMesh );
					}
				}
			}
		}
	}

	// create big cube of cubes
	{
		const auto cubeGeometry = GeometryPrefabs::CubePU0( 4.0f );

		const auto materialSimple = resources.Get<CMaterial>( "materials/standard.mat" );
		const CMesh::TMeshTextureSlots simpleMeshTextureSlots = { { "diffuseTexture", std::make_shared<CMeshTextureSlot>( resources.Get<CTexture>( "textures/texpack_1/black_border.png" ), samplerManager.GetFromType( CSampler::SamplerType::REPEAT_2D ) ) } };
		const auto cubeMeshSimple = std::make_shared<CMesh>( cubeGeometry, materialSimple, simpleMeshTextureSlots );

		const auto materialTransparent = resources.Get<CMaterial>( "materials/standard_blend.mat" );
		const CMesh::TMeshTextureSlots transparentMeshTextureSlots = { { "diffuseTexture", std::make_shared<CMeshTextureSlot>( resources.Get<CTexture>( "textures/texpack_2/stained_glass.png" ), samplerManager.GetFromType( CSampler::SamplerType::REPEAT_2D ) ) } };
		const auto cubeMeshTransparent = std::make_shared<CMesh>( cubeGeometry, materialTransparent, transparentMeshTextureSlots );

		{
			const u16 cubeSize { 10 };

			for( u16 i = 0; i < cubeSize; i++ )
			{
				for( u16 j = 0; j < cubeSize; j++ )
				{
					for( u16 k = 0; k < cubeSize; k++ )
					{
						const auto cubeEntity = m_scene.CreateEntity( "cube" );
						cubeEntity->Transform.Position = { -40.0f + i * 4.0f, ( 0.0f + j * 4.0f ) + 2, 50.0f + k * 4.0f };

						if( Random::get<bool>() )
						{
							cubeEntity->Add<CModelComponent>( cubeMeshSimple );
						}
						else
						{
							cubeEntity->Add<CModelComponent>( cubeMeshTransparent );
						}
					}
				}
			}
		}

		{
			const u16 cubeSize { 10 };

			for( u16 i = 0; i < cubeSize; i++ )
			{
				for( u16 j = 0; j < cubeSize; j++ )
				{
					for( u16 k = 0; k < cubeSize; k++ )
					{
						const auto cubeEntity = m_scene.CreateEntity( "cube" );
						cubeEntity->Transform.Position = { -90.0f + i * 4.0f, ( 0.0f + j * 4.0f ) + 2, 50.0f + k * 4.0f };
						cubeEntity->Transform.Scale = { 2.0f, 2.0f, 2.0f };
						cubeEntity->Transform.Rotate( Random::get<f16>( 0, 90 ), Random::get<f16>( 0, 90 ), Random::get<f16>( 0, 90 ) );

						if( Random::get<bool>() )
						{
							cubeEntity->Add<CModelComponent>( cubeMeshSimple );
						}
						else
						{
							cubeEntity->Add<CModelComponent>( cubeMeshTransparent );
						}
					}
				}
			}
		}
	}

	{
		const auto fireMaterial = resources.Get<CMaterial>( "materials/flames.mat" );

		const CMesh::TMeshTextureSlots flamesMeshTextureSlots = { { "diffuseTexture", std::make_shared<CMeshTextureSlot>( resources.Get<CTexture>( "textures/array/fire/fire.arr" ), samplerManager.GetFromType( CSampler::SamplerType::EDGE_2D ) ) } };

		const auto flamesMesh = std::make_shared<CMesh>( GeometryPrefabs::RectanglePNU0( 8.0f, 16.0f ), fireMaterial, flamesMeshTextureSlots );

		const auto flamesEntity = m_scene.CreateEntity( "flames" );
		flamesEntity->Transform.Position = { -5.0f, 10.0f, 1.0f };
		flamesEntity->Add<CModelComponent>( flamesMesh );
	}

	{
		const auto greenMaterial = resources.Get<CMaterial>( "materials/green.mat" );

		const auto blockMesh = std::make_shared<CMesh>( GeometryPrefabs::CuboidP( 4.0f, 4.0f, 2.0f ), greenMaterial );

		const auto blockEntity = m_scene.CreateEntity( "green_block" );
		blockEntity->Transform.Position = { -4.0f, 10.0f, 1.0f };
		blockEntity->Add<CModelComponent>( blockMesh );
	}

	{
		const auto pulseMaterial = resources.Get<CMaterial>( "materials/pulse_green_red.mat" );

		const auto pulseMesh = std::make_shared<CMesh>( GeometryPrefabs::CuboidP( 4.0f, 4.0f, 2.0f ), pulseMaterial );

		m_pulseEntity = m_scene.CreateEntity( "pulse_block" );
		m_pulseEntity->Transform.Position = { 0.0f, 10.0f, 1.0f };
		m_pulseEntity->Add<CModelComponent>( pulseMesh );
	}

	{
		const auto redMaterial = resources.Get<CMaterial>( "materials/red.mat" );

		const auto blockMesh = std::make_shared<CMesh>( GeometryPrefabs::CuboidP( 4.0f, 4.0f, 2.0f ), redMaterial );

		const auto blockEntity = m_scene.CreateEntity( "red_block" );
		blockEntity->Transform.Position = { 4.0f, 10.0f, 1.0f };
		blockEntity->Add<CModelComponent>( blockMesh );
	}

	{
		const auto explodeMaterial = resources.Get<CMaterial>( "materials/explode.mat" );

		const CMesh::TMeshTextureSlots explodeMeshTextureSlots = { { "diffuseTexture", std::make_shared<CMeshTextureSlot>( resources.Get<CTexture>( "textures/texpack_2/pattern_07.png" ), samplerManager.GetFromType( CSampler::SamplerType::REPEAT_2D ) ) } };

		const auto explodeMesh = std::make_shared<CMesh>( GeometryPrefabs::CubePU0( 10.0f ), explodeMaterial, explodeMeshTextureSlots );

		const auto explode = m_scene.CreateEntity( "exploder" );
		explode->Transform.Position = { 50.0f, 10.0f, 1.0f };
		explode->Add<CModelComponent>( explodeMesh );

		m_cameraEntity->Transform.Direction( explode->Transform.Position - m_cameraEntity->Transform.Position );
	}

	{
		const Geometry<VertexPN> particlesGeometry =	{
															GL_POINTS,
															{
																// use normal to store an offset into the array texture
																{	{ -1.0f, -1.0f, -1.0f }, {  0.0f, 0.0f, 0.0f }	},
																{	{ -1.0f, -1.0f, +1.0f }, { 24.0f, 0.0f, 0.0f }	},
																{	{ -1.0f, +1.0f, -1.0f }, {  6.0f, 0.0f, 0.0f }	},
																{	{ -1.0f, +1.0f, +1.0f }, { 30.0f, 0.0f, 0.0f }	},
																{	{ +1.0f, -1.0f, -1.0f }, { 12.0f, 0.0f, 0.0f }	},
																{	{ +1.0f, -1.0f, +1.0f }, { 36.0f, 0.0f, 0.0f }	},
																{	{ +1.0f, +1.0f, -1.0f }, { 18.0f, 0.0f, 0.0f }	},
																{	{ +1.0f, +1.0f, +1.0f }, { 42.0f, 0.0f, 0.0f }	}
															},
															{
																{ 0, 1, 2, 3, 4, 5, 6, 7 }
															}
														};

		const auto particleMaterial = resources.Get<CMaterial>( "materials/particle.mat" );

		const CMesh::TMeshTextureSlots particleMeshTextureSlots = { { "diffuseTexture", std::make_shared<CMeshTextureSlot>( resources.Get<CTexture>( "textures/array/fireball_small/fireball_small.arr" ), samplerManager.GetFromType( CSampler::SamplerType::REPEAT_2D ) ) } };

		const auto particleMesh = std::make_shared<CMesh>( particlesGeometry, particleMaterial, particleMeshTextureSlots );

		const auto particleEntity = m_scene.CreateEntity( "particles" );
		particleEntity->Transform.Position = { 80.0f, 10.0f, 1.0f };
		particleEntity->Transform.Scale = { 5.0f, 5.0f, 5.0f };
		particleEntity->Add<CModelComponent>( particleMesh );
	}
	
	{
		const auto explodeMaterial = resources.Get<CMaterial>( "materials/explode.mat" );
		
		const CMesh::TMeshTextureSlots explodeMeshSphereTextureSlots = { { "diffuseTexture", std::make_shared<CMeshTextureSlot>( resources.Get<CTexture>( "textures/texpack_2/pattern_07.png" ), samplerManager.GetFromType( CSampler::SamplerType::REPEAT_2D ) ) } };

		const auto sphereMesh = std::make_shared<CMesh>( GeometryPrefabs::SpherePU0( 10, 8, 10.0f ), explodeMaterial, explodeMeshSphereTextureSlots );

		const auto blockEntity = m_scene.CreateEntity( "exploding sphere" );
		blockEntity->Transform.Position = { 110.0f, 10.0f, 1.0f };
		blockEntity->Add<CModelComponent>( sphereMesh );
	}

	{
		const auto fireballMaterial = resources.Get<CMaterial>( "materials/fireball.mat" );

		const auto fireballMesh = std::make_shared<CMesh>( GeometryPrefabs::SpherePNU0( 200, 160, 10.0f ), fireballMaterial );

		const auto firebalEntity = m_scene.CreateEntity( "fireball" );
		firebalEntity->Transform.Position = { 140.0f, 10.0f, 1.0f };
		firebalEntity->Add<CModelComponent>( fireballMesh );
	}
	
	{ // font test
		
		const auto &fontbuilder = m_engineInterface.FontBuilder;
			
		const auto fontComfortaa64 = fontbuilder.FromFile( "Comfortaa", "fonts/Comfortaa/Regular.ttf", "fonts/Comfortaa/Bold.ttf", 64, CGlyphRange::Default() );

		CGlyphRange glyphRangeFontAwesome( 0xf000, 0xf897 );
		// TODO texture is too small for using regular and bold
		// const auto fontFontAwesome64 = engineInterface.FontBuilder.FromFile( "FontAwesome", "fonts/fontawesome/fa-regular-400.ttf", "fonts/fontawesome/fa-solid-900.ttf", 64, glyphRangeFontAwesome );
		const auto fontFontAwesome64 = engineInterface.FontBuilder.FromFile( "FontAwesome", "fonts/fontawesome/fa-regular-400.ttf", 64, glyphRangeFontAwesome );

		// TODO const auto font = fontbuilder.FromFile( "NovaCut", "fonts/NovaCut.ttf", 64, glyphRange );

		{ // show whole atlas
			const auto material = resources.Get<CMaterial>( "materials/basic_font.mat" );
		
			const CMesh::TMeshTextureSlots textureSlots = {	{ "fontTexture", std::make_shared<CMeshTextureSlot>( fontComfortaa64->Texture, samplerManager.GetFromType( CSampler::SamplerType::EDGE_2D ) ) } };
			
			const auto mesh = std::make_shared<CMesh>( GeometryPrefabs::QuadPU0( 10.0f ), material, textureSlots );

			const auto entity = m_scene.CreateEntity( "font_atlas_test" );
			entity->Transform.Position = { 40.0f, 10.0f, 20.0f };
			entity->Add<CModelComponent>( mesh );
		}
		
		{ // just one text object
			STextOptions textOptions;
			textOptions.LineSpacing = 32;
			textOptions.HorizontalAnchor = EHorizontalAnchor::CENTER;
			textOptions.VerticalAnchor = EVerticalAnchor::CENTER;
			textOptions.HorizontalAlign = EHorizontalAlign::CENTER;
			
			const auto text = engineInterface.TextBuilder.Create( fontComfortaa64, textOptions, "A<b>A</b> Du hast <#{0}>doofe</#> <b>Ohren</b> und eine <#4444FF><b>krumme</b></#> Nase!\nDies ist die zweite Zeile! <#FFBAFF>Mit zeilenübergreifender Formatierung\nbis in die</#> dritte Zeile", CColor( 0.0, 1.0, 0.0 ).rgbHex() );
			
			const auto entity = m_scene.CreateEntity( "font_test" );
			entity->Transform.Position = { 80.0f, 10.0f, 20.0f };
			entity->Transform.Scale = { 0.05f, 0.05f, 0.05f };
			entity->Add<CModelComponent>( text->Mesh() );
		}

		{
			STextOptions textOptions;
			textOptions.Color = TangoColors::PlumShadow();
			textOptions.HorizontalAnchor = EHorizontalAnchor::CENTER;
			textOptions.VerticalAnchor = EVerticalAnchor::CENTER;
			textOptions.HorizontalAlign = EHorizontalAlign::CENTER;

			// TODO why does only the last character (ICON_FA_ARROW_ALT_CIRCLE_LEFT ) work here?
			// values from https://github.com/juliettef/IconFontCppHeaders/blob/master/IconsFontAwesome5.h
			const auto text = engineInterface.TextBuilder.Create( fontFontAwesome64, textOptions, u8"\uf519\uf05b\uf5d2\uf359" );

			const auto entity = m_scene.CreateEntity( "fontawesome_test" );
			entity->Transform.Position = { 40.0f, 20.0f, 20.0f };
			entity->Transform.Scale = { 0.05f, 0.05f, 0.05f };
			entity->Add<CModelComponent>( text->Mesh() );
		}

		{
			const auto fpsFontSize = m_settings.renderer.window.size.height / 30;

			const auto fpsFont = fontbuilder.FromFile( "Comfortaa", "fonts/Comfortaa/Regular.ttf", "fonts/Comfortaa/Bold.ttf", fpsFontSize, CGlyphRange::Default() );

			{
				STextOptions textOptions;
				textOptions.Color = Colors::Yellow();
				textOptions.LineSpacing = 32;
				textOptions.HorizontalAnchor = EHorizontalAnchor::LEFT;
				textOptions.VerticalAnchor = EVerticalAnchor::TOP;
				textOptions.HorizontalAlign = EHorizontalAlign::LEFT;
				textOptions.RichText = false;

				m_fpsCurrentText = engineInterface.TextBuilder.Create( fpsFont, textOptions, "" );

				const auto fpsCurrentEntity = m_scene.CreateEntity( "current fps" );
				fpsCurrentEntity->Transform.Position = { 0.0f, m_settings.renderer.window.size.height - fpsFontSize, 0.0f };
				fpsCurrentEntity->Add<CGuiModelComponent>( m_fpsCurrentText->Mesh() );
			}

			{
				STextOptions textOptions;
				textOptions.Color = Colors::Red();
				textOptions.LineSpacing = 32;
				textOptions.HorizontalAnchor = EHorizontalAnchor::LEFT;
				textOptions.VerticalAnchor = EVerticalAnchor::TOP;
				textOptions.HorizontalAlign = EHorizontalAlign::LEFT;
				textOptions.RichText = false;

				m_fpsMaxText = engineInterface.TextBuilder.Create( fpsFont, textOptions, "" );

				const auto fpsMaxEntity = m_scene.CreateEntity( "current fps" );
				fpsMaxEntity->Transform.Position = { 0.0f, m_settings.renderer.window.size.height - 2 * fpsFontSize, 0.0f };
				fpsMaxEntity->Add<CGuiModelComponent>( m_fpsMaxText->Mesh() );
			}
		}
	}

	{
		m_fpsGraphGeometry.Mode = GL_LINES;

		m_fpsGraphGeometry.Vertices.emplace_back( VertexPC( { { 0, 0, 0 }, { 0, 1.0, 1.0 } } ) );

		const auto material = resources.Get<CMaterial>( "materials/vertexColor.mat" );

		m_fpsGraphMesh = std::make_shared<CMesh>( m_fpsGraphGeometry, material, CMesh::TMeshTextureSlots(), true );

		const auto fpsGraphEntity = m_scene.CreateEntity( "fps graph" );
		fpsGraphEntity->Transform.Position = { 0.0f, m_settings.renderer.window.size.height - 300, 0.0f };
		fpsGraphEntity->Add<CGuiModelComponent>( m_fpsGraphMesh );
	}

	{
		auto crosshairGeometry = GeometryPrefabs::QuadPU0( 64.0f );

		const auto material = resources.Get<CMaterial>( "materials/standard_blend.mat" );

		const CMesh::TMeshTextureSlots crosshairPassiveTextureSlots = { { "diffuseTexture", std::make_shared<CMeshTextureSlot>( resources.Get<CTexture>( "textures/crosshair/crosshair037.png" ), samplerManager.GetFromType( CSampler::SamplerType::EDGE_2D ) ) } };
		const CMesh::TMeshTextureSlots crosshairActiveTextureSlots = { { "diffuseTexture", std::make_shared<CMeshTextureSlot>( resources.Get<CTexture>( "textures/crosshair/crosshair038.png" ), samplerManager.GetFromType( CSampler::SamplerType::EDGE_2D ) ) } };

		m_crosshairPassiveMesh = std::make_shared<CMesh>( crosshairGeometry, material, crosshairPassiveTextureSlots );
		m_crosshairActiveMesh = std::make_shared<CMesh>( crosshairGeometry, material, crosshairActiveTextureSlots );

		m_crosshairEntity = m_scene.CreateEntity( "crosshair" );
		m_crosshairEntity->Transform.Position = { m_settings.renderer.window.size.width / 2, m_settings.renderer.window.size.height / 2, -20.0f };
		
		m_crosshairEntity->Add<CGuiModelComponent>( m_crosshairPassiveMesh );
	}

	{
		const auto material3 = resources.Get<CMaterial>( "materials/sky.mat" );

		const CMesh::TMeshTextureSlots skyMeshTextureSlots = { { "skyboxTexture", std::make_shared<CMeshTextureSlot>( resources.Get<CTexture>( "textures/cube/sixtine/sixtine.cub" ), samplerManager.GetFromType( CSampler::SamplerType::EDGE_CUBE ) ) } };

		const auto skyboxMesh = std::make_shared<CMesh>( GeometryPrefabs::CubePU0(), material3, skyMeshTextureSlots );

		m_skyboxEntity = m_scene.CreateEntity( "skybox" );
		m_skyboxEntity->Add<CModelComponent>( skyboxMesh );
	}

	m_backgroundMusic = std::make_shared<CAudioSource>( resources.Get<CAudioBuffer>( "music/rise_of_spirit.ogg" ) );
	m_backgroundMusic->Play();
	m_backgroundMusic->SetLooping( true );

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// TEST LOADING MODELS
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//TODO const auto model = resources.Get<CModel>( "models/stuff/torus.dae" );
}

CStateGame::~CStateGame()
{
}

std::shared_ptr<CState> CStateGame::OnUpdate()
{
	const u64 elapsedTime = m_timer.Time();

	auto &audio = m_engineInterface.Audio;

	const auto &input = m_engineInterface.Input;

	if( input.KeyDown( SDL_SCANCODE_ESCAPE ) )
	{
		logINFO( "pause" );
		return( std::make_shared<CStatePause>( m_filesystem, m_settings, m_engineInterface, shared_from_this() ) );
	}

	if( m_updateFpsTime <= elapsedTime )
	{
		m_updateFpsTime = elapsedTime + 250000.0f;

		const f16 fps = ( 1000.0f / m_engineInterface.Stats.frameTime * 1000.0f );
		
		m_fpsCurrentText->Text( "fps: {0:0.1f}", fps );

		if( fps > m_maxFps )
		{
			m_maxFps = fps;
			m_fpsMaxText->Text( "max fps: {0:0.1f}", m_maxFps );
		}

		m_fpsGraphGeometry.Vertices.emplace_back( VertexPC( { { m_fpsGeometryIndex + 1, fps, 0 }, { 0, 1.0, 1.0 } } ) );
		m_fpsGraphGeometry.Indices.emplace_back( m_fpsGeometryIndex );
		m_fpsGraphGeometry.Indices.emplace_back( m_fpsGeometryIndex + 1 );

		m_fpsGeometryIndex++;

		m_fpsGraphMesh->SetGeometry( m_fpsGraphGeometry );
	}


	const f16 spp = 2.0f * m_settings.engine.tick / 1000000;

	// TODO maybe create a way for systems to hook into the inputsystem, so we can provide globally active keyboard-shortcuts?

	/*
	 * move pulseMesh
	 */
	m_pulseEntity->Transform.Position.y = 10.0f + ( sin( elapsedTime / 2000000.0f ) * 5.0f );

	/*
	 * move meshMovable
	 */

	if( input.KeyStillDown( SDL_SCANCODE_KP_6 ) )
	{
		m_movableEntity->Transform.Position.x += spp;
	}
	if( input.KeyStillDown( SDL_SCANCODE_KP_4 ) )
	{
		m_movableEntity->Transform.Position.x -= spp;
	}

	if( input.KeyStillDown( SDL_SCANCODE_KP_8 ) )
	{
		m_movableEntity->Transform.Position.y += spp;
	}
	if( input.KeyStillDown( SDL_SCANCODE_KP_2 ) )
	{
		m_movableEntity->Transform.Position.y -= spp;
	}

	if( input.KeyStillDown( SDL_SCANCODE_KP_PLUS ) )
	{
		m_movableEntity->Transform.Position.z += spp;
	}
	if( input.KeyStillDown( SDL_SCANCODE_KP_MINUS ) )
	{
		m_movableEntity->Transform.Position.z -= spp;
	}

	if( input.KeyStillDown( SDL_SCANCODE_KP_5 ) )
	{
		m_movableEntity->Transform.Position = glm::vec3( 1.0f );
	}

	/*
	 * move camera
	 */

	const f16 ctrlPressedMult = input.KeyStillDown( SDL_SCANCODE_LCTRL ) ? 1.0f : 10.0f;

	const auto &cameraFree = m_cameraEntity->Get<CCameraFreeComponent>();

	// TODO only change crosshair when hovering over an entity
	// TODO then show its name in a new GUI element
	if( input.MouseDown( SDL_BUTTON_LEFT )
		||
		input.MouseStillDown( SDL_BUTTON_LEFT ) )
	{
		m_crosshairEntity->Get<CGuiModelComponent>()->Mesh = m_crosshairActiveMesh;
	}
	else
	{
		m_crosshairEntity->Get<CGuiModelComponent>()->Mesh = m_crosshairPassiveMesh;
	}

	if( input.MouseDown( SDL_BUTTON_LEFT )
		||
		input.MouseStillDown( SDL_BUTTON_LEFT ) )
	{
		cameraFree->Rotate( static_cast<f16>( input.MouseDeltaY() ), static_cast<f16>( input.MouseDeltaX() ) );
	}

	if( input.KeyDown( SDL_SCANCODE_A )
        ||
		input.KeyStillDown( SDL_SCANCODE_A ) )
	{
		cameraFree->MoveLeft( spp * ctrlPressedMult );
	}

	if( input.KeyDown( SDL_SCANCODE_D )
        ||
		input.KeyStillDown( SDL_SCANCODE_D ) )
	{
		cameraFree->MoveRight( spp * ctrlPressedMult );
	}

	if( input.KeyDown( SDL_SCANCODE_W )
        ||
		input.KeyStillDown( SDL_SCANCODE_W ) )
	{
		cameraFree->MoveForward( spp * ctrlPressedMult );
	}

	if( input.KeyDown( SDL_SCANCODE_S )
        ||
		input.KeyStillDown( SDL_SCANCODE_S ) )
	{
		cameraFree->MoveBackward( spp * ctrlPressedMult );
	}

	if( input.KeyDown( SDL_SCANCODE_SPACE )
        ||
		input.KeyStillDown( SDL_SCANCODE_SPACE ) )
	{
		cameraFree->MoveUp( spp * ctrlPressedMult );
	}

	if( input.KeyDown( SDL_SCANCODE_LSHIFT )
        ||
		input.KeyStillDown( SDL_SCANCODE_LSHIFT ) )
	{
		cameraFree->MoveDown( spp * ctrlPressedMult );
	}

	{
		const auto &position = m_cameraEntity->Transform.Position;
		const auto direction = m_cameraEntity->Transform.Direction();
		const auto up = m_cameraEntity->Transform.Up();

		audio.SetListener( position, direction, up );
	}

	/*
	 * change FOV
	 */
	if( input.KeyStillDown( SDL_SCANCODE_PAGEUP ) )
	{
		auto fov = cameraFree->FOV();
		fov++;
		cameraFree->FOV( fov );
		logDEBUG( "new FOV: {0}", fov );

	}
	if( input.KeyStillDown( SDL_SCANCODE_PAGEDOWN ) )
	{
		auto fov = cameraFree->FOV();
		fov--;
		cameraFree->FOV( fov );
		logDEBUG( "new FOV: {0}", fov );
	}

	/*
	 * screenshot
	 */

	if( input.KeyDown( SDL_SCANCODE_BACKSPACE ) )
	{
		const std::string screenshotDir { "screenshots" };

		if( !m_filesystem.Exists( screenshotDir ) )
		{
			if( !m_filesystem.MakeDir( screenshotDir ) )
			{
				logERROR( "couldn't create screenshot-directory '{0}' because of: {1}", screenshotDir, m_filesystem.GetLastError() );
			}
		}

		const std::string screenshotPath = fmt::format( "{0}/{1:%Y-%m-%d_%H-%M-%S}.{2}", screenshotDir, fmt::localtime( Date::GetCurrentDateTime() ), m_settings.renderer.screenshot.format );

		logINFO( "taking screenshot '{0}'", screenshotPath );

		const CImage screenshot = m_frameBuffer.ToImage();

		if( !ImageHandler::Save( m_filesystem, screenshot, m_settings.renderer.screenshot.format, screenshotPath ) )
		{
			logERROR( "screenshot '{0}' couldn't be saved", screenshotPath );
		}
		else
		{
			if( !m_filesystem.Exists( screenshotPath ) )
			{
				logERROR( "screenshot '{0}' doesn't exist", screenshotPath );
			}
		}
	}

	/*
	 * stuff
	 */

	if( input.KeyDown( SDL_SCANCODE_F1 ) )
	{
		logINFO( "frame-time is {0}ms", ( m_engineInterface.Stats.frameTime / 1000.0f ) );
	}

	if( !input.MouseStillDown( SDL_BUTTON_LEFT) )
	{
		m_movableEntity->Transform.Rotate( m_roty_ps, m_rotx_ps, 0.0f );
	}

	m_rotx_ps = static_cast<f16>( input.MouseDeltaX() );
	m_roty_ps = static_cast<f16>( input.MouseDeltaY() );

	// TODO cameraFree->Direction( m_movableEntity->Transform.Position() - m_cameraFree->Transform.Position() );

	m_skyboxEntity->Transform.Position = m_cameraEntity->Transform.Position;

	return( shared_from_this() );
}
