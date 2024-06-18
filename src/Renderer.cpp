#include <cvid/Renderer.h>
#include <cvid/Rasterizer.h>

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
	void DrawVerticesWireframe(Window* window, Camera* cam, std::vector<Vertice> vertices, std::vector<Vector3Int> indices, Matrix4 model)
	{
		//Apply model view projection transforms
		for (Vertice& vert : vertices)
		{
			Vector4 v = Vector4(vert.position, 1.0);
			//Apply the mvp
			v = model * v;
			v = cam->GetView() * v;
			//v = cam->GetProjection() * v;

			vert.position = Vector3(v);
		}

		//Draw each triangle defined by the indices
		for (Vector3Int& triangle : indices)
		{
			DrawTriangleWireframe(window, 
								  Vector2Int(vertices[triangle.x].position), 
								  Vector2Int(vertices[triangle.y].position), 
								  Vector2Int(vertices[triangle.z].position), 
								  Color::BrightGreen);
		}
	}
	//Render an amount of vertices as wireframes to the window's framebuffer
	void DrawVerticesWireframe(Window* window, std::vector<Vertice> vertices, std::vector<Vector3Int> indices, glm::mat4 model)
	{
		//Apply model view projection transforms
		for (Vertice& vert : vertices)
		{
			glm::vec4 newVert(vert.position.x, vert.position.y, vert.position.z, 1.0);
			newVert = newVert * model;
			vert.position = Vector3(newVert.x, newVert.y, newVert.z);
		}

		//Draw each triangle defined by the indices
		for (Vector3Int& triangle : indices)
		{
			DrawTriangleWireframe(window, Vector2Int(vertices[triangle.x].position), Vector2Int(vertices[triangle.y].position), Vector2Int(vertices[triangle.z].position), Color::BrightCyan);
		}
	}
}