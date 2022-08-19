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

long double GetDistance(Vec2 &vec1, Vec2 &vec2)
{
	return sqrt(pow(vec2.x - vec1.x, 2) + pow(vec2.y - vec1.y, 2));
}

bool SameDouble(long double a, long double b)
{
	if (std::abs(a - b) <= 1.0e-5)
	{
		return true;
	}
	return std::abs(a - b) <= 1.0e-5 * std::max(std::abs(a), std::abs(b));
}

Vec2 CalculateConnectionPoint(Vec2 &p1, Vec2 &p2, long double m, long double n, Camera2D &camera)
{
	Vec2 connectionPoint;

	if (p1 == p2)
	{
		return p1;
	}

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

long double ArcTan(long double tan)
{
	return atan(tan) * 180 / PI;
}

long double CalculateAngleTan(Vec2 &circleCenter, Vec2 &point)
{
	Vec2 rightTrianglePoint = {circleCenter.x, point.y};

	if ((circleCenter.y > point.y && circleCenter.x < point.x) || (circleCenter.y < point.y && circleCenter.x > point.x))
	{
		rightTrianglePoint = {point.x, circleCenter.y};
	}

	long double opposite = GetDistance(rightTrianglePoint, point);
	long double adjacent = GetDistance(rightTrianglePoint, circleCenter);

	return (opposite) / (adjacent);
}

long double AcuteAngleToObtuseAngle(long double angle, Vec2 &circleCenter, Vec2 &point)
{
	if (circleCenter.y > point.y && circleCenter.x < point.x)
	{
		angle += 90;
	}

	if (circleCenter.y > point.y && circleCenter.x > point.x)
	{
		angle += 180;
	}

	if (circleCenter.y < point.y && circleCenter.x > point.x)
	{
		angle += 270;
	}

	if (angle > 360)
	{
		angle -= 360;
	}

	int circleX = circleCenter.x, circleY = circleCenter.y;
	int pointX = point.x, pointY = point.y;

	if (circleX == pointX)
	{
		angle = 180;
		if (circleY < pointY)
		{
			angle += 180;
		}
	}
	if (circleY == pointY)
	{
		angle = 90;
		if (circleX > pointX)
		{
			angle += 180;
		}
	}

	return angle;
}

long double CirclePointToAngle(Vec2 &center, Vec2 &point)
{
	return AcuteAngleToObtuseAngle(ArcTan(CalculateAngleTan(center, point)), center, point);
}

Sector PointsToSector(Vec2 &center, Vec2 &startPoint, Vec2 &endPoint)
{
	long double startAngle = CirclePointToAngle(center, startPoint);
	long double endAngle = CirclePointToAngle(center, endPoint);

	return {startAngle, endAngle, startPoint, endPoint};
}

bool SectorIncludesAngle(Sector &sector, long double angle, bool includeSectorEndPoints)
{
	if (SameDouble(angle, sector.startAngle) || SameDouble(angle, sector.endAngle))
	{
		return includeSectorEndPoints; // point is included by the circle because it is a sector end point
	}

	if (sector.startAngle > sector.endAngle)
	{
		if ((angle > sector.startAngle && angle > sector.endAngle) || (angle < sector.startAngle && angle < sector.endAngle))
		{
			return true;
		}
		return false;
	}
	if (angle > sector.startAngle && angle < sector.endAngle)
	{
		return true;
	}
	return false;
}

bool IsPointOnCircle(Vec2 &connectionPoint, Circle &circle)
{
	long double pointAngle = CirclePointToAngle(circle.center, connectionPoint);
	for (auto sector : circle.sectors)
	{
		if (SectorIncludesAngle(sector, pointAngle, false))
		{
			return false;
		}
	}
	return true;
}