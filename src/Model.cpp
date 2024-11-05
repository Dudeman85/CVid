#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <cvid/Model.h>
#include <cvid/Helpers.h>
#include <cvid/Window.h>

namespace cvid
{
	//Model

	Model::Model(std::string path)
	{
		LoadModel(path);
	}

	//Load a model from disk
	void Model::LoadModel(std::string path)
	{
		tinyobj::ObjReaderConfig reader_config;
		//Path to material files reative to obj file
		reader_config.mtl_search_path = "";
		reader_config.triangulate = true;

		//Read obj from path
		tinyobj::ObjReader reader;
		bool ret = reader.ParseFromFile(path, reader_config);

		if (!ret)
		{
			//Check for any errors
			if (!reader.Error().empty())
				cvid::LogError("Cvid Error in Load Model: " + reader.Error());
			return;
		}
		//Check for any warnings
		if (!reader.Warning().empty())
			cvid::LogWarning("Cvid Warning in Load Model: " + reader.Warning());

		const tinyobj::attrib_t& attrib = reader.GetAttrib();
		auto& materials = reader.GetMaterials();

		//Copy the attrib vertices to custom vertices
		vertices.reserve(attrib.vertices.size());
		for (size_t i = 0; i < attrib.vertices.size(); i += 3)
		{
			Vertex v;
			//Copy the vertex position
			v.position.x = attrib.vertices[i + 0];
			v.position.y = attrib.vertices[i + 1];
			v.position.z = attrib.vertices[i + 2];
			vertices.push_back(v);
		}

		//For every shape in the model
		for (tinyobj::shape_t shape : reader.GetShapes())
		{
			//For each face in the shape
			for (size_t face = 0; face < shape.mesh.num_face_vertices.size(); face++)
			{
				//Make the face
				Face f;

				std::vector<Vector3> vertices;
				//For each of the 3 vertices in a face (forced triangulation)
				for (size_t v = 0; v < 3; v++)
				{
					//Indexes of this vertice to the attributes array
					tinyobj::index_t idx = shape.mesh.indices[face * 3 + v];

					//Add vertex indices
					f.verticeIndices[v] = idx.vertex_index;

					//Add texture coords if applicable
					if (idx.texcoord_index >= 0)
						f.texCoordIndices[v] = idx.texcoord_index;

					//Store the vertice for nomal calculation
					Vector3 vertice
					{
						attrib.vertices[3 * size_t(idx.vertex_index) + 0],
						attrib.vertices[3 * size_t(idx.vertex_index) + 1],
						attrib.vertices[3 * size_t(idx.vertex_index) + 2]
					};
					vertices.push_back(vertice);
				}

				//TODO: TEMPORARY FOR TESTING
				f.color = cvid::RandomColor();
				//f.color = cvid::Color::BrightBlue;

				faces.push_back(f);
			}
		}
	}


	//ModelInstance

	//Make a renderable instance from a model
	ModelInstance::ModelInstance(Model* model) 
	{
		SetBaseModel(model);
		staleTransform = true;
	}

	//Set the model this intance will use
	void ModelInstance::SetBaseModel(Model* model)
	{
		this->model = model;
		staleBounds = true;
	}
	//Get a pointer to the base model of this instance
	Model* ModelInstance::GetBaseModel() const
	{
		return model;
	}

	//Transform setters
	//Move this model in world space by translation
	void ModelInstance::Translate(Vector3 translation) 
	{
		this->position += translation;
		staleTransform = true;
		staleBounds = 1;
	}
	//Set this model's position in world space
	void ModelInstance::SetPosition(Vector3 position) 
	{
		this->position = position;
		staleTransform = true;
		staleBounds = 1;
	}
	//Rotate this model by euler angles in world space in radians
	void ModelInstance::Rotate(Vector3 rotation) 
	{
		this->rotation += rotation;
		staleTransform = true;
		staleBounds = 1;
	}
	//Set this model's euler rotation in world space by rotation in radians
	void ModelInstance::SetRotation(Vector3 rotation) 
	{
		this->rotation = rotation;
		staleTransform = true;
		staleBounds = 1;
	}
	//Scale this model in world space
	void ModelInstance::Scale(Vector3 scale) 
	{
		this->scale += scale;
		staleTransform = true;
		staleBounds = 2;

	}
	//Set this model's scale in world space
	void ModelInstance::SetScale(Vector3 scale) 
	{
		this->scale = scale;
		staleTransform = true;
		staleBounds = 2;
	}

	//Transform getters
	//Get the world space position of this model
	Vector3 ModelInstance::GetPosition() const
	{
		return position;
	}
	//Get the euler rotation of this model in radians
	Vector3 ModelInstance::GetRotation() const
	{
		return rotation;
	}
	//Get the scale of this model
	Vector3 ModelInstance::GetScale() const
	{
		return scale;
	}

	//Recalculate the bounding sphere, this should be called after scale has been changed
	void ModelInstance::RecalculateBounds() 
	{
		//Calculate the center point of the vertices
		boundingSphere.center = Vector3();
		for (const Vertex& vert : model->vertices)
		{
			boundingSphere.center += vert.position;
		}
		boundingSphere.center /= model->vertices.size();

		//Recalculate the radius if scale has been changed
		if (staleBounds == 2) 
		{
			//The radius of the sphere is defined as the distance from the center to the furthest polygon
			boundingSphere.radius = 0;
			for (const Vertex& vert : model->vertices)
			{
				double dist = vert.position.Distance(boundingSphere.center);
				if (dist > boundingSphere.radius)
					boundingSphere.radius = dist;
			}
		}

		staleBounds = 0;
	}

	//Recalculate the transform matrix, this should be called after any transform has been changed
	void ModelInstance::RecalculateTransform()
	{
		transform = cvid::Matrix4::Identity();
		transform = transform.Scale(scale);
		transform = transform.Rotate(rotation);
		transform = transform.Translate(position);

		staleTransform = false;
	}

	//Get the rough bounding sphere of this model in world coords
	Sphere ModelInstance::GetBoundingSphere() 
	{
		if (staleBounds > 0)
			RecalculateBounds();
		return boundingSphere;
	}
	//Get the transform matrix
	Matrix4 ModelInstance::GetTransform()
	{
		if (staleTransform)
			RecalculateTransform();
		return transform;
	}
}