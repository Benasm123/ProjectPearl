#pragma once
#include <vector>

#include "Renderer.h"
#include "../Types/Types2D.h"

class Renderer2D : public Renderer
{
public:
	virtual void DrawLine(Types2D::Point2D start, Types2D::Point2D end) = 0;
	virtual void DrawLines(std::vector<Types2D::Point2D> points) = 0;

	virtual void DrawRect(Types2D::Rect2D rect) = 0;
	virtual void DrawRects(std::vector<Types2D::Rect2D> rects) = 0;
};
