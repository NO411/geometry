#include "GeometryBoard.h"
#include "GeometryApp.h"
#include "GeometryObjetcs.h"
#include "math/MathMisc.h"
#include "math/Intersections.h"
#include "raylib.h"

const float GeometryBoard::movementSpeed = 6;
const float GeometryBoard::minZoom = 0.125;
const float GeometryBoard::maxZoom = 6;
const int GeometryBoard::connectionDistance_ = 8;
int GeometryBoard::connectionDistance = connectionDistance_;

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
	for (auto &point : points)
	{
		point.Render(camera, app_->font);
	}
	
	DrawDrawingObj();
	currentPoint.Render(camera);

	BeginMode2D(camera);
	EndMode2D();
}

void GeometryBoard::DrawDrawingObj()
{
	if (!firstPointed)
	{
		return;
	}

	Vector2 currentPos = currentPoint.GetPos();

	switch (editMode)
	{
	case DRAW_CIRCLE:
	{
		Circle currentCircle(firstPoint, GetDistance(firstPoint, currentPos));
		currentCircle.Render(camera);
	}
	break;
	case DRAW_DISTANCE:
	{
		Distance currentDistance(firstPoint, currentPos);
		currentDistance.Render(camera);
	}
	break;
	case DRAW_RAY:
	{
		Ray2 currentRay(firstPoint, currentPos, camera);
		currentRay.Render(camera);
	}
	break;
	case DRAW_STRAIGHT_LINE:
	{
		StraightLine currentStraightLine(firstPoint, currentPos, camera);
		currentStraightLine.Render();
	}
	break;
	case CIRCLE_ERASER:
	{
		// DrawCurrentCircleSector();
	}
	break;
	default:
		break;
	}
}

void GeometryBoard::Update()
{
	InputHandler();
	CheckResized();
}

void GeometryBoard::UpdateConnectionDistance()
{
	connectionDistance = 8 / camera.zoom;
}

void GeometryBoard::CheckResized()
{
	if (!IsWindowResized())
	{
		return;
	}

	UpdateAllDrawPoints();
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
		firstPoint = currentPoint.GetPos();
		switch (editMode)
		{
		case DRAW_POINT:
			firstPointed = false;
			for (auto &point : points)
			{
				if (GetDistance(point.GetPos(), currentPoint.GetPos()) < connectionDistance)
				{
					return;
				}
			}
			points.push_back(Point{currentPoint.GetPos(), this});
			break;
		case ERASER:
			firstPointed = false;
			// EraseObj(objTuple);
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
			// firstCircleEraserPoint = currentPoint;
			// currentObjPos = objPos;
			break;
		default:
			break;
		}
	}
	else
	{
		firstPointed = false;
		Vector2 currentPos = currentPoint.GetPos();
		switch (editMode)
		{
		case DRAW_CIRCLE:
		{
			Circle newCircle = {firstPoint, GetDistance(firstPoint, currentPos)};
			circles.push_back(newCircle);
			AddIntersections(newCircle);
		}
		break;
		case DRAW_DISTANCE:
		{
			Distance newDistance = {firstPoint, currentPos};
			distances.push_back(newDistance);
			AddIntersections(newDistance);
		}
		break;
		case DRAW_RAY:
		{
			Ray2 newRay = {firstPoint, currentPos, camera};
			rays.push_back(newRay);
			AddIntersections(newRay);
		}
		break;
		case DRAW_STRAIGHT_LINE:
		{
			StraightLine newStraightLine = {firstPoint, currentPos, camera};
			straightLines.push_back(newStraightLine);
			AddIntersections(newStraightLine);
		}
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
			// circles.at(objPos).EraseSector(&firstCircleEraserPoint, &currentPoint);
			break;
		default:
			break;
		}
	}
}

void GeometryBoard::UpdateCurrentPoint()
{
	Vector2 newPos = GetMousePosition2(camera);
	currentPoint.SetPos(newPos);
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

		UpdateConnectionDistance();
	}

	bool viewFieldModified = zoom || resetZoom || IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN);

	if (viewFieldModified)
	{
		UpdateAllDrawPoints();
	}
}

void GeometryBoard::UpdateAllDrawPoints()
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


bool GeometryBoard::PointLetterExists(std::string &letter)
{
	for (auto &point : points)
	{
		if (point.GetLetter() == letter)
		{
			return true;
		}
	}
	return false;
}

void GeometryBoard::AddIntersections(Circle &circle)
{
	for (auto distance : distances)
	{
		GetDistanceCircleIntersections(intersections, distance, circle);
	}
	for (auto straightLine : straightLines)
	{
		GetStraightLineCircleIntersections(intersections, straightLine, circle);
	}
	for (auto ray : rays)
	{
		GetRayCircleIntersections(intersections, ray, circle);
	}
	for (auto circle_ : circles)
	{
		GetCircleCircleIntersections(intersections, circle, circle_);
	}
}

void GeometryBoard::AddIntersections(Distance &distance)
{
	for (auto distance_ : distances)
	{
		GetDistanceDistanceIntersections(intersections, distance, distance_);
	}
	for (auto straightLine : straightLines)
	{
		GetStraightLineDistanceIntersections(intersections, straightLine, distance);
	}
	for (auto ray : rays)
	{
		GetRayDistanceIntersections(intersections, ray, distance);
	}
	for (auto circle : circles)
	{
		GetDistanceCircleIntersections(intersections, distance, circle);
	}
}

void GeometryBoard::AddIntersections(Ray2 &ray)
{
	for (auto distance : distances)
	{
		GetRayDistanceIntersections(intersections, ray, distance);
	}
	for (auto straightLine : straightLines)
	{
		GetStraightLineRayIntersections(intersections, straightLine, ray);
	}
	for (auto ray_ : rays)
	{
		GetRayRayIntersections(intersections, ray, ray_);
	}
	for (auto circle : circles)
	{
		GetRayCircleIntersections(intersections, ray, circle);
	}
}

void GeometryBoard::AddIntersections(StraightLine &straightLine)
{
	for (auto distance : distances)
	{
		GetStraightLineDistanceIntersections(intersections, straightLine, distance);
	}
	for (auto straightLine_ : straightLines)
	{
		GetStraightLineStraightLinieIntersections(intersections, straightLine, straightLine_);
	}
	for (auto ray : rays)
	{
		GetStraightLineRayIntersections(intersections, straightLine, ray);
	}
	for (auto circle : circles)
	{
		GetStraightLineCircleIntersections(intersections, straightLine, circle);
	}	
}
