#include "GeometryApp.h"
#include "raylib.h"

GeometryApp::GeometryApp(int width, int height, int fps, std::string title)
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	SetTraceLogLevel(LOG_NONE);

	InitWindow(width, height, title.c_str());

    SetTargetFPS(fps);
	SetExitKey(0);
	SetWindowIcon(LoadImage("resources/icon.png"));
	ChangeDirectory(GetApplicationDirectory());

    font = LoadFont("resources/anonymous_pro_bold.ttf");
	SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);

	camera = {0};
    camera.target = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
	camera.offset = camera.target;
	camera.zoom = 1;
}

GeometryApp::~GeometryApp() noexcept
{
    UnloadFont(font);
    CloseWindow();
}

void GeometryApp::Run()
{
    while (!GeometryAppShouldClose())
	{
		Tick();
	}
}

bool GeometryApp::GeometryAppShouldClose() const
{
    return WindowShouldClose();
}

void GeometryApp::Tick()
{
    Update();
    BeginDrawing();
    BeginMode2D(camera);
    Draw();
    EndMode2D();
    EndDrawing();
}

void GeometryApp::Draw()
{
    
}

void GeometryApp::Update()
{
    
}
