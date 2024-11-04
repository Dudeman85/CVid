#include <cvid/Renderer.h>
#include <cvid/Rasterizer.h>

namespace cvid
{
	std::vector<Color> colors{ Color::Red, Color::Blue, Color::Green, Color::Magenta, Color::BrightBlue, Color::BrightCyan,
		Color::Red, Color::Blue, Color::Green, Color::Magenta, Color::BrightBlue, Color::BrightCyan };

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
		Vector4 v2 = Vector4(p2, 1.0);
		v1 = transform * v1;
		v1 = cam->GetView() * v1;
		v2 = transform * v2;
		v2 = cam->GetView() * v2;

		//Apply perspective projection
		if (cam->IsPerspective())
		{
			//Prevent divide by 0
			if (v1.z == 0)
				v1.z = 0.1;
			if (v2.z == 0)
				v2.z = 0.1;

			//Apply perspective
			v1.x = v1.x * -cam->distance / v1.z;
			v1.y = v1.y * -cam->distance / v1.z;
			v2.x = v2.x * -cam->distance / v2.z;
			v2.y = v2.y * -cam->distance / v2.z;
		}

		RasterizeLine(window, v1, v2, color);
	}

	//Render an amount of vertices to the window's framebuffer
	void DrawVertices(std::vector<Vertex> vertices, std::vector<Vector3Int> indices, Matrix4 transform, Camera* cam, Window* window)
	{
		//Apply model view projection transforms
		for (Vertex& vert : vertices)
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
				vertices[triangle.x].position,
				vertices[triangle.y].position,
				vertices[triangle.z].position,
				colors[i]);
			i++;
		}
	}

	//Render an amount of vertices as wireframes to the window's framebuffer
	void DrawVerticesWireframe(std::vector<Vertex> vertices, std::vector<Vector3Int> indices, Matrix4 transform, Camera* cam, Window* window)
	{
		//Apply model view projection transforms
		for (Vertex& vert : vertices)
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
	void DrawModel(Model* model, Matrix4 transform, Camera* cam, Window* window)
	{
		std::vector<Vertex> vertices = model->vertices;
		//Apply transform to all vertices
		for (Vertex& vert : vertices)
		{
			Vector4 v = Vector4(vert.position, 1.0);
			//Apply the model
			v = transform * v;

			vert.position = Vector3(v);
		}

		//TODO optimize hopefully
		//Recalculate normals
		for (Face& face : model->faces)
		{
			//Calculate the surface normal
			Vector3 v1 = vertices[face.verticeIndices[1]].position - vertices[face.verticeIndices[0]].position;
			Vector3 v2 = vertices[face.verticeIndices[2]].position - vertices[face.verticeIndices[0]].position;
			face.normal = v1.Cross(v2);

			//Cull backwards facing faces
			Vector3 vc = vertices[face.verticeIndices[0]].position - cam->GetPosition();
			face.culled = vc.Dot(face.normal) >= 0;
		}

		//Apply view and projection to all vertices
		for (Vertex& vert : vertices)
		{
			Vector4 v = Vector4(vert.position, 1.0);
			//Apply the view
			v = cam->GetView() * v;

			//Apply perspective projection
			if (cam->IsPerspective())
			{
				v.x = v.x * -cam->distance / v.z;
				v.y = v.y * -cam->distance / v.z;
			}

			vert.position = Vector3(v);
		}

		//Draw each face (triangle)
		for (const Face& face : model->faces)
		{
			//Backface culling
			if (!face.culled)
			{
				RasterizeTriangle(window,
					vertices[face.verticeIndices[0]].position,
					vertices[face.verticeIndices[1]].position,
					vertices[face.verticeIndices[2]].position,
					face.color);
			}
		}
	}

	//Returns true if a model falls entirely inside a camera's clip space
	//This function expects vertices with model and view transforms applied
	bool ClipModel(const std::vector<Vertex>& modelVerts, Camera* cam)
	{
		//Calculate the center point of the vertices
		Vector3 center;
		for (const Vertex& vert : modelVerts)
		{
			center += vert.position;
		}
		center /= modelVerts.size();

		//The radius of the sphere is defined as the distance from the center to the furthest polygon
		float radius = 0;
		for (const Vertex& vert : modelVerts)
		{
			if(vert.position)
		}
	}

	//Returns true if a tri falls entirely inside a camera's clip space
	bool ClipTri(const Vector3& v1, const Vector3& v2, const Vector3& v3, Camera* cam) 
	{

	}
}