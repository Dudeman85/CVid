#pragma once
#include <vector>
#include <bitset>
#include <cvid/Vector.h>
#include <cvid/Window.h>
#include <cvid/Matrix.h>
#include <cvid/Camera.h>
#include <cvid/Model.h>

namespace cvid
{
	//Render a point to the window's framebuffer
	void DrawPoint(Vector3 point, Color color, Matrix4 transform, Camera* cam, Window* window);
	//Render a line to the window's framebuffer
	void DrawLine(Vector3 p1, Vector3 p2, Color color, Matrix4 transform, Camera* cam, Window* window);
	//Render a model to the window's framebuffer
	void DrawModel(ModelInstance* model, Camera* cam, Window* window);
	//Render a model's vertices as wireframe to the window's framebuffer
	void DrawModelWireframe(ModelInstance* model, Camera* cam, Window* window);

	//Utility Functions
	//Returns 0 if a model falls entirely outside a camera's clip space, 1 if it's entirely inside, and >1 if it falls in between
	//If >1 the intersected planes can be acquired by checking each bit corresponding to a plane: 1 = near, 2 = left, 3 = right, 4 = bottom, and 5 = top
	std::bitset<8> ClipModel(ModelInstance* model, Camera* cam);
	//Returns a vector with 0, 1, or more triangles clipped against every specified plane
	//Planes are determined by checking the corresponding bit: 1 = near, 2 = left, 3 = right, 4 = bottom, and 5 = top
	std::vector<Face> ClipFace(const Face& triangle, Camera* cam, std::bitset<8> planes = 0b11111111);
	//Clips a line against every specified camera clip plane
	//Planes are determined by checking the corresponding bit: 1 = near, 2 = left, 3 = right, 4 = bottom, and 5 = top
	std::pair<Vector3, Vector3> ClipSegment(Vector3 p1, Vector3 p2, Camera* cam, std::bitset<8> planes = 0b11111111);
	//Calculate the intersection of a segment and a clip plane, not suitable for general use
	inline Vector3 SPIntersect(Vector3 a, Vector3 b, Vector3 planeNormal);
}