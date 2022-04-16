#include "raylib.h"
#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include <algorithm>

Vector2 firstPoint;
Vector2 secondPoint;
bool firstPointed = false;

int drawObject = 2;

void Init()
{
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	SetTraceLogLevel(LOG_NONE);
	InitWindow(900, 600, "geometry");
	SetTargetFPS(60);
	SetExitKey(0);
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

	GeometryObj()
	{
		// std::cout << "obj created" << std::endl;
	}
	~GeometryObj() {}

	void CheckIntersections(Vector2 vec1, Vector2 vec2, float r);

	void FindCircleCircleIntersections(Vector2 A, Vector2 B, float a, float b);
	void FindLineLineIntersections(Vector2 A1, Vector2 A2, Vector2 B1, Vector2 B2);
	void FindCircleLineIntersections(Vector2 A, float a, Vector2 B1, Vector2 B2);
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
};

class Line : public GeometryObj
{
public:
	Vector2 pointA;
	Vector2 pointB;

	Vector2 firstConnectionPoint;
	Vector2 secondConnectionPoint;

	Line(Vector2 pointA, Vector2 pointB, int n) : pointA(pointA), pointB(pointB)
	{
		objectNumber = n;

		UpdateConnectionPoints();
		UpdateIntersections();
	}
	~Line() {}

	void UpdateFirstConnectionPoint();
	void UpdateSecondConnectionPoint();

	void UpdateConnectionPoints();
	void UpdateIntersections();
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
	Vector2 connectionPoint = {};

	float y = m * GetScreenWidth() + n;
	connectionPoint.x = GetScreenWidth();
	if (p1.x > p2.x)
	{
		y = n;
		connectionPoint.x = 0;
	}

	if (y > GetScreenHeight())
	{
		connectionPoint.y = GetScreenHeight();
		connectionPoint.x = (connectionPoint.y - n) / m;
	}
	else if (y < 0)
	{
		connectionPoint.y = 0;
		connectionPoint.x = (connectionPoint.y - n) / m;
	}
	else
	{
		connectionPoint.y = y;
	}

	// dividing by 0 is not possible ...
	// needed to get vertical lines
	if (p1.x == p2.x)
	{
		connectionPoint.x = p1.x;
		if (p1.y > p2.y)
		{
			connectionPoint.y = 0;
		}
		else
		{
			connectionPoint.y = GetScreenHeight();
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
std::vector<Vector2> points;	 // 5
std::vector<Vector2> intersections;

Line currentLine = {{0, 0}, {0, 0}, 2};
Circle currentCircle = {{0, 0}, 0};
Vector2 currentPoint = GetMousePosition();

float GetDistance(Vector2 vec1, Vector2 vec2)
{
	return sqrt(pow(vec2.x - vec1.x, 2) + pow(vec2.y - vec1.y, 2));
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
		intersections.push_back({Q1x, Q1y});
		return;
	}
	float Q2x = Q1x - y * ey0;
	float Q2y = Q1y - y * ey1;
	Q1x += y * ey0;
	Q1y += y * ey1;
	intersections.push_back({Q1x, Q1y});
	intersections.push_back({Q2x, Q2y});
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

	float switch_var;
	if (A1.x > A2.x)
	{
		switch_var = A1.x;
		A1.x = A2.x;
		A2.x = switch_var;
	}
	if (A1.y > A2.y)
	{
		switch_var = A1.y;
		A1.y = A2.y;
		A2.y = switch_var;
	}
	if (B1.x > B2.x)
	{
		switch_var = B1.x;
		B1.x = B2.x;
		B2.x = switch_var;
	}
	if (B1.y > B2.y)
	{
		switch_var = B1.y;
		B1.y = B2.y;
		B2.y = switch_var;
	}

	if (!CheckCollisionPointRec(intersection, GetCollisionRec({A1.x, A1.y, A2.x - A1.x, A2.y - A1.y}, {B1.x, B1.y, B2.x - B1.x, B2.y - B1.y})))
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

	if (t1 >= 0 && t1 <= 1)
	{
		float x = v1 * t1 + B1.x;
		float y = mn.x * x + mn.y;
		intersections.push_back({x + A.x, y + A.y});
	}

	if (t2 >= 0 && t2 <= 1)
	{
		float x = v1 * t2 + B1.x;
		float y = mn.x * x + mn.y;
		intersections.push_back({x + A.x, y + A.y});
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
	DrawCircleSectorLines(circle.middle, circle.radius, 0, 360, 2 * circle.radius, BLACK);
}

void DrawDistanceObj(Line line)
{
	DrawLineEx(line.pointA, line.pointB, 1, BLACK);
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

	DrawLineEx(line.pointA, line.secondConnectionPoint, 1, BLACK);
}

void DrawStraightLineObj(Line line)
{
	DrawLineEx(line.firstConnectionPoint, line.secondConnectionPoint, 1, BLACK);
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
		drawObject = 1;
		break;
	case KEY_D:
		drawObject = 2;
		break;
	case KEY_R:
		drawObject = 3;
		break;
	case KEY_S:
		drawObject = 4;
		break;
	case KEY_P:
		drawObject = 5;
		break;

	default:
		break;
	}
}

void UpdateCurrentPoint()
{
	std::vector<float> intersectionDistances;

	for (auto &intersection : intersections)
	{
		intersectionDistances.push_back(GetDistance(GetMousePosition(), intersection));
	}

	auto minPos = std::min_element(intersectionDistances.begin(), intersectionDistances.end()) - intersectionDistances.begin();
	if (!intersectionDistances.empty())
	{
		if (intersectionDistances.at(minPos) <= 10)
		{
			currentPoint = intersections.at(minPos);
			return;
		}
	}

	intersectionDistances.clear();
	currentPoint = GetMousePosition();
}

void DrawObj()
{
	UpdateCurrentPoint();

	if (!IsMouseButtonPressed(0))
	{
		return;
	}

	if (!firstPointed)
	{
		firstPointed = true;
		firstPoint = currentPoint;
		if (drawObject == 5)
		{
			firstPointed = false;

			for (auto &point : points)
			{
				if (SameVector2(point, currentPoint))
				{
					return;
				}
			}
			points.push_back(currentPoint);
		}
	}
	else
	{
		firstPointed = false;
		secondPoint = currentPoint;
		switch (drawObject)
		{
		case 1:
			circles.push_back(Circle{firstPoint, GetDistance(firstPoint, secondPoint)});
			break;
		case 2:
			distances.push_back(Line{firstPoint, secondPoint, 2});
			break;
		case 3:
			rays.push_back(Line{firstPoint, secondPoint, 3});
			break;
		case 4:
			straightLines.push_back(Line{firstPoint, secondPoint, 4});
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

void InputHandler()
{
	DrawObj();
	SetDrawObj();
	InterruptDrawing();
}

void CheckResized()
{
	if (!IsWindowResized())
	{
		return;
	}

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

void Update()
{
	InputHandler();
	CheckResized();
}

void DrawDrawingObj()
{
	DrawPointObj(currentPoint);

	if (!firstPointed)
	{
		return;
	}

	currentLine.pointA = firstPoint;
	currentLine.pointB = currentPoint;

	switch (drawObject)
	{
	case 1:
	{
		currentCircle.middle = firstPoint;
		currentCircle.radius = GetDistance(firstPoint, currentPoint);
		DrawCircleObj(currentCircle);
	}
	break;
	case 2:
		DrawDistanceObj(currentLine);
		break;
	case 3:
	{
		currentLine.UpdateSecondConnectionPoint();
		DrawRayObj(currentLine);
	}
	break;
	case 4:
	{
		currentLine.UpdateConnectionPoints();
		DrawStraightLineObj(currentLine);
	}
	break;
	default:
		break;
	}
}

void Draw()
{
	ClearBackground(RAYWHITE);

	for (auto &line : distances)
	{
		DrawDistanceObj(line);
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
	for (auto &point : points)
	{
		DrawPointObj(point);
	}

	DrawDrawingObj();
}

int main()
{
	Init();

	while (!WindowShouldClose())
	{
		Update();

		BeginDrawing();
		Draw();
		EndDrawing();
	}
	CloseWindow();
	return 0;
}
