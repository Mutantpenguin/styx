#include "ctransformsystem.hpp"

#include "../logger/CLogger.hpp"

#include "centity.hpp"
#include "csystem.hpp"

#include "components/transformcomponent.hpp"

glm::vec3 CTransformSystem::GetPosition( EntityID id ) const
{
	auto it = m_components.find( id );
	if( it == m_components.end() )
	{
		LOG( logWARNING ) << "could not find a component with the id '" << id << "'";
		return( glm::vec3( 0 ) );
	}
	else
	{
		return( it->second->position );
	}
}

void CTransformSystem::SetPosition( EntityID id, glm::vec3 pos )
{
	auto it = m_components.find( id );
	if( it == m_components.end() )
	{
		LOG( logWARNING ) << "could not find a component with the id '" << id << "'";
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
