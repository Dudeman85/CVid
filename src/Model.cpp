#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <cvid/Model.h>
#include <cvid/Helpers.h>

namespace cvid
{
	Model::Model(std::string path)
	{
		tinyobj::ObjReaderConfig reader_config;
		//Path to material files reative to obj file
		reader_config.mtl_search_path = "./";

		//Read obj from path
		tinyobj::ObjReader reader;
		if (!reader.ParseFromFile(path, reader_config))
		{
			//Check for any errors
			if (!reader.Error().empty())
				cvid::LogError("Cvid Error in Load Model: " + reader.Error());
			exit(1);
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
				//For each of the 3 vertices in a face (forced triangulation)
				for (size_t v = 0; v < 3; v++)
				{
					//Indexes of this vertice to the attributes array
					tinyobj::index_t idx = shape.mesh.indices[face * 3 + v];

					//Make the face
					Face f;

					//Add vertex indices
					f.verticeIndices[v] = idx.vertex_index;

					//Add texture coords if applicable
					if (idx.texcoord_index >= 0) 
						f.texCoordIndices[v] = idx.texcoord_index;

					//Add normal if applicable
					if (idx.normal_index >= 0)
						f.normal = idx.normal_index;
				}
			}

			//For every face in the shape
			size_t indexOffset = 0;
			for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
			{
				size_t fv = size_t(shape.mesh.num_face_vertices[f]);

			}
		}

		/*
		// Loop over shapes
		for (size_t s = 0; s < shapes.size(); s++)
		{
			// Loop over faces(polygon)
			size_t index_offset = 0;
			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
			{
				size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

				// Loop over vertices in the face.
				for (size_t v = 0; v < fv; v++)
				{
					// access to vertex
					tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
					tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
					tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
					tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

					// Check if `normal_index` is zero or positive. negative = no normal data
					if (idx.normal_index >= 0)
					{
						tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
						tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
						tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
					}

					// Check if `texcoord_index` is zero or positive. negative = no texcoord data
					if (idx.texcoord_index >= 0)
					{
						tinyobj::real_t tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
						tinyobj::real_t ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
					}

					// Optional: vertex colors
					// tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
					// tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
					// tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
				}
				index_offset += fv;

				// per-face material
				shapes[s].mesh.material_ids[f];
			}
		}
		*/
	}
}