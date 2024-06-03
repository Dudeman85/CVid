#include <cvid/Rasterizer.h>

namespace cvid
{
	//Draw a line onto a window's framebuffer
	void DrawLine(Window* window, Vector2Int p1, Vector2Int p2, Color color)
	{
		int dx = p2.x - p1.x;
		int dy = p2.y - p1.y;

		//Slope is < 1
		if (abs(dx) > abs(dy))
		{
			//If slope is positive increment y, else decrement
			int yi = 1;
			if (dy < 0)
			{
				yi = -1;
				dy = -dy;
			}
			//Make sure starting point is before ending point
			if (p1.x > p2.x)
			{
				//Swap p1 and p2
				Vector2Int tmp = p2;
				p2 = p1;
				p1 = tmp;
			}

			//Keep track of closest y and the error to actual y
			int y = p1.y;
			int error = 0;

			//For each x position, plot the corresponding y
			for (int x = p1.x; x <= p2.x; x++)
			{
				window->PutPixel(x, y, color);

				error += 2 * dy;
				if (error > abs(dx))
				{
					y += yi;
					error -= 2 * dx;
				}
			}
		}
		//Slope is > 1
		else
		{
			//If slope is positive increment x, else decrement
			int xi = 1;
			if (dx < 0)
			{
				xi = -1;
				dx = -dx;
			}
			//Make sure starting point is before ending point
			if (p1.y > p2.y)
			{
				//Swap p1 and p2
				Vector2Int tmp = p2;
				p2 = p1;
				p1 = tmp;
			}

			//Keep track of closest x and the error to actual x
			int x = p1.x;
			int error = 0;

			//For each y position, plot the corresponding x
			for (int y = p1.y; y <= p2.y; y++)
			{
				window->PutPixel(x, y, color);

				error += 2 * dx;
				if (error > abs(dy))
				{
					x += xi;
					error -= 2 * dy;
				}
			}
		}
	}

	//Draw a triangle onto a window's framebuffer
	void DrawTriangle(Window* window, Vector2Int p1, Vector2Int p2, Vector2Int p3, Color color)
	{

	}

	//Draw a wireframe triangle onto a window's framebuffer
	void DrawTriangleWireframe(Window* window, Vector2Int p1, Vector2Int p2, Vector2Int p3, Color color)
	{
		DrawLine(window, p1, p2, color);
		DrawLine(window, p2, p3, color);
		DrawLine(window, p1, p3, color);
	}
}