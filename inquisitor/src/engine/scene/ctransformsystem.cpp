#include "ctransformsystem.hpp"

#include "src/engine/logger/CLogger.hpp"

#include "CEntity.hpp"
#include "csystem.hpp"

#include "components/CTransformComponent.hpp"

/* TODO maybe delete?
 *

glm::vec3 CTransformSystem::GetPosition( EntityID id ) const
{
	const auto it = m_components.find( id );
	if( it == std::end( m_components ) )
	{
		logWARNING( "could not find a component with the id '{0}'", id );
		return( glm::vec3( 0 ) );
	}
	else
	{
		return( it->second->position );
	}
}

void CTransformSystem::SetPosition( EntityID id, glm::vec3 pos )
{
	const auto it = m_components.find( id );
	if( it == std::end( m_components ) )
	{
		logWARNING( "could not find a component with the id '{0}'", id );
	}
	else
	{
		it->second->position = pos;
	}
}

void CSystem::Update( const double )
{
}

SystemID CTransformSystem::systemID = -1;
*/
