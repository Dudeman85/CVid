#include <format>
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
	//Control speed
	const float rotationSpeed = 0.01;
	const float minScrollSpeed = 2;
	const float maxScrollSpeed = 40;
	//Zoom parameters
	const float minZoom = 0;
	const float maxZoom = 700;
	//Transition parameters
	const float minSpawn = 250;
	const float maxSpawn = 1500;
	const float minSpeed = 1000;
	const float maxSpeed = 4000;
	const float transitionDelay = 0.5;
	double transitionTimer = 0;
	double transitionDir = 0;

	//Cursor data
	POINT currentCursorPos;
	POINT lastCursorPos;
	HWND hWnd = GetConsoleWindow();
	bool clickedInWindow = false;

	//Performance data
	double deltaTime = 0;
	double timeSinceLastAvg = 0;
	double framesSinceLastAvg = 0;
	double diagDt = 0;
	double avgDt = 0;
	double windowLatency = 0;
	double avgLatency = 0;
	double renderTime = 0;
	double avgRender = 0;


	//Make window
	cvid::Vector2Int windowSize = { 160, 90 };
	cvid::Window window(windowSize.x, windowSize.y, "CVid Demo", false);
	window.enableDepthTest = true;

	//Make camera
	cvid::Camera cam(cvid::Vector3(0, -5, 150), windowSize.x, windowSize.y);
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
	quaternion::Quaternion<double> modelRotationQuat(0.7150, -0.4207, -0.3940, -0.0848);
	cvid::Matrix4 rotation = cvid::Matrix4::Identity();
	//Convert from quaternion to cvid matrix
	auto mat = quaternion::to_rotation_matrix(modelRotationQuat);
	for (size_t i = 0; i < mat.size(); i++)
	{
		for (size_t j = 0; j < mat[0].size(); j++)
		{
			rotation[i][j] = mat[i][j];
		}
	}

	//Make the second model in swapping
	cvid::ModelInstance swapModel(&models[1]);
	swapModel.SetScale(20);
	swapModel.SetPosition({ 0, 0, 0 });

	while (true)
	{
		cvid::StartTimePoint();

		if (GetKeyState(VK_ESCAPE) & 0x8000)
			return 0;

		//Update cursor delta
		if (GetKeyState(VK_LBUTTON) & 0x8000 && clickedInWindow)
		{
			lastCursorPos = currentCursorPos;
			GetCursorPos(&currentCursorPos);

			double dx = currentCursorPos.x - lastCursorPos.x;
			double dy = currentCursorPos.y - lastCursorPos.y;

			//Rotate model by mouse delta
			if (dx != 0 || dy != 0)
			{
				//Rotate using quaternions
				dx *= -rotationSpeed / 2;
				dy *= -rotationSpeed / 2;
				modelRotationQuat *= quaternion::Quaternion(cos(dx), 0.0, sin(dx), 0.0);
				modelRotationQuat *= quaternion::Quaternion(cos(dy), sin(dy), 0.0, 0.0);

				//Convert from quaternion to cvid matrix
				auto mat = quaternion::to_rotation_matrix(modelRotationQuat);
				rotation = cvid::Matrix4::Identity();
				for (size_t i = 0; i < mat.size(); i++)
				{
					for (size_t j = 0; j < mat[0].size(); j++)
					{
						rotation[i][j] = mat[i][j];
					}
				}
			}
		}
		else
		{
			GetCursorPos(&currentCursorPos);
			//Make sure the user clicked insisde the window
			RECT bounds;
			GetClientRect(hWnd, &bounds);
			ClientToScreen(hWnd, (LPPOINT)&bounds);
			ClientToScreen(hWnd, (LPPOINT)&bounds + 1);
			clickedInWindow = currentCursorPos.x > bounds.left && currentCursorPos.x < bounds.right
				&& currentCursorPos.y > bounds.top && currentCursorPos.y < bounds.bottom;
		}

		//When the model transition animation is going dont respond to zoom or arrows
		if (transitionTimer <= 0)
		{
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
							if (cam.GetPosition().z > minZoom)
							{
								//Move towards when scrolling forward
								cam.Translate(cam.GetForward() * std::lerp(minScrollSpeed, maxScrollSpeed, cam.GetPosition().z / maxZoom));
							}
						}
						else
						{
							if (cam.GetPosition().z < maxZoom)
							{
								//Move away when scrolling backwards
								cam.Translate(cam.GetForward() * -std::lerp(minScrollSpeed, maxScrollSpeed, cam.GetPosition().z / maxZoom));
							}
						}
					}
				}
			}

			//Decrement the display model
			if (GetKeyState(VK_LEFT) & 0x8000)
			{
				swapModel.SetBaseModel(&models[currentModel]);
				swapModel.SetPosition({ 0, 0, 0 });

				currentModel--;
				if (currentModel < 0)
					currentModel = models.size() - 1;

				displayModel.SetBaseModel(&models[currentModel]);
				displayModel.SetPosition({ -std::lerp(minSpawn, maxSpawn, cam.GetPosition().z / maxZoom), 0, 0 });
				transitionTimer = transitionDelay;
				transitionDir = std::lerp(minSpeed, maxSpeed, cam.GetPosition().z / maxZoom);
			}
			//Increment the display model
			if (GetKeyState(VK_RIGHT) & 0x8000)
			{
				swapModel.SetBaseModel(&models[currentModel]);
				swapModel.SetPosition({ 0, 0, 0 });

				currentModel++;
				if (currentModel > models.size() - 1)
					currentModel = 0;

				displayModel.SetBaseModel(&models[currentModel]);
				displayModel.SetPosition({ std::lerp(minSpawn, maxSpawn, cam.GetPosition().z / maxZoom), 0, 0 });
				transitionTimer = transitionDelay;
				transitionDir = -std::lerp(minSpeed, maxSpeed, cam.GetPosition().z / maxZoom);
			}
		}
		else
		{
			transitionTimer -= deltaTime;

			//Move the new model in from the side
			if (abs(displayModel.GetPosition().x) > abs(transitionDir) * deltaTime)
				displayModel.Translate({ transitionDir * deltaTime, 0, 0 });
			else
				displayModel.SetPosition({ 0, 0, 0 });

			//Move the old model to the side
			swapModel.Translate({ transitionDir * deltaTime, 0, 0 });
		}


		//Start rendering
		window.Fill({ 0, 0, 0 });
		window.ClearDepthBuffer();

		//Manually create the transform matrix using a quaternion rotation
		cvid::Matrix4 transform = cvid::Matrix4::Identity();
		transform = transform.Scale(displayModel.GetScale());
		transform = transform * rotation;
		transform = transform.Translate(displayModel.GetPosition());
		displayModel.SetTransform(transform);

		cvid::DrawModel(&displayModel, &cam, &window);

		//Draw the swap model if transitioning
		if (transitionTimer > 0)
		{
			cvid::Matrix4 transform = cvid::Matrix4::Identity();
			transform = transform.Scale(swapModel.GetScale());
			transform = transform * rotation;
			transform = transform.Translate(swapModel.GetPosition());
			swapModel.SetTransform(transform);
			cvid::DrawModel(&swapModel, &cam, &window);
		}

		//Display the model name text
		std::string name = std::format("<- {} ->", displayModel.GetBaseModel()->name);
		cvid::Vector2Int namePos = { windowSize.x / 2 - (int)name.size() / 2, windowSize.y / 2 - 2 };
		window.PutString(namePos, name);

		//Update the info texts 5 times a second
		if (timeSinceLastAvg > 0.2)
		{
			diagDt = avgDt / framesSinceLastAvg;
			renderTime = avgRender / framesSinceLastAvg;
			windowLatency = avgLatency / framesSinceLastAvg;
			avgRender = 0;
			avgLatency = 0;
			avgDt = 0;
			framesSinceLastAvg = 0;
			timeSinceLastAvg = 0;
		}
		timeSinceLastAvg += deltaTime;
		std::string fps = std::format("{} fps", std::floor(1 / diagDt));
		std::string render = std::format("Render: {} ms", std::floor(renderTime * 1000));
		std::string latency = std::format("Window: {} ms", std::floor(windowLatency * 1000));
		cvid::Vector2Int infoPos = { windowSize.x - (int)fps.size() - 2, 1 };
		window.PutString(infoPos + cvid::Vector2Int(-8, 0), render);
		window.PutString(infoPos + cvid::Vector2Int(-8, 1), latency);
		window.PutString(infoPos + cvid::Vector2Int(0, 2), fps);

		double renderDone = cvid::EndTimePoint();
		avgRender += renderDone;

		if (!window.DrawFrame())
			return 0;
		//For some reason this stops the window from freezing
		window.SendData("\x1b[0;0H", 7, cvid::DataType::String);

		double response = cvid::EndTimePoint();
		avgLatency += response - renderDone;

		deltaTime = cvid::EndTimePoint();
		avgDt += deltaTime;
		framesSinceLastAvg++;
	}

	return 0;
}