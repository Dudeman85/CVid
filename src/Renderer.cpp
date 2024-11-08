#include <bitset>
#include <cvid/Renderer.h>
#include <cvid/Rasterizer.h>
#include <cvid/Math.h>

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
			v1 = cam->GetProjection() * v1;
			v1.x /= v1.w;
			v1.y /= v1.w;
			v1.z /= v1.w;
			v2 = cam->GetProjection() * v2;
			v2.x /= v2.w;
			v2.y /= v2.w;
			v2.z /= v2.w;
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
	//TODO: fix this function it is extremely convoluted and unoptimized
	void DrawModel(ModelInstance* model, Camera* cam, Window* window)
	{
		//Check if the model is inside, outside, or partially inside the clip space
		std::bitset<8> clip = ClipModel(model, cam);

		//Fully outside clip space
		if (clip.none())
			return;

		//Copy the vertices from the base model
		std::vector<Vertex> vertices = model->GetBaseModel()->vertices;

		//Apply transform to all vertices
		//TODO: could optimize by caching transformed verts per instance
		for (Vertex& vert : vertices)
		{
			Vector4 v = Vector4(vert.position, 1.0);
			//Apply the model
			v = model->GetTransform() * v;

			vert.position = Vector3(v);
		}

		//TODO optimize hopefully
		//Recalculate normals
		for (Face& face : model->GetBaseModel()->faces)
		{
			//Calculate the surface normal
			Vector3 v1 = vertices[face.verticeIndices[1]].position - vertices[face.verticeIndices[0]].position;
			Vector3 v2 = vertices[face.verticeIndices[2]].position - vertices[face.verticeIndices[0]].position;
			face.normal = v1.Cross(v2);

			//Cull backwards facing faces
			Vector3 vc = vertices[face.verticeIndices[0]].position - cam->GetPosition();
			face.culled = vc.Dot(face.normal) >= 0;
		}

		//Apply view to all vertices
		for (Vertex& vert : vertices)
		{
			Vector4 v = Vector4(vert.position, 1.0);
			//Apply the view
			v = cam->GetView() * v;

			vert.position = Vector3(v);
		}


		//If model is partially intersecting at least one plane
		if (clip.count() > 1)
		{
			std::vector<Tri> clippedTris;
			//For each triangle in the model
			for (Face& face : model->GetBaseModel()->faces)
			{
				Tri tri{
					vertices[face.verticeIndices[0]].position,
					vertices[face.verticeIndices[1]].position,
					vertices[face.verticeIndices[2]].position
				};

				//Clip the triangle against every intersecting plane
				std::vector<Tri> decomposedTri = ClipTriangle(tri, cam, clip);

				//Add the resulting triangles into the new list
				clippedTris.insert(clippedTris.end(), decomposedTri.begin(), decomposedTri.end());
			}

			//TODO FIX PLEASE
			for (Tri& tri : clippedTris)
			{
				Vector4 v1 = Vector4(tri.v1, 1.0);
				Vector4 v2 = Vector4(tri.v2, 1.0);
				Vector4 v3 = Vector4(tri.v3, 1.0);

				//Apply perspective projection
				if (cam->IsPerspective())
				{
					v1 = cam->GetProjection() * v1;
					v1 /= v1.w;
					v2 = cam->GetProjection() * v2;
					v2 /= v2.w;
					v3 = cam->GetProjection() * v3;
					v3 /= v3.w;
				}

				tri.v1 = Vector3(v1);
				tri.v2 = Vector3(v2);
				tri.v3 = Vector3(v3);

				//Draw the triangle
				RasterizeTriangle(window,
					tri.v1,
					tri.v2,
					tri.v3,
					cvid::Color::Blue);
			}
		}
		else
		{
			for (Vertex& vert : vertices)
			{
				Vector4 v = Vector4(vert.position, 1.0);

				//Apply projection
				v = cam->GetProjection() * v;
				//Normalize
				v.x /= v.w;
				v.y /= v.w;
				v.z /= v.w;

				vert.position = Vector3(v);
			}

			//Draw each face (triangle)
			for (const Face& face : model->GetBaseModel()->faces)
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
	}

	//Returns 0 if a model falls entirely outside a camera's clip space, 1 if it's entirely inside, and >1 if it falls in between
	//If >1 the intersected planes can be acquired by checking each bit corresponding to a plane: 1 = near, 2 = left, 3 = right, 4 = bottom, and 5 = top
	std::bitset<8> ClipModel(ModelInstance* model, Camera* cam)
	{
		Sphere boundingSphere = model->GetBoundingSphere();
		//Apply view space to bounding sphere
		boundingSphere.center = cam->GetView() * Vector4(boundingSphere.center, 1);
		boundingSphere.farthestPoint = cam->GetView() * Vector4(boundingSphere.farthestPoint, 1);

		//Camera's near, left, right, bottom, and top clip planes in that order as normal vectors pointing inward
		const std::array<Vector3, 5>& clipPlanes = cam->GetClipPlanes();

		//For each plane
		std::bitset<8> ret = 1;
		for (size_t i = 0; i < clipPlanes.size(); i++)
		{
			//Calculate the distance from the bounding sphere's centerpoint to the plane
			float dist = clipPlanes[i].Dot(boundingSphere.center) / sqrt(clipPlanes[i].Dot(clipPlanes[i]));

			//Fully behind
			if (dist < -boundingSphere.radius)
				return 0;
			//Intersecting
			if (abs(dist) < boundingSphere.radius)
			{
				//Set the bit corresponding to the plane
				ret.set(i + 1);
			}
		}

		return ret;
	}

	//Returns a vector with 0, 1, or more triangles clipped against every specified plane
	//Planes are determined by checking the corresponding bit: 1 = near, 2 = left, 3 = right, 4 = bottom, and 5 = top
	std::vector<Tri> ClipTriangle(const Tri& triangle, Camera* cam, std::bitset<8> planes)
	{
		const std::array<Vector3, 5>& clipPlanes = cam->GetClipPlanes();
		std::vector<Tri> tris;
		std::vector<Tri> clippedTris = { triangle };
		//For each clip plane
		for (size_t i = 0; i < clipPlanes.size(); i++)
		{
			//If the bit is set, clip against the corresponding plane
			if (planes.test(i + 1))
			{
				//Set the tris from last plane to be clipped against this plane
				tris = clippedTris;
				clippedTris.clear();

				//For each triangle
				for (const Tri& tri : tris)
				{
					//Calculate the distances from each vertex to the plane
					float n = sqrt(clipPlanes[i].Dot(clipPlanes[i]));
					float d1 = clipPlanes[i].Dot(tri.v1) / n;
					float d2 = clipPlanes[i].Dot(tri.v2) / n;
					float d3 = clipPlanes[i].Dot(tri.v3) / n;

					//If all are positive, the triangle is entirely in front of the plane
					if (d1 >= 0 && d2 >= 0 && d3 >= 0)
					{
						//No decomposition needed
						clippedTris.push_back(tri);
					}
					//If all are negative triangle is entirely behind the plane
					else if (d1 < 0 && d2 < 0 && d3 < 0)
					{
						continue;
					}
					//If one is positive, 2 vertices are behind
					else if (d1 * d2 * d3 > 0)
					{
						//Sort the vertices so that A is the positive one
						Vector3 a = tri.v1;
						Vector3 b = tri.v2;
						Vector3 c = tri.v3;
						if (d2 > 0)
						{
							a = tri.v2;
							b = tri.v1;
							c = tri.v3;
						}
						if (d3 > 0)
						{
							a = tri.v3;
							b = tri.v1;
							c = tri.v2;
						}

						//Calculate the points where the triangle's sides intersect the plane
						Vector3 bi = SPIntersect(a, b, clipPlanes[i]);
						Vector3 ci = SPIntersect(a, c, clipPlanes[i]);

						//Decompose into 1 triangle
						clippedTris.push_back(Tri(a, bi, ci));
					}
					//If two are positive, 1 vertice is behind
					else
					{
						//Sort the vertices so that C is the negative one
						Vector3 a = tri.v1;
						Vector3 b = tri.v2;
						Vector3 c = tri.v3;
						if (d1 < 0)
						{
							a = tri.v3;
							b = tri.v2;
							c = tri.v1;
						}
						if (d2 < 0)
						{
							a = tri.v1;
							b = tri.v3;
							c = tri.v2;
						}

						//Calculate the points where the triangle's sides intersect the plane
						Vector3 ai = SPIntersect(a, c, clipPlanes[i]);
						Vector3 bi = SPIntersect(b, c, clipPlanes[i]);

						//Decompose into 2 triangles
						clippedTris.push_back(Tri(a, b, ai));
						clippedTris.push_back(Tri(ai, b, bi));
					}
				}
			}
		}

		return clippedTris;
	}

	//Calculate the intersection of a segment and a clip plane, not suitable for general use
	Vector3 SPIntersect(Vector3 a, Vector3 b, Vector3 n)
	{
		float t = -n.Dot(a) / n.Dot(b - a);
		return a + (b - a) * t;
	}
}