#include "GeometryObjetcs.h"
#include "raylib.h"

Distance::Distance(Vector2 startPos, Vector2 endPos): startPos(startPos), endPos(endPos)
{
}

void Distance::Render(Camera2D &camera)
{
    DrawLineEx(GetWorldToScreen2D(startPos, camera), GetWorldToScreen2D(endPos, camera), 2, LIGHTGRAY);
}