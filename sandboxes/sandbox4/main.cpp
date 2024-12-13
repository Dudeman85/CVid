#include <cvid/Window.h>
#include <cvid/Renderer.h>
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
	//Make a console window with width, height, and name
	cvid::Window window(64, 64, "CVid");

	//Make the camera with {x, y, z}, width, height
	cvid::Camera cam({ 0, 0, 100 }, 64, 64);
	//Set it as perspective with fov, near, and far
	cam.MakePerspective(90, 1, 100);

	//Load a model from file
	cvid::Model cube("../../../resources/cube.obj");
	//Create an instance of the model and change its transform
	cvid::ModelInstance cubeInstance(&cube);
	cubeInstance.SetScale(20);
	cubeInstance.SetPosition({ 10, 20, 0 });
	//Rotations use radians
	cubeInstance.SetRotation({ 0, cvid::Radians(45), 0 });

	//Render loop
	while (true)
	{
		//Fill the canvas with some rgb value at the start of frame
		window.Fill({0, 0, 0});
		window.ClearDepthBuffer();

		//Draw the model instance to the window's canvas
		cvid::DrawModel(&cubeInstance, &cam, &window);

		//Draw the frame to the window, end the program on failure
		if (!window.DrawFrame())
			return 0;
	}

	return 0;
}