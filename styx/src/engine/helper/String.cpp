#include "String.hpp"

namespace String
{
	std::string trim( const std::string& str )
	{
		const std::string::size_type first = str.find_first_not_of( ' ' );
		const std::string::size_type last  = str.find_last_not_of( ' ' );

		return( str.substr( first, ( last - first + 1 ) ) );
	}
}