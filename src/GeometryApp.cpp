#include "GeometryApp.h"
#include "raylib.h"
#include <iostream>

HelpButton::HelpButton() {
	radius = 20;
	offset = 5;
	ring = radius / 4;
	fontSize = radius * 2;
	center = {GetScreenWidth() - (radius + ring + offset), radius + ring + offset};
	textCenter = {center.x - MeasureText("?", fontSize) / 2, center.y - fontSize / 2};
}

HelpButton::~HelpButton() {}

bool HelpButton::Selected()
{
	return CheckCollisionPointCircle(GetMousePosition(), center, radius + ring);
}

void HelpButton::Update()
{
	color = BLUE;
	center.x = GetScreenWidth() - (radius + ring + offset);
	textCenter.x = center.x - MeasureText("?", fontSize) / 2;

	if (Selected())
	{
		color = {0, 140, 240, 255};
	}
}

void HelpButton::Render(Font &font)
{
	DrawCircleSector(center, radius, 0, 360, 200, color);
	DrawRing(center, radius, radius + ring, 0, 360, 200, DARKBLUE);
	DrawTextCodepoint(font, 63, textCenter, fontSize, DARKBLUE);
}

GeometryApp::GeometryApp(int width, int height, int fps, std::string title)
{
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	SetTraceLogLevel(LOG_NONE);

	InitWindow(width, height, title.c_str());

	SetTargetFPS(fps);
	SetExitKey(0);

	Image iconImage = LoadImage("resources/icon.png");
	SetWindowIcon(iconImage);
	UnloadImage(iconImage);

	appState = GEOMETRY_BOARD;
	showHelpButton = true;

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

	Render();

	EndDrawing();
}

void GeometryApp::Render()
{
	if (showHelpButton)
	{
		button.Render(font);
	}
}

void GeometryApp::Update()
{
	if (showHelpButton)
	{
		button.Update();
	}
}
