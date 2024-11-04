#pragma once
#include <string>
#include <vector>
#include <array>
#include <cvid/Vector.h>
#include <cvid/Helpers.h>

namespace cvid
{
	struct Vertex
	{
		Vector3 position;
		Vector3 color;
	};

	//A face always has 3 vertices (triangle)
	struct Face 
	{
		std::array<uint32_t, 3> verticeIndices = {};
		std::array<uint32_t, 3> texCoordIndices = {};
		//Surface normal (calculated)
		Vector3 normal;
		//Should this face be rendered
		bool culled = false;

		//Temporary for testing
		cvid::Color color;
	};

	//A 3D model loaded from an obj file, this needs to be instanced before it can be rendered
	class Model
	{
	public:
		Model(std::string path);

		//Every face in the model, contains indices to vertices and texCoords vectors
		std::vector<Face> faces;
		//Vertices are shared for the whole model
		std::vector<Vertex> vertices;
		//Texture coordinates are shared for the whole model
		std::vector<Vector2> texCoords;

	private:

		//Load a model from disk
		void LoadModel(std::string path);
	};

	//A renderable instance of a 3D model
	class ModelInstance 
	{
	public:
		//Make a renderable instance from a model
		ModelInstance(Model* model);

		//Set the model this intance will use
		void SetBaseModel(Model* model);

		//Recalculate the bounding sphere, this should be called after scale has been changed
		void RecalculateBounds();

		//The bounding spehere of this instance
		Vector2 centerPoint;
		float radius = 0;

	private:
		Model* model;
	};
}