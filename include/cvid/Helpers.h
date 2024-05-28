//This file contains helper functions mostly used for debugging
#pragma once
#include <string>
#include <iostream>

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
	//Get a random color
	static inline cvid::Color RandomColor()
	{
		int color = rand() % 16;
		if (color > 7)
			color += 82;
		else
			color += 30;
		return (cvid::Color)color;
	}
}