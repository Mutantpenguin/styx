#ifndef CSCENE_HPP
#define CSCENE_HPP

#include <list>

#include "../renderer/CCamera.hpp"
#include "../renderer/CMesh.hpp"

class CScene
{
public:
	CScene();
	~CScene();

	void AddMesh( std::shared_ptr< CMesh > mesh );
	void RemoveMesh( std::shared_ptr< CMesh > mesh );

	const std::list< std::shared_ptr< CMesh > > &Meshes( void ) const;

	const std::shared_ptr< CCamera > Camera( void ) const;
	void Camera( std::shared_ptr< CCamera > camera );

private:
	std::list< std::shared_ptr< CMesh > > m_meshes;

	std::shared_ptr< CCamera > m_camera;
};

#endif // CSCENE_HPP
