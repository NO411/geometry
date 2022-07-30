#include "Intersections.h"
#include "MathMisc.h"
#include "../GeometryObjetcs.h"
#include "raylib.h"
#include <vector>
#include <cmath>

std::vector<Vector2> GetCircleCircleIntersection(Vector2 &center, float radius, Vector2 &center_, float radius_)
{
    std::vector<Vector2> intersections;
    float centerDistance = GetDistance(center, center_);

    if (centerDistance < radius + radius_ || SameFloat(centerDistance, radius + radius_))
    {
        float r2 = pow(radius, 2);
	    float x = (r2 + pow(centerDistance, 2) - pow(radius_, 2)) / (2 * centerDistance);
	    float y = r2 - pow(x, 2);
	
	    if (y < 0)
	    {
            return intersections;
	    }
	    else if (y > 0)
	    {
		    y = sqrt(y);
	    }

	    float ex0 = (center_.x - center.x) / centerDistance;
	    float ex1 = (center_.y - center.y) / centerDistance;
	    float ey0 = -ex1;
	    float ey1 = ex0;
	    float q1X = center.x + x * ex0;
	    float q1Y = center.y + x * ex1;

	    if (SameFloat(y, 0))
	    {
		    intersections.push_back({q1X, q1Y});
		    return intersections;
	    }

	    float q2X = q1X - y * ey0;
	    float q2Y = q1Y - y * ey1;
	    q1X += y * ey0;
	    q1Y += y * ey1;

	    intersections.push_back({q1X, q1Y});
	    intersections.push_back({q2X, q2Y});
    }
    return intersections;
}

std::vector<Vector2> GetLineCircleIntersection(Vector2 &pointA, Vector2 &pointB, Vector2 &center, float radius)
{
    /*

    y = m * x + n
    (x - center.x)² + (y - center.y)² = r²
    (x - center.x)² + (m * x + n - center.y)² = r²

    x₀ = 0.5 * (-2 * m * n + 2 * center.y * m + 2 * center.x + sqrt((-2 * center.y * m - 2 * center.x + 2 * m * n)² - 4*(-2 * center.y * n - r² + center.y² + n² + center.x²) * (1 + m²))) * (1 + m²)^-1
    x₁ = 0.5 * (-2 * m * n + 2 * center.y * m + 2 * center.x - sqrt((-2 * center.y * m - 2 * center.x + 2 * m * n)² - 4*(-2 * center.y * n - r² + center.y² + n² + center.x²) * (1 + m²))) * (1 + m²)^-1

    */
    std::vector<Vector2> intersections;

    LinearFunction f(pointA, pointB);
    float m = f.m;
    float n = f.n;

    // just some var saving to simplify the code
    float mn2 = 2 * m * n;
    float pm = pow(m, 2);
    float cx = center.x;
    float cy = center.y;
    float cx2 = 2 * cx;
    float cy2 = 2 * cy;
    float cy2m = cy2 * m;
    float pm1 = 1 + pm;

    float v1 = -mn2 + cy2m + cx2;
    float v2 = 0.5 * pow(pm1, -1);
    float v3 = sqrt(pow(-cy2m - cx2 + mn2, 2) - 4 * (-cy2 * n - pow(radius, 2) + pow(cy, 2) + pow(n, 2) + pow(cx, 2)) * pm1);

    // solutions
    float x0 = v2 * (v1 + v3);
    float x1 = v2 * (v1 - v3);

    intersections.push_back({x0, m * x0 + n});
    intersections.push_back({x1, m * x1 + n});

    if (SameFloat(pointA.x, pointB.x))
    {
        if (pointA.x - center.x < radius || SameFloat(pointA.x - center.x, radius))
        {
            /*
            a² + b² = r²
            b = sqrt(r² - a²)
            b = sqrt(r² - |center.x - pointA.x|²)

            y = center.y +/- b;
            */

            float b = sqrt(pow(radius, 2) - pow(std::abs(center.x - pointA.x), 2));

            intersections.push_back({pointA.x, center.y - b});
            intersections.push_back({pointA.x, center.y + b});
        }
    }

    return intersections;
}

std::vector<Vector2> GetLineLineIntersection(Vector2 &pointA, Vector2 &pointB, Vector2 &pointA_, Vector2 &pointB_)
{
    /*
    y = m1 * x + n1
    y = m2 * x + n2
    m1 * x + n1 = m2 * x + n2
    x = -((n2-n1) / (m2-m1))
    */
    std::vector<Vector2> intersections;

    LinearFunction f1(pointA, pointB);
    LinearFunction f2(pointA_, pointB_);

    float x = -((f2.n - f1.n) / (f2.m - f1.m));
    float y = f1.m * x + f1.n;

    if (!SameFloat(f1.m, f2.m))
    {
        intersections.push_back({x, y});
    }

    if (f1.vertical)
    {
        intersections.push_back({pointA.x, f2.m * pointA.x + f2.n});
    }
    else if (f2.vertical)
    {
        intersections.push_back({pointB.x, f1.m * pointB.x + f1.n});
    }

    return intersections;
}

bool IsPointOnDistance(Vector2 &intersection, Distance &distance)
{
    float x1 = distance.pointA.x;
    float x2 = distance.pointB.x;
    if ((x1 < x2 && intersection.x > x1 && intersection.x < x2) || (x1 > x2 && intersection.x < x1 && intersection.x > x2) || ((SameFloat(intersection.x, x1) || SameFloat(intersection.x, x2)) && !distance.IsVerticalLine()))
    {
        return true;
    }

    float y1 = distance.pointA.y;
    float y2 = distance.pointB.y;
    if (distance.IsVerticalLine() && (((intersection.y > y1 || SameFloat(intersection.y, y1)) && (intersection.y < y2 || SameFloat(intersection.y, y2))) || (intersection.y < y1 && intersection.y > y2)))
    {
        return true;
    }

    return false;
}

bool IsPointOnRay(Vector2 &intersection, Ray2 &ray)
{
    if ((ray.pointA.x < ray.pointB.x && (intersection.x > ray.pointA.x || SameFloat(intersection.x, ray.pointA.x))) || (ray.pointA.x > ray.pointB.x && (intersection.x < ray.pointA.x || SameFloat(intersection.x, ray.pointA.x))))
    {
        return true;
    }
    
    if (ray.IsVerticalLine() && (((ray.pointA.y < ray.pointB.y || SameFloat(ray.pointA.y, ray.pointB.y)) && (intersection.y > ray.pointA.y || SameFloat(intersection.y, ray.pointA.y))) || (ray.pointA.y > ray.pointB.y && (intersection.y < ray.pointA.y || SameFloat(intersection.y, ray.pointA.y)))))
    {
        return true;
    }

    return false;
}

void GetStraightLineCircleIntersections(std::vector<Vector2> &intersections, StraightLine &straightLine, Circle &circle)
{
    for (auto intersection : GetLineCircleIntersection(straightLine.pointA, straightLine.pointB, circle.center, circle.radius))
    {
        intersections.push_back(intersection);
    }
}

void GetRayCircleIntersections(std::vector<Vector2> &intersections, Ray2 &ray, Circle &circle)
{
    for (auto intersection : GetLineCircleIntersection(ray.pointA, ray.pointB, circle.center, circle.radius))
    {
        if (IsPointOnRay(intersection, ray))
        {
            intersections.push_back(intersection);
        }
    }
}

void GetCircleCircleIntersections(std::vector<Vector2> &intersections, Circle &circle1, Circle &circle2)
{
    for (auto intersection : GetCircleCircleIntersection(circle1.center, circle1.radius, circle2.center, circle2.radius))
    {
        intersections.push_back(intersection);
    }
}

void GetDistanceDistanceIntersections(std::vector<Vector2> &intersections, Distance &distance1, Distance &distance2)
{
    for (auto intersection : GetLineLineIntersection(distance1.pointA, distance1.pointB, distance2.pointA, distance2.pointB))
    {
        if (IsPointOnDistance(intersection, distance1) && IsPointOnDistance(intersection, distance2))
        {
            intersections.push_back(intersection);
        }
    }
}

void GetStraightLineDistanceIntersections(std::vector<Vector2> &intersections, StraightLine &straightLine, Distance &distance)
{
    for (auto intersection : GetLineLineIntersection(straightLine.pointA, straightLine.pointB, distance.pointA, distance.pointB))
    {
        if (IsPointOnDistance(intersection, distance))
        {
            intersections.push_back(intersection);
        }
    }
}

void GetRayDistanceIntersections(std::vector<Vector2> &intersections, Ray2 &ray, Distance &distance)
{
    for (auto intersection : GetLineLineIntersection(ray.pointA, ray.pointB, distance.pointA, distance.pointB))
    {
        if (IsPointOnDistance(intersection, distance) && IsPointOnRay(intersection, ray))
        {
            intersections.push_back(intersection);
        }
    }
}

void GetDistanceCircleIntersections(std::vector<Vector2> &intersections, Distance &distance, Circle &circle)
{
    for (auto intersection : GetLineCircleIntersection(distance.pointA, distance.pointB, circle.center, circle.radius))
    {
        if (IsPointOnDistance(intersection, distance))
        {
            intersections.push_back(intersection);
        }
    }
}

void GetStraightLineRayIntersections(std::vector<Vector2> &intersections, StraightLine &straightLine, Ray2 &ray)
{
    for (auto intersection : GetLineLineIntersection(straightLine.pointA, straightLine.pointB, ray.pointA, ray.pointB))
    {
        if (IsPointOnRay(intersection, ray))
        {
            intersections.push_back(intersection);
        }
    }
}

void GetRayRayIntersections(std::vector<Vector2> &intersections, Ray2 &ray1, Ray2 &ray2)
{
    for (auto intersection : GetLineLineIntersection(ray1.pointA, ray1.pointB, ray2.pointA, ray2.pointB))
    {
        if (IsPointOnRay(intersection, ray1) && IsPointOnRay(intersection, ray2))
        {
            intersections.push_back(intersection);
        }
    }
}

void GetStraightLineStraightLinieIntersections(std::vector<Vector2> &intersections, StraightLine &straightLine1, StraightLine &straightLine2)
{
    for (auto intersection : GetLineLineIntersection(straightLine1.pointA, straightLine1.pointB, straightLine2.pointA, straightLine2.pointB))
    {
        intersections.push_back(intersection);
    }
}
