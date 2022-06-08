#include "geometry_objects.h"
#include "../app/main_logic.h"
#include "../math/math_misc.h"
#include "../math/intersections.h"
#include "raylib.h"

#include <algorithm>
#include <vector>

float scaling = 0.025;
const std::string pointChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

Sector::Sector() {}
Sector::~Sector() {}
Sector::Sector(float startAngle, float endAngle, Vector2 startAnglePoint, Vector2 endAnglePoint) : startAngle(startAngle), endAngle(endAngle), startAnglePoint(startAnglePoint), endAnglePoint(endAnglePoint){};
bool Sector::operator<(const Sector &sector) const
{
	return (startAngle < sector.startAngle);
}

GeometryObj::GeometryObj() {}
GeometryObj::~GeometryObj() {}
void GeometryObj::CheckIntersections(Vector2 *vec1, Vector2 *vec2, float r)
{
	switch (objectNumber)
	{
	case CIRCLE:
	{
		for (auto &circle : circles)
		{
			FindCircleCircleIntersections(vec1, &circle.center, r, circle.radius);
		}
		for (auto &distance : distances)
		{
			FindCircleLineIntersections(vec1, r, &distance.pointA, &distance.pointB);
		}
		for (auto &ray : rays)
		{
			FindCircleLineIntersections(vec1, r, &ray.pointA, &ray.secondConnectionPoint);
		}
		for (auto &straightLine : straightLines)
		{
			FindCircleLineIntersections(vec1, r, &straightLine.firstConnectionPoint, &straightLine.secondConnectionPoint);
		}
	}
	break;
	case POINT:
		break;
	default:
	{
		for (auto &circle : circles)
		{
			FindCircleLineIntersections(&circle.center, circle.radius, vec1, vec2);
		}
		for (auto &distance : distances)
		{
			FindLineLineIntersections(vec1, vec2, &distance.pointA, &distance.pointB);
		}
		for (auto &ray : rays)
		{
			FindLineLineIntersections(vec1, vec2, &ray.pointA, &ray.secondConnectionPoint);
		}
		for (auto &straightLine : straightLines)
		{
			FindLineLineIntersections(vec1, vec2, &straightLine.firstConnectionPoint, &straightLine.secondConnectionPoint);
		}
	}
	break;
	}
}

bool PointLetterExists(std::string *letter)
{
	for (auto &point : points)
	{
		if (point.letter == *letter)
		{
			return true;
		}
	}
	return false;
}

Point::Point() {}
Point::Point(Vector2 point) : point(point)
{
	SetPointLetter();
}
Point::~Point() {}

void Point::SetPointLetter()
{
	for (std::size_t i = 0; i < pointChars.size(); i++)
	{
		std::string _letter = pointChars.substr(i, 1);
		if (!PointLetterExists(&_letter))
		{
			letter = _letter;
			letterNumber = "";
			return;
		}
	}

	std::vector<int> letterQuantity(pointChars.size());
	std::vector<std::vector<int>> letterNumbers(pointChars.size());

	for (auto &point : points)
	{
		auto found = pointChars.find(point.letter);
		if (found != std::string::npos)
		{
			letterQuantity.at(found)++;

			int n = 0;
			if (point.letterNumber != "")
			{
				n = std::stoi(point.letterNumber);
			}
			letterNumbers.at(found).push_back(n);
		}
	}

	auto minPos = std::min_element(letterQuantity.begin(), letterQuantity.end()) - letterQuantity.begin();
	letter = pointChars.substr(minPos, 1);

	int i = 0;
	while (std::count(letterNumbers.at(minPos).begin(), letterNumbers.at(minPos).end(), i))
	{
		i++;
	}

	if (i == 0)
	{
		letterNumber = "";
		return;
	}

	letterNumber = std::to_string(i);
}

Circle::Circle() {}
Circle::Circle(Vector2 center, float radius) : center(center), radius(radius)
{
	objectNumber = CIRCLE;
	UpdateIntersections();
}
Circle::~Circle() {}

void Circle::UpdateIntersections()
{
	CheckIntersections(&center, new Vector2{0, 0}, radius);
}

void Circle::EraseSector(Vector2 *firstCircleEraserPoint, Vector2 *currentPoint)
{
	Sector *newSector = PointsToSector(&center, firstCircleEraserPoint, currentPoint);

	sectors.erase(std::remove_if(sectors.begin(), sectors.end(),
			[&newSector](Sector sector)
			{
				return SectorIncludesAngle(newSector, sector.startAngle) && SectorIncludesAngle(newSector, sector.endAngle);
			}),
		sectors.end());

	sectors.push_back(*newSector);

	// bring sectors in right order to draw them correctly
	std::sort(sectors.begin(), sectors.end());
}

Line::Line() {}
Line::Line(Vector2 pointA, Vector2 pointB, int n) : pointA(pointA), pointB(pointB)
{
	objectNumber = n;

	UpdateConnectionPoints();
	UpdateIntersections();

	UpdateLength();
}
Line::~Line() {}

void Line::UpdateFirstConnectionPoint()
{
	LinearFunction *function = GetLinearFunction(&pointA, &pointB);
	firstConnectionPoint = *CalculateConnectionPoint(&pointB, &pointA, function->m, function->n);
}
void Line::UpdateSecondConnectionPoint()
{
	LinearFunction *function = GetLinearFunction(&pointA, &pointB);
	secondConnectionPoint = *CalculateConnectionPoint(&pointA, &pointB, function->m, function->n);
}
void Line::UpdateConnectionPoints()
{
	UpdateFirstConnectionPoint();
	UpdateSecondConnectionPoint();
}
void Line::UpdateIntersections()
{
	Vector2 v1, v2;
	switch (objectNumber)
	{
	case DISTANCE:
		v1 = pointA, v2 = pointB;
		break;
	case RAY:
		v1 = pointA, v2 = secondConnectionPoint;
		break;
	case STRAIGHTLINE:
		v1 = firstConnectionPoint, v2 = secondConnectionPoint;
		break;
	default:
		break;
	}
	CheckIntersections(&v1, &v2, 0);
}

void Line::UpdateLength()
{
	length = GetDistance(&pointA, &pointB) * scaling;
	middle.point = {(pointA.x + pointB.x) / 2, (pointA.y + pointB.y) / 2};
}
