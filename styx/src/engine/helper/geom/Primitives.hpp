#pragma once

#include <vector>

#include <glm/glm.hpp>

namespace Primitives
{
	struct SVertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoord;
		/* TODO Tangents
		glm::vec3 Tangent;
		glm::vec3 Bitangent;
		*/
	};

	struct SPrimitive
	{
		// TODO put mode (i.e. GL_TRIANGLE_STRIP or GL_TRIANGLES ) into the struct

		std::vector< SVertex > Vertices;
		// TODO assimp std::vector< u32 > Indices;
	};

	static const SPrimitive quad	{
										{
											{	{ -1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, +1.0f }, {  0.0f,  0.0f }	},
											{	{ +1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, +1.0f }, { +1.0f,  0.0f }	},
											{	{ -1.0f, +1.0f, 0.0f }, { 0.0f, 0.0f, +1.0f }, {  0.0f, +1.0f }	},
											{	{ +1.0f, +1.0f, 0.0f }, { 0.0f, 0.0f, +1.0f }, { +1.0f, +1.0f }	}
										}
									};

	static const SPrimitive cube	{
										{
											// back
											{	{ +1.0f, -1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f }, {  0.0f, +1.0f }	},
											{	{ -1.0f, -1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f }, {  0.0f,  0.0f }	},
											{	{ -1.0f, +1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f }, { +1.0f,  0.0f }	},
											{	{ -1.0f, +1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f }, { +1.0f,  0.0f }	},
											{	{ +1.0f, +1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f }, { +1.0f, +1.0f }	},
											{	{ +1.0f, -1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f }, {  0.0f, +1.0f }	},

											// left
											{	{ -1.0f, +1.0f, -1.0f }, { -1.0f,  0.0f,  0.0f }, {  0.0f, +1.0f }	},
											{	{ -1.0f, -1.0f, -1.0f }, { -1.0f,  0.0f,  0.0f }, {  0.0f,  0.0f }	},
											{	{ -1.0f, -1.0f, +1.0f }, { -1.0f,  0.0f,  0.0f }, { +1.0f,  0.0f }	},
											{	{ -1.0f, -1.0f, +1.0f }, { -1.0f,  0.0f,  0.0f }, { +1.0f,  0.0f }	},
											{	{ -1.0f, +1.0f, +1.0f }, { -1.0f,  0.0f,  0.0f }, { +1.0f,  1.0f }	},
											{	{ -1.0f, +1.0f, -1.0f }, { -1.0f,  0.0f,  0.0f }, {  0.0f,  1.0f }	},

											// right
											{	{ +1.0f, +1.0f, +1.0f }, { +1.0f,  0.0f,  0.0f }, {  0.0f, +1.0f }	},
											{	{ +1.0f, -1.0f, +1.0f }, { +1.0f,  0.0f,  0.0f }, {  0.0f,  0.0f }	},
											{	{ +1.0f, -1.0f, -1.0f }, { +1.0f,  0.0f,  0.0f }, { +1.0f,  0.0f }	},
											{	{ +1.0f, -1.0f, -1.0f }, { +1.0f,  0.0f,  0.0f }, { +1.0f,  0.0f }	},
											{	{ +1.0f, +1.0f, -1.0f }, { +1.0f,  0.0f,  0.0f }, { +1.0f, +1.0f }	},
											{	{ +1.0f, +1.0f, +1.0f }, { +1.0f,  0.0f,  0.0f }, {  0.0f, +1.0f }	},

											// front
											{	{ +1.0f, +1.0f, +1.0f }, {  0.0f,  0.0f, +1.0f }, {  0.0f, +1.0f }	},
											{	{ -1.0f, +1.0f, +1.0f }, {  0.0f,  0.0f, +1.0f }, {  0.0f,  0.0f }	},
											{	{ -1.0f, -1.0f, +1.0f }, {  0.0f,  0.0f, +1.0f }, { +1.0f,  0.0f }	},
											{	{ -1.0f, -1.0f, +1.0f }, {  0.0f,  0.0f, +1.0f }, { +1.0f,  0.0f }	},
											{	{ +1.0f, -1.0f, +1.0f }, {  0.0f,  0.0f, +1.0f }, { +1.0f, +1.0f }	},
											{	{ +1.0f, +1.0f, +1.0f }, {  0.0f,  0.0f, +1.0f }, {  0.0f, +1.0f }	},

											// top
											{	{ +1.0f, +1.0f, +1.0f }, {  0.0f, +1.0f,  0.0f }, {  0.0f, +1.0f }	},
											{	{ +1.0f, +1.0f, -1.0f }, {  0.0f, +1.0f,  0.0f }, {  0.0f,  0.0f }	},
											{	{ -1.0f, +1.0f, -1.0f }, {  0.0f, +1.0f,  0.0f }, { +1.0f,  0.0f }	},
											{	{ -1.0f, +1.0f, -1.0f }, {  0.0f, +1.0f,  0.0f }, { +1.0f,  0.0f }	},
											{	{ -1.0f, +1.0f, +1.0f }, {  0.0f, +1.0f,  0.0f }, { +1.0f, +1.0f }	},
											{	{ +1.0f, +1.0f, +1.0f }, {  0.0f, +1.0f,  0.0f }, {  0.0f, +1.0f }	},

											// bottom
											{	{ -1.0f, -1.0f, -1.0f }, {  0.0f, -1.0f,  0.0f }, {  0.0f, +1.0f }	},
											{	{ +1.0f, -1.0f, -1.0f }, {  0.0f, -1.0f,  0.0f }, {  0.0f,  0.0f }	},
											{	{ +1.0f, -1.0f, +1.0f }, {  0.0f, -1.0f,  0.0f }, { +1.0f,  0.0f }	},
											{	{ +1.0f, -1.0f, +1.0f }, {  0.0f, -1.0f,  0.0f }, { +1.0f,  0.0f }	},
											{	{ -1.0f, -1.0f, +1.0f }, {  0.0f, -1.0f,  0.0f }, { +1.0f, +1.0f }	},
											{	{ -1.0f, -1.0f, -1.0f }, {  0.0f, -1.0f,  0.0f }, {  0.0f, +1.0f }	}
										}
									};
}
