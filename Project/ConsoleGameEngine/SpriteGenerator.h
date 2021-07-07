#pragma once
#include <iostream>
#include <chrono>
#include <fstream>
#include <string>

class SpriteApp : public olcConsoleGameEngine
{
private:
	std::wstring GetDefaultSprite(int no)
	{
		switch (no)
		{
		case 0:
			return L"................................................................................................................................................";
		default:
			break;
		}

		return L"";
	}

	void MakeFile(std::string fileName, std::string contents) {
		std::srand(std::time(NULL));
		std::ofstream fileStream;
		fileStream.open(fileName);
		double size;

		if (fileStream.is_open()) {
			std::string data = "";

			for (size_t u = 0; u < contents.length(); u++)
			{
				data += contents[u];
			}

			fileStream << data;
		}

		fileStream.close();
	}

	char* spriteData;

	std::wstring text;

	PIXEL_TYPE selectedType;
	int t;

	int width = 12;
	int height = 12;

public:

	virtual bool OnUserCreate()
	{
		selectedType = PIXEL_TYPE::PIXEL_SOLID;
		spriteData = new char[width * height];

		auto data = GetDefaultSprite(0);

		for (size_t i = 0; i < width * height; i++)
		{
			if (data[i] == '#') {
				spriteData[i] = PIXEL_SOLID;
			}
			else {
				spriteData[i] = data[i];
			}
		}

		return true;
	}

	virtual bool OnUserUpdate()
	{
		int offset = 10;

		int mouseY = mousePosY;
		int mouseX = mousePosX;

		SetPixel(mouseX, mouseY, t);

		for (size_t i = 0; i < width * height; i++)
		{
			int ix = i % width;
			int iy = i / height;

			byte color = FG_BLUE;
			int pixelType = PIXEL_SOLID;

			if (spriteData[i] != '.')
			{
				color = FG_RED;

				switch ((char)spriteData[i])
				{
				case (char)'4':
					pixelType = PIXEL_SOLID;
					break;
				case (char)'3':
					pixelType = PIXEL_THREEQUARTERS;
					break;
				case (char)'2':
					pixelType = PIXEL_HALF;
					break;
				case (char)'1':
					pixelType = PIXEL_QUARTER;
					break;
				default:
					pixelType = 0x1; // Debug char
					break;
				}
			}

			int x = offset + ix;
			int y = offset + iy;

			if (x == mouseX && y == mouseY)
			{
				pixelType = selectedType;
				color = BG_GREEN;
			}

			Draw(x, y, pixelType, color);
		}

		// Drawing
		mouseY -= offset;
		mouseX -= offset;

		if (mouseX < width && mouseX >= 0 && mouseY < height && mouseY >= 0)
		{
			int index = mouseY * width + mouseX;

			if (GetMouse(0).bHeld)
			{
				char pixelType = ' ';

				switch (selectedType)
				{
				case PIXEL_SOLID:
					pixelType = '4';
					break;
				case PIXEL_THREEQUARTERS:
					pixelType = '3';
					break;
				case PIXEL_HALF:
					pixelType = '2';
					break;
				case PIXEL_QUARTER:
					pixelType = '1';
					break;
				default:
					pixelType = 0x1; // Debug char
					break;
				}

				spriteData[index] = pixelType;
			}

			if (GetMouse(1).bHeld)
			{
				spriteData[index] = '.';
			}
		}

		if (GetKey(Key::SPACE).bReleased)
		{
			std::string data;

			for (size_t i = 0; i < width * height; i++)
			{
				switch ((char)spriteData[i])
				{
				case (char)PIXEL_SOLID:
					data += '4';
					break;
				case (char)PIXEL_THREEQUARTERS:
					data += '3';
					break;
				case (char)PIXEL_HALF:
					data += '2';
					break;
				case (char)PIXEL_QUARTER:
					data += '1';
					break;
				default:
					data += spriteData[i];
					break;
				}
			}

			//C:\\Users\\frres\\OneDrive\\Desktop\\sprite

			MakeFile("C:\\Users\\frres\\OneDrive\\Desktop\\sprite.txt", data);
		}

		for (size_t i = 0; i < 100; i++)
		{
			if (GetKey(i).bHeld) {
				text = std::to_wstring((Key)i);
			}
		}

		if (GetKey(Key::ESCAPE).bReleased)
		{
			for (size_t i = 0; i < width * height; i++)
			{
				spriteData[i] == '.';
			}
		}

		switch (selectedType)
		{
		case PIXEL_SOLID:
			t = 0;

			break;
		case PIXEL_THREEQUARTERS:
			t = 1;

			break;
		case PIXEL_HALF:
			t = 2;

			break;
		case PIXEL_QUARTER:
			t = 3;

			break;
		}

		if (GetKey(Key::UP).bReleased)
		{
			switch (selectedType)
			{
			case PIXEL_SOLID:
				selectedType = PIXEL_QUARTER;
				SetAppInfo(L"1/4");

				break;
			case PIXEL_THREEQUARTERS:
				selectedType = PIXEL_SOLID;
				SetAppInfo(L"4/4");

				break;
			case PIXEL_HALF:
				selectedType = PIXEL_THREEQUARTERS;
				SetAppInfo(L"3/4");

				break;
			case PIXEL_QUARTER:
				selectedType = PIXEL_HALF;
				SetAppInfo(L"2/4");

				break;
			}
		}

		if (GetKey(Key::DOWN).bReleased)
		{
			switch (selectedType)
			{
			case PIXEL_SOLID:
				selectedType = PIXEL_THREEQUARTERS;
				SetAppInfo(L"3/4");

				break;
			case PIXEL_THREEQUARTERS:
				selectedType = PIXEL_HALF;
				SetAppInfo(L"2/4");

				break;
			case PIXEL_HALF:
				selectedType = PIXEL_QUARTER;
				SetAppInfo(L"1/4");

				break;
			case PIXEL_QUARTER:
				selectedType = PIXEL_SOLID;
				SetAppInfo(L"4/4");

				break;
			}
		}

		DrawStringToBuffer(0, 0, text, FG_RED);

		return true;
	}

	// Optional for clean up
	virtual bool OnUserDestroy()
	{
		delete[] spriteData;
		return true;
	}
};
