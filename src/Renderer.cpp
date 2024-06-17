#include <cvid/Renderer.h>
#include <cvid/Rasterizer.h>
#include <cvid/Matrix.h>

namespace cvid
{
	std::vector<Color> colors{Color::Red, Color::Blue, Color::Green, Color::Magenta};

	//Render an amount of vertices to the window's framebuffer
	void DrawVertices(Window* window, std::vector<Vertice> vertices, std::vector<Vector3Int> indices, Matrix4 model)
	{
		//Apply model view projection transforms
		for (Vertice& vert : vertices)
		{
			vert.position = Vector3(model * Vector4(vert.position, 1.0));
		}

		int i = 0;
		//Draw each triangle defined by the indices
		for (Vector3Int& triangle : indices)
		{
			DrawTriangle(window, 
						 Vector2Int(vertices[triangle.x].position), 
						 Vector2Int(vertices[triangle.y].position), 
						 Vector2Int(vertices[triangle.z].position), 
						 colors[i]);
			i++;
		}
	}

	//Render an amount of vertices as wireframes to the window's framebuffer
	void DrawVerticesWireframe(Window* window, std::vector<Vertice> vertices, std::vector<Vector3Int> indices, Matrix4 model)
	{
		//Apply model view projection transforms
		for (Vertice& vert : vertices)
		{
			vert.position = Vector3(model * Vector4(vert.position, 1.0));
		}

		//Draw each triangle defined by the indices
		for (Vector3Int& triangle : indices)
		{
			DrawTriangleWireframe(window, Vector2Int(vertices[triangle.x].position), Vector2Int(vertices[triangle.y].position), Vector2Int(vertices[triangle.z].position), Color::BrightGreen);
		}
	}
}