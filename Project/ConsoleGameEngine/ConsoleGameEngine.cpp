#include <iostream>
#include <windows.h>
#include "Engine.h"
#include "UI.h"
#include "SpriteGenerator.h"
#include "TrollGame.h"

int main()
{
	//#define RUN_EDITOR

#ifdef RUN_EDITOR

	// If running the editor run a low resolution app for generating data

	SpriteApp app;
	app.SetAppInfo(L"Sprite editor");
	app.ConstructConsole(80, 50, 12, 12);
	app.Start();

#endif // RUN_EDITOR
#ifndef RUN_EDITOR

	// If not running the editor run a high resolution game with the built in data

	TrollGame game;
	game.SetAppInfo(L"1bit Jam Game");
	game.ConstructConsole(160, 100, 6, 6);
	game.Start();

#endif // !RUN_EDITOR

	return 0;
}