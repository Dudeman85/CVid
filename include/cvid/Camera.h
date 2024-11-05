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
		void Rotate(Vector3 rotation);
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

		//Set the camera to use perspective projection
		void SetPerspective(float fov);
		//Set the camera to use orthographic projection
		void SetOrtho(float width, float height);

		//Get the view  matrix of this camera
		Matrix4 GetView();

		//Get the near, left, right, bottom, and top clip planes in that order
		std::array<Vector3, 5> GetClipPlanes();

		//Vertical fov, for horizontal, multiply by aspect ratio
		float fov;
		float aspectRatio;

	private:
		Vector3 position;
		Vector3 rotation;

		//Directional vectors in world space, updated when transform is changed
		Vector3 forward; // -Z
		Vector3 right; // +X
		Vector3 up; // +Y

		//Clip planes in camera space, updated when fov is changed
		//TODO add far clip plane
		Vector3 nearClip;
		Vector3 leftClip;
		Vector3 rightClip;
		Vector3 bottomClip;
		Vector3 topClip;

		//Is the camera using perspective projection
		bool perspective = false;

		//Should the view matrix be updated on next fetch
		bool updateView = true;
		//Should the directional vectors be updated
		bool updateTransform = true;

		//Update the view matrix
		void UpdateView();
		//Update the directional vectors
		void UpdateTransform();
		//Update the clip planes
		void UpdateClipPlanes();

		Matrix4 view;
		
		//TODO Fix
	public:
		Matrix4 projection;
	};
}