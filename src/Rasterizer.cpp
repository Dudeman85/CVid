#include <cvid/Rasterizer.h>

#define SWAP(a, b) {auto tmp = a; a = b; b = tmp;}

namespace cvid
{
	//Draw a point onto a window's framebuffer
	void DrawPoint(Window* window, Vector2Int pt, Color color)
	{
		//Convert to window coords
		pt.y = -pt.y;
		pt += window->GetDimensions() / 2;
		window->PutPixel(pt, color);
	}

	//Draw a line onto a window's framebuffer
	void DrawLine(Window* window, Vector2Int p1, Vector2Int p2, Color color)
	{
		int dx = p2.x - p1.x;
		int dy = p2.y - p1.y;

		//Slope is < 1
		if (abs(dx) > abs(dy))
		{
			//Make sure starting point is before ending point
			if (p1.x > p2.x)
				SWAP(p1, p2);

			dx = p2.x - p1.x;
			dy = p2.y - p1.y;

			//If slope is positive increment y, else decrement
			int yi = 1;
			if (dy < 0)
			{
				yi = -1;
				dy = -dy;
			}

			//Keep track of closest y and the error to actual y
			int y = p1.y;
			int error = 0;

			//For each x position, plot the corresponding y
			for (int x = p1.x; x <= p2.x; x++)
			{
				DrawPoint(window, { x, y }, color);

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
			//Make sure starting point is before ending point
			if (p1.y > p2.y)
				SWAP(p1, p2);

			dx = p2.x - p1.x;
			dy = p2.y - p1.y;

			//If slope is positive increment x, else decrement
			int xi = 1;
			if (dx < 0)
			{
				xi = -1;
				dx = -dx;
			}

			//Keep track of closest x and the error to actual x
			int x = p1.x;
			int error = 0;

			//For each y position, plot the corresponding x
			for (int y = p1.y; y <= p2.y; y++)
			{
				DrawPoint(window, { x, y }, color);

				error += 2 * dx;
				if (error > abs(dy))
				{
					x += xi;
					error -= 2 * dy;
				}
			}
		}
	}

	//Get the x coordinates of a line at every y point
	std::vector<int> InterpolateX(Vector2Int p1, Vector2Int p2)
	{
		int dx = p2.x - p1.x;
		int dy = p2.y - p1.y;

		std::vector<int> points;
		points.reserve(abs(dy));

		int x;

		//Slope is < 1
		if (abs(dx) > abs(dy))
		{
			//Make sure starting point is before ending point
			if (p1.x > p2.x)
				SWAP(p1, p2);

			dx = p2.x - p1.x;
			dy = p2.y - p1.y;

			//If slope is positive increment y, else decrement
			int yi = 1;
			if (dy < 0)
			{
				yi = -1;
				dy = -dy;
			}

			//Keep track of closest y and the error to actual y
			int y = p1.y;
			int error = 0;

			//For each x position
			for (x = p1.x; x < p2.x; x++)
			{
				error += 2 * dy;
				if (error > abs(dx))
				{
					points.push_back(x);

					y += yi;
					error -= 2 * dx;
				}
			}
			//Make sure the list is in ascending order
			if (yi < 0)
			{
				if (points.size() <= dy)
					points.push_back(x);
				std::reverse(points.begin(), points.end());
			}
		}
		//Slope is > 1
		else
		{
			//Make sure starting point is before ending point
			if (p1.y > p2.y)
				SWAP(p1, p2);

			dx = p2.x - p1.x;
			dy = p2.y - p1.y;

			//If slope is positive increment x, else decrement
			int xi = 1;
			if (dx < 0)
			{
				xi = -1;
				dx = -dx;
			}

			//Keep track of closest x and the error to actual x
			x = p1.x;
			int error = 0;

			//For each y position
			for (int y = p1.y; y < p2.y; y++)
			{
				points.push_back(x);

				error += 2 * dx;
				if (error > abs(dy))
				{
					x += xi;
					error -= 2 * dy;
				}
			}
		}
		//Add final x if it did not change before end
		if (points.size() <= dy)
			points.push_back(x);

		return points;
	}

	//Draw a triangle onto a window's framebuffer
	void DrawTriangle(Window* window, Vector2 p1, Vector2 p2, Vector2 p3, Color color)
	{
		//Sort the vertices in descending order
		if (p1.y > p2.y)
			SWAP(p1, p2);
		if (p1.y > p3.y)
			SWAP(p1, p3);
		if (p2.y > p3.y)
			SWAP(p2, p3);
		if (p1.y < p2.y)
			SWAP(p1, p2);
		if (p1.y < p3.y)
			SWAP(p1, p3);
		if (p2.y < p3.y)
			SWAP(p2, p3);

		//Get every x point of each segment
		std::vector<int> combinedSegment = InterpolateX(p2, p3);
		combinedSegment.pop_back();
		std::vector<int> shortSegment = InterpolateX(p1, p2);
		combinedSegment.insert(combinedSegment.end(), shortSegment.begin(), shortSegment.end());
		std::vector<int> fullSegment = InterpolateX(p1, p3);

		//Figure out which segment is on which side
		std::vector<int> rightSegment = combinedSegment;
		std::vector<int> leftSegment = fullSegment;
		if (leftSegment[std::floor(leftSegment.size() / 2)] > rightSegment[std::floor(rightSegment.size() / 2)])
		{
			leftSegment = combinedSegment;
			rightSegment = fullSegment;
		}

		//TODO: optimize this out
		DrawPoint(window, p1, Color::BrightMagenta);
		DrawPoint(window, p2, Color::BrightMagenta);
		DrawPoint(window, p3, Color::BrightMagenta);
		DrawTriangleWireframe(window, p1, p2, p3, Color::BrightCyan);

		int startY = (int)std::round(p3.y);
		//For each y coordinate in the triangle
		for (int yi =0; yi < fullSegment.size(); yi++)
		{
			//Draw a line from the full segment to the split segment
			for (int x = leftSegment[yi]; x <= rightSegment[yi]; x++)
			{
				DrawPoint(window, { x, startY + yi }, color);
			}
		}
	}

	//Draw a wireframe triangle onto a window's framebuffer
	void DrawTriangleWireframe(Window* window, Vector2 p1, Vector2 p2, Vector2 p3, Color color)
	{
		DrawLine(window, p1, Vector2Int(p2), color);
		DrawLine(window, Vector2Int(p2), p3, color);
		DrawLine(window, p1, p3, color);
	}
}