#pragma once

#include "../GeometryObjetcs.h"
#include "raylib.h"

class LinearFunction
{
public:
	LinearFunction(Vec2 &pointA, Vec2 &pointB);

	long double m;
	long double n;
	bool vertical;
};

Vec2 GetMousePosition2(Camera2D &camera);
long double GetDistance(Vec2 &vec1, Vec2 &vec2);
bool SameDouble(long double a, long double b);
Vec2 CalculateConnectionPoint(Vec2 &p1, Vec2 &p2, long double m, long double n, Camera2D &camera);

template <typename L>
Vec2 GetOrthogonalLinesIntersection(Vec2 &worldMousePos, L &line)
{
	LinearFunction f(line.pointA, line.pointB);
	long double m2 = -1 / f.m;
	long double n2 = worldMousePos.y - m2 * worldMousePos.x;
	long double x = (n2 - f.n) / (f.m - m2);
	long double y = f.m * x + f.n;

	Vec2 connectionPoint = {x, y};

	if (f.vertical)
	{
		connectionPoint = {line.pointA.x, worldMousePos.y};
	}

	if (SameDouble(line.pointA.y, line.pointB.y))
	{
		connectionPoint = {worldMousePos.x, line.pointA.y};
	}

	return connectionPoint;
}

Vec2 GetCircleConnection(Vec2 &worldMousePos, Circle &circle);
