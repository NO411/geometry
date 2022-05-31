#include "app/main_logic.h"
#include "math/math_misc.h"
#include "raylib.h"
#include <iostream>

void Init()
{
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	SetTraceLogLevel(LOG_NONE);
	InitWindow(900, 600, "geometry");
	SetTargetFPS(60);
	SetExitKey(0);
	SetWindowIcon(LoadImage("resources/icon.png"));
	ChangeDirectory(GetApplicationDirectory());
}

int main()
{
	Init();

	Font font = LoadFont("resources/anonymous_pro_bold.ttf");
	SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);

	camera.target = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
	camera.offset = camera.target;
	camera.zoom = 1;

	while (!WindowShouldClose())
	{
		Update();

		BeginDrawing();
		BeginMode2D(camera);

		Draw(&font);

		EndMode2D();
		EndDrawing();
	}
	UnloadFont(font);
	CloseWindow();
	return 0;
}
