#pragma once
#include <cvid/Vector.h>

namespace cvid
{
	//A camera object to render a scene through
	class Camera
	{
	public:
		Camera(Vector3 position, float width, float height, float distance);

		void SetPosition(Vector3 position);
		void GetPosition(Vector3 position);
		//Set the properties of the viewport
		bool SetViewport(float width, float height, float distance);
	private:
		Vector3 position;
		//Viewport properties
		float width;
		float height;
		float distance;
	};
}