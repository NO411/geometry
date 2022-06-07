#ifndef MAIN_LOGIC_H
#define MAIN_LOGIC_H

#include "geometry_objects.h"
#include "raylib.h"

#include <vector>

enum EditMode
{
	DRAWCIRCLE,
	DRAWDISTANCE,
	DRAWRAY,
	DRAWSTRAIGHTLINE,
	DRAWPOINT,
	ERASER,
	DISTANCEMEASUREMENT,
	ANGLEMEASUREMENT,
	DISTANCEMEASUREMENTERASER,
	ANGLEMEASUREMENTERASER,
	CIRCLEERASER,
};

extern bool firstPointed;
extern const float movementSpeed;
extern const int connection_distance;
extern int editMode;

extern std::vector<Circle> circles;
extern std::vector<Line> distances;
extern std::vector<Line> rays;
extern std::vector<Line> straightLines;
extern std::vector<Point> points;

extern Point firstPoint;
extern Line currentLine;
extern Circle currentCircle;
extern Vector2 currentPoint;
extern Vector2 firstEraserPoint;
extern std::size_t currentObjPos;

void SetDrawObj();
std::tuple<int, std::size_t> *UpdateCurrentPoint();
void UpdateAllIntersections();
void EraseObj(std::tuple<int, std::size_t> *objTuple);
void Edit();
void InterruptDrawing();
void Move();
void InputHandler();
void CheckResized();
void Update();
void DrawDrawingObj(Font *font);
void Draw(Font *font);

#endif
