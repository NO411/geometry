#include "Intersections.h"
#include "MathMisc.h"
#include "../GeometryObjetcs.h"
#include "raylib.h"
#include <vector>
#include <cmath>

// LineLine / CircleLine intersection in header

void GetCircleCircleIntersections(IntersectionStorage &intersections, Circle &circle1, Circle &circle2)
{
	if (!CheckCollisionCircles(circle1.center.ToRaylibVec(), circle1.radius, circle2.center.ToRaylibVec(), circle2.radius))
	{
		return;
	}

	long double r2 = pow(circle1.radius, 2);
	long double centerDistance = GetDistance(circle1.center, circle2.center);
	long double x = (r2 + pow(centerDistance, 2) - pow(circle2.radius, 2)) / (2 * centerDistance);
	long double y = r2 - pow(x, 2);

	if (y < 0)
	{
		return;
	}
	else if (y > 0)
	{
		y = sqrt(y);
	}

	long double ex0 = (circle2.center.x - circle1.center.x) / centerDistance;
	long double ex1 = (circle2.center.y - circle1.center.y) / centerDistance;
	long double ey0 = -ex1;
	long double ey1 = ex0;
	long double q1X = circle1.center.x + x * ex0;
	long double q1Y = circle1.center.y + x * ex1;

	auto Push = [&intersections, &circle1, &circle2](Vec2 newIntersection)
	{
		if (!IsPointOnCircle(newIntersection, circle1) || !IsPointOnCircle(newIntersection, circle2))
		{
			return;
		}

		int newID = intersections.Push(newIntersection);
		circle1.intersectionIDs.push_back(newID);
		circle2.intersectionIDs.push_back(newID);
	};

	if (SameDouble(y, 0))
	{
		Push({q1X, q1Y});
		return;
	}

	long double q2X = q1X - y * ey0;
	long double q2Y = q1Y - y * ey1;
	q1X += y * ey0;
	q1Y += y * ey1;

	Push({q1X, q1Y});
	Push({q2X, q2Y});
}
