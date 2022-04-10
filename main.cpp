#include "raylib.h"
#include <iostream>
#include <vector>
#include <string>
#include <math.h>

void Init() {
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	SetTraceLogLevel(LOG_NONE);
	InitWindow(900, 600, "geometry");
	SetTargetFPS(60);
	SetExitKey(0);
}

struct Circle {
	Vector2 middle;
	float radius;
};

class Line {
	public:
	Vector2 pointA;
	Vector2 pointB;

	Line(Vector2& pointA, Vector2& pointB):pointA(pointA), pointB(pointB) {}
	~Line() {}

	// 1 = a, 2 = b
	Vector2 GetMN(int ab);

	Vector2 GetFirstConnectionPoint();
	Vector2 GetSecondConnectionPoint();
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
	// needed to get verticla lines
	if (p1.x - p2.x == 0) {
		connectionPoint.x = p1.x;
		if (p1.y > p2.y) {
			connectionPoint.y = GetScreenHeight();
		} else {
			connectionPoint.y = 0;
		}
	}

	return connectionPoint;
}

Vector2 Line::GetFirstConnectionPoint() {
	Vector2 mn = GetMN(1);
	return CalculateConnectionPoint(pointB, pointA, mn.x, mn.y);
}

Vector2 Line::GetSecondConnectionPoint() {
	Vector2 mn = GetMN(2);

	return CalculateConnectionPoint(pointA, pointB, mn.x, mn.y);
}


std::vector<Circle> circles;     //1
std::vector<Line> distances;     //2
std::vector<Line> rays;          //3
std::vector<Line> straightLines; //4
std::vector<Vector2> points;     //5

float GetDistance(Vector2 vec1, Vector2 vec2) {
	return sqrt(pow(vec2.x - vec1.x, 2) + pow(vec2.y - vec1.y, 2));
}

void DrawPointObj(Vector2 point) {
	DrawRing(point, 2, 5, 0, 360, 1000, {51, 79, 84, 150});
}

void DrawCircleObj(Circle circle) {
	DrawCircleLines(circle.middle.x, circle.middle.y, circle.radius, BLACK);
}

void DrawDistanceObj(Line line) {
	DrawLineEx(line.pointA, line.pointB, 1, BLACK);
}

void DrawRayObj(Line line) {
	DrawLineEx(line.pointA, line.GetSecondConnectionPoint(), 1, BLACK);
}

void DrawStraightLineObj(Line& line) {
	DrawLineEx(line.GetFirstConnectionPoint(), line.GetSecondConnectionPoint(), 1, BLACK);
}

int main() {
	Init();

	Vector2 mousePos = {(float)GetMouseX(), (float)GetMouseY()};

	Vector2 firstPoint;
	Vector2 secondPoint;
	bool firstPointed = false;

	int drawObject = 2;

	while (!WindowShouldClose()) {
		mousePos = {(float)GetMouseX(), (float)GetMouseY()};

		if (IsMouseButtonPressed(0)) {
			if (!firstPointed) {
				firstPointed = true;
				firstPoint = mousePos;
				if (drawObject == 5) {
					points.push_back(firstPoint);
					firstPointed = false;
				}
			} else {
				firstPointed = false;
				secondPoint = mousePos;

				switch (drawObject) {
				case 1:
					circles.push_back(Circle{firstPoint, GetDistance(firstPoint, secondPoint)});
					break;
				case 2:
					distances.push_back(Line{firstPoint, secondPoint});
					break;
				case 3:
					rays.push_back(Line{firstPoint, secondPoint});
					break;
				case 4:
					straightLines.push_back(Line{firstPoint, secondPoint});
					break;
				default:
					break;
				}
			}
		}


		if (IsKeyDown(KEY_LEFT_CONTROL)) {
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

		if (IsKeyPressed(KEY_ESCAPE)) {
			firstPointed = false;
		}

		BeginDrawing();
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

			if (firstPointed) {
				Line line = {firstPoint, mousePos};
				switch (drawObject) {
				case 1:
					DrawCircleObj({firstPoint, GetDistance(firstPoint, mousePos)});
					break;
				case 2:
					DrawDistanceObj(line);
					break;
				case 3:
					DrawRayObj(line);
					break;
				case 4:
					DrawStraightLineObj(line);
					break;
				case 5:
					DrawPointObj(mousePos);
					break;
				default:
					break;
				}
			}

		EndDrawing();
	}
	CloseWindow();
	return 0;
}
