#ifndef DRAW_H
#define DRAW_H

#include "geometry_objects.h"
#include "raylib.h"

void DrawPointObj(Vector2 *point);
void DrawCircleObj(Circle *circle);
void DrawDistanceObj(Line *line);
void DrawDistanceLengths(Line *line, Font *font);
void DrawRayObj(Line *line);
void DrawStraightLineObj(Line *line);

#endif
