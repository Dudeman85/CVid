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

	//Convert a red, green, blue color color to a hue, saturation, value
	static Color RgbToHsv(Color rgb)
	{
		Color hsv;
		unsigned char rgbMin, rgbMax;

		rgbMin = rgb.r < rgb.g ? (rgb.r < rgb.b ? rgb.r : rgb.b) : (rgb.g < rgb.b ? rgb.g : rgb.b);
		rgbMax = rgb.r > rgb.g ? (rgb.r > rgb.b ? rgb.r : rgb.b) : (rgb.g > rgb.b ? rgb.g : rgb.b);

		hsv.b = rgbMax;
		if (hsv.b == 0)
		{
			hsv.r = 0;
			hsv.g = 0;
			return hsv;
		}

		hsv.g = 255 * long(rgbMax - rgbMin) / hsv.b;
		if (hsv.g == 0)
		{
			hsv.r = 0;
			return hsv;
		}

		if (rgbMax == rgb.r)
			hsv.r = 0 + 43 * (rgb.g - rgb.b) / (rgbMax - rgbMin);
		else if (rgbMax == rgb.g)
			hsv.r = 85 + 43 * (rgb.b - rgb.r) / (rgbMax - rgbMin);
		else
			hsv.r = 171 + 43 * (rgb.r - rgb.g) / (rgbMax - rgbMin);

		return hsv;
	}

	//Convert a hue, saturation, value color to a red, green, blue color
	static Color HsvToRgb(Color hsv)
	{
		Color rgb;
		unsigned char region, remainder, p, q, t;

		if (hsv.g == 0)
		{
			rgb.r = hsv.b;
			rgb.g = hsv.b;
			rgb.b = hsv.b;
			return rgb;
		}

		region = hsv.r / 43;
		remainder = (hsv.r - (region * 43)) * 6;

		p = (hsv.b * (255 - hsv.g)) >> 8;
		q = (hsv.b * (255 - ((hsv.g * remainder) >> 8))) >> 8;
		t = (hsv.b * (255 - ((hsv.g * (255 - remainder)) >> 8))) >> 8;

		switch (region)
		{
		case 0:
			rgb.r = hsv.b; rgb.g = t; rgb.b = p;
			break;
		case 1:
			rgb.r = q; rgb.g = hsv.b; rgb.b = p;
			break;
		case 2:
			rgb.r = p; rgb.g = hsv.b; rgb.b = t;
			break;
		case 3:
			rgb.r = p; rgb.g = q; rgb.b = hsv.b;
			break;
		case 4:
			rgb.r = t; rgb.g = p; rgb.b = hsv.b;
			break;
		default:
			rgb.r = hsv.b; rgb.g = p; rgb.b = q;
			break;
		}

		return rgb;
	}
}