#include <cvid/Window.h>

void DrawLine(cvid::Window& w, int x0, int y0, int x1, int y1) 
{
	float m = (float)(y1 - y0) / (x1 - x0);
	float y = y0;

	for (size_t x = x0; x <= x1; x++)
	{
		w.PutPixel(x, std::round(y), {0, 0, 0});
		y += m;
	}
}

int main()
{
	cvid::Window window(64, 64, "CVid");
	window.enableDepthTest = false;

	window.SetPalette(cvid::cmdDefault);

	while (true)
	{
		if (GetKeyState(VK_ESCAPE) & 0x8000)
			return 0;

		window.Fill(cvid::ConsoleColor::BrightWhite);
		window.ClearDepthBuffer();



		DrawLine(window, 0, 63, 32, 0);


		if (!window.DrawFrame())
			return 0;
	}

	return 0;
}