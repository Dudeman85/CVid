#include <bitset>
#include <cvid/Renderer.h>
#include <cvid/Rasterizer.h>
#include <cvid/Math.h>

namespace cvid
{
	//Render a point to the window's framebuffer
	void DrawPoint(Vector3 point, Color color, Matrix4 transform, Camera* cam, Window* window)
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
	void DrawLine(Vector3 p1, Vector3 p2, Color color, Matrix4 transform, Camera* cam, Window* window)
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

		//Apply projection
		v1 = cam->GetProjection() * v1;
		v2 = cam->GetProjection() * v2;
		//Normalize
		v1 /= v1.w;
		v2 /= v2.w;

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

		//Apply model transform to all vertices
		for (Vertex& vert : vertices)
			vert.position = model->GetTransform() * Vector4(vert.position, 1.0);

		//Recalculate normals, and cull backwards faces
		std::vector<bool> culled;
		culled.reserve(model->GetBaseModel()->faces.size());
		std::vector<Vector3> normals;
		normals.reserve(model->GetBaseModel()->faces.size());
		for (const IndexedFace& face : model->GetBaseModel()->faces)
		{
			//Calculate the surface normal
			Vector3 v1 = vertices[face.verticeIndices[1]].position - vertices[face.verticeIndices[0]].position;
			Vector3 v2 = vertices[face.verticeIndices[2]].position - vertices[face.verticeIndices[0]].position;
			Vector3 normal = v1.Cross(v2);
			normals.push_back(normal);

			//Cull backwards facing faces
			Vector3 vc = vertices[face.verticeIndices[0]].position - cam->GetPosition();
			culled.push_back(vc.Dot(normal) >= 0);
		}

		//Apply view to all vertices
		for (Vertex& vert : vertices)
			vert.position = cam->GetView() * Vector4(vert.position, 1.0);
		
		//For each face in the model
		for (size_t i = 0; i < model->GetBaseModel()->faces.size(); i++)
		{
			//Backface culling
			if (culled[i])
				continue;

			const IndexedFace& iFace = model->GetBaseModel()->faces[i];
			//Copy the indexed face's vertices and texture coords to it's own container
			Face face{
				{vertices[iFace.verticeIndices[0]].position, vertices[iFace.verticeIndices[1]].position, vertices[iFace.verticeIndices[2]].position},
				{texCoords[iFace.texCoordIndices[0]], texCoords[iFace.texCoordIndices[1]], texCoords[iFace.texCoordIndices[2]]},
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
				Vector4 v1 = Vector4(face.vertices.v0, 1.0);
				Vector4 v2 = Vector4(face.vertices.v1, 1.0);
				Vector4 v3 = Vector4(face.vertices.v2, 1.0);
				//Apply projection
				v1 = cam->GetProjection() * v1;
				v2 = cam->GetProjection() * v2;
				v3 = cam->GetProjection() * v3;
				//Normalize
				v1.x /= v1.w;
				v1.y /= v1.w;
				v2.x /= v2.w;
				v2.y /= v2.w;
				v3.x /= v3.w;
				v3.y /= v3.w;

				face.vertices = { v1, v2, v3 };
				face.normal = normals[i];

				//Convert from clip space to screen space
				Vector3 windowHalfSize(window->GetSize() / 2, 1);
				face.vertices.v0 *= windowHalfSize;
				face.vertices.v1 *= windowHalfSize;
				face.vertices.v2 *= windowHalfSize;
				face.vertices.v0 += windowHalfSize;
				face.vertices.v1 += windowHalfSize;
				face.vertices.v2 += windowHalfSize;
				face.vertices.v0.z = v1.w;
				face.vertices.v1.z = v2.w;
				face.vertices.v2.z = v3.w;

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
					float d1 = clipPlanes[i].Dot(tri.vertices.v0) / n;
					float d2 = clipPlanes[i].Dot(tri.vertices.v1) / n;
					float d3 = clipPlanes[i].Dot(tri.vertices.v2) / n;

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
						//Sort the vertices so that v1 is the positive one
						Vector2 atc = tri.texCoords.v0;
						Vector2 btc = tri.texCoords.v1;
						Vector2 ctc = tri.texCoords.v2;
						Vector3 a = tri.vertices.v0;
						Vector3 b = tri.vertices.v1;
						Vector3 c = tri.vertices.v2;
						if (d2 > 0)
						{
							atc = tri.texCoords.v1;
							btc = tri.texCoords.v0;
							ctc = tri.texCoords.v2;
							a = tri.vertices.v1;
							b = tri.vertices.v0;
							c = tri.vertices.v2;
						}
						if (d3 > 0)
						{
							atc = tri.texCoords.v2;
							btc = tri.texCoords.v0;
							ctc = tri.texCoords.v1;
							a = tri.vertices.v2;
							b = tri.vertices.v0;
							c = tri.vertices.v1;
						}

						//Calculate the ratios of the old and new sides
						float bit = (-clipPlanes[i].Dot(a) / clipPlanes[i].Dot(b - a));
						float cit = (-clipPlanes[i].Dot(a) / clipPlanes[i].Dot(c - a));

						//Calculate the points where the triangle's sides intersect the plane
						Vector3 bi = a + (b - a) * bit;
						Vector3 ci = a + (c - a) * cit;

						//Calculate the new texture coordinates
						Vector2 bitc = atc + (btc - atc) * bit;
						Vector2 citc = atc + (ctc - atc) * cit;

						//Decompose into 1 triangle
						clippedTris.push_back(Face(Tri(a, bi, ci), Tri2D{ atc, bitc, citc }));
					}
					//If two are positive, 1 vertice is behind
					else
					{
						//Sort the vertices so that C is the negative one
						Vector2 atc = tri.texCoords.v0;
						Vector2 btc = tri.texCoords.v1;
						Vector2 ctc = tri.texCoords.v2;
						Vector3 a = tri.vertices.v0;
						Vector3 b = tri.vertices.v1;
						Vector3 c = tri.vertices.v2;
						if (d1 < 0)
						{
							atc = tri.texCoords.v2;
							btc = tri.texCoords.v1;
							ctc = tri.texCoords.v0;
							a = tri.vertices.v2;
							b = tri.vertices.v1;
							c = tri.vertices.v0;
						}
						if (d2 < 0)
						{
							atc = tri.texCoords.v0;
							btc = tri.texCoords.v2;
							ctc = tri.texCoords.v1;
							a = tri.vertices.v0;
							b = tri.vertices.v2;
							c = tri.vertices.v1;
						}

						//Calculate the ratios of the old and new sides
						float ait = (-clipPlanes[i].Dot(a) / clipPlanes[i].Dot(c - a));
						float bit = (-clipPlanes[i].Dot(b) / clipPlanes[i].Dot(c - b));

						//Calculate the points where the triangle's sides intersect the plane
						Vector3 ai = a + (c - a) * ait;
						Vector3 bi = b + (c - b) * bit;

						//Calculate the new texture coordinates
						Vector2 aitc = atc + (ctc - atc) * ait;
						Vector2 bitc = btc + (ctc - btc) * bit;

						//Decompose into 2 triangles
						clippedTris.push_back(Face(Tri(a, b, ai), Tri2D{ atc, btc, aitc }));
						clippedTris.push_back(Face(Tri(ai, b, bi), Tri2D{ aitc, btc, bitc }));
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
		return a + (b - a) * (-n.Dot(a) / n.Dot(b - a));
	}
}