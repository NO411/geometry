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

Vec2::Vec2(long double x, long double y) : x(x), y(y)
{
}

Vec2::Vec2(Vector2 &v) : x(v.x), y(v.y)
{
}

Vec2::Vec2(Vector2 v) : x(v.x), y(v.y)
{
}

Vec2::Vec2()
{
	x = 0;
	y = 0;
}

Vector2 Vec2::ToRaylibVec()
{
	return {(float)x, (float)y};
}

void GemObj::DrawLineExSmooth(Vec2 &startPos, Vec2 &endPos)
{
	DrawLineEx(startPos.ToRaylibVec(), endPos.ToRaylibVec(), renderThickness - 1, renderColor);
	Color cl = renderColor;
	cl.a = 200;
	DrawLineEx(startPos.ToRaylibVec(), endPos.ToRaylibVec(), renderThickness, cl);
	cl.a = 100;
	DrawLineEx(startPos.ToRaylibVec(), endPos.ToRaylibVec(), renderThickness + 1, cl);
}

void GemObj::DrawRingSmooth(Vec2 &center, long double radius)
{
	DrawRing(center.ToRaylibVec(), radius - renderThickness / 3, radius + renderThickness / 3, 0, 360, 200, renderColor);
	Color cl = renderColor;
	cl.a = 200;
	DrawRing(center.ToRaylibVec(), radius - renderThickness / 2, radius + renderThickness / 2, 0, 360, 200, cl);
	cl.a = 100;
	DrawRing(center.ToRaylibVec(), radius - renderThickness, radius + renderThickness, 0, 360, 200, cl);
}

bool Line::IsVerticalLine()
{
	return SameDouble(pointA.x, pointB.x);
}

Distance::Distance(Vec2 &pointA_, Vec2 &pointB_)
{
	pointA = pointA_;
	pointB = pointB_;
}

void Distance::Render(Camera2D &camera)
{
	Vec2 startPos = {GetWorldToScreen2D(pointA.ToRaylibVec(), camera)};
	Vec2 endPos = {GetWorldToScreen2D(pointB.ToRaylibVec(), camera)};
	DrawLineExSmooth(startPos, endPos);
}

StraightLine::StraightLine(Vec2 &pointA_, Vec2 &pointB_, Camera2D &camera)
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

Ray2::Ray2(Vec2 &pointA_, Vec2 &pointB_, Camera2D &camera)
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
	Vec2 startPos = {GetWorldToScreen2D(pointA.ToRaylibVec(), camera)};
	DrawLineExSmooth(startPos, drawPoint);
}

Circle::Circle(Vec2 &center, long double radius) : center(center), radius(radius)
{
}

void Circle::Render(Camera2D &camera)
{
	Vec2 center_ = {GetWorldToScreen2D(center.ToRaylibVec(), camera)};
	DrawRingSmooth(center_, radius * camera.zoom);
}

Point::Point(Vec2 &pos, GeometryBoard *board) : point(pos)
{
	SetPointLetter(board);
}

Point::Point()
{
}

Vec2 &Point::GetPos()
{
	return point;
}

void Point::SetPos(Vec2 &pos)
{
	point = pos;
}

void Point::Render(Camera2D &camera)
{
	Vector2 screenPos = GetWorldToScreen2D(point.ToRaylibVec(), camera);
	DrawCircleSector(screenPos, 3, 0, 360, 30, BLUE);
	DrawRing(screenPos, 3, 4, 0, 360, 30, DARKBLUE);
}

void Point::Render(Camera2D &camera, Font &font)
{
	Render(camera);

	Vector2 screenPos = GetWorldToScreen2D(point.ToRaylibVec(), camera);
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

std::string &Point::GetLetter()
{
	return letter;
}

bool Distance::IsPointOnLine(Vec2 &point)
{
	long double x1 = pointA.x;
	long double x2 = pointB.x;
	if ((x1 < x2 && point.x > x1 && point.x < x2) || (x1 > x2 && point.x < x1 && point.x > x2) || ((SameDouble(point.x, x1) || SameDouble(point.x, x2)) && !IsVerticalLine()))
	{
		return true;
	}

	long double y1 = pointA.y;
	long double y2 = pointB.y;
	if (IsVerticalLine() && (((point.y > y1 || SameDouble(point.y, y1)) && (point.y < y2 || SameDouble(point.y, y2))) || (point.y < y1 && point.y > y2)))
	{
		return true;
	}

	return false;
}

bool Ray2::IsPointOnLine(Vec2 &point)
{
	if ((pointA.x < pointB.x && (point.x > pointA.x || SameDouble(point.x, pointA.x))) || (pointA.x > pointB.x && (point.x < pointA.x || SameDouble(point.x, pointA.x))))
	{
		return true;
	}

	if (IsVerticalLine() && (((pointA.y < pointB.y || SameDouble(pointA.y, pointB.y)) && (point.y > pointA.y || SameDouble(point.y, pointA.y))) || (pointA.y > pointB.y && (point.y < pointA.y || SameDouble(point.y, pointA.y)))))
	{
		return true;
	}

	return false;
}

bool StraightLine::IsPointOnLine(Vec2 &point)
{
	return true;
}
