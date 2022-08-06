#include "GeometryBoard.h"
#include "GeometryApp.h"
#include "GeometryObjetcs.h"
#include "math/MathMisc.h"
#include "math/Intersections.h"
#include "raylib.h"
#include <tuple>
#include <algorithm>

const float GeometryBoard::movementSpeed = 6;
const float GeometryBoard::minZoom = 0.125;
const float GeometryBoard::maxZoom = 6;
const int GeometryBoard::connectionDistance_ = 8;
int GeometryBoard::connectionDistance = connectionDistance_;

int IntersectionStorage::Push(Vec2 &newIntersection)
{
	currentID++;
	intersections[currentID] = newIntersection;
	return currentID;
}

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

	/*// Render intersections
	for (auto &intersection : intersections.intersections)
	{
		Point point(intersection.second, this);
		point.Render(camera);
	}*/

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

	Vec2 currentPos = currentPoint.GetPos();

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

template <typename O>
void GeometryBoard::Measure(O &object)
{
	object.EnableLength(currentPoint.GetPos(), &app_->font);
	object.UpdateLength();
}

void GeometryBoard::Edit()
{
	auto objTuple = UpdateCurrentPoint();
	int objType = std::get<0>(objTuple);
	std::size_t objPos = std::get<1>(objTuple);

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
			EraseObj(objType, objPos);
			break;
		case LENGTH_MEASUREMENT:
			firstPointed = false;
			switch (objType)
			{
			case DISTANCE:
				Measure(distances[objPos]);
				break;
			case CIRCLE:
				Measure(circles[objPos]);
				break;

			default:
				break;
			}
			break;
		case ANGLE_MEASUREMENT:
			firstPointed = false;
			break;
		case LENGTH_MEASUREMENT_ERASER:
			firstPointed = false;
			switch (objType)
			{
			case DISTANCE:
				distances[objPos].DisableLength();
				break;
			case CIRCLE:
				circles[objPos].DisableLength();
				break;

			default:
				break;
			}
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
		Vec2 currentPos = currentPoint.GetPos();
		switch (editMode)
		{
		case DRAW_CIRCLE:
		{
			circles.emplace_back(firstPoint, GetDistance(firstPoint, currentPos));
			AddIntersections(circles.back());
		}
		break;
		case DRAW_DISTANCE:
		{
			distances.emplace_back(firstPoint, currentPos);
			AddIntersections(distances.back());
		}
		break;
		case DRAW_RAY:
		{
			rays.emplace_back(firstPoint, currentPos, camera);
			AddIntersections(rays.back());
		}
		break;
		case DRAW_STRAIGHT_LINE:
		{
			straightLines.emplace_back(firstPoint, currentPos, camera);
			AddIntersections(straightLines.back());
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

std::tuple<int, std::size_t> GeometryBoard::UpdateCurrentPoint()
{
	std::vector<long double> connectionDistances;
	std::vector<Vec2> connectionPoints;
	std::vector<int> connectionTypes;
	std::vector<std::tuple<int, std::size_t>> objPlaces;

	enum ConnectionOrder
	{
		POINT_CONNECTION,
		INTERSECTION_CONNECTION,
		LINE_END_POINT_CONNECTION,
		LINE_CONNECTION,
		SECTOR_END_POINT_CONNECTION,
		CIRCLE_CONNECTION,
	};

	Vec2 worldMousePos = GetMousePosition2(camera);

	auto ConnectionDistancesPush = [this, &worldMousePos, &connectionDistances, &connectionPoints, &connectionTypes, &objPlaces](Vec2 &point, int connectionType, int objType, std::size_t objPos)
	{
		connectionDistances.push_back(GetDistance(point, worldMousePos));
		connectionPoints.push_back(point);
		connectionTypes.push_back(connectionType);
		objPlaces.push_back(std::make_tuple(objType, objPos));
	};

	for (std::size_t i = 0; i < distances.size(); ++i)
	{
		ConnectionDistancesPush(distances[i].pointA, LINE_END_POINT_CONNECTION, DISTANCE, i);
		ConnectionDistancesPush(distances[i].pointB, LINE_END_POINT_CONNECTION, DISTANCE, i);

		Vec2 intersection = GetOrthogonalLinesIntersection(worldMousePos, distances[i]);
		if (distances[i].IsPointOnLine(intersection))
		{
			ConnectionDistancesPush(intersection, LINE_CONNECTION, DISTANCE, i);
		}
	}
	for (std::size_t i = 0; i < circles.size(); ++i)
	{
		Vec2 connectionPoint = GetCircleConnection(worldMousePos, circles[i]);
		// if (IsPointOnCircle(connectionPoint, circles[i]))
		{
			ConnectionDistancesPush(connectionPoint, CIRCLE_CONNECTION, CIRCLE, i);
		}
		/*
		for (auto sector : circles[i].sectors)
		{
			ConnectionDistancesPush(&sector.startAnglePoint, SECTOR_END_POINT_CONNECTION, CIRCLE, i);
			ConnectionDistancesPush(&sector.endAnglePoint, SECTOR_END_POINT_CONNECTION, CIRCLE, i);
		}
		*/
	}
	for (std::size_t i = 0; i < rays.size(); ++i)
	{
		ConnectionDistancesPush(rays[i].pointA, LINE_END_POINT_CONNECTION, RAY, i);

		Vec2 intersection = GetOrthogonalLinesIntersection(worldMousePos, rays[i]);
		if (rays[i].IsPointOnLine(intersection))
		{
			ConnectionDistancesPush(intersection, LINE_CONNECTION, RAY, i);
		}
	}
	for (std::size_t i = 0; i < straightLines.size(); ++i)
	{
		Vec2 intersection = GetOrthogonalLinesIntersection(worldMousePos, straightLines[i]);
		if (straightLines[i].IsPointOnLine(intersection))
		{
			ConnectionDistancesPush(intersection, LINE_CONNECTION, STRAIGHTLINE, i);
		}
	}
	for (std::size_t i = 0; i < points.size(); ++i)
	{
		ConnectionDistancesPush(points[i].GetPos(), POINT_CONNECTION, POINT, i);
	}
	for (auto &intersection : intersections.intersections)
	{
		ConnectionDistancesPush(intersection.second, INTERSECTION_CONNECTION, -1, 0);
	}

	while (!connectionDistances.empty())
	{
		auto minPos = std::min_element(connectionTypes.begin(), connectionTypes.end()) - connectionTypes.begin();
		if (connectionDistances[minPos] <= connectionDistance)
		{
			currentPoint.SetPos(connectionPoints[minPos]);
			return objPlaces[minPos];
		}
		connectionDistances.erase(connectionDistances.begin() + minPos);
		connectionPoints.erase(connectionPoints.begin() + minPos);
		connectionTypes.erase(connectionTypes.begin() + minPos);
		objPlaces.erase(objPlaces.begin() + minPos);
	}

	currentPoint.SetPos(worldMousePos);
	return std::make_tuple(-1, -1);
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
			if (IsKeyDown(KEY_L))
			{
				editMode = LENGTH_MEASUREMENT_ERASER;
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
		if (IsKeyDown(KEY_L))
		{
			editMode = LENGTH_MEASUREMENT;
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

		UpdateConnectionDistance();
	}

	float wheel = GetMouseWheelMove();
	bool zoom = wheel != 0;
	if (zoom)
	{
		// zoom to the mouse position
		// save this before setting camera.offset because it changes the camera which is needed for the target
		Vector2 mouseWorldPos = GetMousePosition2(camera).ToRaylibVec();
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
	for (auto &distance : distances)
	{
		GetLineCircleIntersections(intersections, distance, circle);
	}
	for (auto &straightLine : straightLines)
	{
		GetLineCircleIntersections(intersections, straightLine, circle);
	}
	for (auto &ray : rays)
	{
		GetLineCircleIntersections(intersections, ray, circle);
	}
	for (auto &circle_ : circles)
	{
		GetCircleCircleIntersections(intersections, circle_, circle);
	}
}

template <typename L>
void GeometryBoard::AddIntersections(L &line)
{
	for (auto &distance : distances)
	{
		GetLineLineIntersections(intersections, line, distance);
	}
	for (auto &straightLine : straightLines)
	{
		GetLineLineIntersections(intersections, line, straightLine);
	}
	for (auto &ray : rays)
	{
		GetLineLineIntersections(intersections, line, ray);
	}
	for (auto &circle : circles)
	{
		GetLineCircleIntersections(intersections, line, circle);
	}
}

void GeometryBoard::EraseObj(int objType, std::size_t objPos)
{
	if (objType < CIRCLE)
	{
		return;
	}

	switch (objType)
	{
	case CIRCLE:
		EraseGemObj(circles, objPos);
		break;
	case DISTANCE:
		EraseGemObj(distances, objPos);
		break;
	case RAY:
		EraseGemObj(rays, objPos);
		break;
	case STRAIGHTLINE:
		EraseGemObj(straightLines, objPos);
		break;
	case POINT:
		ErasePoint(objPos);
		break;
	default:
		break;
	}
}

template <typename T>
void GeometryBoard::EraseGemObj(std::vector<T> &objVec, std::size_t vecPos)
{
	for (auto &ID : objVec[vecPos].intersectionIDs)
	{
		intersections.intersections.erase(ID);
	}

	objVec.erase(objVec.begin() + vecPos);
}

void GeometryBoard::ErasePoint(std::size_t vecPos)
{
	points.erase(points.begin() + vecPos);
}
