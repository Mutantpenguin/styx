#ifndef CSTATE_HPP
#define CSTATE_HPP

#include <memory>

#include "src/engine/system/CEngineInterface.hpp"
#include "src/engine/system/CTimer.hpp"

#include "src/engine/renderer/CFrameBuffer.hpp"

#include "src/engine/scene/CScene.hpp"

#include "src/engine/system/CFileSystem.hpp"
#include "src/engine/system/CSettings.hpp"

class CState : public std::enable_shared_from_this< CState >
{
private:
	CState( const CState &rhs ) = delete;
	CState& operator = ( const CState &rhs ) = delete;

protected:
	CState( const std::string &name, const CFileSystem &filesystem, const CSettings &settings, CEngineInterface &engineInterface );
	virtual ~CState() {};

public:
	[[nodiscard]] virtual std::shared_ptr< CState > Update( void ) final;
	[[nodiscard]] virtual std::shared_ptr< CState > OnUpdate( void ) = 0;

	virtual void Pause( void ) final;
	virtual void OnPause( void ) {};

	virtual void Resume( void ) final;
	virtual void OnResume( void ) {};

	[[nodiscard]] virtual const CScene &Scene( void ) const final;

	[[nodiscard]] virtual const std::string &Name( void ) const final;

	[[nodiscard]] virtual const CFrameBuffer &FrameBuffer( void ) const final;

	[[nodiscard]] virtual const CTimer &Timer( void ) const final;

protected:
	const std::string	m_name;

	const CFrameBuffer m_frameBuffer;

	const CFileSystem	&m_filesystem;
	const CSettings		&m_settings;

	CTimer m_timer;

	CScene m_scene;

	CEngineInterface &m_engineInterface;

private:
	enum class eStatus
	{
		RUNNING,
		PAUSED
	};

	eStatus m_status = eStatus::RUNNING;
};

#endif // CSTATE_HPP