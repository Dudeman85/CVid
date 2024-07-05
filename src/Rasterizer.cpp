#include <cvid/Rasterizer.h>

#define SWAP(a, b) {auto tmp = a; a = b; b = tmp;}

namespace cvid
{
	//Draw a point onto a window's framebuffer
	void RasterizePoint(Window* window, Vector3 pt, Color color)
	{
		//Convert to window coords
		pt.y = -pt.y;
		pt += Vector3(window->GetDimensions() / 2);
		window->PutPixel(Vector2Int(pt), color, pt.z);
	}

	//Draw a line onto a window's framebuffer
	void RasterizeLine(Window* window, Vector3 p1f, Vector3 p2f, Color color)
	{
		//Convert to window coords
		Vector2Int windowHalfSize = window->GetDimensions() / 2;
		Vector2Int p1 = p1f;
		Vector2Int p2 = p2f;
		p1.y = -p1.y;
		p1 += windowHalfSize;
		p2.y = -p2.y;
		p2 += windowHalfSize;

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
		//TODO: this function needs to be updated to work with depth buffer
	void RasterizeTriangle(Window* window, Vector3 p1f, Vector3 p2f, Vector3 p3f, Color color)
	{
		//TODO: optimize this out maybe, or not lol this is totally permanent
		RasterizeTriangleWireframe(window, p1f, p2f, p3f, color);

		//Convert to window coords
		Vector2Int windowHalfSize = window->GetDimensions() / 2;
		Vector2Int p1 = p1f;
		Vector2Int p2 = p2f;
		Vector2Int p3 = p3f;
		p1.y = -p1.y;
		p1 += windowHalfSize;
		p2.y = -p2.y;
		p2 += windowHalfSize;
		p3.y = -p3.y;
		p3 += windowHalfSize;

		//Sort the vertices in descending order
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

		int startY = (int)std::round(p3.y);
		//For each y coordinate in the triangle
		for (int yi = 0; yi < fullSegment.size(); yi++)
		{
			//Draw a line from the full segment to the split segment
			for (int x = leftSegment[yi]; x <= rightSegment[yi]; x++)
			{
				window->PutPixel(x, startY + yi, color);
			}
		}
	}

	//Draw a wireframe triangle onto a window's framebuffer
	void RasterizeTriangleWireframe(Window* window, Vector3 p1, Vector3 p2, Vector3 p3, Color color)
	{
		RasterizeLine(window, p1, p2, color);
		RasterizeLine(window, p2, p3, color);
		RasterizeLine(window, p1, p3, color);
	}
}