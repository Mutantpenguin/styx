#ifndef CCUBEMAPDATA_HPP
#define CCUBEMAPDATA_HPP

#include <array>
#include <memory>

#include "src/engine/helper/image/CImage.hpp"

class CCubemapData final
{
public:
	static const unsigned char countCubemapFaces = 6;

	bool AddFace( const unsigned char faceNum, const std::shared_ptr< const CImage > &image );

	const std::array< std::shared_ptr< const CImage >, countCubemapFaces > &getFaces( void ) const;

	bool isComplete( void ) const;

private:
	std::array< std::shared_ptr< const CImage >, countCubemapFaces > m_faces;
};

#endif // CCUBEMAPDATA_HPP
