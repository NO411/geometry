#include "raylib.h"
#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include <algorithm>
#include <tuple>

Camera2D camera = {0};
enum EditMod
{
	CIRCLE,
	DISTANCE,
	RAY,
	STRAIGHTLINE,
	POINT,
	ERASER,
	DISTANCEMEASUREMENT,
	ANGLEMEASUREMENT,
	DISTANCEMEASUREMENTERASER,
	ANGLEMEASUREMENTERASER
};

bool firstPointed = false;
std::string pointChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const float movementSpeed = 6;
float scaling = 0.025;

int editMode = DISTANCE;

void Init()
{
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	SetTraceLogLevel(LOG_NONE);
	InitWindow(900, 600, "geometry");
	SetTargetFPS(60);
	SetExitKey(0);
	SetWindowIcon(LoadImage("resources/icon.png"));
}

Vector2 GetMN(Vector2 p1, Vector2 p2)
{
	float m = (p1.y - p2.y) / (p1.x - p2.x);
	float n = p2.y - (m * p2.x);
	return {m, n};
}

class GeometryObj
{
public:
	int objectNumber;

	GeometryObj() {}
	~GeometryObj() {}

	void CheckIntersections(Vector2 vec1, Vector2 vec2, float r);

	void FindCircleCircleIntersections(Vector2 A, Vector2 B, float a, float b);
	void FindLineLineIntersections(Vector2 A1, Vector2 A2, Vector2 B1, Vector2 B2);
	void FindCircleLineIntersections(Vector2 A, float a, Vector2 B1, Vector2 B2);
};

class Point
{
public:
	Vector2 point;
	std::string letter;
	std::string letterNumber;

	Point() {}
	Point(Vector2 point) : point(point)
	{
		SetPointLetter();
	}
	~Point() {}

	void SetPointLetter();

	void Move(int direction, bool y);
};

class Circle : public GeometryObj
{
public:
	Vector2 middle;
	float radius;

	Circle(Vector2 middle, float radius) : middle(middle), radius(radius)
	{
		objectNumber = 1;
		UpdateIntersections();
	}
	~Circle() {}

	void UpdateIntersections();

	void Move(int direction, bool y);
};

class Line : public GeometryObj
{
public:
	Vector2 pointA;
	Vector2 pointB;

	Vector2 firstConnectionPoint;
	Vector2 secondConnectionPoint;

	bool showLength = false;
	float length;
	Point middle;

	Line(Vector2 pointA, Vector2 pointB, int n) : pointA(pointA), pointB(pointB)
	{
		objectNumber = n;

		UpdateConnectionPoints();
		UpdateIntersections();

		UpdateLength();
	}
	~Line() {}

	void UpdateFirstConnectionPoint();
	void UpdateSecondConnectionPoint();

	void UpdateConnectionPoints();
	void UpdateIntersections();

	void Move(int direction, bool y);
	void UpdateLength();
};

void Line::UpdateIntersections()
{
	Vector2 v1, v2;
	switch (objectNumber)
	{
	case 2:
		v1 = pointA, v2 = pointB;
		break;
	case 3:
		v1 = pointA, v2 = secondConnectionPoint;
		break;
	case 4:
		v1 = firstConnectionPoint, v2 = secondConnectionPoint;
		break;
	default:
		break;
	}
	CheckIntersections(v1, v2, 0);
}

void Circle::UpdateIntersections()
{
	CheckIntersections(middle, {0, 0}, radius);
}

Vector2 CalculateConnectionPoint(Vector2 &p1, Vector2 &p2, float m, float n)
{
	Vector2 connectionPoint;

	// correct connectionPoint offset
	Vector2 offset = {
		camera.offset.x - GetScreenWidth() / 2,
		camera.offset.y - GetScreenHeight() / 2};

	if (p1.x > p2.x)
	{
		connectionPoint.x = -offset.x;
	}
	else
	{
		connectionPoint.x = GetScreenWidth() - offset.x;
	}

	connectionPoint.y = m * connectionPoint.x + n;

	if (p1.x == p2.x)
	{
		connectionPoint.x = p1.x;
		if (p1.y > p2.y)
		{
			connectionPoint.y = -offset.y;
		}
		else
		{
			connectionPoint.y = GetScreenHeight() - offset.y;
		}
	}

	return connectionPoint;
}

void Line::UpdateFirstConnectionPoint()
{
	Vector2 mn = GetMN(pointA, pointB);
	firstConnectionPoint = CalculateConnectionPoint(pointB, pointA, mn.x, mn.y);
}

void Line::UpdateSecondConnectionPoint()
{
	Vector2 mn = GetMN(pointA, pointB);
	secondConnectionPoint = CalculateConnectionPoint(pointA, pointB, mn.x, mn.y);
}

void Line::UpdateConnectionPoints()
{
	UpdateFirstConnectionPoint();
	UpdateSecondConnectionPoint();
}

std::vector<Circle> circles;	 // 1
std::vector<Line> distances;	 // 2
std::vector<Line> rays;			 // 3
std::vector<Line> straightLines; // 4
std::vector<Point> points;		 // 5
								 // 6 erasers
								 // 7 distance measurement
								 // 8 angle measurement
std::vector<Point> intersections;

Point firstPoint;
Line currentLine = {{0, 0}, {0, 0}, 2};
Circle currentCircle = {{0, 0}, 0};
Vector2 currentPoint = GetMousePosition();

float GetMouseX2()
{
	return (GetMouseX() - camera.offset.x + GetScreenWidth() / 2);
}

float GetMouseY2()
{
	return (GetMouseY() - camera.offset.y + GetScreenHeight() / 2);
}

Vector2 GetMousePosition2()
{
	return {GetMouseX2(), GetMouseY2()};
}

bool PointLetterExists(std::string letter)
{
	for (auto &point : points)
	{
		if (point.letter == letter)
		{
			return true;
		}
	}
	return false;
}

void Point::SetPointLetter()
{
	for (std::size_t i = 0; i < pointChars.size(); i++)
	{
		std::string _letter = pointChars.substr(i, 1);
		if (!PointLetterExists(_letter))
		{
			letter = _letter;
			letterNumber = "";
			return;
		}
	}

	std::vector<int> letterQuantity(pointChars.size());
	std::vector<std::vector<int>> letterNumbers(pointChars.size());

	for (auto &point : points)
	{
		auto found = pointChars.find(point.letter);
		if (found != std::string::npos)
		{
			letterQuantity.at(found)++;

			int n = 0;
			if (point.letterNumber != "")
			{
				n = std::stoi(point.letterNumber);
			}
			letterNumbers.at(found).push_back(n);
		}
	}

	auto minPos = std::min_element(letterQuantity.begin(), letterQuantity.end()) - letterQuantity.begin();
	letter = pointChars.substr(minPos, 1);

	int i = 0;
	while (std::count(letterNumbers.at(minPos).begin(), letterNumbers.at(minPos).end(), i))
	{
		i++;
	}

	if (i == 0)
	{
		letterNumber = "";
		return;
	}

	letterNumber = std::to_string(i);
}

float GetDistance(Vector2 vec1, Vector2 vec2)
{
	return sqrt(pow(vec2.x - vec1.x, 2) + pow(vec2.y - vec1.y, 2));
}

void Line::UpdateLength()
{
	length = GetDistance(pointA, pointB) * scaling;

	middle.point = {(pointA.x + pointB.x) / 2, (pointA.y + pointB.y) / 2};
}

void GeometryObj::FindCircleCircleIntersections(Vector2 A, Vector2 B, float a, float b)
{
	float c = GetDistance(A, B);
	if (c == 0)
	{
		return;
	}

	float powa = pow(a, 2);
	float x = (powa + pow(c, 2) - pow(b, 2)) / (2 * c);
	float y = powa - pow(x, 2);
	if (y < 0)
	{
		return;
	}
	else if (y > 0)
	{
		y = sqrt(y);
	}
	float ex0 = (B.x - A.x) / c, ex1 = (B.y - A.y) / c;
	float ey0 = -ex1, ey1 = ex0;
	float Q1x = A.x + x * ex0;
	float Q1y = A.y + x * ex1;
	if (y == 0)
	{
		intersections.push_back({{Q1x, Q1y}});
		return;
	}
	float Q2x = Q1x - y * ey0;
	float Q2y = Q1y - y * ey1;
	Q1x += y * ey0;
	Q1y += y * ey1;
	intersections.push_back({{Q1x, Q1y}});
	intersections.push_back({{Q2x, Q2y}});
}

void GeometryObj::FindLineLineIntersections(Vector2 A1, Vector2 A2, Vector2 B1, Vector2 B2)
{
	Vector2 mnA = GetMN(A1, A2);
	Vector2 mnB = GetMN(B1, B2);
	float x, y;

	// no intersection if the lines are parallel
	if (mnA.x == mnB.x)
	{
		return;
	}

	// mnA.x * x + mnA.y = mnB.x * x + mnB.y
	x = (mnA.y - mnB.y) / (mnB.x - mnA.x);
	y = mnA.x * x + mnA.y;

	if (A1.x == A2.x)
	{
		x = A1.x;
		y = mnB.x * A1.x + mnB.y;
	}
	else if (B1.x == B2.x)
	{
		x = B1.x;
		y = mnA.x * B1.x + mnA.y;
	}

	Vector2 intersection = {x, y};

	// check whether the intersection is included of both lines

	if (!CheckCollisionLines(A1, A2, B1, B2, &intersection))
	{
		return;
	}

	intersections.push_back(intersection);
}

void GeometryObj::FindCircleLineIntersections(Vector2 A, float r, Vector2 B1, Vector2 B2)
{
	B1 = {B1.x - A.x, B1.y - A.y};
	B2 = {B2.x - A.x, B2.y - A.y};

	float v1 = B2.x - B1.x, v2 = B2.y - B1.y;
	float D = pow(r, 2) * (pow(v1, 2) + pow(v2, 2)) - pow(B1.x * v2 - B1.y * v1, 2);
	float _t1 = -B1.x * v1 - B1.y * v2;
	float _t2 = pow(v1, 2) + pow(v2, 2);
	float t1 = (_t1 + sqrt(D)) / _t2, t2 = (_t1 - sqrt(D)) / _t2;

	Vector2 mn = GetMN(B1, B2);

	if (!(B1.x == B2.x))
	{
		if (t1 >= 0 && t1 <= 1)
		{
			float x = v1 * t1 + B1.x;
			float y = mn.x * x + mn.y;
			intersections.push_back({{x + A.x, y + A.y}});
		}

		if (t2 >= 0 && t2 <= 1)
		{
			float x = v1 * t2 + B1.x;
			float y = mn.x * x + mn.y;
			intersections.push_back({{x + A.x, y + A.y}});
		}
		return;
	}

	B1 = {B1.x + A.x, B1.y + A.y};
	B2 = {B2.x + A.x, B2.y + A.y};

	float undersqrt = 2 * A.x * B1.x + pow(r, 2) - pow(B1.x, 2) - pow(A.x, 2);
	float _y = sqrt(undersqrt);
	float y1 = A.y + _y, y2 = A.y - _y;

	float b1 = B1.y, b2 = B2.y, _var;
	if (b1 > b2)
	{
		_var = b1;
		b1 = b2;
		b2 = _var;
	}

	if (y1 >= b1 && y1 <= b2)
	{
		intersections.push_back({{B1.x, y1}});
	}
	if (y2 >= b1 && y2 <= b2)
	{
		intersections.push_back({{B1.x, y2}});
	}
}

void GeometryObj::CheckIntersections(Vector2 vec1, Vector2 vec2, float r)
{
	switch (objectNumber)
	{
	case 1:
	{
		for (auto &circle : circles)
		{
			FindCircleCircleIntersections(vec1, circle.middle, r, circle.radius);
		}
		for (auto &distance : distances)
		{
			FindCircleLineIntersections(vec1, r, distance.pointA, distance.pointB);
		}
		for (auto &ray : rays)
		{
			FindCircleLineIntersections(vec1, r, ray.pointA, ray.secondConnectionPoint);
		}
		for (auto &straightLine : straightLines)
		{
			FindCircleLineIntersections(vec1, r, straightLine.firstConnectionPoint, straightLine.secondConnectionPoint);
		}
	}
	break;
	case 5:
		break;
	default:
	{
		for (auto &circle : circles)
		{
			FindCircleLineIntersections(circle.middle, circle.radius, vec1, vec2);
		}
		for (auto &distance : distances)
		{
			FindLineLineIntersections(vec1, vec2, distance.pointA, distance.pointB);
		}
		for (auto &ray : rays)
		{
			FindLineLineIntersections(vec1, vec2, ray.pointA, ray.secondConnectionPoint);
		}
		for (auto &straightLine : straightLines)
		{
			FindLineLineIntersections(vec1, vec2, straightLine.firstConnectionPoint, straightLine.secondConnectionPoint);
		}
	}
	break;
	}
}

void DrawPointObj(Vector2 point)
{
	DrawCircleSector(point, 3, 0, 360, 30, BLUE);
	DrawRing(point, 3, 4, 0, 360, 30, DARKBLUE);
}

void DrawCircleObj(Circle circle)
{
	DrawCircleSectorLines(circle.middle, circle.radius, 0, 360, 2 * circle.radius, LIGHTGRAY);
}

void DrawDistanceObj(Line line)
{
	DrawLineEx(line.pointA, line.pointB, 2, LIGHTGRAY);
}

void DrawDistanceLengths(Line line, Font *font)
{
	if (!line.showLength)
	{
		return;
	}
	int fontSize = 12;
	std::string len = std::to_string(line.length);
	Vector2 measure = MeasureTextEx(*font, len.c_str(), fontSize, 0);
	DrawTextEx(*font, len.c_str(), {line.middle.point.x, line.middle.point.y}, fontSize, 0, BLACK);
	DrawRectangleRec({line.middle.point.x - 2, line.middle.point.y - 2, measure.x + 4, measure.y + 4}, {200, 200, 200, 150});
}

bool SameVector2(Vector2 v1, Vector2 v2)
{
	return (v1.x == v2.x && v1.y == v2.y);
}

void DrawRayObj(Line line)
{
	if (SameVector2(line.pointA, line.secondConnectionPoint))
	{
		return;
	}

	DrawLineEx(line.pointA, line.secondConnectionPoint, 2, LIGHTGRAY);
}

void DrawStraightLineObj(Line line)
{
	DrawLineEx(line.firstConnectionPoint, line.secondConnectionPoint, 2, LIGHTGRAY);
}

void SetDrawObj()
{
	if (!IsKeyDown(KEY_LEFT_CONTROL))
	{
		return;
	}

	switch (GetKeyPressed())
	{
	case KEY_C:
		editMode = CIRCLE;
		break;
	case KEY_D:
		editMode = DISTANCE;
		break;
	case KEY_R:
		editMode = RAY;
		break;
	case KEY_S:
		editMode = STRAIGHTLINE;
		break;
	case KEY_P:
		editMode = POINT;
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

bool IsPointOnLine(int pointX, Vector2 pointA, Vector2 pointB)
{
	float x1 = pointA.x, x2 = pointB.x, _var;
	if (x1 > x2)
	{
		_var = x1;
		x1 = x2;
		x2 = _var;
	}
	if (pointX >= x1 && pointX <= x2)
	{
		return true;
	}
	return false;
}

Vector2 GetOrthogonalLinesIntersection(Vector2 point, Vector2 pointA, Vector2 pointB)
{
	Vector2 mn = GetMN(pointA, pointB);
	float m2 = -1 / mn.x;
	float n2 = point.y - m2 * point.x;
	float x = (n2 - mn.y) / (mn.x - m2);
	float y = mn.x * x + mn.y;

	Vector2 connectionPoint = {x, y};

	if (pointA.x == pointB.x)
	{
		connectionPoint = {pointA.x, point.y};
	}

	if (pointA.y == pointB.y)
	{
		connectionPoint = {point.x, pointA.y};
	}

	return connectionPoint;
}

Vector2 GetCircleConnection(Circle circle)
{
	Vector2 mouse = GetMousePosition2();
	float x = circle.middle.x + (mouse.x - circle.middle.x) / (GetDistance(mouse, circle.middle) / circle.radius);
	Vector2 mn = GetMN(circle.middle, mouse);
	float y = mn.x * x + mn.y;

	return {x, y};
}

std::tuple<int, std::size_t> UpdateCurrentPoint()
{
	std::vector<float> connectionDistances;
	std::vector<Vector2> connectionPoints;
	std::vector<int> connectionTypes;
	std::vector<std::tuple<int, std::size_t>> objPlaces;
	/* connection order:
	1 points
	2 intersections
	3 line end point
	4 lines
	5 circles
	*/

	auto ConnectionDistancesPush = [&connectionDistances, &connectionPoints, &connectionTypes, &objPlaces](Vector2 point, int connectionType, int objType, std::size_t objPos)
	{
		connectionDistances.push_back(GetDistance(point, GetMousePosition2()));
		connectionPoints.push_back(point);
		connectionTypes.push_back(connectionType);
		objPlaces.push_back(std::make_tuple(objType, objPos));
	};

	auto LineConnection = [&connectionTypes, &ConnectionDistancesPush](Line line, int lineType, int objType, std::size_t objPos)
	{
		Vector2 pointA = line.pointA, pointB = line.pointB;
		if (lineType > 2)
		{
			pointB = line.secondConnectionPoint;
		}

		if (lineType == 4)
		{
			pointA = line.firstConnectionPoint;
		}

		Vector2 intersection = GetOrthogonalLinesIntersection(GetMousePosition2(), pointA, pointB);
		if (IsPointOnLine(intersection.x, pointA, pointB))
		{
			ConnectionDistancesPush(intersection, 4, objType, objPos);
		}
	};

	for (std::size_t i = 0; i < distances.size(); ++i)
	{
		ConnectionDistancesPush(distances[i].pointA, 3, 2, i);
		ConnectionDistancesPush(distances[i].pointB, 3, 2, i);
		LineConnection(distances[i], 2, 2, i);
	}
	for (std::size_t i = 0; i < circles.size(); ++i)
	{
		ConnectionDistancesPush(GetCircleConnection(circles[i]), 5, 1, i);
	}
	for (std::size_t i = 0; i < rays.size(); ++i)
	{
		ConnectionDistancesPush(rays[i].pointA, 3, 3, i);

		LineConnection(rays[i], 3, 3, i);
	}
	for (std::size_t i = 0; i < straightLines.size(); ++i)
	{
		LineConnection(straightLines[i], 4, 4, i);
	}
	for (std::size_t i = 0; i < points.size(); ++i)
	{
		ConnectionDistancesPush(points[i].point, 1, 5, i);
	}
	for (auto &intersection : intersections)
	{
		ConnectionDistancesPush(intersection.point, 2, 0, 0);
	}

	while (!connectionDistances.empty())
	{
		auto minPos = std::min_element(connectionTypes.begin(), connectionTypes.end()) - connectionTypes.begin();
		if (connectionDistances.at(minPos) <= 8)
		{
			currentPoint = connectionPoints.at(minPos);
			return objPlaces.at(minPos);
		}
		connectionDistances.erase(connectionDistances.begin() + minPos);
		connectionPoints.erase(connectionPoints.begin() + minPos);
		connectionTypes.erase(connectionTypes.begin() + minPos);
		objPlaces.erase(objPlaces.begin() + minPos);
	}

	currentPoint = GetMousePosition2();
	return std::make_tuple(0, 0);
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

void EraseObj(std::tuple<int, std::size_t> objTuple)
{
	int objType = std::get<0>(objTuple);
	std::size_t objPos = std::get<1>(objTuple);

	if (objType < 1)
	{
		return;
	}
	switch (objType)
	{
	case 1:
		circles.erase(circles.begin() + objPos);
		break;
	case 2:
		distances.erase(distances.begin() + objPos);
		break;
	case 3:
		rays.erase(rays.begin() + objPos);
		break;
	case 4:
		straightLines.erase(straightLines.begin() + objPos);
		break;
	case 5:
		points.erase(points.begin() + objPos);
		break;
	default:
		break;
	}

	UpdateAllIntersections();
}

void DrawObj()
{
	auto objTuple = UpdateCurrentPoint();
	int objType = std::get<0>(objTuple);
	std::size_t objPos = std::get<1>(objTuple);
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
		case POINT:
			firstPointed = false;

			for (auto &point : points)
			{
				if (SameVector2(point.point, currentPoint))
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
			if (objType == 2)
			{
				distances.at(objPos).showLength = true;
			}
			break;
		case ANGLEMEASUREMENT:
			firstPointed = false;
			break;
		case DISTANCEMEASUREMENTERASER:
			firstPointed = false;
			if (objType == 2)
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
		case CIRCLE:
			circles.push_back(Circle{firstPoint.point, GetDistance(firstPoint.point, currentPoint)});
			break;
		case DISTANCE:
			distances.push_back(Line{firstPoint.point, currentPoint, 2});
			break;
		case RAY:
			rays.push_back(Line{firstPoint.point, currentPoint, 3});
			break;
		case STRAIGHTLINE:
			straightLines.push_back(Line{firstPoint.point, currentPoint, 4});
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
	DrawObj();
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
	case CIRCLE:
	{
		currentCircle.middle = firstPoint.point;
		currentCircle.radius = GetDistance(firstPoint.point, currentPoint);
		DrawCircleObj(currentCircle);
	}
	break;
	case DISTANCE:
		DrawDistanceObj(currentLine);
		break;
	case RAY:
	{
		currentLine.UpdateSecondConnectionPoint();
		DrawRayObj(currentLine);
	}
	break;
	case STRAIGHTLINE:
	{
		currentLine.UpdateConnectionPoints();
		DrawStraightLineObj(currentLine);
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
		DrawDistanceObj(distance);
	}
	for (auto &circle : circles)
	{
		DrawCircleObj(circle);
	}
	for (auto &ray : rays)
	{
		DrawRayObj(ray);
	}
	for (auto &straightLine : straightLines)
	{
		DrawStraightLineObj(straightLine);
	}

	for (auto &distance : distances)
	{
		DrawDistanceLengths(distance, font);
	}

	for (auto &point : points)
	{
		DrawPointObj(point.point);
		DrawTextEx(*font, point.letter.c_str(), {point.point.x + 4, point.point.y + 4}, 12, 0, BLACK);
		DrawTextEx(*font, point.letterNumber.c_str(), {point.point.x + 12, point.point.y + 8}, 8, 0, GRAY);
	}

	DrawDrawingObj(font);
	DrawPointObj(currentPoint);
}

int main()
{
	Init();

	Font font = LoadFont("resources/anonymous_pro_bold.ttf");
	SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);

	camera.target = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
	camera.offset = camera.target;
	camera.zoom = 1;

	while (!WindowShouldClose())
	{
		Update();

		BeginDrawing();
		BeginMode2D(camera);

		Draw(&font);

		EndMode2D();
		EndDrawing();
	}
	UnloadFont(font);
	CloseWindow();
	return 0;
}
