#include <chrono>
#include <vld.h>
#include <SDL.h>
#include <string>
#include <iostream>

#include <windows.h>
#include <psapi.h> 

#include "DollarGestureManager.h"
#include "PPGestureManager.h"
#include "SDLCanvas.h"
#include "Vector2.h"
#include "XStrokeRecognizer.h"

std::string GetUserInput()
{
	std::string result;
	std::cout << "Enter the name of the gesture: ";
	std::cin >> result;
	return result;
}

auto get_cpu_time() {
	FILETIME createTime, exitTime, kernelTime, userTime;
	GetProcessTimes(GetCurrentProcess(), &createTime, &exitTime, &kernelTime, &userTime);

	ULARGE_INTEGER li = {};
	li.LowPart = userTime.dwLowDateTime;
	li.HighPart = userTime.dwHighDateTime;

	return li.QuadPart;
}

auto get_memory_usage() {
	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	return pmc.WorkingSetSize;
}

int SDL_main(int argc, char* argv[])
{
	constexpr int scale = 1;

	const Vector2 canvasSize{ 192 * scale, 256 * scale };

	SDLCanvas canvas{canvasSize};
	canvas.SetBrushWidth(5 * scale);
	canvas.SetBrushColor(0x00000000);

	canvas.Render();

	DollarGestureManager dgm{};
	PPGestureManager ppgm{};
	XStrokeRecognizer xs{ canvasSize };

	SDL_Event event;
	bool running = true;

	bool buttonPressed = false;

	// TODO : auto drawing and actual experiment

	//TODO : make sure this is window based
	//TODO : Coordinates need to be window based, not screen based

	int i{0};

	std::vector<long> XSDurations{}, D1Durations{}, PPDurations{};

	while (running)
	{
		while (SDL_PollEvent(&event) != 0)
		{
			if (event.type == SDL_QUIT)
			{
				running = false;
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				Vector2 hitPoint;
				const SDL_MouseButtonEvent mEvent = *reinterpret_cast<SDL_MouseButtonEvent*>(&event);
				hitPoint.x = mEvent.x;
				hitPoint.y = mEvent.y;
				canvas.StartDrawing(hitPoint);

				buttonPressed = true;
				SDL_CaptureMouse(SDL_TRUE); // Allows tracking of mouse outside of window
			}
			else if (buttonPressed && event.type == SDL_MOUSEBUTTONUP)
			{
				buttonPressed = false;
				SDL_CaptureMouse(SDL_FALSE);

				auto startTime = std::chrono::high_resolution_clock::now();

				std::cout << "XStroke\n";
				std::cout << xs.DoRecognition(canvas.GetDrawpoints()) << "\n";
				auto XSTime = std::chrono::high_resolution_clock::now();

				std::cout << "$1\n";
				dgm.Process(canvas.GetDrawpoints());
				auto D1Time = std::chrono::high_resolution_clock::now();

				std::cout << "Penny Pincher\n";
				ppgm.Process(canvas.GetDrawpoints());
				auto stopTime = std::chrono::high_resolution_clock::now();

				auto XSDuration = std::chrono::duration_cast<std::chrono::microseconds>(XSTime - startTime).count();
				auto D1Duration = std::chrono::duration_cast<std::chrono::microseconds>(D1Time - XSTime).count();
				auto PPDuration = std::chrono::duration_cast<std::chrono::microseconds>(stopTime - D1Time).count();

				XSDurations.push_back(XSDuration);
				D1Durations.push_back(D1Duration);
				PPDurations.push_back(PPDuration);

				std::cout << "XStroke Time: " << XSDuration << " microseconds\n";
				std::cout << "$1 Time: " << D1Duration << " microseconds\n";
				std::cout << "Penny Pincher Time: " << PPDuration << " microseconds\n";
				i += 1;
				std::cout << "GestureAmount: ";
				std::cout << i;
				std::cout << "\n";
				for (auto duration : XSDurations)
				{
					std::cout << duration;
					std::cout << ", ";
				}
				std::cout << "\n";
				for (auto duration : D1Durations)
				{
					std::cout << duration;
					std::cout << ", ";
				}
				std::cout << "\n";
				for (auto duration : PPDurations)
				{
					std::cout << duration;
					std::cout << ", ";
				}
				std::cout << "\n";
				canvas.EndDrawing();
			}
			else if (buttonPressed && event.type == SDL_MOUSEMOTION)
			{
				Vector2 hitPoint;
				const SDL_MouseButtonEvent mEvent = *reinterpret_cast<SDL_MouseButtonEvent*>(&event);
				hitPoint.x = mEvent.x;
				hitPoint.y = mEvent.y;
				canvas.UpdateDrawing(hitPoint);
			}
			else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE)
			{
				dgm.ToggleSaveAsTemplate();
				xs.ToggleSaveAsTemplate();
				ppgm.ToggleSaveAsTemplate();
			}
			else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN)
			{
				std::string input = GetUserInput();
				dgm.SetName(input);
				xs.SetName(input);
				ppgm.SetName(input);
			}
			else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_s)
			{
				xs.SaveGestureTemplates();
				dgm.SaveTemplates();
				ppgm.SaveTemplates();
			}

			canvas.Render();
		}
	}

	xs.SaveGestureTemplates();
	dgm.SaveTemplates();
	ppgm.SaveTemplates();


	SDL_Quit();
	return 0;
}
