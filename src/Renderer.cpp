#include <cvid/Renderer.h>
#include <cvid/Rasterizer.h>

namespace cvid
{
	std::vector<Color> colors{ Color::Red, Color::Blue, Color::Green, Color::Magenta, Color::BrightBlue, Color::BrightCyan };

	//Render a point to the window's framebuffer
	void DrawPoint(Vector3 point, Color color, Matrix4 transform, Camera* cam, Window* window)
	{
		//Apply the mvp
		Vector4 v = Vector4(point, 1.0);
		v = transform * v;
		v = cam->GetView() * v;
		//v = cam->GetProjection() * v;

		RasterizePoint(window, v, color);
	}

	//Render a line to the window's framebuffer
	void DrawLine(Vector3 p1, Vector3 p2, Color color, Matrix4 transform, Camera* cam, Window* window)
	{
		//Apply the mvp
		Vector4 v1 = Vector4(p1, 1.0);
		v1 = transform * v1;
		v1 = cam->GetView() * v1;
		//v1 = cam->GetProjection() * v1;
		Vector4 v2 = Vector4(p2, 1.0);
		v2 = transform * v2;
		v2 = cam->GetView() * v2;
		//v2 = cam->GetProjection() * v2;

		RasterizeLine(window, v1, v2, color);
	}

	//Render an amount of vertices to the window's framebuffer
	void DrawVertices(std::vector<Vertice> vertices, std::vector<Vector3Int> indices, Matrix4 transform, Camera* cam, Window* window)
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

		int i = 0;
		//Draw each triangle defined by the indices
		for (Vector3Int& triangle : indices)
		{
			RasterizeTriangle(window,
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
			RasterizeTriangleWireframe(window,
								  vertices[triangle.x].position,
								  vertices[triangle.y].position,
								  vertices[triangle.z].position,
								  Color::BrightGreen);
		}
	}

	//Render a model to the window's framebuffer
	inline void DrawModel(Model* model, Matrix4 transform, Camera* cam, Window* window)
	{
		DrawVertices(model->vertices, model->indices, transform, cam, window);
	}

	//Render a model as wireframe to the window's framebuffer
	inline void DrawModelWireframe(Model* model, Matrix4 transform, Camera* cam, Window* window)
	{
		DrawVerticesWireframe(model->vertices, model->indices, transform, cam, window);
	}
}