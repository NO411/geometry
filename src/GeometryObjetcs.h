#pragma once

#include "raylib.h"
#include <string>
#include <vector>

enum GeometryObject
{
	CIRCLE,
	DISTANCE,
	RAY,
	STRAIGHTLINE,
	POINT
};

class GeometryBoard;

class Vec2
{
public:
	Vec2(long double x, long double y);
	Vec2(Vector2 &v);
	Vec2(Vector2 v);
	Vec2();
	long double x;
	long double y;

	bool operator==(Vec2 &v);
	bool operator!=(Vec2 &v);
	void operator+=(Vec2 v);
	Vec2 operator-(Vec2 &v);
	Vec2 operator+(Vec2 &v);

	Vector2 ToRaylibVec();
	void Render(Camera2D &camera, bool renderAsMovingPoint);
};

class GemObj
{
public:
	static const Color renderColor;
	static const int renderThickness;
	static const long double LE_factor;

	void DrawLineExSmooth(Vec2 &startPos, Vec2 &endPos);
	void DrawRingSmooth(Vec2 &center, long double radius, long double startAngle, long double endAngle);

	std::vector<int> intersectionIDs;
};

class LengthMeasurement
{
public:
	void EnableLength(Vec2 &pos, Font *font);
	void DisableLength();
	bool IsLengthEnabled();

	void RenderLength(Camera2D &camera);
	void SetLength(long double newLength);

	bool showLength = false;
	Vec2 lengthPos;
	std::string length;
	Vector2 measure;

	Font *boardFont;
	static const int fontSize;
};

class Sector
{
public:
	long double startAngle;
	long double endAngle;
	Vec2 startAnglePoint;
	Vec2 endAnglePoint;

	Sector();

	Sector(long double startAngle, long double endAngle, Vec2 &startAnglePoint, Vec2 &endAnglePoint);

	bool operator<(const Sector &sector) const;
	long double GetAngle();
};

class Circle : public GemObj, public LengthMeasurement
{
public:
	Circle(Vec2 &center, Vec2 &pointOnCircle);

	void Render(Camera2D &camera, bool renderMovingPoints);
	void MovePos(Vec2 &originalPoint, Vec2 &moveToPoint);
	void MoveRadius(Vec2 &originalPoint, Vec2 &moveToPoint);

	Vec2 center;
	Vec2 pointOnCircle;
	long double radius;
	void UpdateLength();

	std::vector<Sector> sectors;
	bool EraseSector(Vec2 &firstCircleEraserPos, Vec2 &currentPos);
	bool CompletelyErased();

private:
	void UpdateRadius();
};

class Line : public GemObj
{
public:
	Vec2 pointA, pointB;
	bool IsLinePoint();

	bool IsVerticalLine();
	void RenderMovingPoints(Camera2D &camera, bool renderMovingPoints);

	void MovePosGeneral(Vec2 &originalPoint, Vec2 &moveToPoint);
	void MovePointGeneral(Vec2 &originalPoint, Vec2 &moveToPoint, int pointAorB);
};

class Distance : public Line, public LengthMeasurement
{
public:
	Distance(Vec2 &pointA_, Vec2 &pointB_);

	void Render(Camera2D &camera, bool renderMovingPoints);

	bool IsPointOnLine(Vec2 &point);
	void UpdateLength();

	void MovePos(Vec2 &originalPoint, Vec2 &moveToPoint);
	void MovePoint(Vec2 &originalPoint, Vec2 &moveToPoint, int pointAorB);
	void UpdateLengthPos(Vec2 &startPoint, Vec2 &moveToPoint, long double lengthPointPosition_);

	void SetLengthPointPosition();

	/// from pointA to pointB (0.0 - 1.0)
	long double lengthPointPosition[2];
};

class Ray2 : public Line
{
public:
	Ray2(Vec2 &pointA_, Vec2 &pointB_, Camera2D &camera);

	void Render(Camera2D &camera, bool renderMovingPoints);
	void UpdateDrawPoint(Camera2D &camera);

	bool IsPointOnLine(Vec2 &point);

	void MovePos(Vec2 &originalPoint, Vec2 &moveToPoint, Camera2D &camera);
	void MovePoint(Vec2 &originalPoint, Vec2 &moveToPoint, int pointAorB, Camera2D &camera);

private:
	Vec2 drawPoint;
};

class StraightLine : public Line
{
public:
	StraightLine(Vec2 &pointA_, Vec2 &pointB_, Camera2D &camera);

	void Render(Camera2D &camera, bool renderMovingPoints);
	void UpdateDrawPoints(Camera2D &camera);

	bool IsPointOnLine(Vec2 &point);

	void MovePos(Vec2 &originalPoint, Vec2 &moveToPoint, Camera2D &camera);
	void MovePoint(Vec2 &originalPoint, Vec2 &moveToPoint, int pointAorB, Camera2D &camera);

private:
	Vec2 firstDrawPoint, secondDrawPoint;
};

class Point : public GemObj
{
public:
	Point();
	Point(Vec2 &pos, GeometryBoard *board);

	void Render(Camera2D &camera);
	void Render(Camera2D &camera, Font &font);

	Vec2 &GetPos();
	std::string &GetLetter();
	void SetPos(Vec2 &pos);

private:
	void SetPointLetter(GeometryBoard *board);

	Vec2 point;
	std::string letter;
	std::string letterNumber;

	static const std::string pointChars;
};
