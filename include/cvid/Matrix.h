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
		Matrix3(double all);
		Matrix3(double mat[9]);
		static Matrix3 Identity();

		//Indexing, [col][row]
		Vector3& operator[](int i);
		const Vector3& operator[](int i) const;

		bool operator==(const Matrix3& rhs);
		bool operator!=(const Matrix3& rhs);

		//Multiply
		Vector3 operator*(Vector3& rhs);
		Matrix3 operator*(Matrix3& rhs);

		//Functions
		double Determinant();
		Matrix3 Transpose();
		Matrix3 Inverse();

		//Generate Transform Matrices
		Matrix3 Scale(const Vector2& scale);
		Matrix3 Rotate(double radians);
		Matrix3 Translate(const Vector2& translate);

		std::string ToString();

		Vector3 c1, c2, c3;
	};

	//4x4 matrix of floats
	class Matrix4
	{
	public:
		//Constructors
		Matrix4();
		Matrix4(double all);
		Matrix4(double mat[16]);
		static Matrix4 Identity();

		//Indexing, [col][row]
		Vector4& operator[](int i);
		const Vector4& operator[](int i) const;

		//Comparison
		bool operator==(const Matrix4& rhs);
		bool operator!=(const Matrix4& rhs);

		//Multiply
		Vector4 operator*(const Vector4& rhs) const;
		Matrix4 operator*(const Matrix4& rhs) const;

		//Functions
		double Determinant();
		Matrix4 Transpose();
		Matrix4 Inverse();

		//Generate Transform Matrices
		Matrix4 Scale(const Vector3& scale);
		Matrix4 Translate(const Vector3& translation);
		Matrix4 Rotate(const Vector3& rotation);
		Matrix4 RotateX(double radians);
		Matrix4 RotateY(double radians);
		Matrix4 RotateZ(double radians);

		std::string ToString();

		Vector4 c1, c2, c3, c4;
	};
}