#pragma once
#include <string>
#include <cmath>

constexpr double PI = 3.14159265;
constexpr double comparisonPrecision = 0.0001f;

///Radians to degrees
double Degrees(double radians);

///Degrees to radians
double Radians(double degrees);

class Vector3;
class Vector3Int;

//Two 64-bit ints in one
class Vector2Int
{
public:
	//Constructors
	Vector2Int();
	Vector2Int(uint64_t all);
	Vector2Int(uint64_t x, uint64_t y);

	//Indexing
	uint64_t& operator[](int i);

	//Comparison
	bool operator==(const Vector2Int& rhs) const;
	bool operator!=(const Vector2Int& rhs) const;

	//Add
	Vector2Int operator+(const uint64_t& add) const;
	Vector2Int operator+(const Vector2Int& add) const;
	Vector2Int& operator+=(const Vector2Int& add);
	//Subtract
	Vector2Int operator-(const uint64_t& sub) const;
	Vector2Int operator-(const Vector2Int& sub) const;
	Vector2Int& operator-=(const Vector2Int& sub);
	//Multiply
	Vector2Int operator*(const uint64_t& mult) const;
	Vector2Int operator*(const Vector2Int& mult) const;
	Vector2Int& operator*=(const uint64_t& mult);
	Vector2Int& operator*=(const Vector2Int& mult);
	//Divide
	Vector2Int operator/(const uint64_t& div) const;
	Vector2Int operator/(const Vector2Int& div) const;
	Vector2Int& operator/=(const uint64_t& div);

	//Return a string of this vector in format "(x, y)"
	std::string ToString() const;

	uint64_t x, y;
};

//Two doubles in one
class Vector2
{
public:
	//Constructors
	Vector2();
	Vector2(double all);
	Vector2(double x, double y);
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

	//Return a string of this vector in format "(x, y, z)"
	std::string ToString() const;

	double x, y, z;
};

//Four doubles in one
class Vector4
{
public:
	//Constructors
	Vector4();
	Vector4(double all);
	Vector4(double _x, double _y, double _z, double _w);

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
