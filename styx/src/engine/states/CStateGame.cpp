#include "CStateGame.hpp"

#include <algorithm>

#include "src/ext/fmt/time.h"

#include "src/engine/scene/components/camera/CCameraFreeComponent.hpp"
#include "src/engine/renderer/components/CModelComponent.hpp"

#include "src/engine/states/CStatePause.hpp"

#include "src/engine/math/Math.hpp"
#include "src/engine/helper/Date.hpp"
#include "src/engine/helper/geom/Primitives.hpp"
#include "src/engine/helper/image/ImageHandler.hpp"

#include "src/engine/logger/CLogger.hpp"

CStateGame::CStateGame( const CFileSystem &filesystem, const CSettings &settings, CEngineInterface &engineInterface ) :
	CState( "game", filesystem, settings, engineInterface )
{
	m_scene.ClearColor( CColor( 0.0f, 0.0f, 4.0f, 0.0f ) );

	auto &renderer = m_engineInterface.Renderer;

	m_cameraEntity = m_scene.CreateEntity( "free camera" );
	m_cameraEntity->Transform.Position( { 0.0f, 10.0f, 10.0f } );
	m_cameraEntity->Transform.Direction( { 0.0f, 0.0f, -10.0f } );
	m_cameraEntity->Add<CCameraFreeComponent>( m_settings.renderer.window.aspect_ratio, 72.0f, 0.1f, 1000.0f );

	m_scene.Camera( m_cameraEntity );

	auto &soundManager = m_engineInterface.SoundManager;

	soundManager.SetListener( m_cameraEntity->Transform );

	auto &resourceCache = m_engineInterface.ResourceCacheManager;
	auto &samplerManager = renderer.SamplerManager();

	// create floor
	{
		const auto material = resourceCache.GetResource< CMaterial >( "materials/standard.mat" );

		auto floorMeshPrimitive = Primitives::quad;

		for( auto &vertex : floorMeshPrimitive.Vertices )
		{
			vertex.TexCoord *= 10;
		}

		const CMesh::TMeshTextureSlots floorMeshTextureSlots = { { "diffuseTexture", std::make_shared< CMeshTextureSlot >( resourceCache.GetResource< CTexture >( "textures/texpack_2/stone_floor.png" ), samplerManager.GetFromType( CSampler::SamplerType::REPEAT_2D ) ) } };

		const auto floorMesh = std::make_shared< CMesh >( GL_TRIANGLE_STRIP, floorMeshPrimitive, material, floorMeshTextureSlots );

		const auto floorEntity = m_scene.CreateEntity( "floor" );
		floorEntity->Transform.Scale( { 100.0f, 100.0f, 100.0f } );
		floorEntity->Transform.Rotate( -90.0f, 0.0f, 0.0f );
		floorEntity->Add<CModelComponent>( floorMesh );
	}

	//auto material2 = resourceCache.GetResource< CMaterial >( "materials/flames.mat" );
	//auto material2 = resourceCache.GetResource< CMaterial >( "materials/texture_from_zip.mat" );
	//auto material2 = resourceCache.GetResource< CMaterial >( "materials/newscast.mat" );
	//auto material2 = resourceCache.GetResource< CMaterial >( "textures/texpack_1/killblockgeomtrn.png" );

	{
		const auto materialWaitCursor = resourceCache.GetResource< CMaterial >( "materials/wait_cursor.mat" );

		const CMesh::TMeshTextureSlots movableMeshTextureSlots = {	{ "skullTexture", std::make_shared< CMeshTextureSlot >( resourceCache.GetResource< CTexture >( "textures/cursor/skull.png" ), samplerManager.GetFromType( CSampler::SamplerType::EDGE_2D ) ) },
																	{ "waitTexture", std::make_shared< CMeshTextureSlot >( resourceCache.GetResource< CTexture >( "textures/cursor/wait.png" ), samplerManager.GetFromType( CSampler::SamplerType::EDGE_2D ) ) } };

		const auto movableMesh = std::make_shared< CMesh >( GL_TRIANGLE_STRIP, Primitives::quad, materialWaitCursor, movableMeshTextureSlots );

		m_movableEntity = m_scene.CreateEntity( "wait_cursor" );
		m_movableEntity->Transform.Position( { 0.0f, 10.0f, 20.0f } );
		m_movableEntity->Transform.Scale( { 3.0f, 3.0f, 1.0f } );
		m_movableEntity->Add<CModelComponent>( movableMesh );
	}

	// create small cube of cubes
	{
		const auto material1 = resourceCache.GetResource< CMaterial >( "materials/schnarf.mat" );

		const CMesh::TMeshTextureSlots schnarfMeshTextureSlots = { { "diffuseTexture", std::make_shared< CMeshTextureSlot >( resourceCache.GetResource< CTexture >( "textures/texpack_2/pattern_07.png" ), samplerManager.GetFromType( CSampler::SamplerType::REPEAT_2D ) ) } };

		const auto cubeMesh = std::make_shared< CMesh >( GL_TRIANGLES, Primitives::cube, material1, schnarfMeshTextureSlots );

		const std::uint16_t cubeSize { 4 };

		for( std::uint16_t i = 0; i < cubeSize; i++ )
		{
			for( std::uint16_t j = 0; j < cubeSize; j++ )
			{
				for( std::uint16_t k = 0; k < cubeSize; k++ )
				{
					const auto cubeEntity = m_scene.CreateEntity( "cube" );
					cubeEntity->Transform.Position( { 20.0f + i * 4.0f, ( 0.0f + j * 4.0f ) + 2, -10.0f + k * 4.0f } );
					cubeEntity->Transform.Scale( { 2.0f, 2.0f, 2.0f } );
					cubeEntity->Add<CModelComponent>( cubeMesh );
				}
			}
		}
	}

	{
		const auto materialSuperBox = resourceCache.GetResource< CMaterial >( "materials/superBox.mat" );

		const CMesh::TMeshTextureSlots superBoxMeshTextureSlots = {	{ "bgTexture", std::make_shared< CMeshTextureSlot >( resourceCache.GetResource< CTexture >( "textures/texpack_1/mybitmap.bmp" ), samplerManager.GetFromType( CSampler::SamplerType::REPEAT_2D ) ) },
																	{ "fgTexture", std::make_shared< CMeshTextureSlot >( resourceCache.GetResource< CTexture >( "textures/texpack_1/senn_icyfangrate.tga" ), samplerManager.GetFromType( CSampler::SamplerType::REPEAT_2D ) ) },
																	{ "skyBoxTexture", std::make_shared< CMeshTextureSlot >( resourceCache.GetResource< CTexture >( "textures/cube/sixtine/sixtine.cub" ), samplerManager.GetFromType( CSampler::SamplerType::EDGE_CUBE ) ) } };

		{
			const auto superBoxMesh = std::make_shared< CMesh >( GL_TRIANGLES, Primitives::cube, materialSuperBox, superBoxMeshTextureSlots );

			const auto superBoxEntity = m_scene.CreateEntity( "superBox" );
			superBoxEntity->Transform.Position( { 0.0f, 10.0f, -10.0f } );
			superBoxEntity->Transform.Scale( { 10.0f, 10.0f, 10.0f } );
			superBoxEntity->Add<CModelComponent>( superBoxMesh );
		}

		// create big cube of cubes
		{
			const auto superBoxMesh = std::make_shared< CMesh >( GL_TRIANGLES, Primitives::cube, materialSuperBox, superBoxMeshTextureSlots );

			const std::uint16_t cubeSize { 14 };

			for( std::uint16_t i = 0; i < cubeSize; i++ )
			{
				for( std::uint16_t j = 0; j < cubeSize; j++ )
				{
					for( std::uint16_t k = 0; k < cubeSize; k++ )
					{
						const auto superBoxEntity = m_scene.CreateEntity( "cube" );
						superBoxEntity->Transform.Position( { 20.0f + i * 4.0f, ( 0.0f + j * 4.0f ) + 2, 50.0f + k * 4.0f } );
						superBoxEntity->Transform.Scale( { 2.0f, 2.0f, 2.0f } );
						superBoxEntity->Add<CModelComponent>( superBoxMesh );
					}
				}
			}
		}
	}

	// create big cube of cubes
	{
		const auto materialSimple = resourceCache.GetResource< CMaterial >( "materials/standard.mat" );
		const CMesh::TMeshTextureSlots simpleMeshTextureSlots = { { "diffuseTexture", std::make_shared< CMeshTextureSlot >( resourceCache.GetResource< CTexture >( "textures/texpack_1/black_border.png" ), samplerManager.GetFromType( CSampler::SamplerType::REPEAT_2D ) ) } };
		const auto cubeMeshSimple = std::make_shared< CMesh >( GL_TRIANGLES, Primitives::cube, materialSimple, simpleMeshTextureSlots );

		const auto materialTransparent = resourceCache.GetResource< CMaterial >( "materials/standard_blend.mat" );
		const CMesh::TMeshTextureSlots transparentMeshTextureSlots = { { "diffuseTexture", std::make_shared< CMeshTextureSlot >( resourceCache.GetResource< CTexture >( "textures/texpack_2/stained_glass.png" ), samplerManager.GetFromType( CSampler::SamplerType::REPEAT_2D ) ) } };
		const auto cubeMeshTransparent = std::make_shared< CMesh >( GL_TRIANGLES, Primitives::cube, materialTransparent, transparentMeshTextureSlots );

		{
			const std::uint16_t cubeSize { 10 };

			for( std::uint16_t i = 0; i < cubeSize; i++ )
			{
				for( std::uint16_t j = 0; j < cubeSize; j++ )
				{
					for( std::uint16_t k = 0; k < cubeSize; k++ )
					{
						const auto cubeEntity = m_scene.CreateEntity( "cube" );
						cubeEntity->Transform.Position( { -40.0f + i * 4.0f, ( 0.0f + j * 4.0f ) + 2, 50.0f + k * 4.0f } );
						cubeEntity->Transform.Scale( { 2.0f, 2.0f, 2.0f } );

						if( Math::irand( 0, 1 ) == 1 )
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
			const std::uint16_t cubeSize { 10 };

			for( std::uint16_t i = 0; i < cubeSize; i++ )
			{
				for( std::uint16_t j = 0; j < cubeSize; j++ )
				{
					for( std::uint16_t k = 0; k < cubeSize; k++ )
					{
						const auto cubeEntity = m_scene.CreateEntity( "cube" );
						cubeEntity->Transform.Position( { -90.0f + i * 4.0f, ( 0.0f + j * 4.0f ) + 2, 50.0f + k * 4.0f } );
						cubeEntity->Transform.Scale( { 2.0f, 2.0f, 2.0f } );
						cubeEntity->Transform.Rotate( Math::irand( 0, 90 ), Math::irand( 0, 90 ), Math::irand( 0, 90 ) );

						if( Math::irand( 0, 1 ) == 1 )
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
		const auto fireMaterial = resourceCache.GetResource< CMaterial >( "materials/flames.mat" );

		const CMesh::TMeshTextureSlots flamesMeshTextureSlots = { { "diffuseTexture", std::make_shared< CMeshTextureSlot >( resourceCache.GetResource< CTexture >( "textures/array/fire/fire.arr" ), samplerManager.GetFromType( CSampler::SamplerType::EDGE_2D ) ) } };

		const auto flamesMesh = std::make_shared< CMesh >( GL_TRIANGLE_STRIP, Primitives::quad, fireMaterial, flamesMeshTextureSlots );

		const auto flamesEntity = m_scene.CreateEntity( "flames" );
		flamesEntity->Transform.Position( { -5.0f, 10.0f, 1.0f } );
		flamesEntity->Transform.Scale( { 4.0f, 8.0f, 1.0f } );
		flamesEntity->Add<CModelComponent>( flamesMesh );
	}

	{
		const auto greenMaterial = resourceCache.GetResource< CMaterial >( "materials/green.mat" );

		const auto cubeMesh = std::make_shared< CMesh >( GL_TRIANGLE_STRIP, Primitives::cube, greenMaterial );

		const auto cubeEntity = m_scene.CreateEntity( "green_cube" );
		cubeEntity->Transform.Position( { -4.0f, 10.0f, 1.0f } );
		cubeEntity->Transform.Scale( { 2.0f, 2.0f, 1.0f } );
		cubeEntity->Add<CModelComponent>( cubeMesh );
	}

	{
		const auto pulseMaterial = resourceCache.GetResource< CMaterial >( "materials/pulse_green_red.mat" );

		const auto pulseMesh = std::make_shared< CMesh >( GL_TRIANGLE_STRIP, Primitives::cube, pulseMaterial );

		m_pulseEntity = m_scene.CreateEntity( "pulse_cube" );
		m_pulseEntity->Transform.Position( { 0.0f, 10.0f, 1.0f } );
		m_pulseEntity->Transform.Scale( { 2.0f, 2.0f, 1.0f } );
		m_pulseEntity->Add<CModelComponent>( pulseMesh );
	}

	{
		const auto redMaterial = resourceCache.GetResource< CMaterial >( "materials/red.mat" );

		const auto cubeMesh = std::make_shared< CMesh >( GL_TRIANGLE_STRIP, Primitives::cube, redMaterial );

		const auto cubeEntity = m_scene.CreateEntity( "red_cube" );
		cubeEntity->Transform.Position( { 4.0f, 10.0f, 1.0f } );
		cubeEntity->Transform.Scale( { 2.0f, 2.0f, 1.0f } );
		cubeEntity->Add<CModelComponent>( cubeMesh );
	}

	{
		const auto material3 = resourceCache.GetResource< CMaterial >( "materials/sky.mat" );

		const CMesh::TMeshTextureSlots skyMeshTextureSlots = { { "skyboxTexture", std::make_shared< CMeshTextureSlot >( resourceCache.GetResource< CTexture >( "textures/cube/sixtine/sixtine.cub" ), samplerManager.GetFromType( CSampler::SamplerType::EDGE_CUBE ) ) } };

		const auto skyboxMesh = std::make_shared< CMesh >( GL_TRIANGLES, Primitives::cube, material3, skyMeshTextureSlots );

		m_skyboxEntity = m_scene.CreateEntity( "skybox" );
		m_skyboxEntity->Add<CModelComponent>( skyboxMesh );
	}

	m_backgroundMusic = std::make_shared< CSoundSource >( resourceCache.GetResource< CSoundBuffer >( "music/rise_of_spirit.ogg" ) );
	m_backgroundMusic->Play();
	m_backgroundMusic->SetLooping( true );
}

CStateGame::~CStateGame()
{
}

std::shared_ptr< CState > CStateGame::OnUpdate( void )
{
	auto &soundManager = m_engineInterface.SoundManager;

	const auto &input = m_engineInterface.Input;

	if( input.KeyDown( SDL_SCANCODE_ESCAPE ) )
	{
		logINFO( "pause" );
		return( std::make_shared< CStatePause >( m_filesystem, m_settings, m_engineInterface, shared_from_this() ) );
	}

	const float spp = 2.0f * m_settings.engine.tick / 1000000;

	// TODO maybe create a way for systems to hook into the inputsystem, so we can provide globally active keyboard-shortcuts?

	/*
	 * move pulseMesh
	 */
	{
		auto pos = m_pulseEntity->Transform.Position();
		pos.y = 10.0f + ( sin( m_timer.Time() / 2000000.0f ) * 5.0f );
		m_pulseEntity->Transform.Position( pos );
	}

	/*
	 * move meshMovable
	 */

	if( input.KeyStillDown( SDL_SCANCODE_KP_6 ) )
	{
		glm::vec3 pos = m_movableEntity->Transform.Position();
		pos.x += spp;
		m_movableEntity->Transform.Position( pos );
	}
	if( input.KeyStillDown( SDL_SCANCODE_KP_4 ) )
	{
		glm::vec3 pos = m_movableEntity->Transform.Position();
		pos.x -= spp;
		m_movableEntity->Transform.Position( pos );
	}

	if( input.KeyStillDown( SDL_SCANCODE_KP_8 ) )
	{
		glm::vec3 pos = m_movableEntity->Transform.Position();
		pos.y += spp;
		m_movableEntity->Transform.Position( pos );
	}
	if( input.KeyStillDown( SDL_SCANCODE_KP_2 ) )
	{
		glm::vec3 pos = m_movableEntity->Transform.Position();
		pos.y -= spp;
		m_movableEntity->Transform.Position( pos );
	}

	if( input.KeyStillDown( SDL_SCANCODE_KP_PLUS ) )
	{
		glm::vec3 pos = m_movableEntity->Transform.Position();
		pos.z += spp;
		m_movableEntity->Transform.Position( pos );
	}
	if( input.KeyStillDown( SDL_SCANCODE_KP_MINUS ) )
	{
		glm::vec3 pos = m_movableEntity->Transform.Position();
		pos.z -= spp;
		m_movableEntity->Transform.Position( pos );
	}

	if( input.KeyStillDown( SDL_SCANCODE_KP_5 ) )
	{
		m_movableEntity->Transform.Position( glm::vec3( 1.0f ) );
	}

	/*
	 * move camera
	 */

	const float ctrlPressedMult = input.KeyStillDown( SDL_SCANCODE_LCTRL ) ? 1 : 10;

	const auto &cameraFree = m_cameraEntity->Get<CCameraFreeComponent>();

	if( input.MouseStillDown( SDL_BUTTON_LEFT) )
	{
		cameraFree->Rotate( input.MouseDeltaY(), input.MouseDeltaX() );
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

	soundManager.SetListener( m_cameraEntity->Transform );

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

		const std::shared_ptr< const CImage > screenshot = m_frameBuffer.ToImage();

		if( !ImageHandler::Save( m_filesystem, screenshot, m_settings.renderer.screenshot.scale_factor, m_settings.renderer.screenshot.format, screenshotPath ) )
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

	if( !input.MouseStillDown( SDL_BUTTON_LEFT) )
	{
		m_movableEntity->Transform.Rotate( m_roty_ps, m_rotx_ps, 0.0f );
	}

	m_rotx_ps = input.MouseDeltaX();
	m_roty_ps = input.MouseDeltaY();

	// TODO cameraFree->Direction( m_movableEntity->Transform.Position() - m_cameraFree->Transform.Position() );

	m_skyboxEntity->Transform.Position( m_cameraEntity->Transform.Position() );

	return( shared_from_this() );
}