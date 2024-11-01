#pragma once
#include <cvid/Vector.h>
#include <cvid/Matrix.h>

namespace cvid
{
	//A camera object to render a scene through
	class Camera
	{
	public:
		Camera(Vector3 position, float width, float height, float distance);
		
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
		void SetPerspective(float distance);
		//Set the camera to use orthographic projection
		void SetOrtho(float width, float height);

		//Get the view  matrix of this camera
		Matrix4 GetView();

		//Viewport properties
		float width;
		float height;
		float distance;

	private:
		Vector3 position;
		Vector3 rotation;

		//Directional vectors in world space, updated when transform is changed
		Vector3 forward; // -Z
		Vector3 right; // +X
		Vector3 up; // +Y

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

		Matrix4 view;
		Matrix4 projection;
	};
}