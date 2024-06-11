#include <cvid/Renderer.h>
#include <cvid/Rasterizer.h>

namespace cvid
{
	//Render an amount of vertices to the window's framebuffer
	void DrawVertices(Window* window, std::vector<Vertice> vertices, std::vector<Vector3Int> indices, glm::mat4x4 mvp)
	{
		//Apply model view projection transforms
		for (Vertice& vert : vertices)
		{
			glm::vec4 newVert{ vert.position.x, vert.position.y, vert.position.z, 1.0 };
			newVert = newVert * mvp;
			vert.position = Vector3(newVert.x, newVert.y, newVert.z);
		}

		//Draw each triangle defined by the indices
		for (Vector3Int& triangle : indices)
		{
			DrawTriangle(window, vertices[triangle.x].position, vertices[triangle.y].position, vertices[triangle.z].position, Color::BrightGreen);
		}
	}

	//Render an amount of vertices as wireframes to the window's framebuffer
	void DrawVerticesWireframe(Window* window, std::vector<Vertice> vertices, std::vector<Vector3Int> indices, glm::mat4x4 mvp)
	{
		//Apply model view projection transforms
		for (Vertice& vert : vertices)
		{
			glm::vec4 newVert{ vert.position.x, vert.position.y, vert.position.z, 1.0 };
			newVert = newVert * mvp;
			vert.position = Vector3(newVert.x, newVert.y, newVert.z);
		}

		//Draw each triangle defined by the indices
		for (Vector3Int& triangle : indices)
		{
			DrawTriangleWireframe(window, vertices[triangle.x].position, vertices[triangle.y].position, vertices[triangle.z].position, Color::BrightGreen);
		}
	}
}