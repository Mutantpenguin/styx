#ifndef CWORLD_HPP
#define CWORLD_HPP

#include <glm/glm.hpp>

class CWorld final
{
private:
	CWorld(const CWorld& rhs);
	CWorld& operator=(const CWorld& rhs);

public:
	CWorld();
	~CWorld();

	static constexpr glm::vec3 Z	{ 0.0f, 0.0f, 1.0f };
	static constexpr glm::vec3 Y	{ 0.0f, 1.0f, 0.0f };
	static constexpr glm::vec3 X	{ 1.0f, 0.0f, 0.0f };
};

#endif // CWORLD_HPP