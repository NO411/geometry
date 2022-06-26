#include "GeometryBoard.h"
#include "GeometryApp.h"
#include "raylib.h"

const float GeometryBoard::movementSpeed = 6;
const float GeometryBoard::maxZoom = 6;

GeometryBoard::GeometryBoard() {}

GeometryBoard::GeometryBoard(GeometryApp *app) : app_(app)
{
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

void GeometryBoard::Update()
{
	InputHandler();
	CheckResized();
}

void GeometryBoard::CheckResized()
{
	if (!IsWindowResized())
	{
		return;
	}

	camera.target = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
	camera.offset = camera.target;
}

void GeometryBoard::InputHandler()
{
	// Edit();
	// SetDrawObj();
	// InterruptDrawing();
	ModifyViewField();
}

void GeometryBoard::ModifyViewField()
{
	if (IsKeyDown(KEY_LEFT))
	{
		camera.offset.x -= movementSpeed;
	}
	else if (IsKeyDown(KEY_RIGHT))
	{
		camera.offset.x += movementSpeed;
	}

	if (IsKeyDown(KEY_UP))
	{
		camera.offset.y -= movementSpeed;
	}
	else if (IsKeyDown(KEY_DOWN))
	{
		camera.offset.y += movementSpeed;
		camera.zoom = 1;
	}

	camera.zoom += (GetMouseWheelMove() * 0.1f);

    if (camera.zoom > maxZoom)
	{
		camera.zoom = maxZoom;
	}
    else if (camera.zoom < 0.1)
	{
		camera.zoom = 0.1;
	};

	if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyDown(KEY_Y) && IsKeyPressed(KEY_R))
	{
		camera.zoom = 1;
	}
}
