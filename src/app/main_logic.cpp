#include "draw.h"
#include "geometry_objects.h"
#include "main_logic.h"
#include "../math/math_misc.h"
#include "../math/intersections.h"
#include "raylib.h"

#include <algorithm>
#include <tuple>
#include <vector>

bool firstPointed = false;
const float movementSpeed = 6;
const int connection_distance = 8;
int editMode = DRAWDISTANCE;

std::vector<Circle> circles;
std::vector<Line> distances;
std::vector<Line> rays;
std::vector<Line> straightLines;
std::vector<Point> points;

Point firstPoint;
Line currentLine = {{0, 0}, {0, 0}, DISTANCE};
Circle currentCircle = {{0, 0}, 0};
Vector2 currentPoint = GetMousePosition();

void SetDrawObj()
{
	if (!IsKeyDown(KEY_LEFT_CONTROL))
	{
		return;
	}

	switch (GetKeyPressed())
	{
	case KEY_C:
		editMode = DRAWCIRCLE;
		break;
	case KEY_D:
		editMode = DRAWDISTANCE;
		break;
	case KEY_R:
		editMode = DRAWRAY;
		break;
	case KEY_S:
		editMode = DRAWSTRAIGHTLINE;
		break;
	case KEY_P:
		editMode = DRAWPOINT;
		break;
	case KEY_E:
		editMode = ERASER;
		if (IsKeyDown(KEY_M))
		{
			if (IsKeyDown(KEY_D))
			{
				editMode = DISTANCEMEASUREMENTERASER;
			}
			else if (IsKeyDown(KEY_A))
			{
				editMode = ANGLEMEASUREMENTERASER;
			}
		}
		break;
	case KEY_M:
		if (IsKeyDown(KEY_D))
		{
			editMode = DISTANCEMEASUREMENT;
		}
		else if (IsKeyDown(KEY_A))
		{
			editMode = ANGLEMEASUREMENT;
		}
		break;
	default:
		break;
	}
}

std::tuple<int, std::size_t> *UpdateCurrentPoint()
{
	std::vector<float> connectionDistances;
	std::vector<Vector2> connectionPoints;
	std::vector<int> connectionTypes;
	std::vector<std::tuple<int, std::size_t>> objPlaces;

	enum ConnectionOrder
	{
		POINTCONNECTION,
		INTERSECTIONCONNECTION,
		LINEANDPOINTCONNECTION,
		LINECONNECTION,
		CIRCLECONNECTION
	};

	auto ConnectionDistancesPush = [&connectionDistances, &connectionPoints, &connectionTypes, &objPlaces](Vector2 point, int connectionType, int objType, std::size_t objPos)
	{
		connectionDistances.push_back(GetDistance(&point, GetMousePosition2()));
		connectionPoints.push_back(point);
		connectionTypes.push_back(connectionType);
		objPlaces.push_back(std::make_tuple(objType, objPos));
	};

	auto LineConnection = [&connectionTypes, &ConnectionDistancesPush](Line line, int objType, std::size_t objPos)
	{
		Vector2 pointA = line.pointA, pointB = line.pointB;
		if (objType > DISTANCE)
		{
			pointB = line.secondConnectionPoint;
		}

		if (objType == STRAIGHTLINE)
		{
			pointA = line.firstConnectionPoint;
		}

		Vector2 *intersection = GetOrthogonalLinesIntersection(GetMousePosition2(), &pointA, &pointB);
		if (IsPointOnLine(intersection, &pointA, &pointB))
		{
			ConnectionDistancesPush(*intersection, LINECONNECTION, objType, objPos);
		}
	};

	for (std::size_t i = 0; i < distances.size(); ++i)
	{
		ConnectionDistancesPush(distances[i].pointA, LINEANDPOINTCONNECTION, DISTANCE, i);
		ConnectionDistancesPush(distances[i].pointB, LINEANDPOINTCONNECTION, DISTANCE, i);
		LineConnection(distances[i], DISTANCE, i);
	}
	for (std::size_t i = 0; i < circles.size(); ++i)
	{
		ConnectionDistancesPush(*GetCircleConnection(&circles[i]), CIRCLECONNECTION, CIRCLE, i);
	}
	for (std::size_t i = 0; i < rays.size(); ++i)
	{
		ConnectionDistancesPush(rays[i].pointA, LINEANDPOINTCONNECTION, RAY, i);

		LineConnection(rays[i], RAY, i);
	}
	for (std::size_t i = 0; i < straightLines.size(); ++i)
	{
		LineConnection(straightLines[i], STRAIGHTLINE, i);
	}
	for (std::size_t i = 0; i < points.size(); ++i)
	{
		ConnectionDistancesPush(points[i].point, POINTCONNECTION, POINT, i);
	}
	for (auto &intersection : intersections)
	{
		ConnectionDistancesPush(intersection.point, INTERSECTIONCONNECTION, -1, 0);
	}

	while (!connectionDistances.empty())
	{
		auto minPos = std::min_element(connectionTypes.begin(), connectionTypes.end()) - connectionTypes.begin();
		if (connectionDistances.at(minPos) <= connection_distance)
		{
			currentPoint = connectionPoints.at(minPos);
			return new std::tuple<int, std::size_t>{objPlaces.at(minPos)};
		}
		connectionDistances.erase(connectionDistances.begin() + minPos);
		connectionPoints.erase(connectionPoints.begin() + minPos);
		connectionTypes.erase(connectionTypes.begin() + minPos);
		objPlaces.erase(objPlaces.begin() + minPos);
	}

	currentPoint = *GetMousePosition2();
	return new std::tuple<int, std::size_t>{std::make_tuple(-1, -1)};
}

void UpdateAllIntersections()
{
	intersections.clear();
	for (auto &straightLine : straightLines)
	{
		straightLine.UpdateConnectionPoints();
		straightLine.UpdateIntersections();
	}
	for (auto &ray : rays)
	{
		ray.UpdateSecondConnectionPoint();
		ray.UpdateIntersections();
	}
	for (auto &circle : circles)
	{
		circle.UpdateIntersections();
	}
	for (auto &distance : distances)
	{
		distance.UpdateIntersections();
	}
}

void EraseObj(std::tuple<int, std::size_t> *objTuple)
{
	int objType = std::get<0>(*objTuple);
	std::size_t objPos = std::get<1>(*objTuple);

	if (objType < CIRCLE)
	{
		return;
	}
	switch (objType)
	{
	case CIRCLE:
		circles.erase(circles.begin() + objPos);
		break;
	case DISTANCE:
		distances.erase(distances.begin() + objPos);
		break;
	case RAY:
		rays.erase(rays.begin() + objPos);
		break;
	case STRAIGHTLINE:
		straightLines.erase(straightLines.begin() + objPos);
		break;
	case POINT:
		points.erase(points.begin() + objPos);
		break;
	default:
		break;
	}

	UpdateAllIntersections();
}

void Edit()
{
	auto objTuple = UpdateCurrentPoint();
	int objType = std::get<0>(*objTuple);
	std::size_t objPos = std::get<1>(*objTuple);
	if (!IsMouseButtonPressed(0))
	{
		return;
	}

	if (!firstPointed)
	{
		firstPointed = true;
		firstPoint = {currentPoint};
		switch (editMode)
		{
		case DRAWPOINT:
			firstPointed = false;

			for (auto &point : points)
			{
				if (GetDistance(&point.point, &currentPoint) < connection_distance)
				{
					return;
				}
			}
			points.push_back({currentPoint});
			break;
		case ERASER:
			firstPointed = false;
			EraseObj(objTuple);
			break;
		case DISTANCEMEASUREMENT:
			firstPointed = false;
			if (objType == DISTANCE)
			{
				distances.at(objPos).showLength = true;
			}
			break;
		case ANGLEMEASUREMENT:
			firstPointed = false;
			break;
		case DISTANCEMEASUREMENTERASER:
			firstPointed = false;
			if (objType == DISTANCE)
			{
				distances.at(objPos).showLength = false;
			}
			break;
		case ANGLEMEASUREMENTERASER:
			firstPointed = false;
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
		case DRAWCIRCLE:
			circles.push_back(Circle{firstPoint.point, GetDistance(&firstPoint.point, &currentPoint)});
			break;
		case DRAWDISTANCE:
			distances.push_back(Line{firstPoint.point, currentPoint, DISTANCE});
			break;
		case DRAWRAY:
			rays.push_back(Line{firstPoint.point, currentPoint, RAY});
			break;
		case DRAWSTRAIGHTLINE:
			straightLines.push_back(Line{firstPoint.point, currentPoint, STRAIGHTLINE});
			break;
		default:
			break;
		}
	}
}

void InterruptDrawing()
{
	if (IsKeyPressed(KEY_ESCAPE))
	{
		firstPointed = false;
	}
}

void Move()
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

	for (auto &straightLine : straightLines)
	{
		straightLine.UpdateConnectionPoints();
	}

	for (auto &ray : rays)
	{
		ray.UpdateSecondConnectionPoint();
	}

	UpdateAllIntersections();
}

void InputHandler()
{
	Edit();
	SetDrawObj();
	InterruptDrawing();
	Move();
}

void CheckResized()
{
	if (!IsWindowResized())
	{
		return;
	}

	UpdateAllIntersections();
	camera.target = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
	camera.offset = camera.target;
}

void Update()
{
	InputHandler();
	CheckResized();
}

void DrawDrawingObj(Font *font)
{
	if (!firstPointed)
	{
		return;
	}

	currentLine.pointA = firstPoint.point;
	currentLine.pointB = currentPoint;

	switch (editMode)
	{
	case DRAWCIRCLE:
	{
		currentCircle.middle = firstPoint.point;
		currentCircle.radius = GetDistance(&firstPoint.point, &currentPoint);
		DrawCircleObj(&currentCircle);
	}
	break;
	case DRAWDISTANCE:
		DrawDistanceObj(&currentLine);
		break;
	case DRAWRAY:
	{
		currentLine.UpdateSecondConnectionPoint();
		DrawRayObj(&currentLine);
	}
	break;
	case DRAWSTRAIGHTLINE:
	{
		currentLine.UpdateConnectionPoints();
		DrawStraightLineObj(&currentLine);
	}
	break;
	default:
		break;
	}
}

void Draw(Font *font)
{
	ClearBackground(WHITE);

	for (auto &distance : distances)
	{
		DrawDistanceObj(&distance);
	}
	for (auto &circle : circles)
	{
		DrawCircleObj(&circle);
	}
	for (auto &ray : rays)
	{
		DrawRayObj(&ray);
	}
	for (auto &straightLine : straightLines)
	{
		DrawStraightLineObj(&straightLine);
	}

	for (auto &distance : distances)
	{
		DrawDistanceLengths(&distance, font);
	}

	for (auto &point : points)
	{
		DrawPointObj(&point.point);
		DrawTextEx(*font, point.letter.c_str(), {point.point.x + 4, point.point.y + 4}, 12, 0, BLACK);
		DrawTextEx(*font, point.letterNumber.c_str(), {point.point.x + 12, point.point.y + 8}, 8, 0, GRAY);
	}

	DrawDrawingObj(font);
	DrawPointObj(&currentPoint);
}
