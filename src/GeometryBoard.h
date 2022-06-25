#pragma once

#include "raylib.h"

class GeometryBoard
{
public:
	GeometryBoard();
	~GeometryBoard();

	void Render();
private:
	Camera2D camera;
};
