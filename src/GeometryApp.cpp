#include "GeometryApp.h"
#include "raylib.h"

HelpButton::HelpButton(GeometryApp &app) : app_(app) {
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

	if (Selected() || IsKeyPressed(KEY_H))
	{
		color = DARBBLUE2;

		if (IsMouseButtonPressed(0) || IsKeyPressed(KEY_H))
		{
			switch (app_.GetState())
			{
			case GEOMETRY_BOARD:
				app_.SetState(HELP);
				text = "Close";
				break;
			case HELP:
				app_.SetState(GEOMETRY_BOARD);
				text = "Help";
				break;
			
			default:
				break;
			}
		}
	}
}

void HelpButton::Render(Font &font)
{
	DrawRectangleRounded(rectangle, 0.5, 10, color);
	DrawTextEx(font, text.c_str(), textPos, fontSize, 0, WHITE);
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

int GeometryApp::GetState()
{
	return appState;
}

void GeometryApp::SetState(int state){
	appState = state;
}
