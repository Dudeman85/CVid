#include <cvid/Camera.h>

namespace cvid
{

	Camera::Camera(Vector3 position, float width, float height)
	{
		this->position = position;
		this->aspectRatio = width / height;
	}

	void Camera::Translate(Vector3 translation)
	{
		updateView = true;
		updateDirection = true;
		position += translation;
	}
	void Camera::SetPosition(Vector3 position)
	{
		if (position != this->position)
		{
			updateView = true;
			updateDirection = true;
		}
		this->position = position;
	}
	//Rotate this camera in radians
	void Camera::Rotate(Vector3 rotation)
	{
		updateView = true;
		updateDirection = true;
		this->rotation += rotation;
	}
	//Set this camera's rotation in radians
	void Camera::SetRotation(Vector3 rotation)
	{
		if (rotation != this->rotation)
		{
			updateView = true;
			updateDirection = true;
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
		if (updateDirection)
			UpdateDirection();

		return forward;
	}
	//Get the right (+X) axis as a world space vector
	Vector3 Camera::GetRight()
	{
		//Update the vector is needed
		if (updateDirection)
			UpdateDirection();

		return right;
	}
	//Get the up (+Y) axis as a world space vector
	Vector3 Camera::GetUp()
	{
		//Update the vector is needed
		if (updateDirection)
			UpdateDirection();

		return up;
	}

	//Set the field of view
	void Camera::SetFOV(float fov) 
	{
		this->fov = fov;

		UpdateProjection();
		UpdateClipPlanes();
	}

	//Set the camera to use perspective projection
	void Camera::MakePerspective(float fov, float near, float far)
	{
		perspective = true;
		this->fov = fov;
		nearPlane = near;
		farPlane = far;

		UpdateProjection();
		UpdateClipPlanes();
	}

	//Set the camera to use orthographic projection
	void Camera::MakeOrtho(float width, float height)
	{
		perspective = false;
	}

	//Check if the camera is using perspective projection
	bool Camera::IsPerspective()
	{
		return perspective;
	}

	//Get the view matrix of this camera
	const Matrix4& Camera::GetView()
	{
		if (updateView)
			UpdateView();

		return view;
	}

	//Get the peojection matrix of this camera
	const Matrix4& Camera::GetProjection()
	{
		return projection;
	}

	//Get the near, left, right, bottom, and top clip planes in that order
	std::array<Vector3, 5> Camera::GetClipPlanes()
	{
		return { nearClip, leftClip, rightClip, bottomClip, topClip };
	}

	//Update view matrix and facing vector whenever transforms are changed
	void Camera::UpdateView()
	{
		//Calculate the inverse model matrix
		Matrix4 view = Matrix4::Identity();
		view = view.Translate(Vector3(0) - position);
		view = view.RotateZ(-rotation.z);
		view = view.RotateY(-rotation.y);
		view = view.RotateX(-rotation.x);

		this->view = view;
		updateView = false;
	}

	//Update the projection matrix
	void Camera::UpdateProjection()
	{
		projection = Matrix4::Identity();

		if (perspective)
		{
			//Calculate the camera's dimensions for projection matrix
			float top = tan(Radians(fov / 2)) * nearPlane;
			float bottom = -top;
			float right = top * aspectRatio;
			float left = -right;

			//Make the OpenGL standard perspective matrix
			projection[0][0] = 2 * nearPlane / (right - left);
			projection[1][1] = 2 * nearPlane / (top - bottom);
			projection[2][2] = -(farPlane + nearPlane) / (farPlane - nearPlane);
			projection[3][2] = -(2 * farPlane * nearPlane) / (farPlane - nearPlane);
			projection[2][0] = (right + left) / (right - left);
			projection[2][1] = (top + bottom) / (top - bottom);
			projection[2][3] = -1;
			projection[3][3] = 0;
		}
		else 
		{
			//TODO implement ortho projection
		}
	}

	//Update the directional vectors
	void Camera::UpdateDirection()
	{
		//Create a rotation matrix
		Matrix4 rot = cvid::Matrix4::Identity();
		rot = rot.RotateX(rotation.x);
		rot = rot.RotateY(rotation.y);
		rot = rot.RotateZ(rotation.z);

		//Forward is -Z
		forward = rot * Vector4(0, 0, -1, 1);
		//Right is +X
		right = rot * Vector4(1, 0, 0, 1);
		//Up is +Y
		up = rot * Vector4(0, 1, 0, 1);

		updateDirection = false;
	}

	//Update the clip planes
	void Camera::UpdateClipPlanes()
	{
		//Precalculate the angles of the 4 planes
		float angle = Radians(fov / 2);
		float horizontalAngle = atan(angle) * aspectRatio;

		//Near is always the same for now atleast
		nearClip = Vector3(0, 0, -1);
		//These are calculated as normal vectors facing into the clip space
		topClip = Vector3(0, -cos(angle), -sin(angle));
		bottomClip = Vector3(0, cos(angle), -sin(angle));
		leftClip = Vector3(cos(horizontalAngle), 0, -sin(horizontalAngle));
		rightClip = Vector3(-cos(horizontalAngle), 0, -sin(horizontalAngle));
	}
}