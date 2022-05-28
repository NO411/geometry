#include "draw.h"
#include "geometry_objects.h"
#include "../math/math_misc.h"
#include "raylib.h"

void DrawPointObj(Vector2 *point)
{
	DrawCircleSector(*point, 3, 0, 360, 30, BLUE);
	DrawRing(*point, 3, 4, 0, 360, 30, DARKBLUE);
}

void DrawCircleObj(Circle *circle)
{
	DrawCircleSectorLines(circle->middle, circle->radius, 0, 360, 2 * circle->radius, LIGHTGRAY);
}

void DrawDistanceObj(Line *line)
{
	DrawLineEx(line->pointA, line->pointB, 2, LIGHTGRAY);
}

void DrawDistanceLengths(Line *line, Font *font)
{
	if (!line->showLength)
	{
		return;
	}
	int fontSize = 12;
	std::string len = std::to_string(line->length);
	Vector2 measure = MeasureTextEx(*font, len.c_str(), fontSize, 0);
	DrawTextEx(*font, len.c_str(), {line->middle.point.x, line->middle.point.y}, fontSize, 0, BLACK);
	DrawRectangleRec({line->middle.point.x - 2, line->middle.point.y - 2, measure.x + 4, measure.y + 4}, {200, 200, 200, 150});
}

void DrawRayObj(Line *line)
{
	if (SameVector2(&line->pointA, &line->secondConnectionPoint))
	{
		return;
	}

	DrawLineEx(line->pointA, line->secondConnectionPoint, 2, LIGHTGRAY);
}

void DrawStraightLineObj(Line *line)
{
	DrawLineEx(line->firstConnectionPoint, line->secondConnectionPoint, 2, LIGHTGRAY);
}
