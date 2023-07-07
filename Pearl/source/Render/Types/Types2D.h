#pragma once
#include <glm/glm.hpp>

#include "PearlCore.h"

namespace PEARL_NAMESPACE::types2D
{
	struct Point2D
	{
		float x, y;
	};

	struct Point3D
	{
		float x, y, z;


		Point3D operator+(const Point3D& v1) const
		{
			return Point3D{ this->x + v1.x, this->y + v1.y, this->z + v1.z };
		}
	};

	struct Rect2D
	{
		float x, y, w, h;

		Rect2D(const Point2D topLeft, const Point2D bottomRight)
		{
			x = topLeft.x;
			y = topLeft.y;
			w = bottomRight.x - topLeft.x;
			h = bottomRight.y - topLeft.y;
		}
	};
}