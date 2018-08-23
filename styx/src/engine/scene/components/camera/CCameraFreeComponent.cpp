#include "CCameraFreeComponent.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "src/engine/scene/CWorld.hpp"

CCameraFreeComponent::CCameraFreeComponent( const std::shared_ptr< CEntity > &parent, float aspectRatio, float fov, float zNear, float zFar ) :
	CCameraComponent( parent, zNear, zFar ),
	m_aspectRatio { aspectRatio },
	m_fov { fov }
{
}

void CCameraFreeComponent::FOV( float fov )
{
	if( fov > 155 )
	{
		m_fov = 155;
	}
	else if( fov < 25 )
	{
		m_fov = 25;
	}
	else
	{
		m_fov = fov;
	}
}

float CCameraFreeComponent::FOV( void ) const
{
	return( m_fov );
}

void CCameraFreeComponent::MoveForward( const float distance )
{
	auto &transform = m_parent->Transform;
	transform.Position( transform.Position() - ( transform.Direction() * distance ) );
}

void CCameraFreeComponent::MoveBackward( const float distance )
{
	auto &transform = m_parent->Transform;
	transform.Position( transform.Position() + ( transform.Direction() * distance ) );
}

void CCameraFreeComponent::MoveUp( const float distance )
{
	auto &transform = m_parent->Transform;
	transform.Position( transform.Position() + ( CWorld::Y * distance ) );
}

void CCameraFreeComponent::MoveDown( const float distance )
{
	auto &transform = m_parent->Transform;
	transform.Position( transform.Position() - ( CWorld::Y * distance ) );
}

void CCameraFreeComponent::MoveLeft( const float distance )
{
	auto &transform = m_parent->Transform;
	transform.Position( transform.Position() - ( glm::cross( transform.Up(), transform.Direction() ) * distance ) );
}

void CCameraFreeComponent::MoveRight( const float distance )
{
	auto &transform = m_parent->Transform;
	transform.Position( transform.Position() + ( glm::cross( transform.Up(), transform.Direction() ) * distance ) );
}

void CCameraFreeComponent::Rotate( const float pitchAngle, const float yawAngle )
{
	auto &transform = m_parent->Transform;
	transform.Orientation( glm::angleAxis( glm::radians( pitchAngle ), CWorld::X ) * transform.Orientation() * glm::angleAxis( glm::radians( yawAngle ), CWorld::Y ) );
}

const glm::mat4 CCameraFreeComponent::ProjectionMatrix( void ) const
{
	return( glm::perspective( glm::radians( m_fov ), m_aspectRatio, m_zNear, m_zFar ) );
}