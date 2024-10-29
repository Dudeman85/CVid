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

	void Camera::Translate(Vector3 translation)
	{
		updateView = true;
		position += translation;
	}
	void Camera::SetPosition(Vector3 position)
	{
		if (position != this->position)
			updateView = true;
		this->position = position;
	}
	void Camera::Rotate(Vector3 rotation)
	{
		updateView = true;
		this->rotation += rotation;
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

	Vector3 Camera::GetFacing()
	{
		if (updateView)
		{
			//Create a rotation matrix
			Matrix4 rot = cvid::Matrix4::Identity();
			rot = rot.RotateX(cvid::Radians(rotation.x));
			rot = rot.RotateY(cvid::Radians(rotation.y));
			rot = rot.RotateZ(cvid::Radians(rotation.z));

			//Camera is facing towards -Z by default
			facing = Vector4(0, 0, -1, 1);

			facing = rot * facing;
		}
		return facing;
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
			view = view.Rotate(Vector3(0) - rotation);
			view = view.Translate(Vector3(0) - position);
			this->view = view;
			updateView = false;
		}
		return view;
	}
}