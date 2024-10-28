#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <cvid/Model.h>

namespace cvid
{
	Model::Model(std::string path)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn;
		std::string err;

	}
}