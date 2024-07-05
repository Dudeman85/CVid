#include <cvid/Renderer.h>
#include <cvid/Rasterizer.h>

namespace cvid
{
	std::vector<Color> colors{ Color::Red, Color::Blue, Color::Green, Color::Magenta, Color::BrightBlue, Color::BrightCyan };

	//Render an amount of vertices to the window's framebuffer
	void DrawVertices(std::vector<Vertice> vertices, std::vector<Vector3Int> indices, Matrix4 transform, Camera* cam, Window* window)
	{
		//Apply model view projection transforms
		for (Vertice& vert : vertices)
		{
			vert.position = Vector3(transform * Vector4(vert.position, 1.0));
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
	void DrawVerticesWireframe(std::vector<Vertice> vertices, std::vector<Vector3Int> indices, Matrix4 transform, Camera* cam, Window* window)
	{
		//Apply model view projection transforms
		for (Vertice& vert : vertices)
		{
			Vector4 v = Vector4(vert.position, 1.0);
			//Apply the mvp
			v = transform * v;
			v = cam->GetView() * v;
			//v = cam->GetProjection() * v;

			vert.position = Vector3(v);
		}

		//Draw each triangle defined by the indices
		for (Vector3Int& triangle : indices)
		{
			DrawTriangleWireframe(window,
								  vertices[triangle.x].position,
								  vertices[triangle.y].position,
								  vertices[triangle.z].position,
								  Color::BrightGreen);
		}
	}

	//Render a model to the window's frambuffer
	inline void DrawModel(Model* model, Matrix4 transform, Camera* cam, Window* window)
	{
		DrawVertices(model->vertices, model->indices, transform, cam, window);
	}

	//Render a model as wireframe to the window's frambuffer
	inline void DrawModelWireframe(Model* model, Matrix4 transform, Camera* cam, Window* window)
	{
		DrawVerticesWireframe(model->vertices, model->indices, transform, cam, window);
	}
}