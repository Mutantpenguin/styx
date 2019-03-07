#pragma once

#include "src/resource/CResourceCache.hpp"

#include "src/sound/CSoundBuffer.hpp"
#include "src/sound/CSoundBufferLoader.hpp"

class CSoundBufferCache final : public CResourceCache< CSoundBuffer >
{
public:
	explicit CSoundBufferCache( const CFileSystem &filesystem );

public:
	virtual void Load( const std::shared_ptr< CSoundBuffer > &resource, const CSoundBuffer::ResourceIdType &id )
	{
		m_soundBufferloader.FromFile( resource, id );
	};

	i64 GetMtime( const CSoundBuffer::ResourceIdType &id )
	{
		return( m_filesystem.GetLastModTime( id ) );
	}

	const CSoundBufferLoader m_soundBufferloader;
};
