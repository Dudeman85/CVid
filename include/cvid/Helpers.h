//This file contains helper functions mostly used for debugging
#pragma once
#include <string>
#include <iostream>
#include <chrono>

namespace cvid 
{
	using byte = unsigned char;

	//Define colors for errors and warnings
	constexpr const char* errorFormat = "\033[31m";
	constexpr const char* warningFormat = "\033[33m";
	constexpr const char* normalFormat = "\033[37m";

	static inline void LogWarning(std::string message)
	{
		std::cout << warningFormat << message << normalFormat << std::endl;
	}
	static inline void LogError(std::string message)
	{
		std::cout << errorFormat << message << normalFormat << std::endl;
	}

	//Profiler tools
	static std::chrono::time_point<std::chrono::high_resolution_clock> timePointStart;
	static inline void StartTimePoint()
	{
		timePointStart = std::chrono::high_resolution_clock::now();
	}
	static inline double EndTimePoint()
	{
		std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - timePointStart;
		return duration.count();
	}
	
	//Get a random color
	enum class ConsoleColor : uint8_t;
	static inline cvid::ConsoleColor RandomColor()
	{
		int color = rand() % 16;
		if (color > 7)
			color += 82;
		else
			color += 30;
		return (cvid::ConsoleColor)color;
	}
}