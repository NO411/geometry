#include "GeometryObjetcs.h"
#include "GeometryBoard.h"
#include "raylib.h"
#include "math/MathMisc.h"
#include <string>
#include <algorithm>
#include <vector>
#include <cmath>

const Color GemObj::renderColor = GRAY;
const int GemObj::renderThickness = 2;
const long double GemObj::LE_factor = 0.025;

const std::string Point::pointChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const int LengthMeasurement::fontSize = 12;

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

bool Vec2::operator==(Vec2 &v)
{
	return (SameDouble(v.x, x) && SameDouble(v.y, y));
}

bool Vec2::operator!=(Vec2 &v)
{
	return !(v == *this);
}

void Vec2::operator+=(Vec2 v)
{
	x += v.x;
	y += v.y;
}

Vec2 Vec2::operator-(Vec2 &v)
{
	return {x - v.x, y - v.y};
}

Vec2 Vec2::operator+(Vec2 &v)
{
	return {x + v.x, y + v.y};
}

Vector2 Vec2::ToRaylibVec()
{
	return {(float)x, (float)y};
}

void Vec2::Render(Camera2D &camera, bool renderAsMovingPoint)
{
	Vector2 screenPos = GetWorldToScreen2D(ToRaylibVec(), camera);
	Color clr1 = BLUE;
	Color clr2 = DARKBLUE;

	if (renderAsMovingPoint)
	{
		clr1 = ORANGE;
		clr2 = RED;
	}

	DrawCircleSector(screenPos, 3, 0, 360, 30, clr1);
	DrawRing(screenPos, 3, 4, 0, 360, 30, clr2);
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

void GemObj::DrawRingSmooth(Vec2 &center, long double radius, long double startAngle, long double endAngle)
{
	DrawRing(center.ToRaylibVec(), radius - renderThickness / 3, radius + renderThickness / 3, startAngle, endAngle, 500, renderColor);
	Color cl = renderColor;
	cl.a = 200;
	DrawRing(center.ToRaylibVec(), radius - renderThickness / 2, radius + renderThickness / 2, startAngle, endAngle, 500, cl);
	cl.a = 100;
	DrawRing(center.ToRaylibVec(), radius - renderThickness, radius + renderThickness, startAngle, endAngle, 500, cl);
}

bool Line::IsVerticalLine()
{
	return SameDouble(pointA.x, pointB.x);
}

void Line::RenderMovingPoints(Camera2D &camera, bool renderMovingPoints)
{
	if (renderMovingPoints)
	{
		pointA.Render(camera, true);
		pointB.Render(camera, true);
	}
}

void Line::MovePosGeneral(Vec2 &originalPoint, Vec2 &moveToPoint)
{
	Vec2 offset = moveToPoint - originalPoint;
	pointA += offset;
	pointB += offset;
}

void Line::MovePointGeneral(Vec2 &originalPoint, Vec2 &moveToPoint, int pointAorB)
{
	Vec2 offset = moveToPoint - originalPoint;

	if (pointAorB == 1)
	{
		pointA += offset;
	}
	else
	{
		pointB += offset;
	}
}

bool Line::IsLinePoint()
{
	return (pointA == pointB);
}

Distance::Distance(Vec2 &pointA_, Vec2 &pointB_)
{
	pointA = pointA_;
	pointB = pointB_;
}

void Distance::Render(Camera2D &camera, bool renderMovingPoints)
{
	Vec2 startPos = {GetWorldToScreen2D(pointA.ToRaylibVec(), camera)};
	Vec2 endPos = {GetWorldToScreen2D(pointB.ToRaylibVec(), camera)};
	DrawLineExSmooth(startPos, endPos);

	RenderLength(camera);
	RenderMovingPoints(camera, renderMovingPoints);
}

void Distance::UpdateLength()
{
	SetLength(GetDistance(pointA, pointB) * LE_factor);
}

void Distance::MovePos(Vec2 &originalPoint, Vec2 &moveToPoint)
{
	MovePosGeneral(originalPoint, moveToPoint);
	Vec2 offset = moveToPoint - originalPoint;
	lengthPos += offset;
}

void Distance::UpdateLengthPos(Vec2 &startPoint, Vec2 &moveToPoint, long double lengthPointPosition_)
{
	LinearFunction f(startPoint, moveToPoint);

	if (f.vertical)
	{
		long double lengthY = startPoint.y - (startPoint.y - moveToPoint.y) * lengthPointPosition_;

		if (moveToPoint.x > startPoint.y)
		{
			startPoint.y + (moveToPoint.y - startPoint.y) * lengthPointPosition_;
		}

		lengthPos = {startPoint.x, lengthY};
		return;
	}

	long double lengthX = (moveToPoint.x - startPoint.x) * lengthPointPosition_ + startPoint.x;

	lengthPos = {lengthX, f.m * lengthX + f.n};
}

void Distance::MovePoint(Vec2 &originalPoint, Vec2 &moveToPoint, int pointAorB)
{
	MovePointGeneral(originalPoint, moveToPoint, pointAorB);

	if (!IsLengthEnabled())
	{
		return;
	}

	UpdateLength();

	if (pointAorB == 1)
	{
		UpdateLengthPos(pointB, moveToPoint, lengthPointPosition[pointAorB - 1]);
	}
	else
	{
		UpdateLengthPos(pointA, moveToPoint, lengthPointPosition[pointAorB - 1]);
	}
}

void Distance::SetLengthPointPosition()
{
	lengthPointPosition[0] = 1 - GetDistance(pointA, lengthPos) / GetDistance(pointA, pointB);
	lengthPointPosition[1] = 1 - lengthPointPosition[0];
}

void LengthMeasurement::RenderLength(Camera2D &camera)
{
	if (!IsLengthEnabled())
	{
		return;
	}
	Vector2 drawPos = GetWorldToScreen2D(lengthPos.ToRaylibVec(), camera);

	DrawTextEx(*boardFont, length.c_str(), drawPos, fontSize, 0, BLACK);
	DrawRectangleRec({drawPos.x - 2, drawPos.y - 2, measure.x + 4, measure.y + 4}, {200, 200, 200, 150});
}

void LengthMeasurement::SetLength(long double newLength)
{
	length = std::to_string(newLength) + " u"; // in length units
	measure = MeasureTextEx(*boardFont, length.c_str(), fontSize, 0);
}

void LengthMeasurement::EnableLength(Vec2 &pos, Font *font)
{
	lengthPos = pos;
	showLength = true;
	boardFont = font;
}

void LengthMeasurement::DisableLength()
{
	showLength = false;
}

bool LengthMeasurement::IsLengthEnabled()
{
	return showLength;
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

void StraightLine::Render(Camera2D &camera, bool renderMovingPoints)
{
	DrawLineExSmooth(firstDrawPoint, secondDrawPoint);
	RenderMovingPoints(camera, renderMovingPoints);
}

void StraightLine::MovePos(Vec2 &originalPoint, Vec2 &moveToPoint, Camera2D &camera)
{
	MovePosGeneral(originalPoint, moveToPoint);
	UpdateDrawPoints(camera);
}

void StraightLine::MovePoint(Vec2 &originalPoint, Vec2 &moveToPoint, int pointAorB, Camera2D &camera)
{
	MovePointGeneral(originalPoint, moveToPoint, pointAorB);
	UpdateDrawPoints(camera);
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

	if (pointA == pointB)
	{
		drawPoint = pointA;
	}
}

void Ray2::Render(Camera2D &camera, bool renderMovingPoints)
{
	Vec2 startPos = {GetWorldToScreen2D(pointA.ToRaylibVec(), camera)};
	DrawLineExSmooth(startPos, drawPoint);
	RenderMovingPoints(camera, renderMovingPoints);
}

void Ray2::MovePos(Vec2 &originalPoint, Vec2 &moveToPoint, Camera2D &camera)
{
	MovePosGeneral(originalPoint, moveToPoint);
	UpdateDrawPoint(camera);
}

void Ray2::MovePoint(Vec2 &originalPoint, Vec2 &moveToPoint, int pointAorB, Camera2D &camera)
{
	MovePointGeneral(originalPoint, moveToPoint, pointAorB);
	UpdateDrawPoint(camera);
}

Sector::Sector() {}
Sector::Sector(long double startAngle, long double endAngle, Vec2 &startAnglePoint, Vec2 &endAnglePoint) : startAngle(startAngle), endAngle(endAngle), startAnglePoint(startAnglePoint), endAnglePoint(endAnglePoint){};
bool Sector::operator<(const Sector &sector) const
{
	return (startAngle < sector.startAngle);
}

long double Sector::GetAngle()
{
	if (SameDouble(endAngle, startAngle))
	{
		return 360;
	}

	long double angle = (endAngle - startAngle);
	if (endAngle < startAngle)
	{
		return 360 - angle;
	}
	return angle;
}

Circle::Circle(Vec2 &center, Vec2 &pointOnCircle) : center(center), pointOnCircle(pointOnCircle)
{
	UpdateRadius();
}

void Circle::UpdateRadius()
{
	radius = GetDistance(center, pointOnCircle);
}

void Circle::Render(Camera2D &camera, bool renderMovingPoints)
{
	RenderLength(camera);
	if (renderMovingPoints)
	{
		pointOnCircle.Render(camera, true);
		center.Render(camera, true);
	}

	Vec2 center_ = {GetWorldToScreen2D(center.ToRaylibVec(), camera)};

	std::size_t vectorSize = sectors.size();
	if (vectorSize <= 0)
	{
		DrawRingSmooth(center_, radius * camera.zoom, 0, 360);
		return;
	}

	if (vectorSize == 1)
	{
		// the function DrawRingLines() switches start and end point when the end point is smaller than the start point
		float startAngle = sectors[0].endAngle;
		float endAngle = sectors[0].startAngle;
		if (startAngle > endAngle)
		{
			endAngle += 360;
		}
		DrawRingSmooth(center_, radius * camera.zoom, startAngle, endAngle);
		return;
	}

	for (std::size_t i = 0; i < vectorSize; ++i)
	{
		float startAngle = sectors[i].endAngle;
		// use the start angle of the following sector as the endAngle
		float endAngle;

		if (i == vectorSize - 1)
		{
			endAngle = sectors[0].startAngle;
		}
		else
		{
			endAngle = sectors[i + 1].startAngle;
		}

		if (startAngle > endAngle)
		{
			endAngle += 360;
		}

		DrawRingSmooth(center_, radius * camera.zoom, startAngle, endAngle);
	}
}

void Circle::UpdateLength()
{
	SetLength(2 * PI * radius * LE_factor);
}

void Circle::MovePos(Vec2 &originalPoint, Vec2 &moveToPoint)
{
	Vec2 offset = moveToPoint - originalPoint;
	pointOnCircle += (moveToPoint - originalPoint);
	lengthPos += offset;
	center += moveToPoint - originalPoint;
}

void Circle::MoveRadius(Vec2 &originalPoint, Vec2 &moveToPoint)
{
	Vec2 offset = moveToPoint - originalPoint;
	pointOnCircle += (moveToPoint - originalPoint);

	UpdateRadius();

	// update to make it possible to select them after radius moving
	for (auto &sector : sectors)
	{
		sector.startAnglePoint = GetCircleConnection(sector.startAnglePoint, *this);
		sector.endAnglePoint = GetCircleConnection(sector.endAnglePoint, *this);
	}

	if (!IsLengthEnabled())
	{
		return;
	}

	UpdateLength();
	if (moveToPoint != center)
	{
		lengthPos = GetCircleConnection(lengthPos, *this);
	}
}

bool Circle::EraseSector(Vec2 &firstCircleEraserPos, Vec2 &currentPos)
{
	Sector newSector = PointsToSector(center, firstCircleEraserPos, currentPos);

	bool eraseWholeCircle = false;
	sectors.erase(std::remove_if(sectors.begin(), sectors.end(),
		[&newSector, &eraseWholeCircle](Sector sector)
		{
			if (SameDouble(sector.endAngle, newSector.startAngle) && SameDouble(sector.startAngle, newSector.endAngle))
			{
				eraseWholeCircle = true;
				return true;
			}
			if (SameDouble(sector.startAngle, newSector.endAngle))
			{
				newSector = {newSector.startAngle, sector.endAngle, newSector.startAnglePoint, sector.endAnglePoint};
				return true;
			}
			if (SameDouble(sector.endAngle, newSector.startAngle))
			{
				newSector = {sector.startAngle, newSector.endAngle, sector.startAnglePoint, newSector.endAnglePoint};
				return true;
			}
			return SectorIncludesAngle(newSector, sector.startAngle, true) && SectorIncludesAngle(newSector, sector.endAngle, true) && !(SameDouble(newSector.startAngle, sector.endAngle) && SameDouble(newSector.endAngle, sector.startAngle));
		}),
	sectors.end());

	sectors.push_back(newSector);

	// bring sectors in right order to draw them correctly
	std::sort(sectors.begin(), sectors.end());

	return eraseWholeCircle;
}

bool Circle::CompletelyErased()
{
	long double sectorSum;

	for (auto &sector : sectors)
	{
		sectorSum += sector.GetAngle();
	}

	return SameDouble(360, sectorSum);
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
	point.Render(camera, false);
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
			letterQuantity[found]++;

			int n = 0;
			if (point.letterNumber != "")
			{
				n = std::stoi(point.letterNumber);
			}
			letterNumbers[found].push_back(n);
		}
	}

	auto minPos = std::min_element(letterQuantity.begin(), letterQuantity.end()) - letterQuantity.begin();
	letter = pointChars.substr(minPos, 1);

	int i = 0;
	while (std::count(letterNumbers[minPos].begin(), letterNumbers[minPos].end(), i))
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
	if (IsLinePoint())
	{
		return false;
	}

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
	if (IsLinePoint())
	{
		return false;
	}

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
	if (IsLinePoint())
	{
		return false;
	}

	return true;
}
