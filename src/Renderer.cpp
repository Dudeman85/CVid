#include <bitset>
#include <cvid/Renderer.h>
#include <cvid/Rasterizer.h>
#include <cvid/Math.h>

namespace cvid
{
	//Render a point to the window's framebuffer
	void DrawPoint(Vector3 point, ConsoleColor color, Matrix4 transform, Camera* cam, Window* window)
	{
		//Apply the mvp
		Vector4 v = Vector4(point, 1.0);
		v = transform * v;
		v = cam->GetView() * v;

		//Clip it against the camera clip space
		for (const Vector3& plane : cam->GetClipPlanes())
		{
			if (plane.Dot(v) / sqrt(plane.Dot(plane)) < 0)
				return;
		}

		v = cam->GetProjection() * v;
		//Normalize
		v /= v.w;

		RasterizePoint(window, v, color);
	}

	//Render a line to the window's framebuffer
	void DrawLine(Vector3 p1, Vector3 p2, ConsoleColor color, Matrix4 transform, Camera* cam, Window* window)
	{
		//Apply the model and view transforms
		Vector4 v1 = Vector4(p1, 1);
		Vector4 v2 = Vector4(p2, 1);
		v1 = transform * v1;
		v2 = transform * v2;
		v1 = cam->GetView() * v1;
		v2 = cam->GetView() * v2;

		//Clip the Line against the clip space
		std::pair<Vector3, Vector3> clippedLine = ClipSegment(v1, v2, cam);
		if (clippedLine.first == 0 && clippedLine.second == 0)
			return;
		v1 = Vector4(clippedLine.first, 1);
		v2 = Vector4(clippedLine.second, 1);

		Vector4 test = v1;
		if (v1.x > 10000)
			return;

		//Apply projection
		v1 = cam->GetProjection() * v1;
		v2 = cam->GetProjection() * v2;
		Vector4 test2 = v1;
		//Normalize
		v1 /= v1.w;
		v2 /= v2.w;

		if (v1.x > 10000)
			return;

		RasterizeLine(window, v1, v2, color);
	}

	//Render a model to the window's framebuffer
	void DrawModel(ModelInstance* model, Camera* cam, Window* window)
	{
		//Check if the model is inside, outside, or partially inside the clip space
		std::bitset<8> clip = ClipModel(model, cam);

		//Fully outside clip space
		if (clip.none())
			return;

		//Copy the vertices from the base model
		std::vector<Vertex> vertices = model->GetBaseModel()->vertices;
		const std::vector<Vector2>& texCoords = model->GetBaseModel()->texCoords;

		//Apply transform to all vertices
		for (Vertex& vert : vertices)
			vert.position = model->GetTransform() * Vector4(vert.position, 1.0);

		//Recalculate normals, and cull backwards faces
		std::vector<Vector3> normals;
		std::vector<Vector3> culled;
		for (const IndexedFace& face : model->GetBaseModel()->faces)
		{
			//Calculate the surface normal
			Vector3 v1 = vertices[face.verticeIndices[1]].position - vertices[face.verticeIndices[0]].position;
			Vector3 v2 = vertices[face.verticeIndices[2]].position - vertices[face.verticeIndices[0]].position;
			normals.push_back(v1.Cross(v2));

			//Cull backwards facing faces
			Vector3 vc = vertices[face.verticeIndices[0]].position - cam->GetPosition();
			culled.push_back(vc.Dot(face.normal) >= 0);
		}

		//Apply view to all vertices
		for (Vertex& vert : vertices)
		{
			Vector4 v = Vector4(vert.position, 1.0);
			//Apply the view
			v = cam->GetView() * v;

			vert.position = Vector3(v);
		}

		//For each face in the model
		for (const IndexedFace& iFace : model->GetBaseModel()->faces)
		{
			//Copy the indexed face's vertices and texture coords to it's own container
			Face face{
				{vertices[iFace.verticeIndices[0]].position, vertices[iFace.verticeIndices[1]].position, vertices[iFace.verticeIndices[2]].position},
				{texCoords[iFace.texCoordIndices[0]], texCoords[iFace.texCoordIndices[1]], texCoords[iFace.texCoordIndices[2]]},
				iFace.normal,
				iFace.culled,
			};

			//The final list of faces to render
			std::vector<Face> faces{ face };

			//If model is partially intersecting at least one plane
			if (clip.count() > 1)
				//Clip the triangle against every intersecting plane
				faces = ClipFace(face, cam, clip);

			//If the face was decomposed, loop over every new face, otherwise faces will only have one face
			for (Face& face : faces)
			{
				Vector4 v1 = Vector4(face.vertices.v1, 1.0);
				Vector4 v2 = Vector4(face.vertices.v2, 1.0);
				Vector4 v3 = Vector4(face.vertices.v3, 1.0);
				//Apply projection
				v1 = cam->GetProjection() * v1;
				v2 = cam->GetProjection() * v2;
				v3 = cam->GetProjection() * v3;
				//Normalize
				v1 /= v1.w;
				v2 /= v2.w;
				v3 /= v3.w;
				face.vertices = { v1, v2, v3 };

				//Backface culling
				if (!face.culled)
					//Draw the face (triangle)
					RasterizeTriangle(window, face, model->GetMaterial());
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
	std::vector<Face> ClipFace(const Face& face, Camera* cam, std::bitset<8> planes)
	{
		const std::array<Vector3, 5>& clipPlanes = cam->GetClipPlanes();
		std::vector<Face> tris;
		std::vector<Face> clippedTris = { face };
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
				for (const Face& tri : tris)
				{
					//Calculate the distances from each vertex to the plane
					float n = sqrt(clipPlanes[i].Dot(clipPlanes[i]));
					float d1 = clipPlanes[i].Dot(tri.vertices.v1) / n;
					float d2 = clipPlanes[i].Dot(tri.vertices.v2) / n;
					float d3 = clipPlanes[i].Dot(tri.vertices.v3) / n;

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
						Vector3 a = tri.vertices.v1;
						Vector3 b = tri.vertices.v2;
						Vector3 c = tri.vertices.v3;
						if (d2 > 0)
						{
							a = tri.vertices.v2;
							b = tri.vertices.v1;
							c = tri.vertices.v3;
						}
						if (d3 > 0)
						{
							a = tri.vertices.v3;
							b = tri.vertices.v1;
							c = tri.vertices.v2;
						}

						//Calculate the points where the triangle's sides intersect the plane
						Vector3 bi = SPIntersect(a, b, clipPlanes[i]);
						Vector3 ci = SPIntersect(a, c, clipPlanes[i]);

						//Decompose into 1 triangle
						clippedTris.push_back(Face(Tri(a, bi, ci), tri.texCoords));
					}
					//If two are positive, 1 vertice is behind
					else
					{
						//Sort the vertices so that C is the negative one
						Vector3 a = tri.vertices.v1;
						Vector3 b = tri.vertices.v2;
						Vector3 c = tri.vertices.v3;
						if (d1 < 0)
						{
							a = tri.vertices.v3;
							b = tri.vertices.v2;
							c = tri.vertices.v1;
						}
						if (d2 < 0)
						{
							a = tri.vertices.v1;
							b = tri.vertices.v3;
							c = tri.vertices.v2;
						}

						//Calculate the points where the triangle's sides intersect the plane
						Vector3 ai = SPIntersect(a, c, clipPlanes[i]);
						Vector3 bi = SPIntersect(b, c, clipPlanes[i]);

						//Decompose into 2 triangles
						clippedTris.push_back(Face(Tri(a, b, ai), tri.texCoords));
						clippedTris.push_back(Face(Tri(ai, b, bi), tri.texCoords));
					}
				}
			}
		}

		return clippedTris;
	}

	//Clips a line against every specified camera clip plane, if line is entirely outside, both points will be (0, 0, 0)
	//Planes are determined by checking the corresponding bit: 1 = near, 2 = left, 3 = right, 4 = bottom, and 5 = top
	std::pair<Vector3, Vector3> ClipSegment(Vector3 p1, Vector3 p2, Camera* cam, std::bitset<8> planes)
	{
		const std::array<Vector3, 5>& clipPlanes = cam->GetClipPlanes();

		//For each clip plane
		for (size_t i = 0; i < clipPlanes.size(); i++)
		{
			//If the bit is set, clip against the corresponding plane
			if (planes.test(i + 1))
			{
				//Calculate the distances from both endpoints to the plane
				float n = sqrt(clipPlanes[i].Dot(clipPlanes[i]));
				float d1 = clipPlanes[i].Dot(p1) / n;
				float d2 = clipPlanes[i].Dot(p2) / n;

				//If both are positive, line is in front of the plane
				if (d1 >= 0 && d2 >= 0)
					continue;
				//If both are negative , line is behind the plane
				else if (d1 < 0 && d2 < 0)
					return { Vector3(0), Vector3(0) };
				//If p1 is behind the plane clip the line against the plane and replace p1
				else if (d1 < 0)
					p1 = SPIntersect(p1, p2, clipPlanes[i]);
				//If p2 is behind the plane clip the line against the plane and replace p2
				else
					p2 = SPIntersect(p1, p2, clipPlanes[i]);
			}
		}
		return { p1, p2 };
	}

	//Calculate the intersection of a segment and a clip plane, not suitable for general use
	Vector3 SPIntersect(Vector3 a, Vector3 b, Vector3 n)
	{
		float t = -n.Dot(a) / n.Dot(b - a);
		return a + (b - a) * t;
	}
}