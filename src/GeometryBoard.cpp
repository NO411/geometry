#include "GeometryBoard.h"
#include "GeometryApp.h"
#include "math/MathMisc.h"
#include "raylib.h"

const float GeometryBoard::movementSpeed = 6;
const float GeometryBoard::maxZoom = 6;
const int GeometryBoard::connectionDistance = 8;

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
	SetEditMode();
	InterruptDrawing();
	ModifyViewField();
}

void GeometryBoard::SetEditMode()
{
	if (!IsKeyDown(KEY_LEFT_CONTROL))
	{
		return;
	}

	switch (GetKeyPressed())
	{
	case KEY_C:
		editMode = DRAW_CIRCLE;
		break;
	case KEY_D:
		editMode = DRAW_DISTANCE;
		break;
	case KEY_R:
		editMode = DRAW_RAY;
		break;
	case KEY_S:
		editMode = DRAW_STRAIGHT_LINE;
		break;
	case KEY_P:
		editMode = DRAW_POINT;
		break;
	case KEY_E:
		editMode = ERASER;
		if (IsKeyDown(KEY_M))
		{
			if (IsKeyDown(KEY_D))
			{
				editMode = DISTANCE_MEASUREMENT_ERASER;
			}
			else if (IsKeyDown(KEY_A))
			{
				editMode = ANGLE_MEASUREMENT_ERASER;
			}
		}
		if (IsKeyDown(KEY_C))
		{
			editMode = CIRCLE_ERASER;
		}
		break;
	case KEY_M:
		if (IsKeyDown(KEY_D))
		{
			editMode = DISTANCE_MEASUREMENT;
		}
		else if (IsKeyDown(KEY_A))
		{
			editMode = ANGLE_MEASUREMENT;
		}
		break;
	default:
		break;
	}
}

void GeometryBoard::InterruptDrawing()
{
	if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_LEFT_CONTROL))
	{
		firstPointed = false;
	}
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
