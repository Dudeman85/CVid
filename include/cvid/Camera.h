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
		Vector3 GetFacing();

		//Set the properties of the viewport
		bool SetViewport(float width, float height, float distance);

		//Get the projection matrix  of this camera
		Matrix4 GetProjection();
		//Get the view  matrix of this camera
		Matrix4 GetView();

	private:
		Vector3 position;
		Vector3 rotation;

		//Viewport properties
		float width;
		float height;
		float distance;

		//Should the view matrix be updated on next fetch
		bool updateView = true;

		Matrix4 view;
		Matrix4 projection;
		Vector4 facing;
	};
}