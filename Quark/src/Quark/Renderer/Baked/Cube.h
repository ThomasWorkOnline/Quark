#pragma once

#include <cstdint>

namespace Quark {

	namespace Cube {

		static constexpr float Vertices[] = {
			// front
		   -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,   0.0f,  0.0f,  1.0f,
			0.5f, -0.5f,  0.5f,   1.0f, 0.0f,   0.0f,  0.0f,  1.0f,
			0.5f,  0.5f,  0.5f,   1.0f, 1.0f,   0.0f,  0.0f,  1.0f,
		   -0.5f,  0.5f,  0.5f,   0.0f, 1.0f,   0.0f,  0.0f,  1.0f,

		    // right						   
		    0.5f, -0.5f,  0.5f,   0.0f, 0.0f,   1.0f,  0.0f,  0.0f,
		    0.5f, -0.5f, -0.5f,   1.0f, 0.0f,   1.0f,  0.0f,  0.0f,
		    0.5f,  0.5f, -0.5f,   1.0f, 1.0f,   1.0f,  0.0f,  0.0f,
		    0.5f,  0.5f,  0.5f,   0.0f, 1.0f,   1.0f,  0.0f,  0.0f,

		    // back
		    0.5f, -0.5f, -0.5f,   0.0f, 0.0f,   0.0f,  0.0f, -1.0f,
		   -0.5f, -0.5f, -0.5f,   1.0f, 0.0f,   0.0f,  0.0f, -1.0f,
		   -0.5f,  0.5f, -0.5f,   1.0f, 1.0f,   0.0f,  0.0f, -1.0f,
		    0.5f,  0.5f, -0.5f,   0.0f, 1.0f,   0.0f,  0.0f, -1.0f,

		    // left
		   -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,  -1.0f,  0.0f,  0.0f,
		   -0.5f, -0.5f,  0.5f,   1.0f, 0.0f,  -1.0f,  0.0f,  0.0f,
		   -0.5f,  0.5f,  0.5f,   1.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
		   -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
															  
		    // bottom										  
		   -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,   0.0f, -1.0f,  0.0f,
		    0.5f, -0.5f, -0.5f,   1.0f, 0.0f,   0.0f, -1.0f,  0.0f,
		    0.5f, -0.5f,  0.5f,   1.0f, 1.0f,   0.0f, -1.0f,  0.0f,
		   -0.5f, -0.5f,  0.5f,   0.0f, 1.0f,   0.0f, -1.0f,  0.0f,
															  
		    // top											  
		   -0.5f,  0.5f,  0.5f,   0.0f, 0.0f,   0.0f,  1.0f,  0.0f,
		    0.5f,  0.5f,  0.5f,   1.0f, 0.0f,   0.0f,  1.0f,  0.0f,
		    0.5f,  0.5f, -0.5f,   1.0f, 1.0f,   0.0f,  1.0f,  0.0f,
		   -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,   0.0f,  1.0f,  0.0f
		};

		static constexpr uint32_t Indices[] = {
			// front
			0, 2, 3,
			1, 2, 0,

			// right
			4, 6, 7,
			5, 6, 4,

			// back
			8, 10, 11,
			9, 10, 8,

			// left
			12, 14, 15,
			13, 14, 12,

			// bottom
			16, 18, 19,
			17, 18, 16,

			// top
			20, 22, 23,
			21, 22, 20
		};
	}
}
