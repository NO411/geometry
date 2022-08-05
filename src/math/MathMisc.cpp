#include "MathMisc.h"
#include "../GeometryObjetcs.h"
#include "raylib.h"
#include <cmath>

LinearFunction::LinearFunction(Vec2 &p1, Vec2 &p2)
{
	m = (p1.y - p2.y) / (p1.x - p2.x);
	n = p2.y - (m * p2.x);
	vertical = SameDouble(p1.x, p2.x);
}

Vec2 GetMousePosition2(Camera2D &camera)
{
	return {GetScreenToWorld2D(GetMousePosition(), camera)};
}

float GetDistance(Vec2 &vec1, Vec2 &vec2)
{
	return sqrt(pow(vec2.x - vec1.x, 2) + pow(vec2.y - vec1.y, 2));
}

bool SameDouble(long double a, long double b)
{
	if (std::abs(a - b) <= 1.0e-5)
	{
		return true;
	}
	bool w = std::abs(a - b) <= 1.0e-5 * std::max(std::abs(a), std::abs(b));
	return w;
}

Vec2 CalculateConnectionPoint(Vec2 &p1, Vec2 &p2, long double m, long double n, Camera2D &camera)
{
	Vec2 connectionPoint;

	if (p1.x > p2.x)
	{
		connectionPoint.x = GetScreenToWorld2D({0, 0}, camera).x;
	}
	else
	{
		connectionPoint.x = GetScreenToWorld2D({(float)GetScreenWidth(), 0}, camera).x;
	}

	connectionPoint.y = m * connectionPoint.x + n;

	if (SameDouble(p1.x, p2.x))
	{
		connectionPoint.x = p1.x;
		if (p1.y > p2.y)
		{
			connectionPoint.y = GetScreenToWorld2D({0, 0}, camera).y;
		}
		else
		{
			connectionPoint.y = GetScreenToWorld2D({0, (float)GetScreenHeight()}, camera).y;
		}
	}

	return GetWorldToScreen2D(connectionPoint.ToRaylibVec(), camera);
}

Vec2 GetCircleConnection(Vec2 &worldMousePos, Circle &circle)
{
	long double x = circle.center.x + (worldMousePos.x - circle.center.x) / (GetDistance(worldMousePos, circle.center) / circle.radius);
	LinearFunction f(circle.center, worldMousePos);
	long double y = f.m * x + f.n;

	if (SameDouble(worldMousePos.x, circle.center.x))
	{
		x = worldMousePos.x;
		y = circle.center.y - circle.radius;

		if (worldMousePos.y > circle.center.y)
		{
			y = circle.center.y + circle.radius;
		}
	}

	return {x, y};
}
