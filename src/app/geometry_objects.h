#ifndef GEOMETRY_OBJECTS_H
#define GEOMETRY_OBJECTS_H

#include "raylib.h"

#include <string>
#include <vector>

extern float scaling;
extern const std::string pointChars;

enum GeometryObject
{
	CIRCLE,
	DISTANCE,
	RAY,
	STRAIGHTLINE,
	POINT
};

class Sector
{
public:
	float startAngle;
	float endAngle;
	Vector2 startAnglePoint;
	Vector2 endAnglePoint;

	Sector();
	~Sector();

	Sector(float startAngle, float endAngle, Vector2 startAnglePoint, Vector2 endAnglePoint);

	bool operator<(const Sector &sector) const;
};

class GeometryObj
{
public:
	int objectNumber;

	GeometryObj();
	~GeometryObj();

	void CheckIntersections(Vector2 *vec1, Vector2 *vec2, float r);
};

class Point
{
public:
	Vector2 point;
	std::string letter;
	std::string letterNumber;

	Point();
	Point(Vector2 point);
	~Point();

	void SetPointLetter();
};

class Circle : public GeometryObj
{
public:
	Vector2 center;
	float radius;

	std::vector<Sector> sectors;

	Circle();
	Circle(Vector2 center, float radius);
	~Circle();

	void UpdateIntersections();
	void EraseSector(Vector2 *firstCircleEraserPoint, Vector2 *currentPoint);
};

class Line : public GeometryObj
{
public:
	Vector2 pointA;
	Vector2 pointB;
	Vector2 firstConnectionPoint;
	Vector2 secondConnectionPoint;
	bool showLength = false;
	float length;
	Point middle;

	Line();
	Line(Vector2 pointA, Vector2 pointB, int n);
	~Line();

	void UpdateFirstConnectionPoint();
	void UpdateSecondConnectionPoint();
	void UpdateConnectionPoints();
	void UpdateIntersections();
	void UpdateLength();
};

#endif
