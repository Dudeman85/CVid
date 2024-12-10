#include <cvid/Window.h>
#include <cvid/Model.h>

#define SWAP(a, b) {auto tmp = a; a = b; b = tmp;}

void DrawLineBad(cvid::Window& w, int x0, int y0, int x1, int y1)
{
	float m = (float)(y1 - y0) / (x1 - x0);
	float y = y0;
	for (size_t x = x0; x <= x1; x++)
	{
		w.PutPixel(x, std::round(y), { 0, 0, 0 });
		y += m;
	}
}

void DrawLine(cvid::Window& w, int x0, int y0, int x1, int y1)
{
	if (abs(x1 - x0) > abs(y1 - y0))
	{
		if (x0 > x1)
		{
			SWAP(x0, x1);
			SWAP(y0, y1);
		}
		float m = (float)(y1 - y0) / (x1 - x0);
		float y = y0;
		for (size_t x = x0; x <= x1; x++)
		{
			w.PutPixel(x, std::round(y), { 0, 0, 0 });
			y += m;
		}
	}
	else
	{
		if (y0 > y1)
		{
			SWAP(x0, x1);
			SWAP(y0, y1);
		}
		float m = (float)(x1 - x0) / (y1 - y0);
		float x = x0;
		for (size_t y = y0; y <= y1; y++)
		{
			w.PutPixel(x, std::round(y), { 0, 0, 0 });
			x += m;
		}
	}
}

int main()
{
	cvid::Window window(64, 64, "CVid");
	window.enableDepthTest = false;

	cvid::Model cube("../../../resources/cube.obj");

	cvid::ModelInstance cubeInstance(&cube);
	cubeInstance.SetScale(20);
	cubeInstance.SetPosition({ 0, 0, 0 });
	cubeInstance.SetRotation({ 0, cvid::Radians(0), 0 });

	while (true)
	{
		if (GetKeyState(VK_ESCAPE) & 0x8000)
			return 0;

		window.Fill({240, 240, 240});
		window.ClearDepthBuffer();



		DrawLine(window, 0, 63, 32, 0);


		if (!window.DrawFrame())
			return 0;
	}

	return 0;
}