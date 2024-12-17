#include <cvid/Rasterizer.h>
#include <cvid/Math.h>
#include <cvid/Types.h>

#define SWAP(a, b) {auto tmp = a; a = b; b = tmp;}

namespace cvid
{
	//Difference between two attributes
	inline Attributes AttribChangePerD(Attributes a, Attributes b, int d)
	{
		return Attributes{ 0, (a.z - b.z) / d, (a.texCoord - b.texCoord) / d };
	}
	//Add two attributes together
	inline Attributes AddAttribs(Attributes a, Attributes b)
	{
		return Attributes{ a.x + b.x, a.z + b.z, a.texCoord + b.texCoord };
	}

	//Draw a point onto a window's framebuffer
	void RasterizePoint(Window* window, Vector3 pt, Color color)
	{
		//Attempt to draw the pixel
		window->PutPixel(pt.x, pt.y, color, 1 / pt.z);
	}

	//Draw a line onto a window's framebuffer
	void RasterizeLine(Window* window, Vector3 v0, Vector3 v1, Color color)
	{
		Vector2Int p0 = v0;
		Vector2Int p1 = v1;

		int dx = p1.x - p0.x;
		int dy = p1.y - p0.y;

		//Slope is < 1
		if (abs(dx) > abs(dy))
		{
			//Make sure starting point is before ending point
			if (p0.x > p1.x)
			{
				SWAP(p0, p1);
				SWAP(v0, v1);
			}

			//Interpolate for z positions
			std::vector<double> zPositions = LerpRange(p1.x, p0.x, 1 / v0.z, 1 / v1.z);

			dx = p1.x - p0.x;
			dy = p1.y - p0.y;

			//If slope is positive increment y, else decrement
			int yi = 1;
			if (dy < 0)
			{
				yi = -1;
				dy = -dy;
			}

			//Keep track of closest y and the error to actual y
			int y = p0.y;
			int error = 0;

			//For each x position, plot the corresponding y
			int i = 0;
			for (int x = p0.x; x <= p1.x; x++)
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
			if (p0.y > p1.y)
			{
				SWAP(p0, p1);
				SWAP(v0, v1);
			}

			//Interpolate for z positions
			std::vector<double> zPositions = LerpRange(p1.y, p0.y, 1 / v0.z, 1 / v1.z);

			dx = p1.x - p0.x;
			dy = p1.y - p0.y;

			//If slope is positive increment x, else decrement
			int xi = 1;
			if (dx < 0)
			{
				xi = -1;
				dx = -dx;
			}

			//Keep track of closest x and the error to actual x
			int x = p0.x;
			int error = 0;

			//For each y position, plot the corresponding x
			int i = 0;
			for (int y = p0.y; y <= p1.y; y++)
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

	//Interpolate vertex attributes for each position between start and end (inclusive), left or right edge is prioritized
	std::vector<Attributes> InterpolateAttributes(Vector2Int start, Vector2Int end, Attributes a, Attributes b, bool prioritizeLeft)
	{
		int dx = end.x - start.x;
		int dy = end.y - start.y;

		//Can't interpolate a horizontal line
		if (dy == 0)
			return { b };

		std::vector<Attributes> results;
		results.reserve(abs(dy));

		//Right and leftmost interpolated attributes for the y or x position
		Attributes rAttrib = a;
		Attributes lAttrib = a;

		//Slope is < 1
		if (abs(dx) > abs(dy))
		{
			//If going right to left swap priority
			int xi = 1;
			if (dx < 0)
			{
				xi = -1;
				prioritizeLeft = !prioritizeLeft;
			}

			//Calculate the change in float attributes per change in x
			Attributes dAttrib = AttribChangePerD(b, a, abs(dx));

			//Keep track of the error to y
			int error = 0;

			//For each x position
			for (int i = 0; i < abs(dx); i++)
			{
				//Increase error
				error += 2 * dy;

				//If difference to actual y is more than 0.5
				if (error > abs(dx))
				{
					if (prioritizeLeft)
					{
						//Push the leftmost interpolation of this y value
						results.push_back(lAttrib);
						lAttrib = AddAttribs(rAttrib, dAttrib);
						lAttrib.x = rAttrib.x + xi;
					}
					else
					{
						//Push the rightmost interpolation of this y value
						results.push_back(rAttrib);
					}

					//Decrease error
					error -= 2 * abs(dx);
				}

				//Increment rightmost attributes
				rAttrib = AddAttribs(rAttrib, dAttrib);
				rAttrib.x += xi;
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

			//Calculate the change in attributes per change in y
			Attributes dAttrib = AttribChangePerD(b, a, abs(dy));

			//Keep track of the error to x
			int error = 0;

			//For each y position
			for (int i = 0; i < abs(dy); i++)
			{
				results.push_back(rAttrib);
				rAttrib = AddAttribs(rAttrib, dAttrib);

				//Increase error
				error += 2 * dx;
				//If difference to actual x is more than 0.5
				if (error > abs(dy))
				{
					rAttrib.x += xi;
					error -= 2 * dy;
				}
			}
		}
		//Add final x if y did not change before end
		if (results.size() <= dy)
		{
			if (prioritizeLeft && abs(dx) > abs(dy))
				results.push_back(lAttrib);
			else
				results.push_back(rAttrib);
		}

		return results;
	}

	//Draw a triangle onto a window's framebuffer
	//Expects vertices in normalized device coordinates
	void RasterizeTriangle(Window* window, Face tri, const Material* mat)
	{
		Color color = mat != nullptr ? mat->diffuseColor : Color();

		//Get the points and attributes from the tri
		Vector2Int p0 = tri.vertices.v0;
		Vector2Int p1 = tri.vertices.v1;
		Vector2Int p2 = tri.vertices.v2;
		//Correct for perspective correct interpolation
		Attributes a0 = { std::lroundf(tri.vertices.v0.x), 1.0 / tri.vertices.v0.z, tri.texCoords.v0 / tri.vertices.v0.z };
		Attributes a1 = { std::lroundf(tri.vertices.v1.x), 1.0 / tri.vertices.v1.z, tri.texCoords.v1 / tri.vertices.v1.z };
		Attributes a2 = { std::lroundf(tri.vertices.v2.x), 1.0 / tri.vertices.v2.z, tri.texCoords.v2 / tri.vertices.v2.z };

		//Sort the vertices in vertically descending order
		if (p0.y < p1.y)
		{
			SWAP(p0, p1);
			SWAP(a0, a1);
		}
		if (p0.y < p2.y)
		{
			SWAP(p0, p2);
			SWAP(a0, a2);
		}
		if (p1.y < p2.y)
		{
			SWAP(p1, p2);
			SWAP(a1, a2);
		}

		//If p2 is to the left of p1 or p3, the full segment will be on the right
		Vector2 v1 = Vector2(p1 - p0).Normalize();
		Vector2 v2 = Vector2(p2 - p0).Normalize();
		bool fullOnRight = v1.x < v2.x;

		//Interpolate the vertex attributes for each side (x, z, texCoord)
		std::vector<Attributes> combinedSegment = InterpolateAttributes(p2, p1, a2, a1, fullOnRight);
		std::vector<Attributes> shortSegment = InterpolateAttributes(p1, p0, a1, a0, fullOnRight);
		std::vector<Attributes> fullSegment = InterpolateAttributes(p2, p0, a2, a0, !fullOnRight);
		combinedSegment.insert(combinedSegment.end(), shortSegment.begin() + 1, shortSegment.end());

		//Figure out which segment is on which side
		std::vector<Attributes>* rightSegment = &combinedSegment;
		std::vector<Attributes>* leftSegment = &fullSegment;
		//If the full segment is on the right side, swap the segments
		if (fullOnRight)
		{
			rightSegment = &fullSegment;
			leftSegment = &combinedSegment;
		}

		int startY = (int)std::round(p2.y);
		//For each y coordinate in the triangle
		for (int yi = 0; yi < fullSegment.size(); yi++)
		{
			//Interpolate for z positions for each horizontal scanline
			std::vector<double> zPositions = LerpRange(leftSegment->at(yi).x, rightSegment->at(yi).x, leftSegment->at(yi).z, rightSegment->at(yi).z);
			//Interpolate texture coordinates if applicable
			std::vector<Vector2> texCoords;
			if (mat) if (mat->texture)
				texCoords = LerpRange2D(leftSegment->at(yi).x, rightSegment->at(yi).x, leftSegment->at(yi).texCoord, rightSegment->at(yi).texCoord);

			//Draw a line from the full segment to the split segment
			int startX = leftSegment->at(yi).x;
			for (int xi = 0; xi <= rightSegment->at(yi).x - leftSegment->at(yi).x; xi++)
			{
				Color renderColor = color;
				//Get the color from the texture if it exists
				if (!texCoords.empty())
				{
					Vector2Int sampleCoord(std::round((texCoords[xi].x / zPositions[xi]) * (mat->texture->width - 1)), std::round((texCoords[xi].y / zPositions[xi]) * (mat->texture->height - 1)));
					renderColor = mat->texture->GetTexel(sampleCoord);
				}

				//Attempt to draw the pixel
				window->PutPixel(startX + xi, startY + yi, renderColor, zPositions[xi]);
			}
		}
	}

	//Draw a triangle onto a window's framebuffer entirely of one color
	void RasterizeTriangle(Window* window, Tri verts, Color color)
	{
		//Get the points and attributes from the tri
		Vector2Int p0 = verts.v0;
		Vector2Int p1 = verts.v1;
		Vector2Int p2 = verts.v2;
		//Correct for perspective correct interpolation
		Attributes a0 = { std::lroundf(verts.v0.x), 1.0 / verts.v0.z, 0 };
		Attributes a1 = { std::lroundf(verts.v1.x), 1.0 / verts.v1.z, 0 };
		Attributes a2 = { std::lroundf(verts.v2.x), 1.0 / verts.v2.z, 0 };

		//Sort the vertices in vertically descending order
		if (p0.y < p1.y)
		{
			SWAP(p0, p1);
			SWAP(a0, a1);
		}
		if (p0.y < p2.y)
		{
			SWAP(p0, p2);
			SWAP(a0, a2);
		}
		if (p1.y < p2.y)
		{
			SWAP(p1, p2);
			SWAP(a1, a2);
		}

		//If p2 is to the left of p1 or p3, the full segment will be on the right
		Vector2 v1 = Vector2(p1 - p0).Normalize();
		Vector2 v2 = Vector2(p2 - p0).Normalize();
		bool fullOnRight = v1.x < v2.x;

		//Interpolate the vertex attributes for each side (x, z, texCoord)
		std::vector<Attributes> combinedSegment = InterpolateAttributes(p2, p1, a2, a1, fullOnRight);
		std::vector<Attributes> shortSegment = InterpolateAttributes(p1, p0, a1, a0, fullOnRight);
		std::vector<Attributes> fullSegment = InterpolateAttributes(p2, p0, a2, a0, !fullOnRight);
		combinedSegment.insert(combinedSegment.end(), shortSegment.begin() + 1, shortSegment.end());

		//Figure out which segment is on which side
		std::vector<Attributes>* rightSegment = &combinedSegment;
		std::vector<Attributes>* leftSegment = &fullSegment;
		//If the full segment is on the right side, swap the segments
		if (fullOnRight)
		{
			rightSegment = &fullSegment;
			leftSegment = &combinedSegment;
		}

		int startY = (int)std::round(p2.y);
		//For each y coordinate in the triangle
		for (int yi = 0; yi < fullSegment.size(); yi++)
		{
			//Interpolate for z positions for each horizontal scanline
			std::vector<double> zPositions = LerpRange(leftSegment->at(yi).x, rightSegment->at(yi).x, leftSegment->at(yi).z, rightSegment->at(yi).z);

			//Draw a line from the full segment to the split segment
			int startX = leftSegment->at(yi).x;
			for (int xi = 0; xi <= rightSegment->at(yi).x - leftSegment->at(yi).x; xi++)
			{
				//Attempt to draw the pixel
				window->PutPixel(startX + xi, startY + yi, color, zPositions[xi]);
			}
		}
	}

	//Draw a wireframe triangle onto a window's framebuffer
	void RasterizeTriangleWireframe(Window* window, Tri verts, Color color)
	{
		RasterizeLine(window, verts.v0, verts.v1, color);
		RasterizeLine(window, verts.v1, verts.v2, color);
		RasterizeLine(window, verts.v0, verts.v2, color);
	}
}