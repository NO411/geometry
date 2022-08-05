#include "GeometryApp.h"
#include "Settings.h"
#include "raylib.h"
#include <string>

HelpButton::HelpButton(GeometryApp *app) : app_(app)
{
	Update();
	text = "Help";
	fontSize = 16;
}

HelpButton::~HelpButton() {}

bool HelpButton::Selected()
{
	return (CheckCollisionPointRec(GetMousePosition(), rectangle) && app_->showHelpButton);
}

void HelpButton::Update()
{
	color = DARBBLUE3;

	rectangle = {GetScreenWidth() - 80.0f, 5, 75, 30};
	textPos = {rectangle.x + rectangle.width / 2 - MeasureText(text.c_str(), fontSize) / 2, rectangle.y + rectangle.height / 2 - fontSize / 2};

	bool hPressed = (IsKeyPressed(KEY_H) && !IsKeyDown(KEY_LEFT_CONTROL));

	if ((IsMouseButtonPressed(0) && Selected()) || hPressed)
	{
		pressed = true;
	}

	if (Selected() || (IsKeyDown(KEY_H) && !IsKeyDown(KEY_LEFT_CONTROL)))
	{
		color = DARBBLUE2;
	}

	if (pressed && ((IsMouseButtonReleased(0) && app_->showHelpButton && Selected()) || IsKeyReleased(KEY_H)))
	{
		pressed = false;
		switch (app_->GetState())
		{
		case GEOMETRY_BOARD:
			app_->SetState(HELP);
			text = "Close";
			SetWindowTitle("Geometry - Help");
			break;
		case HELP:
			app_->SetState(GEOMETRY_BOARD);
			app_->board.UpdateAllDrawPoints();
			SetWindowTitle("Geometry");
			text = "Help";
			break;
		default:
			break;
		}
	}
}

void HelpButton::Render()
{
	DrawRectangleRounded(rectangle, 0.5, 10, color);
	DrawTextEx(app_->font, text.c_str(), textPos, fontSize, 0, WHITE);
}

HelpWindow::HelpWindow(GeometryApp *app) : app_(app)
{
	description = "Geometry is an app to create Euclidean geometry without any user interface with buttons etc for faster\n"
				  "workflow. It only uses the following keyboard shortcuts:";

	shortcuts = {
		{"`CTRL` + press `C`", "circle drawing mode"},
		{"`CTRL` + press `S`", "straight line drawing mode"},
		{"`CTRL` + press `D`", "distance drawing mode"},
		{"`CTRL` + press `R`", "ray drawing mode"},
		{"`CTRL` + press `P`", "point drawing mode"},
		{"`CTRL` + press `E`", "enable eraser"},
		{"press `left mouse button`", "select point"},
		{"press `ESC`", "interrupt"},
		{"press `left`", "move everything to the left"},
		{"press `right`", "move everything to the right"},
		{"press `up`", "move everything up"},
		{"press `down`", "move everything down"},
		{"`CTRL` + `D` + press `M`", "distance measurement mode"},
		{"`CTRL` + `D` + `M` + press `E`", "distance measurement eraser mode"},
		{"`CTRL` + `C` + press `E`", "circle sector eraser mode"},
		{"press `H`", "switch between geometry board and help"},
		{"`CTRL` + press `H`", "disable help button"},
		{"`mouse wheel`", "zoom in / out"},
		{"`CTRL` + `Z` (`Y` for QWERTY keyboard) + press `R`", "reset zoom and field of view"},
	};

	fontSize = 15;
	tableStart = {10, 90};
	rowSize = fontSize + 10;
	columnSize = 400;
	textStart = tableStart.y + (rowSize / 2) - fontSize / 2;
	textOffsetX = 4;
}

void HelpWindow::Render()
{
	DrawTextEx(app_->font, description.c_str(), {10, 40}, fontSize, 0, DARBBLUE3);

	DrawLineEx({tableStart.x, tableStart.y + rowSize}, {settings::screenWidth - 10, tableStart.y + rowSize}, 2, DARBBLUE3);
	DrawLineEx({columnSize, tableStart.y}, {columnSize, (shortcuts.size() + 1) * rowSize + tableStart.y}, 2, DARBBLUE3);

	DrawTextEx(app_->font, "Keys", {tableStart.x + textOffsetX, textStart}, fontSize, 0, DARBBLUE3);
	DrawTextEx(app_->font, "Action", {columnSize + textOffsetX, textStart}, fontSize, 0, DARBBLUE3);

	Color transparentColor = DARBBLUE3;
	transparentColor.a = 50;

	for (size_t i = 0; i < shortcuts.size(); i++)
	{
		DrawTextEx(app_->font, shortcuts[i][0].c_str(), {tableStart.x + textOffsetX, textStart + rowSize * (i + 1)}, fontSize, 0, DARBBLUE3);
		DrawTextEx(app_->font, shortcuts[i][1].c_str(), {columnSize + textOffsetX, textStart + rowSize * (i + 1)}, fontSize, 0, DARBBLUE3);
		float y = tableStart.y + rowSize * (i + 1);
		DrawLineEx({tableStart.x, y}, {settings::screenWidth - 10, y}, 1, transparentColor);
	}

	transparentColor.a = 100;

	for (size_t i = 0; i < keyHighlightings.size(); i++)
	{
		DrawRectangleRounded(keyHighlightings[i], 0.5, 10, transparentColor);
	}
}

void HelpWindow::CalculateKeyHighlightings()
{
	for (size_t i = 0; i < shortcuts.size(); i++)
	{
		std::string &shortcut = shortcuts[i][0];

		std::size_t charPos = shortcut.find("`");
		while (charPos != std::string::npos)
		{
			shortcut.erase(charPos, 1);

			std::size_t secondCharPos = shortcut.find("`");
			if (secondCharPos == std::string::npos)
			{
				break;
			}

			shortcut.erase(secondCharPos, 1);

			float overlap = 2.5;

			Rectangle rec;
			rec.x = tableStart.x + textOffsetX + MeasureTextEx(app_->font, shortcut.substr(0, charPos).c_str(), fontSize, 0).x - overlap;
			rec.y = textStart + (rowSize * (i + 1)) - overlap;
			rec.width = (float)MeasureTextEx(app_->font, shortcut.substr(charPos, secondCharPos - charPos).c_str(), fontSize, 0).x + 2 * overlap;
			rec.height = (float)fontSize + 2 * overlap;

			keyHighlightings.push_back(rec);
			charPos = shortcut.find("`");
		}
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

	SetState(GEOMETRY_BOARD);
	showHelpButton = true;

	helpWindow.CalculateKeyHighlightings();
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

	switch (GetState())
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

	switch (GetState())
	{
	case GEOMETRY_BOARD:
		board.Update();
		break;

	default:
		break;
	}
}

int GeometryApp::GetState()
{
	return appState;
}

void GeometryApp::SetState(int state)
{
	appState = state;
}
