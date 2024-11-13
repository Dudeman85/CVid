#include <cvid/Rasterizer.h>
#include <cvid/Math.h>
#include <cvid/Types.h>

#define SWAP(a, b) {auto tmp = a; a = b; b = tmp;}

namespace cvid
{
	//Draw a point onto a window's framebuffer
	void RasterizePoint(Window* window, Vector3 pt, ConsoleColor color)
	{
		//Convert to window coords
		Vector3 halfWindow(window->GetDimensions() / 2, 1);
		pt = pt * halfWindow;
		//Convert to window coords
		pt.y = -pt.y;
		pt += Vector3(window->GetDimensions() / 2);

		//Attempt to draw the pixel
		window->PutPixel(pt.x, pt.y, color, pt.z);
	}

	//Draw a line onto a window's framebuffer
	void RasterizeLine(Window* window, Vector3 p1f, Vector3 p2f, ConsoleColor color)
	{
		//Convert to window coords
		Vector3 halfWindow(window->GetDimensions() / 2, 1);
		p1f = p1f * halfWindow;
		p2f = p2f * halfWindow;
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
			{
				SWAP(p1, p2);
				SWAP(p1f, p2f);
			}

			//Interpolate for z positions
			std::vector<float> zPositions = LerpRange(abs(p2.x - p1.x), p1f.z, p2f.z);

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
			int i = 0;
			for (int x = p1.x; x <= p2.x; x++)
			{
				//Attempt to draw the pixel
				window->PutPixel(x, y, color, zPositions[i]);

				//Increase y error
				error += 2 * dy;
				if (error > abs(dx))
				{
					y += yi;
					error -= 2 * dx;
				} 
				i++;
			}
		}
		//Slope is > 1
		else
		{
			//Make sure starting point is before ending point
			if (p1.y > p2.y)
			{
				SWAP(p1, p2);
				SWAP(p1f, p2f);
			}

			//Interpolate for z positions
			std::vector<float> zPositions = LerpRange(abs(p2.y - p1.y), p1f.z, p2f.z);

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
			int i = 0;
			for (int y = p1.y; y <= p2.y; y++)
			{
				//Attempt to draw the pixel
				window->PutPixel(x, y, color, zPositions[i]);

				//Increase error in x
				error += 2 * dx;
				if (error > abs(dy))
				{
					x += xi;
					error -= 2 * dy;
				}
				i++;
			}
		}
	}

	//Get the integer x coordinates of a line at every y point
	//Based on Bresenham's algorithm
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
	//Expects vertices in normalized device coordinates
	void RasterizeTriangle(Window* window, Face tri, const Material* mat)
	{
		//TODO: optimize this out maybe, (or not lol this is totally permanent)
		RasterizeTriangleWireframe(window, tri.vertices.v1, tri.vertices.v2, tri.vertices.v3, ConsoleColor::Red);

		//Convert from ndc to window coords
		Vector3 windowHalfSize(window->GetDimensions() / 2, 1);
		tri.vertices.v1 *= windowHalfSize;
		tri.vertices.v2 *= windowHalfSize;
		tri.vertices.v3 *= windowHalfSize;
		Vector2Int p1 = tri.vertices.v1;
		Vector2Int p2 = tri.vertices.v2;
		Vector2Int p3 = tri.vertices.v3;
		p1.y = -p1.y;
		p2.y = -p2.y;
		p3.y = -p3.y;
		p1 += windowHalfSize;
		p2 += windowHalfSize;
		p3 += windowHalfSize;

		//Sort the vertices in descending order
		if (p1.y < p2.y)
		{
			SWAP(tri.vertices.v1, tri.vertices.v2);
			SWAP(p1, p2);
		}
		if (p1.y < p3.y)
		{
			SWAP(tri.vertices.v1, tri.vertices.v3);
			SWAP(p1, p3);
		}
		if (p2.y < p3.y)
		{
			SWAP(tri.vertices.v2, tri.vertices.v3);
			SWAP(p2, p3);
		}

		//Get every x point of each segment
		std::vector<int> combinedSegment = InterpolateX(p2, p3);
		combinedSegment.pop_back();
		std::vector<int> shortSegment = InterpolateX(p1, p2);
		combinedSegment.insert(combinedSegment.end(), shortSegment.begin(), shortSegment.end());
		std::vector<int> fullSegment = InterpolateX(p1, p3);

		//Interpolate for z positions along the left and right segments
		std::vector<float> combinedZPositions = LerpRange(abs(p3.y - p2.y), tri.vertices.v3.z, tri.vertices.v2.z);
		std::vector<float> shortZPositions = LerpRange(abs(p2.y - p1.y), tri.vertices.v2.z, tri.vertices.v1.z);
		combinedZPositions.insert(combinedZPositions.end(), shortZPositions.begin(), shortZPositions.end());
		std::vector<float> fullZPositions = LerpRange(abs(p3.y - p1.y), tri.vertices.v3.z, tri.vertices.v1.z);

		//Figure out which segment is on which side
		std::vector<int>* rightSegment = &combinedSegment;
		std::vector<int>* leftSegment = &fullSegment;
		std::vector<float>* rightZPositions = &combinedZPositions;
		std::vector<float>* leftZPositions = &fullZPositions;
		if (leftSegment->at(std::floor(leftSegment->size() / 2)) > rightSegment->at(std::floor(rightSegment->size() / 2)))
		{
			leftSegment = &combinedSegment;
			rightSegment = &fullSegment;
			leftZPositions = &combinedZPositions;
			rightZPositions = &fullZPositions;
		}

		int startY = (int)std::round(p3.y);
		//For each y coordinate in the triangle
		for (int yi = 0; yi < fullSegment.size(); yi++)
		{
			//Interpolate for z positions for each horizontal scanline
			std::vector<float> zPositions = LerpRange(abs(leftSegment->at(yi) - rightSegment->at(yi)), leftZPositions->at(yi), rightZPositions->at(yi));

			//Draw a line from the full segment to the split segment
			int i = 0;
			for (int x = leftSegment->at(yi); x <= rightSegment->at(yi); x++)
			{
				//Attempt to draw the pixel
				window->PutPixel(x, startY + yi, ConsoleColor::Red, zPositions[i]);
				i++;
			}
		}
	}

	//Draw a wireframe triangle onto a window's framebuffer
	void RasterizeTriangleWireframe(Window* window, Vector3 p1, Vector3 p2, Vector3 p3, ConsoleColor color)
	{
		RasterizeLine(window, p1, p2, color);
		RasterizeLine(window, p2, p3, color);
		RasterizeLine(window, p1, p3, color);
	}
}