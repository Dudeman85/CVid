#include <format>
#include <thread>
#include <chrono>
#include <cvid/Window.h>
#include <cvid/Helpers.h>
#include <cvid/Renderer.h>
#include <cvid/Model.h>
#include <cvid/Matrix.h>
#include <cvid/Math.h>
#include <cvid/Rasterizer.h>
#include <quaternion.h>

//https://gabrielgambetta.com/computer-graphics-from-scratch/
//https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-defwindowproca
//https://learn.microsoft.com/en-us/windows/win32/winmsg/using-messages-and-message-queues#examining-a-message-queue
//https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix/building-basic-perspective-projection-matrix.html

int main()
{
	//Make window
	cvid::Vector2Int windowSize = { 160, 90 };
	cvid::Window window(windowSize.x, windowSize.y, "CVid", false);
	window.enableDepthTest = true;

	//Make camera
	cvid::Camera cam(cvid::Vector3(0, 0, 100), windowSize.x, windowSize.y);
	float fov = 90;
	cam.MakePerspective(fov, 1, 5000);
	cam.Rotate(cvid::Vector3(0, cvid::Radians(0), 0));

	//Load models
	cvid::Model cube("../../../resources/Achelous.obj");

	//Make the renderable instance
	cvid::ModelInstance modelInstance(&cube);
	modelInstance.SetScale(20);
	modelInstance.SetPosition({ 0, 0, 0 });
	//Use a quaternion here to store cube rotation since it's not properly implemented in engine yet
	quaternion::Quaternion<double> modelRotationQuat = quaternion::from_euler(std::array{ 0, 0, 0 });

	//Control speed
	const float rotationSpeed = 0.01;
	const float scrollSpeed = 10;

	POINT currentCursorPos;
	POINT lastCursorPos;
	bool lcDown = false;

	while (true)
	{
		cvid::StartTimePoint();

		if (GetKeyState(VK_ESCAPE) & 0x8000)
			return 0;

		//On left click
		if (GetKeyState(VK_LBUTTON) & 0x8000)
		{
			if (!lcDown)
			{
				GetCursorPos(&currentCursorPos);
				lastCursorPos = currentCursorPos;
				lcDown = true;
			}
		}
		else
		{
			lcDown = false;
		}
		//Update cursor delta
		if (lcDown)
		{
			lastCursorPos = currentCursorPos;
			GetCursorPos(&currentCursorPos);

			double dx = currentCursorPos.x - lastCursorPos.x;
			double dy = currentCursorPos.y - lastCursorPos.y;

			//Rotate model by mouse delta
			if (dx != 0 || dy != 0)
			{
				//Manually create the transform matrix using a quaternion rotation
				cvid::Matrix4 transform = cvid::Matrix4::Identity();
				transform = transform.Scale(modelInstance.GetScale());

				//Rotate using quaternions
				dx *= -rotationSpeed / 2;
				dy *= -rotationSpeed / 2;
				modelRotationQuat *= quaternion::Quaternion(cos(dx), 0.0, sin(dx), 0.0);
				modelRotationQuat *= quaternion::Quaternion(cos(dy), sin(dy), 0.0, 0.0);

				//Convert from quaternion to cvid matrix
				auto mat = quaternion::to_rotation_matrix(modelRotationQuat);
				cvid::Matrix4 rotation = cvid::Matrix4::Identity();
				for (size_t i = 0; i < mat.size(); i++)
				{
					for (size_t j = 0; j < mat[0].size(); j++)
					{
						rotation[i][j] = mat[i][j];
					}
				}
				transform = transform * rotation;

				transform = transform.Translate(modelInstance.GetPosition());

				modelInstance.SetTransform(transform);
			}
		}

		//Get the console input record for scroll wheel
		//Does not work in seperate window
		std::vector<INPUT_RECORD> ir = window.GetInputRecord();
		for (INPUT_RECORD& input : ir)
		{
			if (input.EventType == MOUSE_EVENT)
			{
				if (input.Event.MouseEvent.dwEventFlags == MOUSE_WHEELED)
				{
					short scrollAmount = HIWORD(input.Event.MouseEvent.dwButtonState);
					if (scrollAmount > 0)
					{
						//Move towards when scrolling forward
						cam.Translate(cam.GetForward() * scrollSpeed);
					}
					else
					{
						//Move away when scrolling backwards
						cam.Translate(cam.GetForward() * -scrollSpeed);
					}
				}
			}
		}

		float moveSpeed = 1;
		//W forward
		if (GetKeyState(87) & 0x8000)
			cam.Translate(cam.GetForward() * moveSpeed);
		//A left
		if (GetKeyState(65) & 0x8000)
			cam.Translate(cam.GetRight() * -moveSpeed);
		//S backward
		if (GetKeyState(83) & 0x8000)
			cam.Translate(cam.GetForward() * -moveSpeed);
		//D right
		if (GetKeyState(68) & 0x8000)
			cam.Translate(cam.GetRight() * moveSpeed);
		//Q down
		if (GetKeyState(81) & 0x8000)
			cam.Translate(cam.GetUp() * -moveSpeed);
		//E up
		if (GetKeyState(69) & 0x8000)
			cam.Translate(cam.GetUp() * moveSpeed);

		//Camera rotation
		float rotationSpeed = 0.02;
		//NP 8 pitch down
		if (GetKeyState(VK_NUMPAD8) & 0x8000)
			cam.Rotate(cvid::Vector3(-1, 0, 0) * rotationSpeed);
		//NP 5 pitch down
		if (GetKeyState(VK_NUMPAD5) & 0x8000)
			cam.Rotate(cvid::Vector3(1, 0, 0) * rotationSpeed);
		//NP 4 yaw left
		if (GetKeyState(VK_NUMPAD4) & 0x8000)
			cam.Rotate(cvid::Vector3(0, 1, 0) * rotationSpeed);
		//NP 6 yaw right
		if (GetKeyState(VK_NUMPAD6) & 0x8000)
			cam.Rotate(cvid::Vector3(0, -1, 0) * rotationSpeed);
		//NP 9 roll right
		if (GetKeyState(VK_NUMPAD9) & 0x8000)
			cam.Rotate(cvid::Vector3(0, 0, -1) * rotationSpeed);
		//NP 7 roll right
		if (GetKeyState(VK_NUMPAD7) & 0x8000)
			cam.Rotate(cvid::Vector3(0, 0, 1) * rotationSpeed);

		//NP 1 fov+
		if (GetKeyState(VK_NUMPAD1) & 0x8000)
			cam.SetFOV(++fov);
		//NP 2 fov-
		if (GetKeyState(VK_NUMPAD2) & 0x8000)
			cam.SetFOV(--fov);


		window.Fill({ 0, 0, 0 });
		window.ClearDepthBuffer();


		cvid::DrawModel(&modelInstance, &cam, &window);

		//std::cout << "Frame rendered in: " << cvid::EndTimePoint() << std::endl;

		if (!window.DrawFrame())
			return 0;

		//For some reason this stops the window from freezing
		window.SendData("\x1b[0;0H", 7, cvid::DataType::String);

		//std::cout << "Window responded in: " << cvid::EndTimePoint() << std::endl;
	}

	return 0;
}