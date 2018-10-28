#pragma once

#include <cstdint>
#include <vector>
#include <memory>

#include "src/engine/helper/image/CImage.hpp"

class C2DArrayData final
{
public:
	bool AddLayer( const std::shared_ptr< const CImage > &image );

	const std::vector< std::shared_ptr< const CImage > > &getLayers( void ) const;

private:
	std::vector< std::shared_ptr< const CImage > > m_layers;
};
