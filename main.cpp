#include "raylib.h"
#include <iostream>
#include <vector>
#include <string>
#include <math.h>

Vector2 firstPoint;
Vector2 secondPoint;
bool firstPointed = false;

int drawObject = 2;

void Init() {
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	SetTraceLogLevel(LOG_NONE);
	InitWindow(900, 600, "geometry");
	SetTargetFPS(60);
	SetExitKey(0);
}

class GeometryObj {
	public:
	std::vector<Vector2> intersections;
	int objectNumber;

	GeometryObj() {
		//std::cout << "obj created" << std::endl;
	}
	~GeometryObj() {}
};

class Circle: public GeometryObj {
	public:
	Vector2 middle;
	float radius;

	Circle(Vector2 middle, float radius):middle(middle), radius(radius) {
		objectNumber = 1;
	}
	~Circle() {}
};

class Line: public GeometryObj {
	public:
	Vector2 pointA;
	Vector2 pointB;

	Vector2 firstConnectionPoint;
	Vector2 secondConnectionPoint;

	Line(Vector2 pointA, Vector2 pointB, int n):pointA(pointA), pointB(pointB) {
		objectNumber = n;
		UpdateConnectionPoints();
	}
	~Line() {}

	// 1 = a, 2 = b
	Vector2 GetMN(int ab);

	void UpdateFirstConnectionPoint();
	void UpdateSecondConnectionPoint();

	void UpdateConnectionPoints();
};

Vector2 Line::GetMN(int ab) {
	Vector2 p1 = pointA;
	Vector2 p2 = pointB;

	if (ab == 2) {
		p1 = pointB;
		p2 = pointA;
	}

	float m = (p1.y - p2.y) / (p1.x - p2.x);
	float n = p2.y - (m * p2.x);
	return {m, n};
}

Vector2 CalculateConnectionPoint(Vector2& p1, Vector2& p2, float m, float n) {
	Vector2 connectionPoint = {};

	float y = m * GetScreenWidth() + n;
	connectionPoint.x = GetScreenWidth();
	if (p1.x > p2.x) {
		y = n;
		connectionPoint.x = 0;
	}

	if (y > GetScreenHeight()) {
		connectionPoint.y = GetScreenHeight();
		connectionPoint.x = (connectionPoint.y - n) / m;
	} else if (y < 0) {
		connectionPoint.y = 0;
		connectionPoint.x = (connectionPoint.y - n) / m;
	} else {
		connectionPoint.y = y;
	}

	// dividing by 0 is not possible ...
	// needed to get vertical lines
	if (p1.x - p2.x == 0) {
		connectionPoint.x = p1.x;
		if (p1.y > p2.y) {
			connectionPoint.y = 0;
		} else {
			connectionPoint.y = GetScreenHeight();
		}
	}

	return connectionPoint;
}

void Line::UpdateFirstConnectionPoint() {
	Vector2 mn = GetMN(1);
	firstConnectionPoint = CalculateConnectionPoint(pointB, pointA, mn.x, mn.y);
}

void Line::UpdateSecondConnectionPoint() {
	Vector2 mn = GetMN(2);

	secondConnectionPoint = CalculateConnectionPoint(pointA, pointB, mn.x, mn.y);
}

void Line::UpdateConnectionPoints() {
	UpdateFirstConnectionPoint();
	UpdateSecondConnectionPoint();
}

std::vector<Circle> circles;     //1
std::vector<Line> distances;     //2
std::vector<Line> rays;          //3
std::vector<Line> straightLines; //4
std::vector<Vector2> points;     //5
Line currentLine = {{0, 0}, {0, 0}, 2};
Circle currentCircle = {{0, 0}, 0};

float GetDistance(Vector2 vec1, Vector2 vec2) {
	return sqrt(pow(vec2.x - vec1.x, 2) + pow(vec2.y - vec1.y, 2));
}

void DrawPointObj(Vector2 point) {
	DrawRing(point, 2, 5, 0, 360, 1000, {51, 79, 84, 150});
}

void DrawCircleObj(Circle circle) {
	DrawCircleSectorLines(circle.middle, circle.radius, 0, 360, 2 * circle.radius, BLACK);
}

void DrawDistanceObj(Line line) {
	DrawLineEx(line.pointA, line.pointB, 1, BLACK);
}

bool SameVector2(Vector2 v1, Vector2 v2) {
	return (v1.x == v2.x && v1.y == v2.y);
}

void DrawRayObj(Line line) {
	if (SameVector2(line.pointA, line.secondConnectionPoint) || SameVector2(line.pointA, GetMousePosition())) {
		return;
	}

	DrawLineEx(line.pointA, line.secondConnectionPoint, 1, BLACK);
}

void DrawStraightLineObj(Line line) {
	DrawLineEx(line.firstConnectionPoint, line.secondConnectionPoint, 1, BLACK);
}

void SetDrawObj() {
	if (!IsKeyDown(KEY_LEFT_CONTROL)) {
		return;
	}

	switch (GetKeyPressed()) {
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

void DrawObj() {
	if (!IsMouseButtonPressed(0)) {
		return;
	}

	if (!firstPointed) {
		firstPointed = true;
		firstPoint = GetMousePosition();
		if (drawObject == 5) {
			points.push_back(firstPoint);
			firstPointed = false;
		}
	} else {
		firstPointed = false;
		secondPoint = GetMousePosition();
		switch (drawObject) {
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

void InterruptDrawing() {
	if (IsKeyPressed(KEY_ESCAPE)) {
		firstPointed = false;
	}
}

void InputHandler() {
	DrawObj();
	SetDrawObj();
	InterruptDrawing();
}

void CheckRezied() {
	if (!IsWindowResized()) {
		return;
	}

	for (auto& straightLine: straightLines)
	{
		straightLine.UpdateConnectionPoints();
	}

	for (auto& ray: rays)
	{
		ray.UpdateSecondConnectionPoint();
	}
}

void Update() {
	InputHandler();
	CheckRezied();
}

void DrawDrawingObj() {
	if (!firstPointed) {
		return;
	}
	
	currentLine.pointA = firstPoint;
	currentLine.pointB = GetMousePosition();

	switch (drawObject) {
	case 1:
		{
			currentCircle.middle = firstPoint;
			currentCircle.radius = GetDistance(firstPoint, GetMousePosition());
			DrawCircleObj(currentCircle);
		}
		break;
	case 2:
		DrawDistanceObj(currentLine);
		break;
	case 3:
		DrawRayObj(currentLine);
		break;
	case 4:
		DrawStraightLineObj(currentLine);
		break;
	case 5:
		DrawPointObj(GetMousePosition());
		break;
	default:
		break;
	}
}

void Draw() {
	ClearBackground(RAYWHITE);

	for (auto& line: distances) {
		DrawDistanceObj(line);
	}
	for (auto& circle: circles) {
		DrawCircleObj(circle);
	}
	for (auto& ray: rays) {
		DrawRayObj(ray);
	}
	for (auto& straightLine: straightLines) {
		DrawStraightLineObj(straightLine);
	}
	for (auto& point: points) {
		DrawPointObj(point);
	}

	DrawDrawingObj();
}

int main() {
	Init();

	while (!WindowShouldClose()) {
		Update();

		BeginDrawing();
			Draw();
		EndDrawing();
	}
	CloseWindow();
	return 0;
}
