#include "draw.h"
#include "geometry_objects.h"
#include "main_logic.h"
#include "../math/math_misc.h"
#include "raylib.h"

void DrawPointObj(Vector2 *point)
{
	DrawCircleSector(*point, 3, 0, 360, 30, BLUE);
	DrawRing(*point, 3, 4, 0, 360, 30, DARKBLUE);
}

void DrawCircleObj(Circle *circle)
{
	std::size_t vectorSize = circle->sectors.size();
	if (vectorSize <= 0)
	{
		DrawRingLines(circle->center, circle->radius, circle->radius, 0, 360, 2 * circle->radius, LIGHTGRAY);
		return;
	}

	if (vectorSize == 1)
	{
		// the function DrawRingLines() switches start and end point when the end point is smaller than the start point
		float startAngle = circle->sectors.at(0).endAngle;
		float endAngle = circle->sectors.at(0).startAngle;
		if (startAngle > endAngle)
		{
			endAngle += 360;
		}
		DrawRingLines(circle->center, circle->radius, circle->radius, startAngle, endAngle, 2 * circle->radius, LIGHTGRAY);
		return;
	}

	for (std::size_t i = 0; i < vectorSize; ++i)
	{
		float startAngle = circle->sectors.at(i).endAngle;
		// use the start angle of the following sector as the endAngle
		float endAngle;

		if (i == vectorSize - 1)
		{
			endAngle = circle->sectors.at(0).startAngle;
		}
		else
		{
			endAngle = circle->sectors.at(i + 1).startAngle;
		}

		if (startAngle > endAngle)
		{
			endAngle += 360;
		}

		DrawRingLines(circle->center, circle->radius, circle->radius, startAngle, endAngle, 2 * circle->radius, LIGHTGRAY);
	}
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
	if (GetDistance(&line->pointA, &line->pointB) < 1)
	{
		return;
	}

	DrawLineEx(line->pointA, line->secondConnectionPoint, 2, LIGHTGRAY);
}

void DrawStraightLineObj(Line *line)
{
	DrawLineEx(line->firstConnectionPoint, line->secondConnectionPoint, 2, LIGHTGRAY);
}

void DrawCurrentCircleSector()
{
	Circle *circle = &circles[currentObjPos];
	Color sectorColor = LIGHTGRAY;

	if (IsPointOnCircle(&currentPoint, circle))
	{
		sectorColor = GREEN;
	}

	sectorColor.a = 100;
	float startAngle = CirclePointToAngle(&circle->center, &firstCircleEraserPoint);
	float endAngle = CirclePointToAngle(&circle->center, &currentPoint);

	if (startAngle > endAngle)
	{
		DrawCircleSector(circle->center, circle->radius, startAngle, 360, 2 * circle->radius, sectorColor);
		DrawCircleSector(circle->center, circle->radius, 0, endAngle, 2 * circle->radius, sectorColor);
		return;
	}

	DrawCircleSector(circle->center, circle->radius, startAngle, endAngle, 2 * circle->radius, sectorColor);
}
