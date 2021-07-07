#pragma once
struct UIBox {
protected:
	olcConsoleGameEngine* renderer;
	UIBox() = default;

public:
	int x, y = 0;

	int width, height;

	int outlineType = 0;
	int fillType = -1;

public:
	virtual void Draw()
	{
		for (size_t ix = 0; ix < width; ix++)
		{
			for (size_t iy = 0; iy < height; iy++)
			{
				if (ix == 0 || ix == width - 1 || iy == 0 || iy == height - 1)
				{
					renderer->SetPixel(ix + x, iy + y, outlineType);
				}
				else if (fillType != -1)
				{
					renderer->SetPixel(ix + x, iy + y, fillType);
				}
			}
		}
	}

public:
	UIBox(olcConsoleGameEngine* renderer, int x, int y, int width, int height, int outlineType = 0, int fillType = 1)
	{
		this->renderer = renderer;

		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;

		this->outlineType = outlineType;
		this->fillType = fillType;
	}
};