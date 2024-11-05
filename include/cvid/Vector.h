#pragma once
#include <string>
#include <cmath>

namespace cvid
{
	constexpr double PI = 3.14159265;
	constexpr double comparisonPrecision = 0.0001f;

	//Radians to degrees
	double Degrees(double radians);

	//Degrees to radians
	double Radians(double degrees);

	class Vector2;
	class Vector2Int;
	class Vector3;
	class Vector3Int;
	class Vector4;

	//Two 64-bit ints in one
	class Vector2Int
	{
	public:
		//Constructors
		Vector2Int();
		Vector2Int(int64_t all);
		Vector2Int(int64_t x, int64_t y);
		Vector2Int(Vector2 v);
		Vector2Int(Vector3Int v);
		Vector2Int(Vector3 v);

		//Indexing
		int64_t& operator[](int i);

		//Comparison
		bool operator==(const Vector2Int& rhs) const;
		bool operator!=(const Vector2Int& rhs) const;

		//Add
		Vector2Int operator+(const int64_t& add) const;
		Vector2Int operator+(const Vector2Int& add) const;
		Vector2Int& operator+=(const Vector2Int& add);
		//Subtract
		Vector2Int operator-(const int64_t& sub) const;
		Vector2Int operator-(const Vector2Int& sub) const;
		Vector2Int& operator-=(const Vector2Int& sub);
		//Multiply
		Vector2Int operator*(const int64_t& mult) const;
		Vector2Int operator*(const Vector2Int& mult) const;
		Vector2Int& operator*=(const int64_t& mult);
		Vector2Int& operator*=(const Vector2Int& mult);
		//Divide
		Vector2Int operator/(const int64_t& div) const;
		Vector2Int operator/(const Vector2Int& div) const;
		Vector2Int& operator/=(const int64_t& div);

		//Return a string of this vector in format "(x, y)"
		std::string ToString() const;

		int64_t x, y;
	};

	//Two doubles in one
	class Vector2
	{
	public:
		//Constructors
		Vector2();
		Vector2(double all);
		Vector2(double x, double y);
		Vector2(Vector2Int v);
		Vector2(Vector3 vec3);

		//Indexing
		double& operator[](int i);

		//Comparison
		bool operator==(const Vector2& rhs) const;
		bool operator!=(const Vector2& rhs) const;

		//Add
		Vector2 operator+(const double& add) const;
		Vector2 operator+(const Vector2& add) const;
		Vector2& operator+=(const Vector2& add);
		//Subtract
		Vector2 operator-(const double& sub) const;
		Vector2 operator-(const Vector2& sub) const;
		Vector2& operator-=(const Vector2& sub);
		//Multiply
		Vector2 operator*(const double& mult) const;
		Vector2 operator*(const Vector2& mult) const;
		Vector2& operator*=(const double& mult);
		Vector2& operator*=(const Vector2& mult);
		//Divide
		Vector2 operator/(const double& div) const;
		Vector2 operator/(const Vector2& div) const;
		Vector2& operator/=(const double& div);

		//Get the length of this vector
		double Length() const;
		//Returns a normalized version of this vector with a lenght of 1
		Vector2 Normalize() const;
		//Returns the left handed normal vector to this vector
		Vector2 LeftNormal() const;
		//Returns the right handed normal vector to this vector
		Vector2 RightNormal() const;
		//Dot product of this vector and vector b
		double Dot(const Vector2& b) const;

		//Return a string of this vector in format "(x, y)"
		std::string ToString() const;

		double x, y;
	};

	//Three doubles in one
	class Vector3
	{
	public:
		//Constructors
		Vector3();
		Vector3(double all);
		Vector3(double x, double y, double z);
		Vector3(Vector2 vec2, double z = 0);
		Vector3(Vector2Int vec2, double z = 0);
		Vector3(Vector3Int v);
		Vector3(Vector4 vec4);

		//Indexing
		double& operator[](int i);
		const double& operator[](int i) const;

		//Comparison
		bool operator==(const Vector3& rhs) const;
		bool operator!=(const Vector3& rhs) const;

		//Add
		Vector3 operator+(const double& add) const;
		Vector3 operator+(const Vector3& add) const;
		Vector3& operator+=(const Vector3& add);
		//Subtract
		Vector3 operator-(const double& sub) const;
		Vector3 operator-(const Vector3& sub) const;
		Vector3& operator-=(const Vector3& sub);
		//Multiply
		Vector3 operator*(const double& mult) const;
		Vector3 operator*(const Vector3& mult) const;
		Vector3& operator*=(const double& mult);
		//Divide
		Vector3 operator/(const double& div) const;
		Vector3 operator/(const Vector3& div) const;
		Vector3& operator/=(const double& div);

		//Get the length of this vector
		double Length() const;
		//Returns a normalized version of this vector with a lenght of 1
		Vector3 Normalize() const;
		//Dot product of this vector and vector b
		double Dot(Vector3 b) const;
		//Cross product of this vector and vector b
		Vector3 Cross(Vector3 b) const;
		//Distance from this point to another
		double Distance(Vector3 b) const;

		//Return a string of this vector in format "(x, y, z)"
		std::string ToString() const;

		double x, y, z;
	};

	//Three ints in one
	class Vector3Int
	{
	public:
		//Constructors
		Vector3Int();
		Vector3Int(int64_t all);
		Vector3Int(int64_t x, int64_t y, int64_t z);
		Vector3Int(Vector2Int vec2, int64_t z = 0);

		//Indexing
		int64_t& operator[](int i);
		const int64_t& operator[](int i) const;

		//Comparison
		bool operator==(const Vector3Int& rhs) const;
		bool operator!=(const Vector3Int& rhs) const;

		//Add
		Vector3Int operator+(const int64_t& add) const;
		Vector3Int operator+(const Vector3Int& add) const;
		Vector3Int& operator+=(const Vector3Int& add);
		//Subtract
		Vector3Int operator-(const int64_t& sub) const;
		Vector3Int operator-(const Vector3Int& sub) const;
		Vector3Int& operator-=(const Vector3Int& sub);
		//Multiply
		Vector3Int operator*(const int64_t& mult) const;
		Vector3Int operator*(const Vector3Int& mult) const;
		Vector3Int& operator*=(const int64_t& mult);
		//Divide
		Vector3Int operator/(const int64_t& div) const;
		Vector3Int operator/(const Vector3Int& div) const;
		Vector3Int& operator/=(const int64_t& div);

		//Return a string of this vector in format "(x, y, z)"
		std::string ToString() const;

		int64_t x, y, z;
	};

	//Four doubles in one
	class Vector4
	{
	public:
		//Constructors
		Vector4();
		Vector4(double all);
		Vector4(double x, double y, double z, double w);
		Vector4(Vector3, double w);

		//Indexing
		double& operator[](int i);
		const double& operator[](int i) const;

		//Comparison
		bool operator==(const Vector4& rhs) const;
		bool operator!=(const Vector4& rhs) const;

		//Add
		Vector4 operator+(const double& add) const;
		Vector4 operator+(const Vector4& add) const;
		Vector4& operator+=(const Vector4& add);
		//Subtract
		Vector4 operator-(const double& sub) const;
		Vector4 operator-(const Vector4& sub) const;
		Vector4& operator-=(const Vector4& sub);
		//Multiply
		Vector4 operator*(const double& mult) const;
		Vector4 operator*(const Vector4& mult) const;
		Vector4& operator*=(const double& mult);
		//Divide
		Vector4 operator/(const double& div) const;
		Vector4 operator/(const Vector4& div) const;
		Vector4& operator/=(const double& div);

		//Return a string of this vector in format "(x, y, z, w)"
		std::string ToString();

		double x, y, z, w;
	};
}