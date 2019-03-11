#pragma once

#include <memory>

#include <glm/glm.hpp>

#include <AL/al.h>
#include <AL/alc.h>

#include "src/sound/CSoundBuffer.hpp"

class CSoundSource final
{
private:
	CSoundSource( const CSoundSource& rhs );
	CSoundSource & operator=( const CSoundSource &rhs );

public:
	explicit CSoundSource( const std::shared_ptr< const CSoundBuffer > &soundBuffer );
	~CSoundSource();

	void SetSoundBuffer( const std::shared_ptr< const CSoundBuffer > &soundBuffer );

	const std::shared_ptr< const CSoundBuffer > Buffer() const;

	void SetPosition( const glm::vec3 &position ) const;
	void SetLooping( const bool looping ) const;
	void SetRelativePositioning( const bool value ) const;

	void Play() const;
	void Pause() const;
	void Stop() const;

private:
	std::shared_ptr< const CSoundBuffer > m_soundBuffer;

	ALuint m_sourceID;
};