#include "GeometryBoard.h"
#include "raylib.h"

GeometryBoard::GeometryBoard() {
	camera = {0};
	camera.target = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
	camera.offset = camera.target;
	camera.rotation = 0;
	camera.zoom = 1;
}

GeometryBoard::~GeometryBoard() {}

void GeometryBoard::Render()
{
	BeginMode2D(camera);
	EndMode2D();
}
