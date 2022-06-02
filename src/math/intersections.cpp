#include "intersections.h"
#include "math_misc.h"
#include "../app/geometry_objects.h"
#include "raylib.h"

#include "math.h"
#include <vector>

std::vector<Point> intersections;

void FindCircleCircleIntersections(Vector2 *circleCenterA, Vector2 *circleCenterB, float circleRadiusA, float circleRadiusB)
{
	float centersDistance = GetDistance(circleCenterA, circleCenterB);
	if (centersDistance == 0)
	{
		return;
	}

	float aSquared = pow(circleRadiusA, 2);
	float x = (aSquared + pow(centersDistance, 2) - pow(circleRadiusB, 2)) / (2 * centersDistance);
	float y = aSquared - pow(x, 2);
	
	if (y < 0)
	{
		return;
	}
	else if (y > 0)
	{
		y = sqrt(y);
	}

	float ex0 = (circleCenterB->x - circleCenterA->x) / centersDistance;
	float ex1 = (circleCenterB->y - circleCenterA->y) / centersDistance;
	float ey0 = -ex1;
	float ey1 = ex0;
	float q1X = circleCenterA->x + x * ex0;
	float q1Y = circleCenterA->y + x * ex1;

	if (y == 0)
	{
		intersections.push_back({{q1X, q1Y}});
		return;
	}

	float q2X = q1X - y * ey0;
	float q2Y = q1Y - y * ey1;
	q1X += y * ey0;
	q1Y += y * ey1;

	intersections.push_back({{q1X, q1Y}});
	intersections.push_back({{q2X, q2Y}});
}

void FindLineLineIntersections(Vector2 *A1, Vector2 *A2, Vector2 *B1, Vector2 *B2)
{
	Vector2 intersection;
	Vector2 *intersection_ptr = &intersection;
	if (CheckCollisionLines(*A1, *A2, *B1, *B2, intersection_ptr))
	{
		intersections.push_back(*intersection_ptr);
	}
}

void FindCircleLineIntersections(Vector2 *A, float r, Vector2 *pointA, Vector2 *pointB)
{
	Vector2 B1 = {pointA->x - A->x, pointA->y - A->y};
	Vector2 B2 = {pointB->x - A->x, pointB->y - A->y};

	float v1 = B2.x - B1.x, v2 = B2.y - B1.y;
	float D = pow(r, 2) * (pow(v1, 2) + pow(v2, 2)) - pow(B1.x * v2 - B1.y * v1, 2);
	float _t1 = -B1.x * v1 - B1.y * v2;
	float _t2 = pow(v1, 2) + pow(v2, 2);
	float t1 = (_t1 + sqrt(D)) / _t2, t2 = (_t1 - sqrt(D)) / _t2;

	LinearFunction *function = GetLinearFunction(&B1, &B2);

	if (!((int)B1.x == (int)B2.x))
	{
		if (t1 >= 0 && t1 <= 1)
		{
			float x = v1 * t1 + B1.x;
			float y = function->m * x + function->n;
			intersections.push_back({{x + A->x, y + A->y}});
		}

		if (t2 >= 0 && t2 <= 1)
		{
			float x = v1 * t2 + B1.x;
			float y = function->m * x + function->n;
			intersections.push_back({{x + A->x, y + A->y}});
		}
		return;
	}

	B1 = {B1.x + A->x, B1.y + A->y};
	B2 = {B2.x + A->x, B2.y + A->y};

	float undersqrt = 2 * A->x * B1.x + pow(r, 2) - pow(B1.x, 2) - pow(A->x, 2);
	float _y = sqrt(undersqrt);
	float y1 = A->y + _y, y2 = A->y - _y;

	float b1 = B1.y, b2 = B2.y, _var;
	if (b1 > b2)
	{
		_var = b1;
		b1 = b2;
		b2 = _var;
	}

	if (y1 >= b1 && y1 <= b2)
	{
		intersections.push_back({{B1.x, y1}});
	}
	if (y2 >= b1 && y2 <= b2)
	{
		intersections.push_back({{B1.x, y2}});
	}
}
