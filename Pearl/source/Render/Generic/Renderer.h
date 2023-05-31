#pragma once

class Renderer
{
public:
	virtual ~Renderer() = default;
	virtual bool Render() = 0;
	virtual bool Update() = 0;
};