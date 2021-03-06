#include "math_misc.h"
#include "../app/geometry_objects.h"
#include "raylib.h"

#include <math.h>

Camera2D camera = {0};

float GetMouseX2()
{
	return (GetMouseX() - camera.offset.x + GetScreenWidth() / 2);
}

float GetMouseY2()
{
	return (GetMouseY() - camera.offset.y + GetScreenHeight() / 2);
}

Vector2 *GetMousePosition2()
{
	return new Vector2{GetMouseX2(), GetMouseY2()};
}

LinearFunction *GetLinearFunction(Vector2 *p1, Vector2 *p2)
{
	float m = (p1->y - p2->y) / (p1->x - p2->x);
	float n = p2->y - (m * p2->x);
	return new LinearFunction{m, n};
}

float GetDistance(Vector2 *vec1, Vector2 *vec2)
{
	return sqrt(pow(vec2->x - vec1->x, 2) + pow(vec2->y - vec1->y, 2));
}

bool IsPointOnLine(Vector2 *point, Vector2 *pointA, Vector2 *pointB)
{
	float x1 = pointA->x, x2 = pointB->x, _var;
	if (x1 > x2)
	{
		_var = x1;
		x1 = x2;
		x2 = _var;
	}
	if (point->x >= x1 && point->x <= x2)
	{
		return true;
	}
	if ((int)x1 == (int)x2)
	{
		float y1 = pointA->y, y2 = pointB->y;
		if (y1 > y2)
		{
			_var = y1;
			y1 = y2;
			y2 = _var;
		}
		if (point->y >= y1 && point->y <= y2)
		{
			return true;
		}
	}
	return false;
}

Vector2 *GetOrthogonalLinesIntersection(Vector2 *point, Vector2 *pointA, Vector2 *pointB)
{
	LinearFunction *function = GetLinearFunction(pointA, pointB);
	float m2 = -1 / function->m;
	float n2 = point->y - m2 * point->x;
	float x = (n2 - function->n) / (function->m - m2);
	float y = function->m * x + function->n;

	Vector2 connectionPoint = {x, y};

	if ((int)pointA->x == (int)pointB->x)
	{
		connectionPoint = {pointA->x, point->y};
	}

	if ((int)pointA->y == (int)pointB->y)
	{
		connectionPoint = {point->x, pointA->y};
	}

	return new Vector2{connectionPoint};
}

Vector2 *GetCircleConnection(Circle *circle)
{
	Vector2 *mouse = GetMousePosition2();
	float x = circle->center.x + (mouse->x - circle->center.x) / (GetDistance(mouse, &circle->center) / circle->radius);
	LinearFunction *function = GetLinearFunction(&circle->center, mouse);
	float y = function->m * x + function->n;

	return new Vector2{x, y};
}

Vector2 *CalculateConnectionPoint(Vector2 *p1, Vector2 *p2, float m, float n)
{
	Vector2 connectionPoint;

	// correct connectionPoint offset
	Vector2 offset = {
		camera.offset.x - GetScreenWidth() / 2,
		camera.offset.y - GetScreenHeight() / 2};

	if (p1->x > p2->x)
	{
		connectionPoint.x = -offset.x;
	}
	else
	{
		connectionPoint.x = GetScreenWidth() - offset.x;
	}

	connectionPoint.y = m * connectionPoint.x + n;

	if ((int)p1->x == (int)p2->x)
	{
		connectionPoint.x = p1->x;
		if (p1->y > p2->y)
		{
			connectionPoint.y = -offset.y;
		}
		else
		{
			connectionPoint.y = GetScreenHeight() - offset.y;
		}
	}

	return new Vector2{connectionPoint};
}

float ArcTan(float tan)
{
	return atan(tan) * 180 / PI;
}

float CalculateAngleTan(Vector2 *circleCenter, Vector2 *point)
{
	Vector2 rightTrianglePoint = {circleCenter->x, point->y};

	if ((circleCenter->y > point->y && circleCenter->x < point->x) || (circleCenter->y < point->y && circleCenter->x > point->x))
	{
		rightTrianglePoint = {point->x, circleCenter->y};
	}

	float opposite = GetDistance(&rightTrianglePoint, point);
	float adjacent = GetDistance(&rightTrianglePoint, circleCenter);

	return (opposite) / (adjacent);
}

float AcuteAngleToObtuseAngle(float angle, Vector2 *circleCenter, Vector2 *point)
{
	if (circleCenter->y > point->y && circleCenter->x < point->x)
	{
		angle += 90;
	}

	if (circleCenter->y > point->y && circleCenter->x > point->x)
	{
		angle += 180;
	}

	if (circleCenter->y < point->y && circleCenter->x > point->x)
	{
		angle += 270;
	}

	if (angle > 360)
	{
		angle -= 360;
	}

	int circleX = circleCenter->x, circleY = circleCenter->y;
	int pointX = point->x, pointY = point->y;

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

float CirclePointToAngle(Vector2 *center, Vector2 *point)
{
	return AcuteAngleToObtuseAngle(ArcTan(CalculateAngleTan(center, point)), center, point);
}

Sector *PointsToSector(Vector2 *center, Vector2 *startPoint, Vector2 *endPoint)
{
	float startAngle = CirclePointToAngle(center, startPoint);
	float endAngle = CirclePointToAngle(center, endPoint);

	return new Sector(startAngle, endAngle, *startPoint, *endPoint);
}

bool SectorIncludesAngle(Sector *sector, float angle)
{
	if (sector->startAngle > sector->endAngle)
	{
		if ((angle > sector->startAngle && angle > sector->endAngle) || (angle < sector->startAngle && angle < sector->endAngle))
		{
			return true;
		}
		return false;
	}
	if (angle > sector->startAngle && angle < sector->endAngle)
	{
		return true;
	}
	return false;
}

bool IsPointOnCircle(Vector2 *connectionPoint, Circle *circle)
{
	float pointAngle = CirclePointToAngle(&circle->center, connectionPoint);
	for (auto sector : circle->sectors)
	{
		if (!SectorIncludesAngle(&sector, pointAngle))
		{
			continue;
		}
		return false;
	}
	return true;
}
