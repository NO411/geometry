#pragma once

#include "../GeometryObjetcs.h"
#include "../GeometryBoard.h"
#include "raylib.h"
#include <vector>
#include "MathMisc.h"
#include <cmath>

void GetCircleCircleIntersections(IntersectionStorage &intersections, Circle &circle1, Circle &circle2);

template <typename L1, typename L2>
void GetLineLineIntersections(IntersectionStorage &intersections, L1 &line1, L2 &line2)
{
	auto Push = [&intersections, &line1, &line2](Vec2 newIntersection)
	{
		if (line1.IsPointOnLine(newIntersection) && line2.IsPointOnLine(newIntersection))
		{
			int newID = intersections.Push(newIntersection);
			line1.intersectionIDs.push_back(newID);
			line2.intersectionIDs.push_back(newID);
		}
	};

	LinearFunction f1(line1.pointA, line1.pointB);
	LinearFunction f2(line2.pointA, line2.pointB);

	long double x = -((f2.n - f1.n) / (f2.m - f1.m));
	long double y = f1.m * x + f1.n;

	if (!SameDouble(f1.m, f2.m))
	{
		Push({x, y});
	}

	if (f1.vertical)
	{
		Push({line1.pointA.x, f2.m * line1.pointA.x + f2.n});
	}
	else if (f2.vertical)
	{
		Push({line2.pointA.x, f1.m * line2.pointA.x + f1.n});
	}
}

template <typename L>
void GetLineCircleIntersections(IntersectionStorage &intersections, L &line, Circle &circle)
{
	/*
	y = m * x + n
	(x - center.x)² + (y - center.y)² = r²
	(x - center.x)² + (m * x + n - center.y)² = r²

	x₀|₁ = 0.5 * (-2 * m * n + 2 * center.y * m + 2 * center.x +/- sqrt((-2 * center.y * m - 2 * center.x + 2 * m * n)² - 4*(-2 * center.y * n - r² + center.y² + n² + center.x²) * (1 + m²))) * (1 + m²)^-1
	*/

	auto Push = [&intersections, &line, &circle](Vec2 newIntersection)
	{
		if (line.IsPointOnLine(newIntersection))
		{
			int newID = intersections.Push(newIntersection);
			line.intersectionIDs.push_back(newID);
			circle.intersectionIDs.push_back(newID);
		}
	};

	LinearFunction f(line.pointA, line.pointB);
	long double m = f.m;
	long double n = f.n;

	// just some var saving to simplify the code
	long double mn2 = 2 * m * n;
	long double pm = pow(m, 2);
	long double cx = circle.center.x;
	long double cy = circle.center.y;
	long double cx2 = 2 * cx;
	long double cy2 = 2 * cy;
	long double cy2m = cy2 * m;
	long double pm1 = 1 + pm;

	long double v1 = -mn2 + cy2m + cx2;
	long double v2 = 0.5 * pow(pm1, -1);
	long double v3 = sqrt(pow(-cy2m - cx2 + mn2, 2) - 4 * (-cy2 * n - pow(circle.radius, 2) + pow(cy, 2) + pow(n, 2) + pow(cx, 2)) * pm1);

	// solutions
	long double x0 = v2 * (v1 + v3);
	long double x1 = v2 * (v1 - v3);

	Push({x0, m * x0 + n});
	Push({x1, m * x1 + n});

	if (!SameDouble(line.pointA.x, line.pointB.x))
	{
		return;
	}

	if (line.pointA.x - circle.center.x < circle.radius || SameDouble(line.pointA.x - circle.center.x, circle.radius))
	{
		/*
		a² + b² = r²
		b = sqrt(r² - a²)
		b = sqrt(r² - |center.x - pointA.x|²)
		y = center.y +/- b;
		*/

		long double b = sqrt(pow(circle.radius, 2) - pow(std::abs(circle.center.x - line.pointA.x), 2));

		if (SameDouble(circle.center.x, line.pointA.x))
		{
			b = circle.radius;
		}

		Push({line.pointA.x, circle.center.y - b});
		Push({line.pointA.x, circle.center.y + b});
	}
}
