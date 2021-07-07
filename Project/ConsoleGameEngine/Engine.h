/*
OneLoneCoder.com - Command Line Game Engine
"Who needs a frame buffer?" - @Javidx9
The Original & Best :P
One Lone Coder License
~~~~~~~~~~~~~~~~~~~~~~
- This software is Copyright (C) 2018 Javidx9
- This is free software
- This software comes with absolutely no warranty
- The copyright holder is not liable or responsible for anything
  this software does or does not
- You use this software at your own risk
- You can distribute this software
- You can modify this software
- Redistribution of this software or a derivative of this software
  must attribute the Copyright holder named above, in a manner
  visible to the end user
License
~~~~~~~
One Lone Coder Console Game Engine  Copyright (C) 2018  Javidx9
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it
under certain conditions; See license for details.
Original works located at:
	https://www.github.com/onelonecoder
	https://www.onelonecoder.com
	https://www.youtube.com/javidx9
GNU GPLv3
	https://github.com/OneLoneCoder/videos/blob/master/LICENSE
From Javidx9 :)
~~~~~~~~~~~~~~~
Hello! Ultimately I don't care what you use this for. It's intended to be
educational, and perhaps to the oddly minded - a little bit of fun.
Please hack this, change it and use it in any way you see fit. You acknowledge
that I am not responsible for anything bad that happens as a result of
your actions. However this code is protected by GNU GPLv3, see the license in the
github repo. This means you must attribute me if you use it. You can view this
license here: https://github.com/OneLoneCoder/videos/blob/master/LICENSE
Cheers!
Background
~~~~~~~~~~
If you've seen any of my videos - I like to do things using the windows console. It's quick
and easy, and allows you to focus on just the code that matters - ideal when you're
experimenting. Thing is, I have to keep doing the same initialisation and display code
each time, so this class wraps that up.
Author
~~~~~~
Twitter: @javidx9	http://twitter.com/javidx9
Blog:				http://www.onelonecoder.com
YouTube:			http://www.youtube.com/javidx9
Videos:
~~~~~~
Original:				https://youtu.be/cWc0hgYwZyc
Added mouse support:	https://youtu.be/tdqc9hZhHxM
Beginners Guide:		https://youtu.be/u5BhrA8ED0o
Shout Outs!
~~~~~~~~~~~
Thanks to cool people who helped with testing, bug-finding and fixing!
wowLinh, JavaJack59, idkwid, kingtatgi, Return Null, CPP Guy, MaGetzUb
Last Updated: 02/07/2018
Usage:
~~~~~~
This class is abstract, so you must inherit from it. Override the OnUserCreate() function
with all the stuff you need for your application (for thready reasons it's best to do
this in this function and not your class constructor). Override the OnUserUpdate(float fElapsedTime)
function with the good stuff, it gives you the elapsed time since the last call so you
can modify your stuff dynamically. Both functions should return true, unless you need
the application to close.
	int main()
	{
		// Use olcConsoleGameEngine derived app
		OneLoneCoder_Example game;
		// Create a console with resolution 160x100 characters
		// Each character occupies 8x8 pixels
		game.ConstructConsole(160, 100, 8, 8);
		// Start the engine!
		game.Start();
		return 0;
	}
Input is also handled for you - interrogate the m_keys[] array with the virtual
keycode you want to know about. bPressed is set for the frame the key is pressed down
in, bHeld is set if the key is held down, bReleased is set for the frame the key
is released in. The same applies to mouse! m_mousePosX and Y can be used to get
the current cursor position, and m_mouse[1..5] returns the mouse buttons.
The draw routines treat characters like pixels. By default they are set to white solid
blocks - but you can draw any unicode character, using any of the colours listed below.
There may be bugs!
See my other videos for examples!
http://www.youtube.com/javidx9
Lots of programs to try:
http://www.github.com/OneLoneCoder/videos
Chat on the Discord server:
https://discord.gg/WhwHUMV
Be bored by Twitch:
http://www.twitch.tv/javidx9
*/

#pragma once
#pragma comment(lib, "winmm.lib")

#ifndef UNICODE
#error Please enable UNICODE for your compiler! VS: Project Properties -> General -> \
Character Set -> Use Unicode. Thanks! - Javidx9
#endif

#include <windows.h>

#include <iostream>
#include <chrono>
#include <vector>
#include <list>
#include <thread>
#include <atomic>
#include <condition_variable>

const double PI = 3.14159265f;

struct TimeConstruct
{
	float timeSinceLevelLoad;
	float time;
	float elapsedTime;

	void UpdateTime(float _elapsedTime)
	{
		time += _elapsedTime;
		timeSinceLevelLoad += _elapsedTime;
		elapsedTime = _elapsedTime;
	}
};

TimeConstruct* Time;

class Collider
{
public:
	Collider() = default;

	Collider(std::wstring spriteData, int spriteWidth, int spriteHeight)
	{
		this->posX = this->posY = 0;

		this->width = -1;
		this->height = -1;

		this->offX = spriteWidth;
		this->offY = spriteHeight;

		for (size_t i = 0; i < spriteWidth * spriteHeight; i++)
		{
			int ix = i % spriteWidth;
			int iy = i / spriteWidth;

			if (spriteData[i] != '.')
			{
				// Calculate the start position
				if (ix < this->offX)
				{
					this->offX = ix;
				}

				if (iy < this->offY)
				{
					this->offY = iy;
				}

				// Calculate the bounds
				if (ix - this->offX > this->width)
				{
					this->width = ix - this->offX;
				}

				if (iy - this->offY > this->height)
				{
					this->height = iy - this->offY;
				}
			}
		}
	}

	Collider(int width, int height)
	{
		this->width = width;
		this->height = height;
	}

public:
	int width = 0, height = 0;
	int offX = 0, offY = 0;

	int posX = 0, posY = 0;

public:
	int GetTop() {
		return posY + offY;
	}

	int GetBottom() {
		return posY + offY + height;
	}

	int GetMiddleY() {
		return (int)(posY + offY + (height / 2.0f));
	}

	int GetLeft() {
		return posX + offX;
	}

	int GetRight() {
		return posX + offX + width;
	}

	int GetMiddleX() {
		return (int)(posX + offX + (width / 2.0f));
	}

public:
	void UpdatePosition(int x, int y)
	{
		posX = x;
		posY = y;
	}

	bool CheckOverlaps(int x, int y) {
		bool withinY = y <= GetBottom() && y >= GetTop();
		bool withinX = x >= GetLeft() && x <= GetRight();

		return withinX && withinY;
	}

	bool CheckCollision(Collider* other) {
		int otherRight = other->GetRight();
		int otherLeft = other->GetLeft();

		int otherTop = other->GetTop();
		int otherBot = other->GetBottom();

		bool withinY = (otherTop <= GetBottom() && otherTop >= GetTop()) || (otherBot <= GetBottom() && otherBot >= GetTop());
		bool withinX = (otherRight >= GetLeft() && otherRight <= GetRight()) || (otherLeft >= GetLeft() && otherLeft <= GetRight());

		return withinX && withinY;
	}
};

int Loop(int value, int min, int max)
{
	if (value > max) {
		return min;
	}

	if (value < min) {
		return max;
	}

	return value;
}

int Clamp(int value, int min, int max)
{
	if (value > max) {
		return max;
	}

	if (value < min) {
		return min;
	}

	return value;
}

float Clamp(float value, float min, float max)
{
	if (value > max) {
		return max;
	}

	if (value < min) {
		return min;
	}

	return value;
}

float GetAngleToTarget(float targetX, float targetY, float startX, float startY)
{
	float angle = 0;

	float distX = targetX - startX;
	float distY = targetY - startY;

	return atan2(distY, distX);
}

enum Key
{
	NONE,
	UP = 38,
	DOWN = 40,
	LEFT = 37,
	RIGHT = 39,
	SPACE = 32,
	BACK = 8,
	TAB = 9,
	SHIFT = 16, CTRL,
	INS = 45, DEL, HOME, END, PGUP, PGDN,
	ESCAPE = 27,
	RETURN = 13, ENTER, PAUSE, SCROLL,
	NP0 = 96, NP1, NP2, NP3, NP4, NP5, NP6, NP7, NP8, NP9,
	NP_MUL, NP_DIV, NP_ADD, NP_SUB, NP_DECIMAL, PERIOD,
	EQUALS, COMMA, MINUS,
	OEM_1, OEM_2, OEM_3, OEM_4, OEM_5, OEM_6, OEM_7, OEM_8,
	CAPS_LOCK, ENUM_END,

	K0 = 49, K1, K2, K3, K4, K5, K6, K7, K8, K9,

	A = 65, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
};

enum COLOUR
{
	FG_BLACK = 0x0000,
	FG_DARK_BLUE = 0x0001,
	FG_DARK_GREEN = 0x0002,
	FG_DARK_CYAN = 0x0003,
	FG_DARK_RED = 0x0004,
	FG_DARK_MAGENTA = 0x0005,
	FG_DARK_YELLOW = 0x0006,
	FG_GREY = 0x0007, // Thanks MS :-/
	FG_DARK_GREY = 0x0008,
	FG_BLUE = 0x0009,
	FG_GREEN = 0x000A,
	FG_CYAN = 0x000B,
	FG_RED = 0x000C,
	FG_MAGENTA = 0x000D,
	FG_YELLOW = 0x000E,
	FG_WHITE = 0x000F,
	BG_BLACK = 0x0000,
	BG_DARK_BLUE = 0x0010,
	BG_DARK_GREEN = 0x0020,
	BG_DARK_CYAN = 0x0030,
	BG_DARK_RED = 0x0040,
	BG_DARK_MAGENTA = 0x0050,
	BG_DARK_YELLOW = 0x0060,
	BG_GREY = 0x0070,
	BG_DARK_GREY = 0x0080,
	BG_BLUE = 0x0090,
	BG_GREEN = 0x00A0,
	BG_CYAN = 0x00B0,
	BG_RED = 0x00C0,
	BG_MAGENTA = 0x00D0,
	BG_YELLOW = 0x00E0,
	BG_WHITE = 0x00F0,
};

enum PIXEL_TYPE
{
	PIXEL_SOLID = 0x2588,
	PIXEL_THREEQUARTERS = 0x2593,
	PIXEL_HALF = 0x2592,
	PIXEL_QUARTER = 0x2591,
};

int GetPixel(PIXEL_TYPE type)
{
	switch (type)
	{
	case PIXEL_SOLID:
		return 0;
	case PIXEL_THREEQUARTERS:
		return 1;
	case PIXEL_HALF:
		return 2;
	case PIXEL_QUARTER:
		return 3;
	default:
		return 0x1; // Debug char
	}
}

int GetPixelFromChar(char type)
{
	switch (type)
	{
	case (char)'4':
		return 0;
	case (char)'3':
		return 1;
	case (char)'2':
		return 2;
	case (char)'1':
		return 3;
	default:
		return 0x1; // Debug char
	}
}

namespace SPRITES {
	std::wstring GetTrollSprite(int no)
	{
		switch (no)
		{
		case 0:
			// Idle
			return L"................4444......44444444..444333333444.4311331134...43333334....44444444.....422224......442244......444444......44..44...............";
		case 1:
			// Walk 1
			return L"................4444......44444444..444333333444.4311331134...43333334....44444444.....422224......442244......444444..........44...............";
		case 2:
			// Walk 2
			return L"................4444......44444444..444333333444.4311331134...43333334....44444444.....422224......442244......444444......44...................";
		case 3:
			return L"...........##....#....#..#...#........#.............#..#.......#....#......#......####.....#.........##.......#.....#....#.......#..............";
		default:
			break;
		}

		return L"";
	}

	std::wstring GetPersonSprite(int no)
	{
		std::wstring map;
		switch (no)
		{
		case 0:
			// Idle
			map = L"............................4444.......433334......413314......433334......444444.......4334........4444........4444........4..4................";
			break;
		case 1:
			// Walk 1
			map = L"............................4444.......433334......413314......433334......444444.......4334........44444.......444.........4...................";
			break;
		case 2:
			// Walk 2
			map = L"............................4444.......433334......413314......433334......444444.......4334.......44444.........444...........4................";
			break;
		case 3:
			return L"...........##....#....#..#...#........#.............#..#.......#....#......#......####.....#.........##.......#.....#....#.......#..............";
		default:
			break;
		}

		return map;
	}

	std::wstring GetExplosionSprite() {
		return L"...........##....#....#..#...#........#.............#..#.......#....#......#......####.....#.........##.......#.....#....#.......#..............";
	}

	std::wstring GetProjectileSprite() {
		return L"...............22322........232..........3...........3...........3...........3...........3...........3..........444..........4..................";
	}

	std::wstring GetBridgeStrut() {
		return L"...............................................................444444.....44333344....43222234....43211234....43211234....43211234....43211234..";
	}

	enum SpriteTag {
		Troll,
		Person,
		Explosion,
		Projectile,
		BridgeStrut
	};

	std::wstring GetSprite(SpriteTag tag, int index = 0)
	{
		switch (tag)
		{
		case 0:
			return GetTrollSprite(index);
		case 1:
			return GetPersonSprite(index);
		case 2:
			return GetExplosionSprite();
		case 3:
			return GetProjectileSprite();
		case 4:
			return GetBridgeStrut();
		default:
			break;
		}
	}
}

class Sprite {
private:
	int width, height;
	int spriteLength;

	short* glyphs = nullptr;
	short* colors = nullptr;
	int currentSpriteIndex = 0;
	std::wstring spriteData;

	SPRITES::SpriteTag spriteTag;

public:
	int GetWidth()
	{
		return width;
	}
	int GetHeight()
	{
		return height;
	}

	void SetSpriteIndex(int index)
	{
		currentSpriteIndex = index;
	}

	int GetSpriteIndex()
	{
		return currentSpriteIndex;
	}

	std::wstring GetSprite(int index = 0)
	{
		return SPRITES::GetSprite(spriteTag, index);
	}

	std::wstring GetCurrentSprite()
	{
		return SPRITES::GetSprite(spriteTag, currentSpriteIndex);
	}

	void ResetSprite()
	{
		SetSpriteIndex(0);
		SetupSprite();
	}

	void SetupSprite()
	{
		spriteData = GetSprite();

		// Calculate the sprite
		for (size_t i = 0; i < spriteLength; i++)
		{
			int x = i % width;
			int y = i / width;

			SetGlyph(x, y, spriteData[i]);
			SetColor(x, y, spriteData[i] != '.' ? FG_WHITE : FG_BLACK);
		}
	}

public:
	void SetGlyph(int x, int y, short c)
	{
		if (x < 0 || x >= width || y < 0 || y >= height)
			return;
		else
			glyphs[y * width + x] = c;
	}

	void SetColor(int x, int y, short c)
	{
		if (x < 0 || x >= width || y < 0 || y >= height)
			return;
		else
			colors[y * width + x] = c;
	}

	short GetGlyph(int x, int y)
	{
		if (x < 0 || x >= width || y < 0 || y >= height)
			return L' ';
		else
			return glyphs[y * width + x];
	}

	short GetColor(int x, int y)
	{
		if (x < 0 || x >= width || y < 0 || y >= height)
			return FG_BLACK;
		else
			return colors[y * width + x];
	}
public:
	Sprite() = default;

	Sprite(int width, int height, SPRITES::SpriteTag spriteTag)
	{
		this->width = width;
		this->height = height;
		this->spriteTag = spriteTag;
		spriteLength = width * height;

		glyphs = new short[spriteLength];
		colors = new short[spriteLength];

		SetupSprite();
	}
};

class olcSprite
{
public:
	olcSprite()
	{
	}

	olcSprite(int w, int h)
	{
		Create(w, h);
	}

	olcSprite(std::wstring sFile)
	{
		if (!Load(sFile))
			Create(8, 8);
	}

	int nWidth = 0;
	int nHeight = 0;

private:
	short* m_Glyphs = nullptr;
	short* m_Colours = nullptr;

	void Create(int w, int h)
	{
		nWidth = w;
		nHeight = h;
		m_Glyphs = new short[w * h];
		m_Colours = new short[w * h];
		for (int i = 0; i < w * h; i++)
		{
			m_Glyphs[i] = L' ';
			m_Colours[i] = FG_BLACK;
		}
	}

public:
	void SetGlyph(int x, int y, short c)
	{
		if (x < 0 || x >= nWidth || y < 0 || y >= nHeight)
			return;
		else
			m_Glyphs[y * nWidth + x] = c;
	}

	void SetColour(int x, int y, short c)
	{
		if (x < 0 || x >= nWidth || y < 0 || y >= nHeight)
			return;
		else
			m_Colours[y * nWidth + x] = c;
	}

	short GetGlyph(int x, int y)
	{
		if (x < 0 || x >= nWidth || y < 0 || y >= nHeight)
			return L' ';
		else
			return m_Glyphs[y * nWidth + x];
	}

	short GetColour(int x, int y)
	{
		if (x < 0 || x >= nWidth || y < 0 || y >= nHeight)
			return FG_BLACK;
		else
			return m_Colours[y * nWidth + x];
	}

	short SampleGlyph(float x, float y)
	{
		int sx = (int)(x * (float)nWidth);
		int sy = (int)(y * (float)nHeight - 1.0f);
		if (sx < 0 || sx >= nWidth || sy < 0 || sy >= nHeight)
			return L' ';
		else
			return m_Glyphs[sy * nWidth + sx];
	}

	short SampleColour(float x, float y)
	{
		int sx = (int)(x * (float)nWidth);
		int sy = (int)(y * (float)nHeight - 1.0f);
		if (sx < 0 || sx >= nWidth || sy < 0 || sy >= nHeight)
			return FG_BLACK;
		else
			return m_Colours[sy * nWidth + sx];
	}

	bool Save(std::wstring sFile)
	{
		FILE* f = nullptr;
		_wfopen_s(&f, sFile.c_str(), L"wb");
		if (f == nullptr)
			return false;

		fwrite(&nWidth, sizeof(int), 1, f);
		fwrite(&nHeight, sizeof(int), 1, f);
		fwrite(m_Colours, sizeof(short), nWidth * nHeight, f);
		fwrite(m_Glyphs, sizeof(short), nWidth * nHeight, f);

		fclose(f);

		return true;
	}

	bool Load(std::wstring sFile)
	{
		delete[] m_Glyphs;
		delete[] m_Colours;
		nWidth = 0;
		nHeight = 0;

		FILE* f = nullptr;
		_wfopen_s(&f, sFile.c_str(), L"rb");
		if (f == nullptr)
			return false;

		std::fread(&nWidth, sizeof(int), 1, f);
		std::fread(&nHeight, sizeof(int), 1, f);

		Create(nWidth, nHeight);

		std::fread(m_Colours, sizeof(short), nWidth * nHeight, f);
		std::fread(m_Glyphs, sizeof(short), nWidth * nHeight, f);

		std::fclose(f);
		return true;
	}
};

class olcConsoleGameEngine
{
public:
	olcConsoleGameEngine()
	{
		screenWidth = 80;
		screenHeight = 30;

		Time = new TimeConstruct();

		m_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		m_hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);

		std::memset(m_keyNewState, 0, 256 * sizeof(short));
		std::memset(m_keyOldState, 0, 256 * sizeof(short));
		std::memset(m_keys, 0, 256 * sizeof(sKeyState));
		mousePosX = 0;
		mousePosY = 0;

		m_bEnableSound = false;

		SetAppInfo(L"Default", L"ConsoleGameEngine - ");
	}

	void SetAppInfo(std::wstring name, std::wstring info = L"") {
		m_appName = name;

		m_appInfo = info;
	}

	void EnableSound()
	{
		m_bEnableSound = true;
	}

	int ConstructConsole(int width, int height, int fontw, int fonth)
	{
		if (m_hConsole == INVALID_HANDLE_VALUE)
			return Error(L"Bad Handle");

		screenWidth = width;
		screenHeight = height;

		bufferSize = screenWidth * screenHeight;

		// Update 13/09/2017 - It seems that the console behaves differently on some systems
		// and I'm unsure why this is. It could be to do with windows default settings, or
		// screen resolutions, or system languages. Unfortunately, MSDN does not offer much
		// by way of useful information, and so the resulting sequence is the reult of experiment
		// that seems to work in multiple cases.
		//
		// The problem seems to be that the SetConsoleXXX functions are somewhat circular and
		// fail depending on the state of the current console properties, i.e. you can't set
		// the buffer size until you set the screen size, but you can't change the screen size
		// until the buffer size is correct. This coupled with a precise ordering of calls
		// makes this procedure seem a little mystical :-P. Thanks to wowLinh for helping - Jx9

		// Change console visual size to a minimum so ScreenBuffer can shrink
		// below the actual visual size
		m_rectWindow = { 0, 0, 1, 1 };
		SetConsoleWindowInfo(m_hConsole, TRUE, &m_rectWindow);

		// Set the size of the screen buffer
		COORD coord = { (short)screenWidth, (short)screenHeight };
		if (!SetConsoleScreenBufferSize(m_hConsole, coord))
			Error(L"SetConsoleScreenBufferSize");

		// Assign screen buffer to the console
		if (!SetConsoleActiveScreenBuffer(m_hConsole))
			return Error(L"SetConsoleActiveScreenBuffer");

		// Set the font size now that the screen buffer has been assigned to the console
		CONSOLE_FONT_INFOEX cfi;
		cfi.cbSize = sizeof(cfi);
		cfi.nFont = 0;
		cfi.dwFontSize.X = fontw;
		cfi.dwFontSize.Y = fonth;
		cfi.FontFamily = FF_DONTCARE;
		cfi.FontWeight = FW_NORMAL;

		/*	DWORD version = GetVersion();
			DWORD major = (DWORD)(LOBYTE(LOWORD(version)));
			DWORD minor = (DWORD)(HIBYTE(LOWORD(version)));*/

			//if ((major > 6) || ((major == 6) && (minor >= 2) && (minor < 4)))
			//	wcscpy_s(cfi.FaceName, L"Raster"); // Windows 8 :(
			//else
			//	wcscpy_s(cfi.FaceName, L"Lucida Console"); // Everything else :P

			//wcscpy_s(cfi.FaceName, L"Liberation Mono");
		wcscpy_s(cfi.FaceName, L"Consolas");
		if (!SetCurrentConsoleFontEx(m_hConsole, false, &cfi))
			return Error(L"SetCurrentConsoleFontEx");

		// Get screen buffer info and check the maximum allowed window size. Return
		// error if exceeded, so user knows their dimensions/fontsize are too large
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		if (!GetConsoleScreenBufferInfo(m_hConsole, &csbi))
			return Error(L"GetConsoleScreenBufferInfo");
		if (screenHeight > csbi.dwMaximumWindowSize.Y)
			return Error(L"Screen Height / Font Height Too Big");
		if (screenWidth > csbi.dwMaximumWindowSize.X)
			return Error(L"Screen Width / Font Width Too Big");

		// Set Physical Console Window Size
		m_rectWindow = { 0, 0, (short)screenWidth - 1, (short)screenHeight - 1 };
		if (!SetConsoleWindowInfo(m_hConsole, TRUE, &m_rectWindow))
			return Error(L"SetConsoleWindowInfo");

		// Set flags to allow mouse input
		if (!SetConsoleMode(m_hConsoleIn, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT))
			return Error(L"SetConsoleMode");

		// Allocate memory for screen buffer
		m_bufScreen = new CHAR_INFO[screenWidth * screenHeight];
		memset(m_bufScreen, 0, sizeof(CHAR_INFO) * screenWidth * screenHeight);

		SetConsoleCtrlHandler((PHANDLER_ROUTINE)CloseHandler, TRUE);
		return 1;
	}

	virtual void Draw(int x, int y, short c = 0x2588, short col = 0x000F)
	{
		if (x >= 0 && x < screenWidth && y >= 0 && y < screenHeight)
		{
			int index = y * screenWidth + x;

			m_bufScreen[index].Char.UnicodeChar = c;
			m_bufScreen[index].Attributes = col;
		}
	}

	void Fill(int x1, int y1, int x2, int y2, short c = 0x2588, short col = 0x000F)
	{
		Clip(x1, y1);
		Clip(x2, y2);
		for (int x = x1; x < x2; x++)
			for (int y = y1; y < y2; y++)
				Draw(x, y, c, col);
	}

	void DrawStringToBuffer(int x, int y, std::wstring c, short col = 0x000F)
	{
		for (size_t i = 0; i < c.size(); i++)
		{
			m_bufScreen[y * screenWidth + x + i].Char.UnicodeChar = c[i];
			m_bufScreen[y * screenWidth + x + i].Attributes = col;
		}
	}

	void DrawStringAlpha(int x, int y, std::wstring c, short col = 0x000F)
	{
		for (size_t i = 0; i < c.size(); i++)
		{
			if (c[i] != L' ')
			{
				m_bufScreen[y * screenWidth + x + i].Char.UnicodeChar = c[i];
				m_bufScreen[y * screenWidth + x + i].Attributes = col;
			}
		}
	}

	void DrawString(int32_t x, int32_t y, const std::string& sText, uint32_t scale = 1, short col = 0x000f)
	{
		int32_t sx = 0;
		int32_t sy = 0;

		for (auto c : sText)
		{
			if (c == '\n')
			{
				sx = 0; sy += 8 * scale;
			}
			else
			{
				int32_t ox = (c - 32) % 16;
				int32_t oy = (c - 32) / 16;

				if (scale > 1)
				{
					for (uint32_t i = 0; i < 8; i++)
						for (uint32_t j = 0; j < 8; j++)
							if (fontSprite->GetColour(i + ox * 8, j + oy * 8) > 0)
								for (uint32_t is = 0; is < scale; is++)
									for (uint32_t js = 0; js < scale; js++)
										Draw(x + sx + (i * scale) + is, y + sy + (j * scale) + js, col);
				}
				else
				{
					for (uint32_t i = 0; i < 8; i++)
						for (uint32_t j = 0; j < 8; j++)
							if (fontSprite->GetColour(i + ox * 8, j + oy * 8) > 0)
								Draw(x + sx + i, y + sy + j, col);
				}
				sx += 8 * scale;
			}
		}
	}

	void ConstructFontSheet()
	{
		std::string data;
		data += "?Q`0001oOch0o01o@F40o0<AGD4090LAGD<090@A7ch0?00O7Q`0600>00000000";
		data += "O000000nOT0063Qo4d8>?7a14Gno94AA4gno94AaOT0>o3`oO400o7QN00000400";
		data += "Of80001oOg<7O7moBGT7O7lABET024@aBEd714AiOdl717a_=TH013Q>00000000";
		data += "720D000V?V5oB3Q_HdUoE7a9@DdDE4A9@DmoE4A;Hg]oM4Aj8S4D84@`00000000";
		data += "OaPT1000Oa`^13P1@AI[?g`1@A=[OdAoHgljA4Ao?WlBA7l1710007l100000000";
		data += "ObM6000oOfMV?3QoBDD`O7a0BDDH@5A0BDD<@5A0BGeVO5ao@CQR?5Po00000000";
		data += "Oc``000?Ogij70PO2D]??0Ph2DUM@7i`2DTg@7lh2GUj?0TO0C1870T?00000000";
		data += "70<4001o?P<7?1QoHg43O;`h@GT0@:@LB@d0>:@hN@L0@?aoN@<0O7ao0000?000";
		data += "OcH0001SOglLA7mg24TnK7ln24US>0PL24U140PnOgl0>7QgOcH0K71S0000A000";
		data += "00H00000@Dm1S007@DUSg00?OdTnH7YhOfTL<7Yh@Cl0700?@Ah0300700000000";
		data += "<008001QL00ZA41a@6HnI<1i@FHLM81M@@0LG81?O`0nC?Y7?`0ZA7Y300080000";
		data += "O`082000Oh0827mo6>Hn?Wmo?6HnMb11MP08@C11H`08@FP0@@0004@000000000";
		data += "00P00001Oab00003OcKP0006@6=PMgl<@440MglH@000000`@000001P00000000";
		data += "Ob@8@@00Ob@8@Ga13R@8Mga172@8?PAo3R@827QoOb@820@0O`0007`0000007P0";
		data += "O`000P08Od400g`<3V=P0G`673IP0`@3>1`00P@6O`P00g`<O`000GP800000000";
		data += "?P9PL020O`<`N3R0@E4HC7b0@ET<ATB0@@l6C4B0O`H3N7b0?P01L3R000000020";

		fontSprite = new olcSprite(128, 48);
		int px = 0, py = 0;
		for (size_t b = 0; b < 1024; b += 4)
		{
			uint32_t sym1 = (uint32_t)data[b + 0] - 48;
			uint32_t sym2 = (uint32_t)data[b + 1] - 48;
			uint32_t sym3 = (uint32_t)data[b + 2] - 48;
			uint32_t sym4 = (uint32_t)data[b + 3] - 48;
			uint32_t r = sym1 << 18 | sym2 << 12 | sym3 << 6 | sym4;

			for (int i = 0; i < 24; i++)
			{
				int k = r & (1 << i) ? 255 : 0;

				// Figure this out
				fontSprite->SetColour(px, py, k);
				if (++py == 48) { px++; py = 0; }
			}
		}
	}

	void Clip(int& x, int& y)
	{
		if (x < 0) x = 0;
		if (x >= screenWidth) x = screenWidth;
		if (y < 0) y = 0;
		if (y >= screenHeight) y = screenHeight;
	}

	void IncrementPixel(int x, int y)
	{
		long index = y * screenWidth + x;

		switch (m_bufScreen[index].Char.UnicodeChar)
		{
		case PIXEL_SOLID:
			break;
		case PIXEL_THREEQUARTERS:
			SetPixel(x, y, 0);
			break;
		case PIXEL_HALF:
			SetPixel(x, y, 1);
			break;
		case PIXEL_QUARTER:
			SetPixel(x, y, 2);
			break;
		default:
			SetPixel(x, y, 3);
			break;
		}
	}

	void SetPixel(int x, int y, int fillType)
	{
		if (x >= screenWidth || x < 0 || y > screenHeight || y < 0) return;
		long index = y * screenWidth + x;

		m_bufScreen[index].Attributes = FG_WHITE;

		switch (fillType)
		{
		case 0:
			m_bufScreen[index].Char.UnicodeChar = PIXEL_SOLID;
			break;
		case 1:
			m_bufScreen[index].Char.UnicodeChar = PIXEL_THREEQUARTERS;
			break;
		case 2:
			m_bufScreen[index].Char.UnicodeChar = PIXEL_HALF;
			break;
		case 3:
			m_bufScreen[index].Char.UnicodeChar = PIXEL_QUARTER;
			break;
		default:
			m_bufScreen[index].Attributes = FG_BLACK;
			m_bufScreen[index].Char.UnicodeChar = PIXEL_SOLID;
			break;
		}
	}

	void SetPixel(int x, int y, bool colored, int fillType)
	{
		if (x >= screenWidth || x < 0 || y > screenHeight || y < 0) return;
		long index = y * screenWidth + x;

		if (colored == false)
		{
			m_bufScreen[index].Attributes = FG_BLACK;
		}
		else
		{
			switch (fillType)
			{
			case 0:
				m_bufScreen[index].Char.UnicodeChar = PIXEL_SOLID;
				break;
			case 1:
				m_bufScreen[index].Char.UnicodeChar = PIXEL_THREEQUARTERS;
				break;
			case 2:
				m_bufScreen[index].Char.UnicodeChar = PIXEL_HALF;
				break;
			case 3:
				m_bufScreen[index].Char.UnicodeChar = PIXEL_QUARTER;
				break;
			default:
				m_bufScreen[index].Char.UnicodeChar = PIXEL_SOLID;
				break;
			}

			m_bufScreen[index].Attributes = FG_WHITE;
		}
	}

	void DrawLine(int x1, int y1, int x2, int y2, short c = 0x2588, short col = 0x000F)
	{
		int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
		dx = x2 - x1; dy = y2 - y1;
		dx1 = abs(dx); dy1 = abs(dy);
		px = 2 * dy1 - dx1;	py = 2 * dx1 - dy1;
		if (dy1 <= dx1)
		{
			if (dx >= 0)
			{
				x = x1; y = y1; xe = x2;
			}
			else
			{
				x = x2; y = y2; xe = x1;
			}

			Draw(x, y, c, col);

			for (i = 0; x < xe; i++)
			{
				x = x + 1;
				if (px < 0)
					px = px + 2 * dy1;
				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) y = y + 1; else y = y - 1;
					px = px + 2 * (dy1 - dx1);
				}
				Draw(x, y, c, col);
			}
		}
		else
		{
			if (dy >= 0)
			{
				x = x1; y = y1; ye = y2;
			}
			else
			{
				x = x2; y = y2; ye = y1;
			}

			Draw(x, y, c, col);

			for (i = 0; y < ye; i++)
			{
				y = y + 1;
				if (py <= 0)
					py = py + 2 * dx1;
				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) x = x + 1; else x = x - 1;
					py = py + 2 * (dx1 - dy1);
				}
				Draw(x, y, c, col);
			}
		}
	}

	void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, short c = 0x2588, short col = 0x000F)
	{
		DrawLine(x1, y1, x2, y2, c, col);
		DrawLine(x2, y2, x3, y3, c, col);
		DrawLine(x3, y3, x1, y1, c, col);
	}

	// https://www.avrfreaks.net/sites/default/files/triangles.c
	void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, short c = 0x2588, short col = 0x000F)
	{
		auto SWAP = [](int& x, int& y) { int t = x; x = y; y = t; };
		auto drawline = [&](int sx, int ex, int ny) { for (int i = sx; i <= ex; i++) Draw(i, ny, c, col); };

		int t1x, t2x, y, minx, maxx, t1xp, t2xp;
		bool changed1 = false;
		bool changed2 = false;
		int signx1, signx2, dx1, dy1, dx2, dy2;
		int e1, e2;
		// Sort vertices
		if (y1 > y2) { SWAP(y1, y2); SWAP(x1, x2); }
		if (y1 > y3) { SWAP(y1, y3); SWAP(x1, x3); }
		if (y2 > y3) { SWAP(y2, y3); SWAP(x2, x3); }

		t1x = t2x = x1; y = y1;   // Starting points
		dx1 = (int)(x2 - x1); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
		else signx1 = 1;
		dy1 = (int)(y2 - y1);

		dx2 = (int)(x3 - x1); if (dx2 < 0) { dx2 = -dx2; signx2 = -1; }
		else signx2 = 1;
		dy2 = (int)(y3 - y1);

		if (dy1 > dx1) {   // swap values
			SWAP(dx1, dy1);
			changed1 = true;
		}
		if (dy2 > dx2) {   // swap values
			SWAP(dy2, dx2);
			changed2 = true;
		}

		e2 = (int)(dx2 >> 1);
		// Flat top, just process the second half
		if (y1 == y2) goto next;
		e1 = (int)(dx1 >> 1);

		for (int i = 0; i < dx1;) {
			t1xp = 0; t2xp = 0;
			if (t1x < t2x) { minx = t1x; maxx = t2x; }
			else { minx = t2x; maxx = t1x; }
			// process first line until y value is about to change
			while (i < dx1) {
				i++;
				e1 += dy1;
				while (e1 >= dx1) {
					e1 -= dx1;
					if (changed1) t1xp = signx1;//t1x += signx1;
					else          goto next1;
				}
				if (changed1) break;
				else t1x += signx1;
			}
			// Move line
		next1:
			// process second line until y value is about to change
			while (1) {
				e2 += dy2;
				while (e2 >= dx2) {
					e2 -= dx2;
					if (changed2) t2xp = signx2;//t2x += signx2;
					else          goto next2;
				}
				if (changed2)     break;
				else              t2x += signx2;
			}
		next2:
			if (minx > t1x) minx = t1x; if (minx > t2x) minx = t2x;
			if (maxx < t1x) maxx = t1x; if (maxx < t2x) maxx = t2x;
			drawline(minx, maxx, y);    // Draw line from min to max points found on the y
										 // Now increase y
			if (!changed1) t1x += signx1;
			t1x += t1xp;
			if (!changed2) t2x += signx2;
			t2x += t2xp;
			y += 1;
			if (y == y2) break;
		}
	next:
		// Second half
		dx1 = (int)(x3 - x2); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
		else signx1 = 1;
		dy1 = (int)(y3 - y2);
		t1x = x2;

		if (dy1 > dx1) {   // swap values
			SWAP(dy1, dx1);
			changed1 = true;
		}
		else changed1 = false;

		e1 = (int)(dx1 >> 1);

		for (int i = 0; i <= dx1; i++) {
			t1xp = 0; t2xp = 0;
			if (t1x < t2x) { minx = t1x; maxx = t2x; }
			else { minx = t2x; maxx = t1x; }
			// process first line until y value is about to change
			while (i < dx1) {
				e1 += dy1;
				while (e1 >= dx1) {
					e1 -= dx1;
					if (changed1) { t1xp = signx1; break; }//t1x += signx1;
					else          goto next3;
				}
				if (changed1) break;
				else   	   	  t1x += signx1;
				if (i < dx1) i++;
			}
		next3:
			// process second line until y value is about to change
			while (t2x != x3) {
				e2 += dy2;
				while (e2 >= dx2) {
					e2 -= dx2;
					if (changed2) t2xp = signx2;
					else          goto next4;
				}
				if (changed2)     break;
				else              t2x += signx2;
			}
		next4:

			if (minx > t1x) minx = t1x; if (minx > t2x) minx = t2x;
			if (maxx < t1x) maxx = t1x; if (maxx < t2x) maxx = t2x;
			drawline(minx, maxx, y);
			if (!changed1) t1x += signx1;
			t1x += t1xp;
			if (!changed2) t2x += signx2;
			t2x += t2xp;
			y += 1;
			if (y > y3) return;
		}
	}

	void DrawCircle(int xc, int yc, int r, short c = 0x2588, short col = 0x000F)
	{
		int x = 0;
		int y = r;
		int p = 3 - 2 * r;
		if (!r) return;

		while (y >= x) // only formulate 1/8 of circle
		{
			Draw(xc - x, yc - y, c, col);//upper left left
			Draw(xc - y, yc - x, c, col);//upper upper left
			Draw(xc + y, yc - x, c, col);//upper upper right
			Draw(xc + x, yc - y, c, col);//upper right right
			Draw(xc - x, yc + y, c, col);//lower left left
			Draw(xc - y, yc + x, c, col);//lower lower left
			Draw(xc + y, yc + x, c, col);//lower lower right
			Draw(xc + x, yc + y, c, col);//lower right right
			if (p < 0) p += 4 * x++ + 6;
			else p += 4 * (x++ - y--) + 10;
		}
	}

	void FillCircle(int xc, int yc, int r, short c = 0x2588, short col = 0x000F)
	{
		// Taken from wikipedia
		int x = 0;
		int y = r;
		int p = 3 - 2 * r;
		if (!r) return;

		auto drawline = [&](int sx, int ex, int ny)
		{
			for (int i = sx; i <= ex; i++)
				Draw(i, ny, c, col);
		};

		while (y >= x)
		{
			// Modified to draw scan-lines instead of edges
			drawline(xc - x, xc + x, yc - y);
			drawline(xc - y, xc + y, yc - x);
			drawline(xc - x, xc + x, yc + y);
			drawline(xc - y, xc + y, yc + x);
			if (p < 0) p += 4 * x++ + 6;
			else p += 4 * (x++ - y--) + 10;
		}
	};

	void DrawSprite(int x, int y, olcSprite* sprite)
	{
		if (sprite == nullptr)
			return;

		for (int i = 0; i < sprite->nWidth; i++)
		{
			for (int j = 0; j < sprite->nHeight; j++)
			{
				if (sprite->GetGlyph(i, j) != L' ')
					Draw(x + i, y + j, sprite->GetGlyph(i, j), sprite->GetColour(i, j));
			}
		}
	}

	void DrawPartialSprite(int x, int y, olcSprite* sprite, int ox, int oy, int w, int h)
	{
		if (sprite == nullptr)
			return;

		for (int i = 0; i < w; i++)
		{
			for (int j = 0; j < h; j++)
			{
				if (sprite->GetGlyph(i + ox, j + oy) != L' ') {
					Draw(x + i, y + j, sprite->GetGlyph(i + ox, j + oy), sprite->GetColour(i + ox, j + oy));
				}
			}
		}
	}

	void DrawSprite(int x, int y, Sprite* sprite)
	{
		if (sprite == nullptr)
			return;

		for (int i = 0; i < sprite->GetWidth(); i++)
		{
			for (int j = 0; j < sprite->GetHeight(); j++)
			{
				if (sprite->GetGlyph(i, j) != L' ')
					Draw(x + i, y + j, sprite->GetGlyph(i, j), sprite->GetColor(i, j));
			}
		}

		for (size_t w = 0; w < sprite->GetWidth(); w++)
		{
			for (size_t h = 0; h < sprite->GetHeight(); h++)
			{
				short color = sprite->GetColor(w, h);

				if (color != FG_BLACK)
				{
					short glyph = sprite->GetGlyph(w, h);
					SetPixel(x + w, y + h, GetPixelFromChar(glyph));
				}
			}
		}
	}

	void DrawWireFrameModel(const std::vector<std::pair<float, float>>& vecModelCoordinates, float x, float y, float r = 0.0f, float s = 1.0f, short col = FG_WHITE, short c = PIXEL_SOLID)
	{
		// pair.first = x coordinate
		// pair.second = y coordinate

		// Create translated model vector of coordinate pairs
		std::vector<std::pair<float, float>> vecTransformedCoordinates;
		int verts = vecModelCoordinates.size();
		vecTransformedCoordinates.resize(verts);

		// Rotate
		for (int i = 0; i < verts; i++)
		{
			vecTransformedCoordinates[i].first = vecModelCoordinates[i].first * cosf(r) - vecModelCoordinates[i].second * sinf(r);
			vecTransformedCoordinates[i].second = vecModelCoordinates[i].first * sinf(r) + vecModelCoordinates[i].second * cosf(r);
		}

		// Scale
		for (int i = 0; i < verts; i++)
		{
			vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first * s;
			vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second * s;
		}

		// Translate
		for (int i = 0; i < verts; i++)
		{
			vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first + x;
			vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second + y;
		}

		// Draw Closed Polygon
		for (int i = 0; i < verts + 1; i++)
		{
			int j = (i + 1);
			DrawLine((int)vecTransformedCoordinates[i % verts].first, (int)vecTransformedCoordinates[i % verts].second,
				(int)vecTransformedCoordinates[j % verts].first, (int)vecTransformedCoordinates[j % verts].second, c, col);
		}
	}

	void ClearScreen()
	{
		for (int i = 0; i < bufferSize; i++)
		{
			m_bufScreen[i].Attributes = FG_BLACK;
			m_bufScreen[i].Char.UnicodeChar = 0;
		}
	}

	void FadeScreen()
	{
		for (int i = 0; i < bufferSize; i++)
		{
			switch (m_bufScreen[i].Char.UnicodeChar)
			{
			case PIXEL_SOLID:
				m_bufScreen[i].Attributes = FG_WHITE;
				m_bufScreen[i].Char.UnicodeChar = PIXEL_THREEQUARTERS;
				break;
			case PIXEL_THREEQUARTERS:
				m_bufScreen[i].Attributes = FG_WHITE;
				m_bufScreen[i].Char.UnicodeChar = PIXEL_HALF;
				break;
			case PIXEL_HALF:
				m_bufScreen[i].Attributes = FG_WHITE;
				m_bufScreen[i].Char.UnicodeChar = PIXEL_QUARTER;
				break;
			default:
				m_bufScreen[i].Attributes = FG_BLACK;
				m_bufScreen[i].Char.UnicodeChar = 0;
				break;
			}
		}
	}

	~olcConsoleGameEngine()
	{
		SetConsoleActiveScreenBuffer(m_hOriginalConsole);
		delete[] m_bufScreen;
	}

public:
	void Start()
	{
		// Start the thread
		m_bAtomActive = true;
		std::thread t = std::thread(&olcConsoleGameEngine::GameThread, this);

		ConstructFontSheet();

		// Wait for thread to be exited
		t.join();
	}

private:
	void GameThread()
	{
		// Create user resources as part of this thread
		if (!OnUserCreate())
			m_bAtomActive = false;

		// Check if sound system should be enabled
		if (m_bEnableSound)
		{
			if (!CreateAudio())
			{
				m_bAtomActive = false; // Failed to create audio system
				m_bEnableSound = false;
			}
		}

		auto tp1 = std::chrono::system_clock::now();
		auto tp2 = std::chrono::system_clock::now();

		while (m_bAtomActive)
		{
			// Run as fast as possible
			while (m_bAtomActive)
			{
				// Handle Timing
				tp2 = std::chrono::system_clock::now();
				std::chrono::duration<float> elapsedTime = tp2 - tp1;
				tp1 = tp2;
				float fElapsedTime = elapsedTime.count();

				// Handle Keyboard Input
				for (int i = 0; i < 256; i++)
				{
					m_keyNewState[i] = GetAsyncKeyState(i);

					m_keys[i].bPressed = false;
					m_keys[i].bReleased = false;

					if (m_keyNewState[i] != m_keyOldState[i])
					{
						if (m_keyNewState[i] & 0x8000)
						{
							m_keys[i].bPressed = !m_keys[i].bHeld;
							m_keys[i].bHeld = true;
						}
						else
						{
							m_keys[i].bReleased = true;
							m_keys[i].bHeld = false;
						}
					}

					m_keyOldState[i] = m_keyNewState[i];
				}

				// Handle Mouse Input - Check for window events
				INPUT_RECORD inBuf[32];
				DWORD events = 0;
				GetNumberOfConsoleInputEvents(m_hConsoleIn, &events);
				if (events > 0)
					ReadConsoleInput(m_hConsoleIn, inBuf, events, &events);

				// Handle events - we only care about mouse clicks and movement
				// for now
				for (DWORD i = 0; i < events; i++)
				{
					switch (inBuf[i].EventType)
					{
					case FOCUS_EVENT:
					{
						m_bConsoleInFocus = inBuf[i].Event.FocusEvent.bSetFocus;
					}
					break;

					case MOUSE_EVENT:
					{
						switch (inBuf[i].Event.MouseEvent.dwEventFlags)
						{
						case MOUSE_MOVED:
						{
							mousePosX = inBuf[i].Event.MouseEvent.dwMousePosition.X;
							mousePosY = inBuf[i].Event.MouseEvent.dwMousePosition.Y;
						}
						break;

						case 0:
						{
							for (int m = 0; m < 5; m++)
								m_mouseNewState[m] = (inBuf[i].Event.MouseEvent.dwButtonState & (1 << m)) > 0;
						}
						break;

						default:
							break;
						}
					}
					break;

					default:
						break;
						// We don't care just at the moment
					}
				}

				for (int m = 0; m < 5; m++)
				{
					m_mouse[m].bPressed = false;
					m_mouse[m].bReleased = false;

					if (m_mouseNewState[m] != m_mouseOldState[m])
					{
						if (m_mouseNewState[m])
						{
							m_mouse[m].bPressed = true;
							m_mouse[m].bHeld = true;
						}
						else
						{
							m_mouse[m].bReleased = true;
							m_mouse[m].bHeld = false;
						}
					}

					m_mouseOldState[m] = m_mouseNewState[m];
				}

				// Handle Frame Update

				/// <summary>
				///
				/// </summary>
				if (frameCount % (256 - fadeSpeed) == 0)
					FadeScreen();

				Time->UpdateTime(fElapsedTime);

				if (!OnUserUpdate())
					m_bAtomActive = false;

				frameCount++;

				if (frameCount >= SHRT_MAX - 100)
					frameCount = 0;

				// Update Title & Present Screen Buffer
				wchar_t s[256];
				swprintf_s(s, 256, L"%s%s - FPS: %3.2f", m_appInfo.c_str(), m_appName.c_str(), 1.0f / fElapsedTime);
				SetConsoleTitle(s);
				WriteConsoleOutput(m_hConsole, m_bufScreen, { (short)screenWidth, (short)screenHeight }, { 0,0 }, &m_rectWindow);
			}

			if (m_bEnableSound)
			{
				// Close and Clean up audio system
			}

			// Allow the user to free resources if they have overrided the destroy function
			if (OnUserDestroy())
			{
				// User has permitted destroy, so exit and clean up
				delete[] m_bufScreen;
				SetConsoleActiveScreenBuffer(m_hOriginalConsole);
				m_cvGameFinished.notify_one();
			}
			else
			{
				// User denied destroy for some reason, so continue running
				m_bAtomActive = true;
			}
		}
	}

public:
	// User MUST OVERRIDE THESE!!
	virtual bool OnUserCreate() = 0;
	virtual bool OnUserUpdate() = 0;

	// Optional for clean up
	virtual bool OnUserDestroy() { return true; }

protected: // Audio Engine =====================================================================

	class olcAudioSample
	{
	public:
		olcAudioSample()
		{
		}

		olcAudioSample(std::wstring sWavFile)
		{
			// Load Wav file and convert to float format
			FILE* f = nullptr;
			_wfopen_s(&f, sWavFile.c_str(), L"rb");
			if (f == nullptr)
				return;

			char dump[4];
			std::fread(&dump, sizeof(char), 4, f); // Read "RIFF"
			if (strncmp(dump, "RIFF", 4) != 0) return;
			std::fread(&dump, sizeof(char), 4, f); // Not Interested
			std::fread(&dump, sizeof(char), 4, f); // Read "WAVE"
			if (strncmp(dump, "WAVE", 4) != 0) return;

			// Read Wave description chunk
			std::fread(&dump, sizeof(char), 4, f); // Read "fmt "
			std::fread(&dump, sizeof(char), 4, f); // Not Interested
			std::fread(&wavHeader, sizeof(WAVEFORMATEX) - 2, 1, f); // Read Wave Format Structure chunk
																	// Note the -2, because the structure has 2 bytes to indicate its own size
																	// which are not in the wav file

			// Just check if wave format is compatible with olcCGE
			if (wavHeader.wBitsPerSample != 16 || wavHeader.nSamplesPerSec != 44100)
			{
				std::fclose(f);
				return;
			}

			// Search for audio data chunk
			long nChunksize = 0;
			std::fread(&dump, sizeof(char), 4, f); // Read chunk header
			std::fread(&nChunksize, sizeof(long), 1, f); // Read chunk size
			while (strncmp(dump, "data", 4) != 0)
			{
				// Not audio data, so just skip it
				std::fseek(f, nChunksize, SEEK_CUR);
				std::fread(&dump, sizeof(char), 4, f);
				std::fread(&nChunksize, sizeof(long), 1, f);
			}

			// Finally got to data, so read it all in and convert to float samples
			nSamples = nChunksize / (wavHeader.nChannels * (wavHeader.wBitsPerSample >> 3));
			nChannels = wavHeader.nChannels;

			// Create floating point buffer to hold audio sample
			fSample = new float[nSamples * nChannels];
			float* pSample = fSample;

			// Read in audio data and normalise
			for (long i = 0; i < nSamples; i++)
			{
				for (int c = 0; c < nChannels; c++)
				{
					short s = 0;
					std::fread(&s, sizeof(short), 1, f);
					*pSample = (float)s / (float)(MAXSHORT);
					pSample++;
				}
			}

			// All done, flag sound as valid
			std::fclose(f);
			bSampleValid = true;
		}

		WAVEFORMATEX wavHeader;
		float* fSample = nullptr;
		long nSamples = 0;
		int nChannels = 0;
		bool bSampleValid = false;
	};

	// This vector holds all loaded sound samples in memory
	std::vector<olcAudioSample> vecAudioSamples;

	struct BeepSample
	{
	public:
		float freq = 0;
		float dur = 0;
		bool finished = false;
	};
	std::list<BeepSample> listActiveBeepSamples;

	// This structure represents a sound that is currently playing. It only
	// holds the sound ID and where this instance of it is up to for its
	// current playback
	struct sCurrentlyPlayingSample
	{
		int nAudioSampleID = 0;
		long nSamplePosition = 0;
		bool bFinished = false;
		bool bLoop = false;
	};
	std::list<sCurrentlyPlayingSample> listActiveSamples;

	// Load a 16-bit WAVE file @ 44100Hz ONLY into memory. A sample ID
	// number is returned if successful, otherwise -1
	unsigned int LoadAudioSample(std::wstring sWavFile)
	{
		if (!m_bEnableSound)
			return -1;

		olcAudioSample a(sWavFile);
		if (a.bSampleValid)
		{
			vecAudioSamples.push_back(a);
			return vecAudioSamples.size();
		}
		else
			return -1;
	}

	void PlayBeep(float freq, float duration) {
		BeepSample a;
		a.freq = freq;
		a.dur = duration;
		listActiveBeepSamples.push_back(a);
	}

	// Add sample 'id' to the mixers sounds to play list
	void PlaySample(int id, bool bLoop = false)
	{
		sCurrentlyPlayingSample a;
		a.nAudioSampleID = id;
		a.nSamplePosition = 0;
		a.bFinished = false;
		a.bLoop = bLoop;
		listActiveSamples.push_back(a);
	}

	void StopSample(int id)
	{
	}

	// The audio system uses by default a specific wave format
	bool CreateAudio(unsigned int nSampleRate = 44100, unsigned int nChannels = 1,
		unsigned int nBlocks = 8, unsigned int nBlockSamples = 512)
	{
		// Initialise Sound Engine
		m_bAudioThreadActive = false;
		m_nSampleRate = nSampleRate;
		m_nChannels = nChannels;
		m_nBlockCount = nBlocks;
		m_nBlockSamples = nBlockSamples;
		m_nBlockFree = m_nBlockCount;
		m_nBlockCurrent = 0;
		m_pBlockMemory = nullptr;
		m_pWaveHeaders = nullptr;

		// Device is available
		WAVEFORMATEX waveFormat;
		waveFormat.wFormatTag = WAVE_FORMAT_PCM;
		waveFormat.nSamplesPerSec = m_nSampleRate;
		waveFormat.wBitsPerSample = sizeof(short) * 8;
		waveFormat.nChannels = m_nChannels;
		waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
		waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
		waveFormat.cbSize = 0;

		// Open Device if valid
		if (waveOutOpen(&m_hwDevice, WAVE_MAPPER, &waveFormat, (DWORD_PTR)waveOutProcWrap, (DWORD_PTR)this, CALLBACK_FUNCTION) != S_OK)
			return DestroyAudio();

		// Allocate Wave|Block Memory
		m_pBlockMemory = new short[m_nBlockCount * m_nBlockSamples];
		if (m_pBlockMemory == nullptr)
			return DestroyAudio();
		ZeroMemory(m_pBlockMemory, sizeof(short) * m_nBlockCount * m_nBlockSamples);

		m_pWaveHeaders = new WAVEHDR[m_nBlockCount];
		if (m_pWaveHeaders == nullptr)
			return DestroyAudio();
		ZeroMemory(m_pWaveHeaders, sizeof(WAVEHDR) * m_nBlockCount);

		// Link headers to block memory
		for (unsigned int n = 0; n < m_nBlockCount; n++)
		{
			m_pWaveHeaders[n].dwBufferLength = m_nBlockSamples * sizeof(short);
			m_pWaveHeaders[n].lpData = (LPSTR)(m_pBlockMemory + (n * m_nBlockSamples));
		}

		m_bAudioThreadActive = true;
		m_AudioThread = std::thread(&olcConsoleGameEngine::AudioThread, this);

		// Start the ball rolling with the sound delivery thread
		std::unique_lock<std::mutex> lm(m_muxBlockNotZero);
		m_cvBlockNotZero.notify_one();
		return true;
	}

	// Stop and clean up audio system
	bool DestroyAudio()
	{
		m_bAudioThreadActive = false;
		return false;
	}

	// Handler for soundcard request for more data
	void waveOutProc(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwParam1, DWORD dwParam2)
	{
		if (uMsg != WOM_DONE) return;
		m_nBlockFree++;
		std::unique_lock<std::mutex> lm(m_muxBlockNotZero);
		m_cvBlockNotZero.notify_one();
	}

	// Static wrapper for sound card handler
	static void CALLBACK waveOutProcWrap(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
	{
		((olcConsoleGameEngine*)dwInstance)->waveOutProc(hWaveOut, uMsg, dwParam1, dwParam2);
	}

	// Audio thread. This loop responds to requests from the soundcard to fill 'blocks'
	// with audio data. If no requests are available it goes dormant until the sound
	// card is ready for more data. The block is fille by the "user" in some manner
	// and then issued to the soundcard.
	void AudioThread()
	{
		m_fGlobalTime = 0.0f;
		float fTimeStep = 1.0f / (float)m_nSampleRate;

		// Goofy hack to get maximum integer for a type at run-time
		short nMaxSample = (short)pow(2, (sizeof(short) * 8) - 1) - 1;
		float fMaxSample = (float)nMaxSample;
		short nPreviousSample = 0;

		while (m_bAudioThreadActive)
		{
			// Wait for block to become available
			if (m_nBlockFree == 0)
			{
				std::unique_lock<std::mutex> lm(m_muxBlockNotZero);
				while (m_nBlockFree == 0) // sometimes, Windows signals incorrectly
					m_cvBlockNotZero.wait(lm);
			}

			// Block is here, so use it
			m_nBlockFree--;

			// Prepare block for processing
			if (m_pWaveHeaders[m_nBlockCurrent].dwFlags & WHDR_PREPARED)
				waveOutUnprepareHeader(m_hwDevice, &m_pWaveHeaders[m_nBlockCurrent], sizeof(WAVEHDR));

			short nNewSample = 0;
			int nCurrentBlock = m_nBlockCurrent * m_nBlockSamples;

			auto clip = [](float fSample, float fMax)
			{
				if (fSample >= 0.0)
					return fmin(fSample, fMax);
				else
					return fmax(fSample, -fMax);
			};

			for (unsigned int n = 0; n < m_nBlockSamples; n += m_nChannels)
			{
				// User Process
				for (unsigned int c = 0; c < m_nChannels; c++)
				{
					nNewSample = (short)(clip(GetMixerOutput(c, m_fGlobalTime, fTimeStep), 1.0) * fMaxSample);
					m_pBlockMemory[nCurrentBlock + n + c] = nNewSample;
					nPreviousSample = nNewSample;
				}

				m_fGlobalTime = m_fGlobalTime + fTimeStep;
			}

			// Send block to sound device
			waveOutPrepareHeader(m_hwDevice, &m_pWaveHeaders[m_nBlockCurrent], sizeof(WAVEHDR));
			waveOutWrite(m_hwDevice, &m_pWaveHeaders[m_nBlockCurrent], sizeof(WAVEHDR));
			m_nBlockCurrent++;
			m_nBlockCurrent %= m_nBlockCount;
		}
	}

	// Overridden by user if they want to generate sound in real-time
	virtual float onUserSoundSample(int nChannel, float fGlobalTime, float fTimeStep)
	{
		return 0.0f;
	}

	// Overriden by user if they want to manipulate the sound before it is played
	virtual float onUserSoundFilter(int nChannel, float fGlobalTime, float fSample)
	{
		return fSample;
	}

	// The Sound Mixer - If the user wants to play many sounds simultaneously, and
	// perhaps the same sound overlapping itself, then you need a mixer, which
	// takes input from all sound sources for that audio frame. This mixer maintains
	// a list of sound locations for all concurrently playing audio samples. Instead
	// of duplicating audio data, we simply store the fact that a sound sample is in
	// use and an offset into its sample data. As time progresses we update this offset
	// until it is beyound the length of the sound sample it is attached to. At this
	// point we remove the playing souind from the list.
	//
	// Additionally, the users application may want to generate sound instead of just
	// playing audio clips (think a synthesizer for example) in whcih case we also
	// provide an "onUser..." event to allow the user to return a sound for that point
	// in time.
	//
	// Finally, before the sound is issued to the operating system for performing, the
	// user gets one final chance to "filter" the sound, perhaps changing the volume
	// or adding funky effects
	float GetMixerOutput(int nChannel, float fGlobalTime, float fTimeStep)
	{
		// Accumulate sample for this channel
		float fMixerSample = 0.0f;

		for (auto& s : listActiveBeepSamples)
		{
			Beep(s.freq, s.dur);
			s.finished = true;
		}

		//listActiveBeepSamples.clear();

		listActiveBeepSamples.remove_if([](const BeepSample& s) {return s.finished; });

		for (auto& s : listActiveSamples)
		{
			// Calculate sample position
			s.nSamplePosition += (long)((float)vecAudioSamples[s.nAudioSampleID - 1].wavHeader.nSamplesPerSec * fTimeStep);

			// If sample position is valid add to the mix
			if (s.nSamplePosition < vecAudioSamples[s.nAudioSampleID - 1].nSamples)
				fMixerSample += vecAudioSamples[s.nAudioSampleID - 1].fSample[(s.nSamplePosition * vecAudioSamples[s.nAudioSampleID - 1].nChannels) + nChannel];
			else
				s.bFinished = true; // Else sound has completed
		}

		// If sounds have completed then remove them
		listActiveSamples.remove_if([](const sCurrentlyPlayingSample& s) {return s.bFinished; });

		// The users application might be generating sound, so grab that if it exists
		fMixerSample += onUserSoundSample(nChannel, fGlobalTime, fTimeStep);

		// Return the sample via an optional user override to filter the sound
		return onUserSoundFilter(nChannel, fGlobalTime, fMixerSample);
	}

	unsigned int m_nSampleRate;
	unsigned int m_nChannels;
	unsigned int m_nBlockCount;
	unsigned int m_nBlockSamples;
	unsigned int m_nBlockCurrent;

	short* m_pBlockMemory = nullptr;
	WAVEHDR* m_pWaveHeaders = nullptr;
	HWAVEOUT m_hwDevice = nullptr;

	std::thread m_AudioThread;
	std::atomic<bool> m_bAudioThreadActive = false;
	std::atomic<unsigned int> m_nBlockFree = 0;
	std::condition_variable m_cvBlockNotZero;
	std::mutex m_muxBlockNotZero;
	std::atomic<float> m_fGlobalTime = 0.0f;

protected:

	struct sKeyState
	{
		bool bPressed;
		bool bReleased;
		bool bHeld;
	} m_keys[256], m_mouse[5];

public:
	int mousePosX;
	int mousePosY;

	int screenWidth;
	int screenHeight;

	sKeyState GetKey(int nKeyID) { return m_keys[nKeyID]; }
	sKeyState GetMouse(int nMouseButtonID) { return m_mouse[nMouseButtonID]; }
	bool IsFocused() { return m_bConsoleInFocus; }

protected:
	int Error(const wchar_t* msg)
	{
		wchar_t buf[256];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, 256, NULL);
		SetConsoleActiveScreenBuffer(m_hOriginalConsole);
		wprintf(L"ERROR: %s\n\t%s\n", msg, buf);
		return 0;
	}

	static BOOL CloseHandler(DWORD evt)
	{
		// Note this gets called in a seperate OS thread, so it must
		// only exit when the game has finished cleaning up, or else
		// the process will be killed before OnUserDestroy() has finished
		if (evt == CTRL_CLOSE_EVENT)
		{
			m_bAtomActive = false;

			// Wait for thread to be exited
			std::unique_lock<std::mutex> ul(m_muxGame);
			m_cvGameFinished.wait(ul);
		}
		return true;
	}

public:
	olcSprite* fontSprite;

	byte fadeSpeed = 255;
	int bufferSize;
	short frameCount = 0;
	CHAR_INFO* m_bufScreen;
	std::wstring m_appName;
	std::wstring m_appInfo;

	HANDLE m_hOriginalConsole;
	CONSOLE_SCREEN_BUFFER_INFO m_OriginalConsoleInfo;
	HANDLE m_hConsole;
	HANDLE m_hConsoleIn;
	SMALL_RECT m_rectWindow;
	short m_keyOldState[256] = { 0 };
	short m_keyNewState[256] = { 0 };
	bool m_mouseOldState[5] = { 0 };
	bool m_mouseNewState[5] = { 0 };
	bool m_bConsoleInFocus = true;
	bool m_bEnableSound = false;

	// These need to be static because of the OnDestroy call the OS may make. The OS
	// spawns a special thread just for that
	static std::atomic<bool> m_bAtomActive;
	static std::condition_variable m_cvGameFinished;
	static std::mutex m_muxGame;
};

// Define our static variables
std::atomic<bool> olcConsoleGameEngine::m_bAtomActive(false);
std::condition_variable olcConsoleGameEngine::m_cvGameFinished;
std::mutex olcConsoleGameEngine::m_muxGame;