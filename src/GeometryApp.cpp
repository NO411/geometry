#include "GeometryApp.h"
#include "math/MathMisc.h"
#include "Settings.h"
#include "raylib.h"
#include <string>

const Color GeometryApp::DARBBLUE1 = {70, 70, 80, 255};
const Color GeometryApp::DARBBLUE2 = {50, 50, 60, 255};
const Color GeometryApp::DARBBLUE3 = {30, 30, 40, 255};

const Color CloseButton::RED1 = {185, 75, 75, 255};
const Color CloseButton::RED2 = {165, 55, 55, 255};

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

	helpWindow.Init();
	saveWindow.Init();
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
		button.Render();
		break;
	case HELP:
		helpWindow.Render();
		button.Render();
		break;
	case SAVE:
		saveWindow.Render();
		break;

	default:
		break;
	}

	EndDrawing();
}

void GeometryApp::Update()
{
	switch (GetState())
	{
	case GEOMETRY_BOARD:
		button.Update();
		board.Update();

		if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyDown(KEY_C) && IsKeyPressed(KEY_E))
		{
			ClearBoard();
		}
		break;
	case HELP:
		button.Update();
		helpWindow.Update();
		break;
	case SAVE:
		saveWindow.Update();
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

Button::Button()
{
}

HelpButton::HelpButton(GeometryApp *app)
{
	Update();
	text = "Help";
	fontSize = 16;
	showHelpButton = true;
	app_ = app;
}

HelpButton::~HelpButton() {}

bool HelpButton::Selected()
{
	return (CheckCollisionPointRec(GetMousePosition(), rectangle) && showHelpButton);
}

void HelpButton::Update()
{
	if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_H))
	{
		showHelpButton = !showHelpButton;
	}

	color = app_->DARBBLUE3;

	rectangle = {GetScreenWidth() - 80.0f, 5, 75, 30};
	pos = {rectangle.x + rectangle.width / 2 - MeasureText(text.c_str(), fontSize) / 2, rectangle.y + rectangle.height / 2 - fontSize / 2};

	bool hPressed = (IsKeyPressed(KEY_H) && !IsKeyDown(KEY_LEFT_CONTROL));

	if ((IsMouseButtonPressed(0) && Selected()) || hPressed)
	{
		pressed = true;
	}

	if (Selected() || (IsKeyDown(KEY_H) && !IsKeyDown(KEY_LEFT_CONTROL)))
	{
		color = app_->DARBBLUE2;
	}

	if (pressed && ((IsMouseButtonReleased(0) && showHelpButton && Selected()) || IsKeyReleased(KEY_H)))
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

Window::Window()
{
	camera = {0};
	camera.zoom = 1;
}

void HelpButton::Render()
{
	if (!showHelpButton)
	{
		return;
	}

	DrawRectangleRounded(rectangle, 0.5, 10, color);
	DrawTextEx(app_->font, text.c_str(), pos.ToRaylibVec(), fontSize, 0, WHITE);
}

void HelpWindow::Init()
{
	CalculateKeyHighlightings();
	float scrollBarWidth = 20;
	Rectangle box = {(float)GetScreenWidth() - scrollBarWidth - 5, app_->button.rectangle.y + app_->button.rectangle.height + 5, scrollBarWidth, (float)GetScreenHeight() - 5};
	box.height -= box.y;
	scrollBar = {box, this};
}

HelpWindow::HelpWindow(GeometryApp *app)
{
	app_ = app;
	description = "Geometry is an app to create Euclidean geometry without any user interface with buttons etc\n"
				  "for a faster workflow. It only uses the following keyboard shortcuts:";

	shortcuts = {
		{"press `left mouse button`", "select point"},
		{"`CTRL` + press `C`", "circle drawing mode"},
		{"`CTRL` + press `S`", "straight line drawing mode"},
		{"`CTRL` + press `D`", "distance drawing mode"},
		{"`CTRL` + press `R`", "ray drawing mode"},
		{"`CTRL` + press `P`", "point drawing mode"},
		{"`CTRL` + press `E`", "enable eraser"},
		{"`CTRL` + press `M`", "move object mode"},
		{"press `ESC`", "interrupt"},
		{"press `left`", "move everything to the left"},
		{"press `right`", "move everything to the right"},
		{"press `up`", "move everything up"},
		{"press `down`", "move everything down"},
		{"`CTRL` + `L` + press `M`", "length measurement mode (enable)"},
		{"`CTRL` + `L` + `M` + press `E`", "length measurement eraser mode (disable)"},
		{"`CTRL` + `E` + press `C`", "circle sector eraser mode"},
		{"press `H`", "switch between geometry board and help"},
		{"`CTRL` + press `H`", "disable help button"},
		{"`mouse wheel`", "zoom in / out | scroll in help menu"},
		{"`CTRL` + `Z` (`Y` for QWERTY keyboard) + press `R`", "reset zoom and field of view"},
		{"`CTRL` + `C` + press `E`", "clear everything"},
		{"`CTRL` + `ALT` + press `S`", "save"},
	};

	fontSize = 15;
	tableStart = {10, 90};
	rowSize = fontSize + 10;
	columnSize = 400;
	textStart = tableStart.y + (rowSize / 2) - fontSize / 2;
	textOffsetX = 4;

	textRenderHeight = (shortcuts.size() + 1) * rowSize + tableStart.y + 5;
}

void HelpWindow::Render()
{
	BeginMode2D(camera);

	DrawTextEx(app_->font, description.c_str(), {10, 40}, fontSize, 0, app_->DARBBLUE3);

	float linesEnd = std::min(scrollBar.box.x - 5, 2 * columnSize);

	if (!scrollBar.Enabled())
	{
		linesEnd = std::min((float)GetScreenWidth() - 5, 2 * columnSize);
	}

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

ScrollBar::ScrollBar(Rectangle boxRect, Window *window_) : box(boxRect), dragBox(boxRect), window_(window_)
{
	UpdateDragBoxHeight();
}

void ScrollBar::UpdateDragBoxHeight()
{
	dragBox.height = std::min((float)GetScreenHeight() / window_->textRenderHeight * box.height, box.height);
}

void ScrollBar::UpdateOnResize()
{
	box.x = (float)GetScreenWidth() - 25;
	box.height = (float)GetScreenHeight() - 5 - box.y;
	dragBox.x = box.x;
	UpdateDragBoxHeight();

	auto AttachCameraToBottom = [this]()
	{
		window_->camera.offset.y = -window_->textRenderHeight + (float)GetScreenHeight();
	};

	if (dragBox.y + dragBox.height >= box.y + box.height)
	{
		AttachCameraToBottom();
		dragBox.y -= (dragBox.y + dragBox.height) - (box.y + box.height);
	}

	if (GetScreenHeight() - window_->camera.offset.y >= window_->textRenderHeight)
	{
		AttachCameraToBottom();
		window_->camera.offset.y = -window_->textRenderHeight + (float)GetScreenHeight();
	}

	if (GetScreenHeight() >= window_->textRenderHeight)
	{
		window_->camera.offset = {0, 0};
		dragBox = box;
	}
}

void ScrollBar::Update()
{
	if (!Enabled())
	{
		return;
	}

	if (IsWindowResized())
	{
		UpdateOnResize();

		return;
	}

	if (Selected() || move)
	{
		dragBoxColor = window_->app_->DARBBLUE1;
		if (IsMouseButtonPressed(0) && Selected())
		{
			move = true;
			moveStartY = dragBox.y;
			movePos = GetMousePosition();
		}
	}
	else
	{
		dragBoxColor = window_->app_->DARBBLUE2;
	}

	if (IsMouseButtonReleased(0))
	{
		move = false;
	}

	float wheel = GetMouseWheelMove();
	bool scroll = wheel != 0;
	if (scroll)
	{
		dragBox.y -= 10 * wheel;
		if (dragBox.y < box.y)
		{
			dragBox.y = box.y;
		}
		else if (dragBox.y + dragBox.height > box.y + box.height)
		{
			dragBox.y = box.y + box.height - dragBox.height;
		}
	}

	bool setInstantPos = BoxSelected() && IsMouseButtonPressed(0);
	if (move || setInstantPos)
	{
		float newY = moveStartY - (movePos.y - GetMousePosition().y);

		if (setInstantPos)
		{
			newY = (float)GetMouseY() - dragBox.height / 2;
		}

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
	}

	if (GetScreenHeight() < window_->textRenderHeight && (move || scroll || setInstantPos))
	{
		window_->camera.offset.y = -((dragBox.y - box.y) * ((window_->textRenderHeight - (float)GetScreenHeight()) / (box.height - dragBox.height)));
	}
}

void ScrollBar::Render()
{
	if (!Enabled())
	{
		return;
	}
	DrawRectangleRec(box, window_->app_->DARBBLUE3);
	DrawRectangleRec(dragBox, dragBoxColor);
}

bool ScrollBar::Selected()
{
	return CheckCollisionPointRec(GetMousePosition(), dragBox);
}

bool ScrollBar::BoxSelected()
{
	return (CheckCollisionPointRec(GetMousePosition(), box) && !Selected());
}

bool ScrollBar::Enabled()
{
	return (GetScreenHeight() < window_->textRenderHeight);
}

CloseButton::CloseButton()
{
}

CloseButton::CloseButton(GeometryApp *app)
{
	app_ = app;
}

void CloseButton::Render()
{
	DrawCircleSector(pos.ToRaylibVec(), radius, 0, 360, 100, color);
	DrawLineEx((pos - crossPointOffset).ToRaylibVec(), (pos + crossPointOffset).ToRaylibVec(), 3, WHITE);
	DrawLineEx((pos - crossPointOffset2).ToRaylibVec(), (pos + crossPointOffset2).ToRaylibVec(), 3, WHITE);
}

bool CloseButton::Selected()
{
	return CheckCollisionPointCircle(GetMousePosition(), pos.ToRaylibVec(), radius);
}

void CloseButton::Update()
{
	color = RED2;

	if (IsMouseButtonPressed(0) && Selected())
	{
		pressed = true;
	}

	if (Selected())
	{
		color = RED1;
	}

	if (pressed && IsMouseButtonReleased(0) && Selected())
	{
		pressed = false;
		app_->SetState(GEOMETRY_BOARD);
	}
}

void CloseButton::Init()
{
	pos = {45, 45};
	crossPointOffset = {10, 10};
	crossPointOffset2 = {crossPointOffset.x, -crossPointOffset.y};
	Vec2 crossPos = (pos - crossPointOffset);
	radius = GetDistance(crossPos, pos) + 5;
	color = RED2;
}

SaveWindow::SaveWindow(GeometryApp *app)
{
	closeButton = {app};
}

void SaveWindow::Render()
{
	closeButton.Render();
}

void SaveWindow::Update()
{
	closeButton.Update();
}

void SaveWindow::Init()
{
	closeButton.Init();
}
