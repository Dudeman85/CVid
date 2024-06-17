#pragma once
#include <string>
#include <cvid/Vector.h>

namespace cvid
{
	//3x3 matrix of floats
	class Matrix3
	{
	public:
		//Constructors
		Matrix3();
		Matrix3(float all);
		Matrix3(float mat[9]);

		//Indexing
		Vector3& operator[](int i);
		const Vector3& operator[](int i) const;

		bool operator==(const Matrix3& rhs);
		bool operator!=(const Matrix3& rhs);

		//Multiply
		Vector3 operator*(Vector3& rhs);
		Matrix3 operator*(Matrix3& rhs);

		//Functions
		float Determinant();
		Matrix3 Transpose();
		Matrix3 Inverse();

		//Generate Transform Matrices
		static Matrix3 Scaling(const Vector2& scale);
		static Matrix3 Rotation(float radians);
		static Matrix3 Translation(const Vector2& translate);

		std::string ToString();

		Vector3 c1, c2, c3;
	};

	//4x4 matrix of floats
	class Matrix4
	{
	public:
		//Constructors
		Matrix4();
		Matrix4(float all);
		Matrix4(float mat[16]);

		//Indexing
		Vector4& operator[](int i);
		const Vector4& operator[](int i) const;

		//Comparison
		bool operator==(const Matrix4& rhs);
		bool operator!=(const Matrix4& rhs);

		//Multiply
		Vector4 operator*(Vector4& rhs);
		Matrix4 operator*(Matrix4& rhs);

		//Functions
		float Determinant();
		Matrix4 Transpose();
		Matrix4 Inverse();

		//Generate Transform Matrices
		static Matrix4 Scaling(const Vector3& scale);
		static Matrix4 Translation(const Vector3& translate);
		static Matrix4 RotationX(float radians);
		static Matrix4 RotationY(float radians);
		static Matrix4 RotationZ(float radians);

		std::string ToString();

		Vector4 c1, c2, c3, c4;
	};
}