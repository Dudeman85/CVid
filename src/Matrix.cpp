#include <stdexcept>
#include <format>
#include <cvid/Matrix.h>

namespace cvid
{
	//////////// Matrix 3 ///////////////

	//Constructors
	Matrix3::Matrix3()
	{
		c1 = Vector3(0);
		c2 = Vector3(0);
		c3 = Vector3(0);
	}
	Matrix3::Matrix3(double all)
	{
		c1 = Vector3(all);
		c2 = Vector3(all);
		c3 = Vector3(all);
	}
	Matrix3::Matrix3(double mat[9])
	{
		c1 = Vector3(mat[0], mat[3], mat[6]);
		c2 = Vector3(mat[1], mat[4], mat[7]);
		c3 = Vector3(mat[2], mat[5], mat[8]);
	}
	Matrix3 Matrix3::Identity()
	{
		Matrix3 mat;
		mat[0][0] = 1;
		mat[1][1] = 1;
		mat[2][2] = 1;
		return mat;
	}

	//Indexing
	Vector3& Matrix3::operator[](int i)
	{
		switch (i)
		{
		case 0:
			return c1;
		case 1:
			return c2;
		case 2:
			return c3;
		default:
			throw std::out_of_range("Index in Matrix3 out of range");
		}
	}
	const Vector3& Matrix3::operator[](int i) const
	{
		switch (i)
		{
		case 0:
			return c1;
		case 1:
			return c2;
		case 2:
			return c3;
		default:
			throw std::out_of_range("Index in Matrix3 out of range");
		}
	}

	bool Matrix3::operator==(const Matrix3& rhs)
	{
		for (int x = 0; x < 3; x++)
		{
			for (int y = 0; y < 3; y++)
			{
				//Get rid of float impresicion
				if (abs(this[0][x][y] - rhs[x][y]) > 0.000001)
					return false;
			}
		}
		return true;
	}
	bool Matrix3::operator!=(const Matrix3& rhs)
	{
		for (int x = 0; x < 3; x++)
		{
			for (int y = 0; y < 3; y++)
			{
				//Get rid of float impresicion
				if (abs(this[0][x][y] - rhs[x][y]) > 0.000001)
					return true;
			}
		}
		return false;
	}

	Vector3 Matrix3::operator*(Vector3& rhs)
	{
		Matrix3& lhs = *this;
		Vector3 mult;
		mult[0] = (lhs[0][0] * rhs[0] + lhs[1][0] * rhs[1] + lhs[2][0] * rhs[2]);
		mult[1] = (lhs[0][1] * rhs[0] + lhs[1][1] * rhs[1] + lhs[2][1] * rhs[2]);
		mult[2] = (lhs[0][2] * rhs[0] + lhs[1][2] * rhs[1] + lhs[2][2] * rhs[2]);

		return mult;
	}
	Matrix3 Matrix3::operator*(Matrix3& rhs)
	{
		Matrix3& lhs = *this;
		Matrix3 mult;
		mult[0][0] = (lhs[0][0] * rhs[0][0] + lhs[1][0] * rhs[0][1] + lhs[2][0] * rhs[0][2]);
		mult[1][0] = (lhs[0][0] * rhs[1][0] + lhs[1][0] * rhs[1][1] + lhs[2][0] * rhs[1][2]);
		mult[2][0] = (lhs[0][0] * rhs[2][0] + lhs[1][0] * rhs[2][1] + lhs[2][0] * rhs[2][2]);
		mult[0][1] = (lhs[0][1] * rhs[0][0] + lhs[1][1] * rhs[0][1] + lhs[2][1] * rhs[0][2]);
		mult[1][1] = (lhs[0][1] * rhs[1][0] + lhs[1][1] * rhs[1][1] + lhs[2][1] * rhs[1][2]);
		mult[2][1] = (lhs[0][1] * rhs[2][0] + lhs[1][1] * rhs[2][1] + lhs[2][1] * rhs[2][2]);
		mult[0][2] = (lhs[0][2] * rhs[0][0] + lhs[1][2] * rhs[0][1] + lhs[2][2] * rhs[0][2]);
		mult[1][2] = (lhs[0][2] * rhs[1][0] + lhs[1][2] * rhs[1][1] + lhs[2][2] * rhs[1][2]);
		mult[2][2] = (lhs[0][2] * rhs[2][0] + lhs[1][2] * rhs[2][1] + lhs[2][2] * rhs[2][2]);

		return mult;
	}

	//Operations
	double Matrix3::Determinant()
	{
		Matrix3& self = *this;

		return self[0][0] * (self[1][1] * self[2][2] - self[2][1] * self[1][2])
			- self[1][0] * (self[0][1] * self[2][2] - self[2][1] * self[0][2])
			+ self[2][0] * (self[0][1] * self[1][2] - self[1][1] * self[0][2]);
	}
	Matrix3 Matrix3::Transpose()
	{
		double t[]
		{
			c1[0], c1[1], c1[2],
			c2[0], c2[1], c2[2],
			c3[0], c3[1], c3[2],
		};
		return Matrix3(t);
	}
	Matrix3 Matrix3::Inverse()
	{
		double inverseDet = 1 / Determinant();

		Matrix3& self = *this;
		Matrix3 inverse;
		inverse[0][0] = (self[1][1] * self[2][2] - self[2][1] * self[1][2]) * inverseDet;
		inverse[0][1] = (self[0][2] * self[2][1] - self[0][1] * self[2][2]) * inverseDet;
		inverse[0][2] = (self[0][1] * self[1][2] - self[0][2] * self[1][1]) * inverseDet;
		inverse[1][0] = (self[1][2] * self[2][0] - self[1][0] * self[2][2]) * inverseDet;
		inverse[1][1] = (self[0][0] * self[2][2] - self[0][2] * self[2][0]) * inverseDet;
		inverse[1][2] = (self[1][0] * self[0][2] - self[0][0] * self[1][2]) * inverseDet;
		inverse[2][0] = (self[1][0] * self[2][1] - self[2][0] * self[1][1]) * inverseDet;
		inverse[2][1] = (self[2][0] * self[0][1] - self[0][0] * self[2][1]) * inverseDet;
		inverse[2][2] = (self[0][0] * self[1][1] - self[1][0] * self[0][1]) * inverseDet;

		return inverse;
	}

	Matrix3 Matrix3::Scale(const Vector2& scale)
	{
		Matrix3 scalingMatrix;
		scalingMatrix[0][0] = scale.x;
		scalingMatrix[1][1] = scale.y;
		scalingMatrix[2][2] = 1;
		return scalingMatrix;
	}
	Matrix3 Matrix3::Rotate(double radians)
	{
		Matrix3 rotationMatrix;
		rotationMatrix[0][0] = cos(radians);
		rotationMatrix[1][0] = -sin(radians);
		rotationMatrix[0][1] = sin(radians);
		rotationMatrix[1][1] = cos(radians);
		rotationMatrix[2][2] = 1;
		return rotationMatrix;
	}
	Matrix3 Matrix3::Translate(const Vector2& translate)
	{
		Matrix3 translationMatrix;
		translationMatrix[0][0] = 1;
		translationMatrix[1][1] = 1;
		translationMatrix[2][2] = 1;
		translationMatrix[2][0] = translate.x;
		translationMatrix[2][1] = translate.y;
		return translationMatrix;
	}

	std::string Matrix3::ToString()
	{
		Matrix3& self = *this;
		return std::format("[{}, {}, {},\n {}, {}, {},\n {}, {}, {}]", self[0][0], self[1][0], self[2][0], self[0][1], self[1][1], self[2][1], self[0][2], self[1][2], self[2][2]);
	}


	//////////// Matrix 4 ///////////////

	//Constructors
	Matrix4::Matrix4()
	{
		c1 = Vector4(0);
		c2 = Vector4(0);
		c3 = Vector4(0);
		c4 = Vector4(0);
	}
	Matrix4::Matrix4(double all)
	{
		c1 = Vector4(all);
		c2 = Vector4(all);
		c3 = Vector4(all);
		c4 = Vector4(all);
	}
	Matrix4::Matrix4(double mat[16])
	{
		c1 = Vector4(mat[0], mat[4], mat[8], mat[12]);
		c2 = Vector4(mat[1], mat[5], mat[9], mat[13]);
		c3 = Vector4(mat[2], mat[6], mat[10], mat[14]);
		c4 = Vector4(mat[3], mat[7], mat[11], mat[15]);
	}
	Matrix4 Matrix4::Identity()
	{
		Matrix4 mat;
		mat[0][0] = 1;
		mat[1][1] = 1;
		mat[2][2] = 1;
		mat[3][3] = 1;
		return mat;
	}

	//Indexing
	Vector4& Matrix4::operator[](int i)
	{
		switch (i)
		{
		case 0:
			return c1;
		case 1:
			return c2;
		case 2:
			return c3;
		case 3:
			return c4;
		default:
			throw std::out_of_range("Index in Matrix4 out of range");
		}
	}
	const Vector4& Matrix4::operator[](int i) const
	{
		switch (i)
		{
		case 0:
			return c1;
		case 1:
			return c2;
		case 2:
			return c3;
		case 3:
			return c4;
		default:
			throw std::out_of_range("Index in Matrix4 out of range");
		}
	}

	//Comparison
	bool Matrix4::operator==(const Matrix4& rhs)
	{
		for (int x = 0; x < 4; x++)
		{
			for (int y = 0; y < 4; y++)
			{
				//Ignore float imprecision
				if (abs(this[0][x][y] - rhs[x][y]) > 0.000001)
					return false;
			}
		}
		return true;
	}
	bool Matrix4::operator!=(const Matrix4& rhs)
	{
		for (int x = 0; x < 4; x++)
		{
			for (int y = 0; y < 4; y++)
			{
				//Ignore float imprecision
				if (abs(this[0][x][y] - rhs[x][y]) > 0.000001)
					return true;
			}
		}
		return false;
	}

	//Operations
	double Matrix4::Determinant()
	{
		Matrix4& self = *this;

		//Laplace expansion
		int det = 0;
		int sign = 1;
		for (int i = 0; i < 4; i++)
		{
			Matrix3 submatrix;

			for (int a = 1; a < 4; a++)
			{
				int b = 0;
				for (int j = 0; j < 4; j++)
				{
					if (j == i) continue;
					submatrix[a - 1][b] = self[a][j];
					b++;
				}
			}

			det += sign * (self[0][i] * (submatrix[0][0] * (submatrix[1][1] * submatrix[2][2] - submatrix[1][2] * submatrix[2][1]) - submatrix[0][1] * (submatrix[1][0] * submatrix[2][2] - submatrix[1][2] * submatrix[2][0]) + submatrix[0][2] * (submatrix[1][0] * submatrix[2][1] - submatrix[1][1] * submatrix[2][0])));
			sign = -sign;
		}

		return det;
	}
	Matrix4 Matrix4::Transpose()
	{
		double t[]
		{
			c1[0], c1[1], c1[2], c1[3],
			c2[0], c2[1], c2[2], c2[3],
			c3[0], c3[1], c3[2], c3[3],
			c4[0], c4[1], c4[2], c4[3],
		};
		return Matrix4(t);
	}
	Matrix4 Matrix4::Inverse()
	{
		Matrix4& self = *this;
		Matrix4 inverse;

		double det = 1 / Determinant();

		//Derived from https://stackoverflow.com/questions/1148309/inverting-a-4x4-matrix
		double A2323 = self[2][2] * self[3][3] - self[2][3] * self[3][2];
		double A1323 = self[2][1] * self[3][3] - self[2][3] * self[3][1];
		double A1223 = self[2][1] * self[3][2] - self[2][2] * self[3][1];
		double A0323 = self[2][0] * self[3][3] - self[2][3] * self[3][0];
		double A0223 = self[2][0] * self[3][2] - self[2][2] * self[3][0];
		double A0123 = self[2][0] * self[3][1] - self[2][1] * self[3][0];
		double A2313 = self[1][2] * self[3][3] - self[1][3] * self[3][2];
		double A1313 = self[1][1] * self[3][3] - self[1][3] * self[3][1];
		double A1213 = self[1][1] * self[3][2] - self[1][2] * self[3][1];
		double A2312 = self[1][2] * self[2][3] - self[1][3] * self[2][2];
		double A1312 = self[1][1] * self[2][3] - self[1][3] * self[2][1];
		double A1212 = self[1][1] * self[2][2] - self[1][2] * self[2][1];
		double A0313 = self[1][0] * self[3][3] - self[1][3] * self[3][0];
		double A0213 = self[1][0] * self[3][2] - self[1][2] * self[3][0];
		double A0312 = self[1][0] * self[2][3] - self[1][3] * self[2][0];
		double A0212 = self[1][0] * self[2][2] - self[1][2] * self[2][0];
		double A0113 = self[1][0] * self[3][1] - self[1][1] * self[3][0];
		double A0112 = self[1][0] * self[2][1] - self[1][1] * self[2][0];

		inverse[0][0] = det * (self[1][1] * A2323 - self[1][2] * A1323 + self[1][3] * A1223);
		inverse[0][1] = det * -(self[0][1] * A2323 - self[0][2] * A1323 + self[0][3] * A1223);
		inverse[0][2] = det * (self[0][1] * A2313 - self[0][2] * A1313 + self[0][3] * A1213);
		inverse[0][3] = det * -(self[0][1] * A2312 - self[0][2] * A1312 + self[0][3] * A1212);
		inverse[1][0] = det * -(self[1][0] * A2323 - self[1][2] * A0323 + self[1][3] * A0223);
		inverse[1][1] = det * (self[0][0] * A2323 - self[0][2] * A0323 + self[0][3] * A0223);
		inverse[1][2] = det * -(self[0][0] * A2313 - self[0][2] * A0313 + self[0][3] * A0213);
		inverse[1][3] = det * (self[0][0] * A2312 - self[0][2] * A0312 + self[0][3] * A0212);
		inverse[2][0] = det * (self[1][0] * A1323 - self[1][1] * A0323 + self[1][3] * A0123);
		inverse[2][1] = det * -(self[0][0] * A1323 - self[0][1] * A0323 + self[0][3] * A0123);
		inverse[2][2] = det * (self[0][0] * A1313 - self[0][1] * A0313 + self[0][3] * A0113);
		inverse[2][3] = det * -(self[0][0] * A1312 - self[0][1] * A0312 + self[0][3] * A0112);
		inverse[3][0] = det * -(self[1][0] * A1223 - self[1][1] * A0223 + self[1][2] * A0123);
		inverse[3][1] = det * (self[0][0] * A1223 - self[0][1] * A0223 + self[0][2] * A0123);
		inverse[3][2] = det * -(self[0][0] * A1213 - self[0][1] * A0213 + self[0][2] * A0113);
		inverse[3][3] = det * (self[0][0] * A1212 - self[0][1] * A0212 + self[0][2] * A0112);

		return inverse;
	}
	Vector4 Matrix4::operator*(const Vector4& rhs) const
	{
		const Matrix4& lhs = *this;
		Vector4 mult;
		mult[0] = (lhs[0][0] * rhs[0] + lhs[1][0] * rhs[1] + lhs[2][0] * rhs[2] + lhs[3][0] * rhs[3]);
		mult[1] = (lhs[0][1] * rhs[0] + lhs[1][1] * rhs[1] + lhs[2][1] * rhs[2] + lhs[3][1] * rhs[3]);
		mult[2] = (lhs[0][2] * rhs[0] + lhs[1][2] * rhs[1] + lhs[2][2] * rhs[2] + lhs[3][2] * rhs[3]);
		mult[3] = (lhs[0][3] * rhs[0] + lhs[1][3] * rhs[1] + lhs[2][3] * rhs[2] + lhs[3][3] * rhs[3]);

		double q = lhs[0][3] * rhs[0];
		double w = lhs[1][3] * rhs[1];
		double e = lhs[2][3] * rhs[2];
		double r = lhs[3][3] * rhs[3];
		double a = (q + w + e + r);

		return mult;
	}
	Matrix4 Matrix4::operator*(const Matrix4& rhs) const
	{
		const Matrix4& lhs = *this;
		Matrix4 mult;
		mult[0][0] = (lhs[0][0] * rhs[0][0] + lhs[1][0] * rhs[0][1] + lhs[2][0] * rhs[0][2] + lhs[3][0] * rhs[0][3]);
		mult[1][0] = (lhs[0][0] * rhs[1][0] + lhs[1][0] * rhs[1][1] + lhs[2][0] * rhs[1][2] + lhs[3][0] * rhs[1][3]);
		mult[2][0] = (lhs[0][0] * rhs[2][0] + lhs[1][0] * rhs[2][1] + lhs[2][0] * rhs[2][2] + lhs[3][0] * rhs[2][3]);
		mult[3][0] = (lhs[0][0] * rhs[3][0] + lhs[1][0] * rhs[3][1] + lhs[2][0] * rhs[3][2] + lhs[3][0] * rhs[3][3]);
		mult[0][1] = (lhs[0][1] * rhs[0][0] + lhs[1][1] * rhs[0][1] + lhs[2][1] * rhs[0][2] + lhs[3][1] * rhs[0][3]);
		mult[1][1] = (lhs[0][1] * rhs[1][0] + lhs[1][1] * rhs[1][1] + lhs[2][1] * rhs[1][2] + lhs[3][1] * rhs[1][3]);
		mult[2][1] = (lhs[0][1] * rhs[2][0] + lhs[1][1] * rhs[2][1] + lhs[2][1] * rhs[2][2] + lhs[3][1] * rhs[2][3]);
		mult[3][1] = (lhs[0][1] * rhs[3][0] + lhs[1][1] * rhs[3][1] + lhs[2][1] * rhs[3][2] + lhs[3][1] * rhs[3][3]);
		mult[0][2] = (lhs[0][2] * rhs[0][0] + lhs[1][2] * rhs[0][1] + lhs[2][2] * rhs[0][2] + lhs[3][2] * rhs[0][3]);
		mult[1][2] = (lhs[0][2] * rhs[1][0] + lhs[1][2] * rhs[1][1] + lhs[2][2] * rhs[1][2] + lhs[3][2] * rhs[1][3]);
		mult[2][2] = (lhs[0][2] * rhs[2][0] + lhs[1][2] * rhs[2][1] + lhs[2][2] * rhs[2][2] + lhs[3][2] * rhs[2][3]);
		mult[3][2] = (lhs[0][2] * rhs[3][0] + lhs[1][2] * rhs[3][1] + lhs[2][2] * rhs[3][2] + lhs[3][2] * rhs[3][3]);
		mult[0][3] = (lhs[0][3] * rhs[0][0] + lhs[1][3] * rhs[0][1] + lhs[2][3] * rhs[0][2] + lhs[3][3] * rhs[0][3]);
		mult[1][3] = (lhs[0][3] * rhs[1][0] + lhs[1][3] * rhs[1][1] + lhs[2][3] * rhs[1][2] + lhs[3][3] * rhs[1][3]);
		mult[2][3] = (lhs[0][3] * rhs[2][0] + lhs[1][3] * rhs[2][1] + lhs[2][3] * rhs[2][2] + lhs[3][3] * rhs[2][3]);
		mult[3][3] = (lhs[0][3] * rhs[3][0] + lhs[1][3] * rhs[3][1] + lhs[2][3] * rhs[3][2] + lhs[3][3] * rhs[3][3]);

		return mult;
	}

	Matrix4 Matrix4::Scale(const Vector3& sv)
	{
		Matrix4 m = *this;
		m[0] = m[0] * sv[0];
		m[1] = m[1] * sv[1];
		m[2] = m[2] * sv[2];
		m[3] = m[3];
		return m;
	}
	Matrix4 Matrix4::Translate(const Vector3& tv)
	{
		Matrix4 m = *this;
		m[3] = Vector4(tv, 1.0);
		return m;
	}
	Matrix4 Matrix4::Rotate(const Vector3& rotation)
	{
		Matrix4 m = *this;
		m = m.RotateX(rotation.x);
		m = m.RotateY(rotation.y);
		m = m.RotateZ(rotation.z);
		return m;
	}
	Matrix4 Matrix4::RotateX(double angle)
	{
		const double c = cos(angle);
		const double s = sin(angle);

		Matrix4 m;
		m[0][0] = 1;
		m[1][1] = c;
		m[2][1] = -s;
		m[1][2] = s;
		m[2][2] = c;
		m[3][3] = 1;
		return m * (*this);
	}
	Matrix4 Matrix4::RotateY(double angle)
	{
		const double c = cos(angle);
		const double s = sin(angle);

		Matrix4 m;
		m[0][0] = c;
		m[2][0] = s;
		m[1][1] = 1;
		m[0][2] = -s;
		m[2][2] = c;
		m[3][3] = 1;
		return m * (* this);
	}
	Matrix4 Matrix4::RotateZ(double angle)
	{
		const double c = cos(angle);
		const double s = sin(angle);

		Matrix4 m;
		m[0][0] = c;
		m[1][0] = -s;
		m[0][1] = s;
		m[1][1] = c;
		m[2][2] = 1;
		m[3][3] = 1;
		return m * (*this);
	}

	std::string Matrix4::ToString()
	{
		Matrix4& self = *this;
		return std::format("[{}, {}, {}, {},\n {}, {}, {}, {},\n {}, {}, {}, {},\n {}, {}, {}, {}]", self[0][0], self[1][0], self[2][0], self[3][0], self[0][1], self[1][1], self[2][1], self[3][1], self[0][2], self[1][2], self[2][2], self[3][2], self[0][3], self[1][3], self[2][3], self[3][3]);
	}
}