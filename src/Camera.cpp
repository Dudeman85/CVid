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
		updateTransform = true;
		position += translation;
	}
	void Camera::SetPosition(Vector3 position)
	{
		if (position != this->position) 
		{
			updateView = true;
			updateTransform = true;
		}
		this->position = position;
	}
	void Camera::Rotate(Vector3 rotation)
	{
		updateView = true;
		updateTransform = true;
		this->rotation += rotation;
	}
	void Camera::SetRotation(Vector3 rotation)
	{
		if (rotation != this->rotation) 
		{
			updateView = true;
			updateTransform = true;
		}
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

	//Get the forward (-Z) axis as a world space vector
	Vector3 Camera::GetForward()
	{
		//Update the vector is needed
		if (updateTransform)
			UpdateTransform();

		return forward;
	}
	//Get the right (+X) axis as a world space vector
	Vector3 Camera::GetRight()
	{
		//Update the vector is needed
		if (updateTransform)
			UpdateTransform();

		return right;
	}
	//Get the up (+Y) axis as a world space vector
	Vector3 Camera::GetUp()
	{
		//Update the vector is needed
		if (updateTransform)
			UpdateTransform();

		return up;
	}

	//Set the camera to use perspective projection
	void Camera::SetPerspective(float distance)
	{
		this->distance = distance;
		projection = Matrix4::Identity();
		projection[0][0] = distance;
		projection[1][1] = distance;
		projection[2][2] = 1;

		perspective = true;
	}

	//Set the camera to use orthographic projection
	void Camera::SetOrtho(float width, float height)
	{
		this->width = width;
		this->height = height;
		perspective = false;
	}

	//Check if the camera is using perspective projection
	bool Camera::IsPerspective()
	{
		return perspective;
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
		view = view.Translate(Vector3(0) - position);
		view = view.Rotate(Vector3(0) - rotation);
		this->view = view;
		updateView = false;
	}

	//Update the directional vectors
	void Camera::UpdateTransform()
	{
		//Create a rotation matrix
		Matrix4 rot = cvid::Matrix4::Identity();
		rot = rot.RotateZ(rotation.z);
		rot = rot.RotateY(rotation.y);
		rot = rot.RotateX(rotation.x);

		//Forward is -Z
		forward = rot * Vector4(0, 0, -1, 1);
		//Right is +X
		right = rot * Vector4(1, 0, 0, 1);
		//Up is +Y
		up = rot * Vector4(0, 1, 0, 1);

		updateTransform = false;
	}
}