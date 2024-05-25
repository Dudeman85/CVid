//This file contains helper functions mostly used for debugging
#pragma once
#include <string>
#include <iostream>

namespace cvid 
{
	//Define colors for errors and warnings
	constexpr const char* errorFormat = "\033[31m";
	constexpr const char* warningFormat = "\033[33m";
	constexpr const char* normalFormat = "\033[37m";

	inline void LogWarning(std::string message)
	{
		std::cout << warningFormat << message << normalFormat << std::endl;
	}
	inline void LogError(std::string message)
	{
		std::cout << errorFormat << message << normalFormat << std::endl;
	}
}