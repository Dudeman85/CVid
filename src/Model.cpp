#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <cvid/Model.h>
#include <cvid/Helpers.h>
#include <cvid/Window.h>

namespace cvid
{
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

		if(!ret)
			//Check for any errors
			if (!reader.Error().empty())
				cvid::LogError("Cvid Error in Load Model: " + reader.Error());
		//Check for any warnings
		if(!reader.Warning().empty())
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
}