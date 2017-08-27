#ifndef CRESOURCECACHEMANAGER_HPP
#define CRESOURCECACHEMANAGER_HPP

#include <unordered_map>

#include <typeindex>

#include <memory>

#include "src/engine/resource/CResourceCache.hpp"

class CResourceCacheManager final
{
private:
	CResourceCacheManager( const CResourceCacheManager& rhs );
	CResourceCacheManager& operator = ( const CResourceCacheManager& rhs );

public:
	CResourceCacheManager( void );
	~CResourceCacheManager( void );

	template < typename T >
	void Register( const std::shared_ptr< CResourceCache< T > > &resourceCache )
	{
		auto type_index = std::type_index( typeid( T ) );

		const auto it = m_resourceCaches.find( type_index );

		if( it == std::cend( m_resourceCaches ) )
		{
			m_resourceCaches.insert( std::make_pair( type_index, resourceCache ) );
		}
		else
		{
			logWARNING( "resource cache '{0}' already registered", resourceCache->Name() )
		}
	}

	void DeRegister( const std::shared_ptr< CResourceCacheBase > &resourceCache );

	void CollectGarbage( void );
	void Reload( void );

	template < typename T >
	const std::shared_ptr< const T > GetResource( const std::string &path )
	{
		#ifdef INQ_DEBUG
			auto it = m_resourceCaches.find( std::type_index( typeid( T ) ) );

			if( std::cend( m_resourceCaches ) == it )
			{
				const std::string msg = fmt::format( "no resource cache registered for type '{0}'", typeid( T ).name() );
				logERROR( msg );
				throw std::runtime_error( msg );
			}
		#endif

		auto resourceCache = std::static_pointer_cast< CResourceCache< T > >( m_resourceCaches[ std::type_index( typeid( T ) ) ] );

		return( resourceCache->GetResource( path ) );
	}

private:
	std::unordered_map< std::type_index, const std::shared_ptr< CResourceCacheBase > > m_resourceCaches;
};

#endif // CRESOURCECACHEMANAGER_HPP
