#include "intersections.h"
#include "math_misc.h"
#include "../app/geometry_objects.h"
#include "raylib.h"

#include "math.h"
#include <vector>

std::vector<Point> intersections;

void FindCircleCircleIntersections(Vector2 *A, Vector2 *B, float a, float b)
{
	float c = GetDistance(A, B);
	if (c == 0)
	{
		return;
	}

	float powa = pow(a, 2);
	float x = (powa + pow(c, 2) - pow(b, 2)) / (2 * c);
	float y = powa - pow(x, 2);
	if (y < 0)
	{
		return;
	}
	else if (y > 0)
	{
		y = sqrt(y);
	}
	float ex0 = (B->x - A->x) / c, ex1 = (B->y - A->y) / c;
	float ey0 = -ex1, ey1 = ex0;
	float Q1x = A->x + x * ex0;
	float Q1y = A->y + x * ex1;
	if (y == 0)
	{
		intersections.push_back({{Q1x, Q1y}});
		return;
	}
	float Q2x = Q1x - y * ey0;
	float Q2y = Q1y - y * ey1;
	Q1x += y * ey0;
	Q1y += y * ey1;
	intersections.push_back({{Q1x, Q1y}});
	intersections.push_back({{Q2x, Q2y}});
}

void FindLineLineIntersections(Vector2 *A1, Vector2 *A2, Vector2 *B1, Vector2 *B2)
{
	Vector2 mnA = *GetMN(A1, A2);
	Vector2 mnB = *GetMN(B1, B2);
	float x, y;

	// no intersection if the lines are parallel
	if ((int)mnA.x == (int)mnB.x)
	{
		return;
	}

	// mnA.x * x + mnA.y = mnB.x * x + mnB.y
	x = (mnA.y - mnB.y) / (mnB.x - mnA.x);
	y = mnA.x * x + mnA.y;

	if ((int)A1->x == (int)A2->x)
	{
		x = A1->x;
		y = mnB.x * A1->x + mnB.y;
	}
	else if ((int)B1->x == (int)B2->x)
	{
		x = B1->x;
		y = mnA.x * B1->x + mnA.y;
	}

	Vector2 intersection = {x, y};

	// check whether the intersection is included of both lines

	if (!CheckCollisionLines(*A1, *A2, *B1, *B2, &intersection))
	{
		return;
	}

	intersections.push_back(intersection);
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

	Vector2 mn = *GetMN(&B1, &B2);

	if (!((int)B1.x == (int)B2.x))
	{
		if (t1 >= 0 && t1 <= 1)
		{
			float x = v1 * t1 + B1.x;
			float y = mn.x * x + mn.y;
			intersections.push_back({{x + A->x, y + A->y}});
		}

		if (t2 >= 0 && t2 <= 1)
		{
			float x = v1 * t2 + B1.x;
			float y = mn.x * x + mn.y;
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
