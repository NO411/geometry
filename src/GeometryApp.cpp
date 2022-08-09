#include "GeometryApp.h"
#include "math/MathMisc.h"
#include "Settings.h"
#include "raylib.h"
#include <string>

const Color GeometryApp::DARBBLUE1 = {70, 70, 80, 255};
const Color GeometryApp::DARBBLUE2 = {50, 50, 60, 255};
const Color GeometryApp::DARBBLUE3 = {30, 30, 40, 255};

GeometryApp::GeometryApp(int width, int height, int fps, std::string title)
{
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	SetTraceLogLevel(LOG_NONE);

	InitWindow(width, height, title.c_str());
	SetWindowMinSize(770, 200);

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

	helpWindow.Init();
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
	{
		board.Update();

		if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyDown(KEY_C) && IsKeyPressed(KEY_E))
		{
			ClearBoard();
		}
	}
	case HELP:
	{
		helpWindow.Update();
	}
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

void GeometryApp::ClearBoard()
{
	board = {this};
}

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
	color = app_->DARBBLUE3;

	rectangle = {GetScreenWidth() - 80.0f, 5, 75, 30};
	textPos = {rectangle.x + rectangle.width / 2 - MeasureText(text.c_str(), fontSize) / 2, rectangle.y + rectangle.height / 2 - fontSize / 2};

	bool hPressed = (IsKeyPressed(KEY_H) && !IsKeyDown(KEY_LEFT_CONTROL));

	if ((IsMouseButtonPressed(0) && Selected()) || hPressed)
	{
		pressed = true;
	}

	if (Selected() || (IsKeyDown(KEY_H) && !IsKeyDown(KEY_LEFT_CONTROL)))
	{
		color = app_->DARBBLUE2;
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

void HelpWindow::Init()
{
	CalculateKeyHighlightings();
	float scrollBarWidth = 20;
	Rectangle box = {(float)GetScreenWidth() - scrollBarWidth - 5, app_->button.rectangle.y + app_->button.rectangle.height + 5, scrollBarWidth, (float)GetScreenHeight() - 5};
	box.height -= box.y;
	scrollBar = {box, this};
}

HelpWindow::HelpWindow(GeometryApp *app) : app_(app)
{
	description = "Geometry is an app to create Euclidean geometry without any user interface with buttons etc\n"
				  "for a faster workflow. It only uses the following keyboard shortcuts:";

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
		{"`CTRL` + `L` + press `M`", "length measurement mode (enable)"},
		{"`CTRL` + `L` + `M` + press `E`", "length measurement eraser mode (disable)"},
		{"`CTRL` + `C` + press `E`", "circle sector eraser mode (WIP)"},
		{"press `H`", "switch between geometry board and help"},
		{"`CTRL` + press `H`", "disable help button"},
		{"`mouse wheel`", "zoom in / out"},
		{"`CTRL` + `Z` (`Y` for QWERTY keyboard) + press `R`", "reset zoom and field of view"},
		{"`CTRL` + `C` + `E`", "clear everything"},
	};

	fontSize = 15;
	tableStart = {10, 90};
	rowSize = fontSize + 10;
	columnSize = 400;
	textStart = tableStart.y + (rowSize / 2) - fontSize / 2;
	textOffsetX = 4;

	textRenderHeight = (shortcuts.size() + 1) * rowSize + tableStart.y + 5;

	camera = {0};
	camera.zoom = 1;
}

void HelpWindow::Render()
{
	BeginMode2D(camera);

	DrawTextEx(app_->font, description.c_str(), {10, 40}, fontSize, 0, app_->DARBBLUE3);

	float linesEnd = std::min(scrollBar.box.x - 5, 2 * columnSize);

	DrawLineEx({tableStart.x, tableStart.y + rowSize}, {linesEnd, tableStart.y + rowSize}, 2, app_->DARBBLUE3);
	DrawLineEx({columnSize, tableStart.y}, {columnSize, textRenderHeight - 5}, 2, app_->DARBBLUE3);

	DrawTextEx(app_->font, "Keys", {tableStart.x + textOffsetX, textStart}, fontSize, 0, app_->DARBBLUE3);
	DrawTextEx(app_->font, "Action", {columnSize + textOffsetX, textStart}, fontSize, 0, app_->DARBBLUE3);

	Color transparentColor = app_->DARBBLUE3;
	transparentColor.a = 50;

	for (size_t i = 0; i < shortcuts.size(); i++)
	{
		DrawTextEx(app_->font, shortcuts[i][0].c_str(), {tableStart.x + textOffsetX, textStart + rowSize * (i + 1)}, fontSize, 0, app_->DARBBLUE3);
		DrawTextEx(app_->font, shortcuts[i][1].c_str(), {columnSize + textOffsetX, textStart + rowSize * (i + 1)}, fontSize, 0, app_->DARBBLUE3);
		float y = tableStart.y + rowSize * (i + 1);
		DrawLineEx({tableStart.x, y}, {linesEnd, y}, 1, transparentColor);
	}

	transparentColor.a = 100;

	for (size_t i = 0; i < keyHighlightings.size(); i++)
	{
		DrawRectangleRounded(keyHighlightings[i], 0.5, 10, transparentColor);
	}

	EndMode2D();

	scrollBar.Render();
}

void HelpWindow::Update()
{
	scrollBar.Update();
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

ScrollBar::ScrollBar()
{
}

ScrollBar::ScrollBar(Rectangle boxRect, HelpWindow *helpWindow_) : box(boxRect), dragBox(boxRect), helpWindow_(helpWindow_)
{
	UpdateDragBoxHeight();
}

void ScrollBar::UpdateDragBoxHeight()
{
	dragBox.height = std::min((float)GetScreenHeight() / helpWindow_->textRenderHeight * box.height, box.height); //works
}

void ScrollBar::Update()
{
	if (IsWindowResized())
	{
		// TODO: fix weird behaviour!
		box.x = (float)GetScreenWidth() - 25;
		box.height = (float)GetScreenHeight() - 5 - box.y;
		dragBox.x = box.x;
		UpdateDragBoxHeight();

		auto AttachCameraToBottom = [this]()
		{
			helpWindow_->camera.offset.y = -helpWindow_->textRenderHeight + (float)GetScreenHeight();
		};

		if (dragBox.y + dragBox.height >= box.y + box.height)
		{
			AttachCameraToBottom();
			dragBox.y -= (dragBox.y + dragBox.height) - (box.y + box.height);
		}

		if (GetScreenHeight() - helpWindow_->camera.offset.y >= helpWindow_->textRenderHeight)
		{
			AttachCameraToBottom();
			helpWindow_->camera.offset.y = -helpWindow_->textRenderHeight + (float)GetScreenHeight();
		}

		if (GetScreenHeight() >= helpWindow_->textRenderHeight)
		{
			helpWindow_->camera.offset = {0, 0};
			dragBox = box;
		}

		return;
	}

	if (Selected() || move)
	{
		dragBoxColor = helpWindow_->app_->DARBBLUE1;
		if (IsMouseButtonPressed(0) && Selected())
		{
			move = true;
			moveStartY = dragBox.y;
			movePos = GetMousePosition();
		}
	}
	else
	{
		dragBoxColor = helpWindow_->app_->DARBBLUE2;
	}

	if (IsMouseButtonReleased(0))
	{
		move = false;
	}

	if (move)
	{
		float newY = moveStartY - (movePos.y - GetMousePosition().y);
		if (newY > box.y)
		{
			dragBox.y = newY;
		}
		else
		{
			dragBox.y = box.y;
		}

		if (newY + dragBox.height > box.y + box.height)
		{
			dragBox.y = box.y + box.height - dragBox.height;
		}

		if (GetScreenHeight() < helpWindow_->textRenderHeight)
		{
			helpWindow_->camera.offset.y = -((dragBox.y - box.y) * ((helpWindow_->textRenderHeight - (float)GetScreenHeight()) / (box.height - dragBox.height)));
		}
	}
}

void ScrollBar::Render()
{
	DrawRectangleRec(box, helpWindow_->app_->DARBBLUE3);
	DrawRectangleRec(dragBox, dragBoxColor);
}

bool ScrollBar::Selected()
{
	return CheckCollisionPointRec(GetMousePosition(), dragBox);
}
