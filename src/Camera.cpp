#include <cvid/Camera.h>

namespace cvid
{

	Camera::Camera(Vector3 position, float width, float height, float distance)
	{
		this->position = position;
		this->width = width;
		this->height = height;
		this->distance = distance;
	}

	void Camera::SetPosition(Vector3 position)
	{
		if (position != this->position)
			updateView = true;
		this->position = position;
	}
	void Camera::SetRotation(Vector3 rotation)
	{
		if (rotation != this->rotation)
			updateView = true;
		this->rotation = rotation;
	}
	Vector3 Camera::GetPosition()
	{
		return position;
	}

	//Set the properties of the viewport
	bool Camera::SetViewport(float width, float height, float distance)
	{
		this->width = width;
		this->height = height;
		this->distance = distance;

		return true;
	}

	//Get the projection matrix  of this camera
	Matrix4 Camera::GetProjection()
	{


		return projection;
	}
	//Get the view  matrix of this camera
	Matrix4 Camera::GetView()
	{
		if (updateView)
		{
			//Calculate the inverse model matrix
			Matrix4 view = Matrix4::Identity();
			view = view.Translate(position);
			view = view.Rotate(rotation);
			view = view.Inverse();
			this->view = view;
			updateView = false;
		}
		return view;
	}
}