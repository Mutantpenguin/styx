#include "CCamera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "src/engine/logger/CLogger.hpp"


CCamera::CCamera( float aspectRatio, float fov, float zNear, float zFar ) :
	m_aspectRatio { aspectRatio },
	m_fov { fov },
	m_zNear { zNear },
	m_zFar { zFar }
{
}

void CCamera::UpdateFrustum( void )
{
	m_frustum.Update( CalculateViewProjectionMatrix() );
}

void CCamera::SetFOV( float fov )
{
	m_fov = fov;
}

void CCamera::SetZNear( float zNear )
{
	m_zNear = zNear;
}

void CCamera::SetZFar( float zFar )
{
	m_zFar = zFar;
}

void CCamera::SetPosition( const glm::vec3 &position )
{
	m_position = position;
}

float CCamera::FOV( void ) const
{
	return( m_fov );
}

float CCamera::ZNear( void ) const
{
	return( m_zNear );
}

float CCamera::ZFar( void ) const
{
	return( m_zFar );
}

glm::vec3 const &CCamera::Position( void ) const
{
	return( m_position );
}

glm::vec3 const CCamera::Direction( void ) const
{
	return( worldZ * m_orientation );
}

glm::vec3 const CCamera::Up( void ) const
{
	return( worldY * m_orientation );
}

const CFrustum &CCamera::Frustum( void ) const
{
	return( m_frustum );
}

const glm::mat4 CCamera::CalculateProjectionMatrix( void ) const
{
	return( glm::perspective( m_fov, m_aspectRatio, m_zNear, m_zFar ) );
}

const glm::mat4 CCamera::CalculateViewMatrix( void ) const
{
	return( glm::translate( glm::toMat4( m_orientation ), -m_position ) );
}

const glm::mat4 CCamera::CalculateViewProjectionMatrix( void ) const
{
	return( CalculateProjectionMatrix() * CalculateViewMatrix() );
}
