#include <iostream>
#include <cvid/Window.h>
#include <cvid/Matrix.h>
#include <glm/gtc/matrix_transform.hpp>

using namespace cvid;

int main()
{
	glm::mat4 mvp = glm::mat4(1);

	//mvp = glm::scale(mvp, glm::vec3(4, 5, 6));

	mvp = glm::rotate(mvp, glm::radians(7.f), glm::vec3(1.0f, 0.0f, 0.0f));
	mvp = glm::rotate(mvp, glm::radians(8.f), glm::vec3(0.0f, 1.0f, 0.0f));
	mvp = glm::rotate(mvp, glm::radians(9.f), glm::vec3(0.0f, 0.0f, 1.0f));

	//mvp = glm::translate(mvp, glm::vec3(1, 2, 3));


	Matrix4 model = Matrix4::Identity();

	//model = model.Scale({ 4, 5, 6 });
	std::cout << model.ToString() << std::endl;

	model = model.Rotate({ cvid::Radians(7.0), cvid::Radians(8.0), cvid::Radians(9.0) });
	std::cout << model.ToString() << std::endl;

	//model = model.Translate({ 1, 2, 3 });
	std::cout << model.ToString() << std::endl;


	return 0;
}