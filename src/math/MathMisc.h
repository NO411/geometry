#pragma once

#include "../GeometryObjetcs.h"
#include "raylib.h"

class LinearFunction
{
public:
	LinearFunction(Vector2 &pointA, Vector2 &pointB);

	float m;
	float n;
	bool vertical;
};

Vector2 GetMousePosition2(Camera2D &camera);
float GetDistance(Vector2 &vec1, Vector2 &vec2);
bool SameFloat(float a, float b);
Vector2 CalculateConnectionPoint(Vector2 &p1, Vector2 &p2, float m, float n, Camera2D &camera);

template<typename L>
Vector2 GetOrthogonalLinesIntersection(Vector2 &worldMousePos, L &line)
{
	LinearFunction f(line.pointA, line.pointB);
	float m2 = -1 / f.m;
	float n2 = worldMousePos.y - m2 * worldMousePos.x;
	float x = (n2 - f.n) / (f.m - m2);
	float y = f.m * x + f.n;

	Vector2 connectionPoint = {x, y};

	if (f.vertical)
	{
		connectionPoint = {line.pointA.x, worldMousePos.y};
	}

	if (SameFloat(line.pointA.y, line.pointB.y))
	{
		connectionPoint = {worldMousePos.x, line.pointA.y};
	}

	return connectionPoint;
}

Vector2 GetCircleConnection(Vector2 &worldMousePos, Circle &circle);
