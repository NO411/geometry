#include "GeometryBoard.h"
#include "GeometryApp.h"
#include "GeometryObjetcs.h"
#include "math/MathMisc.h"
#include "raylib.h"

const float GeometryBoard::movementSpeed = 6;
const float GeometryBoard::minZoom = 0.125;
const float GeometryBoard::maxZoom = 6;
const int GeometryBoard::connectionDistance = 8;

GeometryBoard::GeometryBoard() {}

GeometryBoard::GeometryBoard(GeometryApp *app) : app_(app)
{
	camera = {0};
	camera.zoom = 1;
}

GeometryBoard::~GeometryBoard() {}

void GeometryBoard::Render()
{
	for (auto &distance : distances)
	{
		distance.Render(camera);
	}
	for (auto &straightLine : straightLines)
	{
		straightLine.Render();
	}
	for (auto &ray : rays)
	{
		ray.Render(camera);
	}
	for (auto &circle : circles)
	{
		circle.Render(camera);
	}

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

	// update straight line connection points
}

void GeometryBoard::InputHandler()
{
	Edit();
	SetEditMode();
	InterruptDrawing();
	ModifyViewField();
}

void GeometryBoard::Edit()
{
	UpdateCurrentPoint();
	if (!(IsMouseButtonPressed(0) || IsKeyPressed(KEY_ENTER)) || app_->button.Selected())
	{
		return;
	}

	if (!firstPointed)
	{
		firstPointed = true;
		firstPoint = {currentPoint};
		switch (editMode)
		{
		case DRAW_POINT:
			firstPointed = false;
			/*
			for (auto &point : points)
			{
				
				if (GetDistance(&point.point, &currentPoint) < connection_distance)
				{
					return;
				}
				
			}
			*/
			//points.push_back({currentPoint});
			break;
		case ERASER:
			firstPointed = false;
			//EraseObj(objTuple);
			break;
		case DISTANCE_MEASUREMENT:
			firstPointed = false;
			/*
			if (objType == DISTANCE)
			{
				distances.at(objPos).showLength = true;
			}
			*/
			break;
		case ANGLE_MEASUREMENT:
			firstPointed = false;
			break;
		case DISTANCE_MEASUREMENT_ERASER:
			firstPointed = false;
			/*
			if (objType == DISTANCE)
			{
				distances.at(objPos).showLength = false;
			}
			*/
			break;
		case ANGLE_MEASUREMENT_ERASER:
			firstPointed = false;
			break;
		case CIRCLE_ERASER:
			/*
			if (objType != CIRCLE)
			{
				firstPointed = false;
				break;
			}
			*/
			//firstCircleEraserPoint = currentPoint;
			//currentObjPos = objPos;
			break;
		default:
			break;
		}
	}
	else
	{
		firstPointed = false;
		switch (editMode)
		{
		case DRAW_CIRCLE:
			circles.push_back(Circle{firstPoint, GetDistance(firstPoint, currentPoint)});
			break;
		case DRAW_DISTANCE:
			distances.push_back(Distance{firstPoint, currentPoint});
			break;
		case DRAW_RAY:
			rays.push_back(Ray2{firstPoint, currentPoint, camera});
			break;
		case DRAW_STRAIGHT_LINE:
			straightLines.push_back(StraightLine{firstPoint, currentPoint, camera});
			break;
		case CIRCLE_ERASER:
			/*3
			if (objType != CIRCLE)
			{
				firstPointed = true;
				break;
			}
			if (objPos != currentObjPos)
			{
				firstPointed = true;
				firstCircleEraserPoint = currentPoint;
				currentObjPos = objPos;
				break;
			}
			*/
			//circles.at(objPos).EraseSector(&firstCircleEraserPoint, &currentPoint);
			break;
		default:
			break;
		}
	}
}

void GeometryBoard::UpdateCurrentPoint()
{
	currentPoint = GetMousePosition2(camera);
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

	bool resetZoom = IsKeyDown(KEY_LEFT_CONTROL) && IsKeyDown(KEY_Y) && IsKeyPressed(KEY_R);
	if (resetZoom)
	{
		camera.zoom = 1;
		camera.target = {0, 0};
		camera.offset = {0, 0};
	}

	float wheel = GetMouseWheelMove();
	bool zoom = wheel != 0;
	if (zoom)
	{
		// zoom to the mouse position
		// save this before setting camera.offset because it changes the camera which is needed for the target
		Vector2 mouseWorldPos = GetMousePosition2(camera);
		camera.offset = GetMousePosition();
		camera.target = mouseWorldPos;

		camera.zoom += wheel * 0.125f;
		if (camera.zoom < minZoom)
		{
			camera.zoom = minZoom;
		}
		else if (camera.zoom > maxZoom)
		{
			camera.zoom = maxZoom;
		}
	}

	bool viewFieldModified = zoom || resetZoom || IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN);

	if (viewFieldModified)
	{
		for (auto &straightLine : straightLines)
		{
			straightLine.UpdateDrawPoints(camera);
		}
		for (auto &ray : rays)
		{
			ray.UpdateDrawPoint(camera);
		}
	}
}
