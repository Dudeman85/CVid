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
	Vector3 Camera::GetRotation()
	{
		return rotation;
	}

	Vector3 Camera::GetFacing()
	{
		if (updateView)
			UpdateView();
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

	//Get the view matrix of this camera
	Matrix4 Camera::GetView()
	{
		if (updateView)
			UpdateView();

		return view;
	}

	//Update view matrix and facing vector whenever transforms are changed
	void Camera::UpdateView()
	{
		//Calculate the inverse model matrix
		Matrix4 view = Matrix4::Identity();
		view = view.Rotate(Vector3(0) - rotation);
		view = view.Translate(Vector3(0) - position);
		this->view = view;
		updateView = false;

		//Calculate the facing vector
		//Create a rotation matrix
		Matrix4 rot = cvid::Matrix4::Identity();
		rot = rot.RotateZ(rotation.z);
		rot = rot.RotateY(rotation.y);
		rot = rot.RotateX(rotation.x);

		//Camera is facing towards -Z by default
		facing = Vector4(0, 0, -1, 1);

		facing = rot * facing;
	}
}