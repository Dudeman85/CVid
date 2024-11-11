#pragma once
#include <string>
#include <vector>
#include <array>
#include <cvid/Vector.h>
#include <cvid/Matrix.h>
#include <cvid/Helpers.h>

namespace cvid
{
	struct Sphere 
	{
		//Center point in world space
		Vector3 center;
		Vector3 farthestPoint;
		float radius = 0;
	};

	struct Tri
	{
		Vector3 v1;
		Vector3 v2;
		Vector3 v3;
	};
	struct Tri2D
	{
		Vector2 v1;
		Vector2 v2;
		Vector2 v3;
	};

	struct Vertex
	{
		Vector3 position;
		Vector3 color;
	};

	//A face (triangle) that used indexed texture coords and vertices
	struct IndexedFace 
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

	//A face (triangle) that uses it's own texture coords and vertices
	struct Face 
	{
		Tri vertices;
		Tri2D texCoords;
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
		std::vector<IndexedFace> faces;
		//Vertices are shared for the whole model
		std::vector<Vertex> vertices;
		//Texture coordinates are shared for the whole model
		std::vector<Vector2> texCoords;

	private:

		//Load a model from disk
		void LoadModel(std::string path);
	};

	//A renderable instance of a 3D model with it's own transform
	class ModelInstance 
	{
	public:
		//Make a renderable instance from a model
		ModelInstance(Model* model);

		//Set the model this intance will use
		void SetBaseModel(Model* model);
		//Get a pointer to the base model of this instance
		Model* GetBaseModel() const;

		//Transform setters
		//Move this model in world space by translation
		void Translate(Vector3 translation);
		//Set this model's position in world space
		void SetPosition(Vector3 position);
		//Rotate this model by euler angles in world space in radians
		void Rotate(Vector3 rotation);
		//Set this model's euler rotation in world space by rotation in radians
		void SetRotation(Vector3 rotation);
		//Scale this model in world space
		void Scale(Vector3 scale);
		//Set this model's scale in world space
		void SetScale(Vector3 scale);

		//Transform getters
		//Get the world space position of this model
		Vector3 GetPosition() const;
		//Get the euler rotation of this model in radians
		Vector3 GetRotation() const;
		//Get the scale of this model
		Vector3 GetScale() const;

		//Recalculate the bounding sphere, this should be called after scale has been changed
		void RecalculateBounds();
		//Recalculate the transform matrix, this should be called after any transform has been changed
		void RecalculateTransform();

		//Get the rough bounding sphere of this model in world space
		Sphere GetBoundingSphere();
		//Get the transform matrix
		const Matrix4& GetTransform();

	private:
		Model* model;
		Matrix4 transform;
		Sphere boundingSphere;
		
		//Transforms
		Vector3 position;
		Vector3 rotation;
		Vector3 scale = Vector3(1);

		//Vertices of this instance in world space
		std::vector<Vertex> worldVertices;

		//Does the transform matrix need to be recalculated
		bool staleTransform = true;
		//Does the bounding sphere need to be recalculated, 0b01 = center point, 0b10 = everything, 0b00 = nothing
		int staleBounds = 0b10;
	};
}