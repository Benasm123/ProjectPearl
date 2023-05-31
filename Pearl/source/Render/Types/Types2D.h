#pragma once

namespace Types2D
{
	struct Point2D
	{
		float x, y;
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