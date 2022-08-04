#include "Intersections.h"
#include "MathMisc.h"
#include "../GeometryObjetcs.h"
#include "raylib.h"
#include <vector>
#include <cmath>

// LineLine / CircleLine intersection in header

void GetCircleCircleIntersections(IntersectionStorage &intersections, Circle &circle1, Circle &circle2)
{
	if (!CheckCollisionCircles(circle1.center, circle1.radius, circle2.center, circle2.radius))
	{
		return;
	}

	float r2 = pow(circle1.radius, 2);
	float centerDistance = GetDistance(circle1.center, circle2.center);
	float x = (r2 + pow(centerDistance, 2) - pow(circle2.radius, 2)) / (2 * centerDistance);
	float y = r2 - pow(x, 2);

	if (y < 0)
	{
		return;
	}
	else if (y > 0)
	{
		y = sqrt(y);
	}

	float ex0 = (circle2.center.x - circle1.center.x) / centerDistance;
	float ex1 = (circle2.center.y - circle1.center.y) / centerDistance;
	float ey0 = -ex1;
	float ey1 = ex0;
	float q1X = circle1.center.x + x * ex0;
	float q1Y = circle1.center.y + x * ex1;

	auto Push = [&intersections, &circle1, &circle2](Vector2 newIntersection)
	{
		int newID = intersections.Push(newIntersection);
		circle1.intersectionIDs.push_back(newID);
		circle2.intersectionIDs.push_back(newID);
	};

	if (SameFloat(y, 0))
	{
		Push({q1X, q1Y});
		return;
	}

	float q2X = q1X - y * ey0;
	float q2Y = q1Y - y * ey1;
	q1X += y * ey0;
	q1Y += y * ey1;

	Push({q1X, q1Y});
	Push({q2X, q2Y});
}
