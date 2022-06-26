#include "GeometryApp.h"
#include "Settings.h"
#include "raylib.h"
#include <string>

HelpButton::HelpButton(GeometryApp &app) : app_(app)
{
	Update();
	text = "Help";
	fontSize = 16;
}

HelpButton::~HelpButton() {}

bool HelpButton::Selected()
{
	return CheckCollisionPointRec(GetMousePosition(), rectangle);
}

void HelpButton::Update()
{
	color = DARBBLUE3;

	rectangle = {GetScreenWidth() - 80.0f, 5, 75, 30};
	textPos = {rectangle.x + rectangle.width / 2 - MeasureText(text.c_str(), fontSize) / 2, rectangle.y + rectangle.height / 2 - fontSize / 2};

	bool hPressed = (IsKeyPressed(KEY_H) && !IsKeyDown(KEY_LEFT_CONTROL));

	if (Selected() || hPressed)
	{
		color = DARBBLUE2;

		if ((IsMouseButtonPressed(0) && app_.showHelpButton) || hPressed)
		{
			switch (app_.GetState())
			{
			case GEOMETRY_BOARD:
				app_.SetState(HELP);
				text = "Close";
				SetWindowTitle("Help");
				break;
			case HELP:
				app_.SetState(GEOMETRY_BOARD);
				SetWindowTitle("Geometry");
				text = "Help";
				break;

			default:
				break;
			}
		}
	}
}

void HelpButton::Render()
{
	DrawRectangleRounded(rectangle, 0.5, 10, color);
	DrawTextEx(app_.font, text.c_str(), textPos, fontSize, 0, WHITE);
}

HelpWindow::HelpWindow(GeometryApp &app) : app_(app)
{
	shortcuts = {
		{"`CTRL` + press `C`", "circle drawing mode"},
		{"`CTRL` + press `S`", "straight line drawing mode"}, 
		{"`CTRL` + press `D`", "distance drawing mode"}, 
		{"`CTRL` + press `R`", "ray drawing mode"},  
		{"`CTRL` + press `P`", "point drawing mode"},
		{"`CTRL` + press `E`", "enable eraser"},
		{"press `left mouse button`", "select point"},
		{"press `ESC`", "place / use"},
		{"press `left`", "move everything to the left"},
		{"press `right`", "move everything to the right"}, 
		{"press `up`", "move everything up"},
		{"press `down`", "move everything down"},
		{"`CTRL` + `D` + press `M`", "distance measurement mode"},
		{"`CTRL` + `D` + `M` + press `E`", "distance measurement eraser mode"},
		{"`CTRL` + `C` + press `E`", "circle sector eraser mode"},
		{"press `H`", "switch between geometry board and help"},
		{"`CTRL` + press `H`", "disable help button"},
	};

	description = "Geometry is an app to create Euclidean geometry without any user interface with buttons etc for faster\n"
		"workflow. It only uses the following keyboard shortcuts:";

	fontSize = 15;
	tableStart = {10, 90};
	rowSize = fontSize + 10;
	columnSize = 400;
	textStart = tableStart.y + (rowSize / 2) - fontSize / 2;
	textOffsetX = 4;
}

void HelpWindow::Render()
{
	DrawTextEx(app_.font, description.c_str(), {10, 40}, fontSize, 0, DARBBLUE3);

	DrawLineEx({tableStart.x, tableStart.y + rowSize}, {settings::screenWidth - 10, tableStart.y + rowSize}, 2, DARBBLUE3);
	DrawLineEx({columnSize, tableStart.y}, {columnSize, (shortcuts.size() + 1) * rowSize + tableStart.y}, 2, DARBBLUE3);

	DrawTextEx(app_.font, "Keys", {tableStart.x + textOffsetX, textStart}, fontSize, 0, DARBBLUE3);
	DrawTextEx(app_.font, "Action", {columnSize + textOffsetX, textStart}, fontSize, 0, DARBBLUE3);
	
	for (size_t i = 0; i < shortcuts.size(); i++)
	{
		DrawTextEx(app_.font, shortcuts[i][0].c_str(), {tableStart.x + textOffsetX, textStart + rowSize * (i + 1)}, fontSize, 0, DARBBLUE3);
		DrawTextEx(app_.font, shortcuts[i][1].c_str(), {columnSize + textOffsetX, textStart + rowSize * (i + 1)}, fontSize, 0, DARBBLUE3);
		float y = tableStart.y + rowSize * (i + 1);
		DrawLineEx({tableStart.x, y}, {settings::screenWidth - 10, y}, 1, DARBBLUE3);
	}
}

GeometryApp::GeometryApp(int width, int height, int fps, std::string title)
{
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	SetTraceLogLevel(LOG_NONE);

	InitWindow(width, height, title.c_str());
	SetWindowMinSize(width, height);

	SetTargetFPS(fps);
	SetExitKey(0);

	ChangeDirectory(GetApplicationDirectory());

	Image iconImage = LoadImage("resources/icon.png");
	SetWindowIcon(iconImage);
	UnloadImage(iconImage);

	font = LoadFont("resources/anonymous_pro_bold.ttf");
	SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);

	appState = GEOMETRY_BOARD;
	showHelpButton = true;
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

	ClearBackground(WHITE);

	switch (appState)
	{
	case GEOMETRY_BOARD:
		board.Render();
		break;
	case HELP:
		helpWindow.Render();

	default:
		break;
	}

	if (showHelpButton)
	{
		button.Render();
	}

	EndDrawing();
}

void GeometryApp::Update()
{
	if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_H))
	{
		showHelpButton = !showHelpButton;
	}
	button.Update();
}

int GeometryApp::GetState()
{
	return appState;
}

void GeometryApp::SetState(int state)
{
	appState = state;
}
