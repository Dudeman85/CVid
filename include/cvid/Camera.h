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
		//Get the camera's forward vector in world space
		Vector3 GetFacing();
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

		bool perspective = false;

		//Should the view matrix be updated on next fetch
		bool updateView = true;

		void UpdateView();

		Matrix4 view;
		Matrix4 projection;
		Vector4 facing;
	};
}