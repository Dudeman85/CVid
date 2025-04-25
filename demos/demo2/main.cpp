#include <format>
#include <chrono>
#include <filesystem>
#include <cvid/Window.h>
#include <cvid/Rasterizer.h>
#include <cvid/Helpers.h>
#include <cvid/Renderer.h>
#include <cvid/Model.h>
#include <cvid/Matrix.h>
#include <cvid/Math.h>
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
	double programTime = 0;
	double timeSinceLastAvg = 0;
	double framesSinceLastAvg = 0;
	double diagDt = 0;
	double avgDt = 0;
	double windowLatency = 0;
	double avgLatency = 0;
	double renderTime = 0;
	double avgRender = 0;

	cvid::Color bgColor = { 12, 12, 12 };

	//Make window
	cvid::Vector2Int maxWindowSize = cvid::MaxWindowSize();
	cvid::Vector2Int windowSize = { std::min((int64_t)170, maxWindowSize.x), std::min((int64_t)100, maxWindowSize.y) };
	cvid::Window window(windowSize.x, windowSize.y, "CVid Demo", false);
	window.enableDepthTest = true;

	//Make camera
	cvid::Camera cam(cvid::Vector3(0, -25, 150), windowSize.x, windowSize.y);
	float fov = 90;
	cam.MakePerspective(fov, 1, 5000);
	cam.Rotate(cvid::Vector3(0, cvid::Radians(0), 0));
	cvid::ambientLightIntensity = 0.5;
	cvid::directionalLight = { 0, 0.5, 0.5 };
	cvid::directionalLightIntensity = 1;

	//Load all models in resources folder
	std::string resources = "../../../resources/";
	std::vector<cvid::Model> models;
	int currentModel = 0;
	cvid::Model* logo;
	//Make sure resources exists
	if (std::filesystem::exists(resources))
	{
		for (auto& p : std::filesystem::recursive_directory_iterator(resources))
		{
			cvid::LogWarning("e");
			if (p.path().extension().string() == ".obj")
			{
				if (p.path().filename().string() == "CVid.obj")
					logo = new cvid::Model(p.path().string());
				else
					models.push_back(cvid::Model(p.path().string()));
			}
		}
	}

	//Make sure resources were loaded
	if (models.empty())
	{
		window.Resize(windowSize.x, windowSize.y);
		cvid::LogError("Error: No models found in " + resources);
		system("pause");
		return -1;
	}

	//Set up the logo screen
	cvid::ModelInstance* logoInstance;
	cvid::Material logoMat;
	bool inTitleScreen = false;
	double spinVelocity = 0;
	double hue = 0;
	if (logo)
	{
		logoMat.diffuseColor = { 0, 255, 0 };
		logo->material = logoMat;
		logoInstance = new cvid::ModelInstance(logo);
		logoInstance->SetScale(130);
		logoInstance->SetRotation({ 0, cvid::Radians(35), 0 });
		inTitleScreen = true;
	}

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
		for (size_t j = 0; j < mat[0].size(); j++)
			rotation[i][j] = mat[i][j];

	//Make the second model in swapping
	cvid::ModelInstance swapModel(&models[1]);
	swapModel.SetScale(20);
	swapModel.SetPosition({ 0, 0, 0 });

	//Reset the properties of the Window
	window.Resize(windowSize.x, windowSize.y);
	//Hide the cursor
	std::cout << "\x1b[?25l";

	while (true)
	{
		cvid::StartTimePoint();

		//Start rendering
		window.Fill(bgColor);
		window.ClearDepthBuffer();

		if (GetKeyState(VK_ESCAPE) & 0x8000)
			break;

		//Get the console input record for scroll wheel
		//Does not work in seperate window
		std::vector<INPUT_RECORD> ir = window.GetInputRecord();

		if (inTitleScreen)
		{
			//Start demo mode with enter
			if (GetKeyState(VK_RETURN) & 0x8000)
			{
				inTitleScreen = false;
				delete logoInstance;
				delete logo;
			}
			//Update cursor delta
			if (GetKeyState(VK_LBUTTON) & 0x8000 && clickedInWindow)
			{
				lastCursorPos = currentCursorPos;
				GetCursorPos(&currentCursorPos);

				double dx = currentCursorPos.x - lastCursorPos.x;

				if (dx != 0)
				{
					//Increase spin velocity by mouse delta
					dx *= -rotationSpeed / 2;
					logoInstance->Rotate({ 0, -dx, 0 });
					spinVelocity += dx * 3;
				}
				else
				{
					//Degrade the spin velocity significantly when not moving mouse
					spinVelocity /= 300 * deltaTime;
				}
				//Degrade and clamp the spin velocity
				spinVelocity -= deltaTime * spinVelocity;
				spinVelocity = std::clamp(spinVelocity, -40.0, 40.0);
			}
			else
			{
				//Degrade the spin velocity
				spinVelocity -= deltaTime * spinVelocity;
				if (abs(spinVelocity) < 0.1)
					spinVelocity = 0;
				//Rotate the logo by spin
				logoInstance->Rotate({ 0, cvid::Radians(-spinVelocity), 0 });

				GetCursorPos(&currentCursorPos);
				//Make sure the user clicked insisde the window
				RECT bounds;
				GetClientRect(hWnd, &bounds);
				ClientToScreen(hWnd, (LPPOINT)&bounds);
				ClientToScreen(hWnd, (LPPOINT)&bounds + 1);
				clickedInWindow = currentCursorPos.x > bounds.left && currentCursorPos.x < bounds.right
					&& currentCursorPos.y > bounds.top && currentCursorPos.y < bounds.bottom;
			}

			//Floating animation
			logoInstance->SetPosition({ -40, sin(programTime * 3) * 4 - 10, 0 });

			//Rainbow effect
			hue += deltaTime * 40;
			if (hue > 255)
				hue = 0;
			logoMat.diffuseColor = cvid::HsvToRgb({ (uint8_t)hue, 200, 200 });
			logoInstance->SetMaterial(&logoMat);

			cvid::DrawModel(logoInstance, &cam, &window);

			//Write the title texts
			cvid::Vector2Int textPos(floor(windowSize.x / 1.45), floor(windowSize.y / 5));
			window.PutString(textPos, "Welcome to the CVid demo!", { 0 }, cvid::HsvToRgb({ (uint8_t)(255 - hue), 200, 200 }));
			window.PutString(textPos.x + 2, textPos.y + 2, "Press ENTER to start");
			//Instructions 
			window.PutString(textPos.x, floor(windowSize.y / 2.6), "Click and drag to rotate");
			window.PutString(textPos.x + 5, floor(windowSize.y / 2.6) + 1, "Scroll to zoom");
			window.PutString(textPos.x - 1, floor(windowSize.y / 2.6) + 2, "Arrow keys to cycle models");
			window.PutString(textPos.x - 11, floor(windowSize.y / 2.6) + 4, "Place .obj models to be rendered in resources/");
			window.PutString(textPos.x - 8, floor(windowSize.y / 2.6) + 5, "(Works best with single material objects)");
		}
		else
		{
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
						for (size_t j = 0; j < mat[0].size(); j++)
							rotation[i][j] = mat[i][j];
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
				if (GetKeyState(VK_LEFT) & 0x8000 || GetKeyState('A') & 0x8000)
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
				else if (GetKeyState(VK_RIGHT) & 0x8000 || GetKeyState('D') & 0x8000)
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
			window.PutString(namePos, name, bgColor);

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
			std::string tris = std::format("Triangles: {} ", displayModel.GetBaseModel()->faces.size());
			std::string render = std::format("Render: {} ms", std::floor(renderTime * 1000));
			std::string latency = std::format("Window: {} ms", std::floor(windowLatency * 1000));
			std::string fps = std::format("{} fps", std::floor(1 / diagDt));
			cvid::Vector2Int infoPos = { windowSize.x - (int)fps.size() - 2, 1 };
			window.PutString(infoPos + cvid::Vector2Int(-11, 0), tris, bgColor);
			window.PutString(infoPos + cvid::Vector2Int(-8, 1), render, bgColor);
			window.PutString(infoPos + cvid::Vector2Int(-8, 2), latency, bgColor);
			window.PutString(infoPos + cvid::Vector2Int(0, 3), fps, bgColor);
		}

		double renderDone = cvid::EndTimePoint();
		avgRender += renderDone;

		if (!window.DrawFrame())
			return 0;
		//For some reason this stops the window from freezing
		window.SendData("\x1b[0;0H", 7, cvid::DataType::String);

		double response = cvid::EndTimePoint();
		avgLatency += response - renderDone;

		deltaTime = cvid::EndTimePoint();
		programTime += deltaTime;
		avgDt += deltaTime;
		framesSinceLastAvg++;
	}

	return 0;
}