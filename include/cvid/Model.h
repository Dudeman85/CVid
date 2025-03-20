#pragma once
#include <string>
#include <vector>
#include <memory>
#include <array>
#include <cvid/Vector.h>
#include <cvid/Matrix.h>
#include <cvid/Types.h>
#include <cvid/Texture.h>

namespace cvid
{
	//Material of a model
	class Material
	{
	public:
		//Default constructor initializes nothing
		Material() {};
		//Load a material from file
		Material(std::string path);

		std::string name;
		//Smart pointer cuz im lazy
		std::shared_ptr<Texture> texture = nullptr;
		//Currently we only use diffuse
		Color diffuseColor;
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
		//Default material of this model, all instances automatically inherit it
		Material material;

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
		const Model* GetBaseModel() const;

		//Set the material this intance will use
		void SetMaterial(Material* mat);
		//Get a pointer to the material of this model
		const Material* GetMaterial() const;

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
		//Manually set the transform matrix
		void SetTransform(const Matrix4& mat);
		//Get the transform matrix
		const Matrix4& GetTransform();

		Matrix4 rotationMatrix = Matrix4::Identity();

	private:
		Model* model;
		Material* material;

		Matrix4 transform;
		Sphere boundingSphere;

		//Transforms
		Vector3 position;
		Vector3 rotation;
		Vector3 scale = Vector3(1);

		//Does the transform matrix need to be recalculated
		bool staleTransform = true;
		//Does the bounding sphere need to be recalculated, 0b01 = center point, 0b10 = everything, 0b00 = nothing
		int staleBounds = 0b10;
	};
}