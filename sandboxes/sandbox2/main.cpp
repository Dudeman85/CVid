#include <format>
#include <thread>
#include <chrono>
#include <filesystem>
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
	cvid::Window window(windowSize.x, windowSize.y, "CVid", true);
	window.enableDepthTest = true;

	//Make camera
	cvid::Camera cam(cvid::Vector3(0, 0, 100), windowSize.x, windowSize.y);
	float fov = 90;
	cam.MakePerspective(fov, 1, 5000);
	cam.Rotate(cvid::Vector3(0, cvid::Radians(0), 0));

	//Load all models in resources folder
	std::vector<cvid::Model> models;
	for (auto& p : std::filesystem::recursive_directory_iterator("../../../resources/"))
	{
		if (p.path().string().ends_with(".obj"))
		{
			models.push_back(cvid::Model(p.path().string()));
		}
	}
	int currentModel = 0;

	//Make the renderable instance
	cvid::ModelInstance displayModel(&models[0]);
	displayModel.SetScale(20);
	displayModel.SetPosition({ 0, 0, 0 });
	//Use a quaternion here to store cube rotation since it's not properly implemented in engine yet
	quaternion::Quaternion<double> modelRotationQuat = quaternion::from_euler(std::array{ 0, 0, 0 });

	//Control speed
	const float rotationSpeed = 0.01;
	const float scrollSpeed = 10;

	double deltaTime = 0;
	POINT currentCursorPos;
	POINT lastCursorPos;
	bool lcDown = false;
	const float transitionDelay = 1;
	float transitionTimer = 0;

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
				transform = transform.Scale(displayModel.GetScale());

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

				transform = transform.Translate(displayModel.GetPosition());

				displayModel.SetTransform(transform);
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

		//Decrement the display model
		if (GetKeyState(VK_LEFT) & 0x8000 && transitionTimer <= 0)
		{
			currentModel--;
			if (currentModel < 0)
				currentModel = models.size() - 1;

			displayModel.SetBaseModel(&models[currentModel]);
			transitionTimer = transitionDelay;
		}
		//Increment the display model
		if (GetKeyState(VK_RIGHT) & 0x8000 && transitionTimer <= 0)
		{
			currentModel++;
			if (currentModel > models.size() - 1)
				currentModel = 0;

			displayModel.SetBaseModel(&models[currentModel]);
			transitionTimer = transitionDelay;
		}
		transitionTimer -= deltaTime;


		//Start rendering
		window.Fill({ 0, 0, 0 });
		window.ClearDepthBuffer();

		cvid::DrawModel(&displayModel, &cam, &window);

		//Display the model name text
		std::string name = std::format("<- {} ->", displayModel.GetBaseModel()->name);
		int namePos = windowSize.x / 2 - name.size() / 2;
		window.PutString({ namePos, windowSize.y / 2 - 2 }, name);


		if (!window.DrawFrame())
			return 0;

		//For some reason this stops the window from freezing
		window.SendData("\x1b[0;0H", 7, cvid::DataType::String);

		deltaTime = cvid::EndTimePoint();
	}

	return 0;
}