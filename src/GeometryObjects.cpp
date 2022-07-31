#include "GeometryObjetcs.h"
#include "GeometryBoard.h"
#include "raylib.h"
#include "math/MathMisc.h"
#include <string>
#include <algorithm>
#include <vector>

const Color GemObj::renderColor = GRAY;
const int GemObj::renderThickness = 2;
const std::string Point::pointChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

void GemObj::DrawLineExSmooth(Vector2 &startPos, Vector2 &endPos)
{
	DrawLineEx(startPos, endPos, renderThickness - 1, renderColor);
	Color cl = renderColor;
	cl.a = 200;
    DrawLineEx(startPos, endPos, renderThickness, cl);
	cl.a = 100;
    DrawLineEx(startPos, endPos, renderThickness + 1, cl);
}

void GemObj::DrawRingSmooth(Vector2 &center, float radius)
{
    DrawRing(center, radius - renderThickness / 3, radius + renderThickness / 3, 0, 360, 200, renderColor);
	Color cl = renderColor;
	cl.a = 200;
	DrawRing(center, radius - renderThickness / 2, radius + renderThickness / 2, 0, 360, 200, cl);
	cl.a = 100;
	DrawRing(center, radius - renderThickness, radius + renderThickness, 0, 360, 200, cl);
}

bool Line::IsVerticalLine()
{
	return SameFloat(pointA.x, pointB.x);
}

Distance::Distance(Vector2 &pointA_, Vector2 &pointB_)
{
    pointA = pointA_;
    pointB = pointB_;
}

void Distance::Render(Camera2D &camera)
{
	Vector2 startPos = GetWorldToScreen2D(pointA, camera);
	Vector2 endPos = GetWorldToScreen2D(pointB, camera);
    DrawLineExSmooth(startPos, endPos);
}

StraightLine::StraightLine(Vector2 &pointA_, Vector2 &pointB_, Camera2D &camera)
{
    pointA = pointA_;
    pointB = pointB_;
    UpdateDrawPoints(camera);
}

void StraightLine::UpdateDrawPoints(Camera2D &camera)
{
    LinearFunction f(pointA, pointB);
    firstDrawPoint = CalculateConnectionPoint(pointA, pointB, f.m, f.n, camera);
    secondDrawPoint = CalculateConnectionPoint(pointB, pointA, f.m, f.n, camera);
}

void StraightLine::Render()
{
    DrawLineExSmooth(firstDrawPoint, secondDrawPoint);
}

Ray2::Ray2(Vector2 &pointA_, Vector2 &pointB_, Camera2D &camera)
{
    pointA = pointA_;
    pointB = pointB_;
    UpdateDrawPoint(camera);
}

void Ray2::UpdateDrawPoint(Camera2D &camera)
{
    LinearFunction f(pointA, pointB);
    drawPoint = CalculateConnectionPoint(pointA, pointB, f.m, f.n, camera);
}

void Ray2::Render(Camera2D &camera)
{
	Vector2 startPos = GetWorldToScreen2D(pointA, camera);
    DrawLineExSmooth(startPos, drawPoint);
}

Circle::Circle(Vector2 &center, float radius) : center(center), radius(radius)
{
}

void Circle::Render(Camera2D &camera)
{
	Vector2 center_ = GetWorldToScreen2D(center, camera);
    DrawRingSmooth(center_, radius * camera.zoom);
}

Point::Point(Vector2 &pos, GeometryBoard *board): point(pos)
{
    SetPointLetter(board);
}

Point::Point()
{
}

Vector2& Point::GetPos()
{
    return point;
}

void Point::SetPos(Vector2 &pos)
{
    point = pos;
}

void Point::Render(Camera2D &camera)
{
    Vector2 screenPos = GetWorldToScreen2D(point, camera);
    DrawCircleSector(screenPos, 3, 0, 360, 30, BLUE);
	DrawRing(screenPos, 3, 4, 0, 360, 30, DARKBLUE);
}

void Point::Render(Camera2D &camera, Font &font)
{
    Render(camera);

    Vector2 screenPos = GetWorldToScreen2D(point, camera);
    DrawTextEx(font, letter.c_str(), {screenPos.x + 4, screenPos.y + 4}, 12, 0, BLACK);
	DrawTextEx(font, letterNumber.c_str(), {screenPos.x + 12, screenPos.y + 8}, 8, 0, GRAY);
}

void Point::SetPointLetter(GeometryBoard *board)
{
    for (std::size_t i = 0; i < pointChars.size(); i++)
	{
		std::string _letter = pointChars.substr(i, 1);
		if (!board->PointLetterExists(_letter))
		{
			letter = _letter;
			letterNumber = "";
			return;
		}
	}

	std::vector<int> letterQuantity(pointChars.size());
	std::vector<std::vector<int>> letterNumbers(pointChars.size());

	for (auto &point : board->points)
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

std::string& Point::GetLetter()
{
    return letter;
}

bool Distance::IsPointOnLine(Vector2 &point)
{
	float x1 = pointA.x;
    float x2 = pointB.x;
    if ((x1 < x2 && point.x > x1 && point.x < x2) || (x1 > x2 && point.x < x1 && point.x > x2) || ((SameFloat(point.x, x1) || SameFloat(point.x, x2)) && !IsVerticalLine()))
    {
        return true;
    }

    float y1 = pointA.y;
    float y2 = pointB.y;
    if (IsVerticalLine() && (((point.y > y1 || SameFloat(point.y, y1)) && (point.y < y2 || SameFloat(point.y, y2))) || (point.y < y1 && point.y > y2)))
    {
        return true;
    }

    return false;
}

bool Ray2::IsPointOnLine(Vector2 &point)
{
	if ((pointA.x < pointB.x && (point.x > pointA.x || SameFloat(point.x, pointA.x))) || (pointA.x > pointB.x && (point.x < pointA.x || SameFloat(point.x, pointA.x))))
    {
        return true;
    }
    
    if (IsVerticalLine() && (((pointA.y < pointB.y || SameFloat(pointA.y, pointB.y)) && (point.y > pointA.y || SameFloat(point.y, pointA.y))) || (pointA.y > pointB.y && (point.y < pointA.y || SameFloat(point.y, pointA.y)))))
    {
        return true;
    }

    return false;
}

bool StraightLine::IsPointOnLine(Vector2 &point)
{
    return true;
}
