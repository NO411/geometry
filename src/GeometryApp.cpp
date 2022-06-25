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

	board = {};
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

	switch (appState)
	{
	case GEOMETRY_BOARD:
		board.Render();
		break;
	
	default:
		break;
	}

	EndDrawing();
}

void GeometryApp::Update()
{
	
}
