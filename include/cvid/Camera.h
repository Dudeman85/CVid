#pragma once
#include <array>
#include <cvid/Vector.h>
#include <cvid/Matrix.h>

namespace cvid
{
	//A camera object to render a scene through
	class Camera
	{
	public:
		Camera(Vector3 position, float width, float height);
		
		//Transform setters
		void Translate(Vector3 translation);
		void SetPosition(Vector3 position);
		//Rotate this camera in radians
		void Rotate(Vector3 rotation);
		//Set this camera's rotation in radians
		void SetRotation(Vector3 rotation);

		//Transform getters
		Vector3 GetPosition();
		Vector3 GetRotation();

		//Get the forward (-Z) axis as a world space vector
		Vector3 GetForward();
		//Get the right (+X) axis as a world space vector
		Vector3 GetRight();
		//Get the up (+Y) axis as a world space vector
		Vector3 GetUp();

		//Check if the camera is using perspective projection
		bool IsPerspective();

		//Set the field of view
		void SetFOV(float fov);

		//Set the camera to use perspective projection
		void MakePerspective(float fov, float near, float far);
		//Set the camera to use orthographic projection
		void MakeOrtho(float width, float height);

		//Get the view  matrix of this camera
		const Matrix4& GetView();
		//Get the peojection matrix of this camera
		const Matrix4& GetProjection();

		//Get the near, left, right, bottom, and top clip planes in that order
		std::array<Vector3, 5> GetClipPlanes();

	private:
		Vector3 position;
		Vector3 rotation;

		//Directional vectors in world space, updated when transform is changed
		Vector3 forward; // -Z
		Vector3 right; // +X
		Vector3 up; // +Y

		//Clip planes in camera space, updated when fov is changed
		Vector3 nearClip;
		Vector3 leftClip;
		Vector3 rightClip;
		Vector3 bottomClip;
		Vector3 topClip;

		//Vertical fov, for horizontal, multiply by aspect ratio
		float fov = 90;
		float aspectRatio;
		float farPlane = 1000;
		float nearPlane = 1;

		//Is the camera using perspective projection
		bool perspective = false;

		//Should the view matrix be updated on next fetch
		bool updateView = true;
		//Should the directional vectors be updated
		bool updateDirection = true;

		//Update the view matrix
		void UpdateView();
		//Update the projection matrix
		void UpdateProjection();
		//Update the directional vectors
		void UpdateDirection();
		//Update the clip planes
		void UpdateClipPlanes();

		Matrix4 view;
		Matrix4 projection;
	};
}