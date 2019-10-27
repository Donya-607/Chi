#ifndef VECTOR_H_
#define VECTOR_H_
#include <cassert>
#include <cmath>
class vector2
{
public:
	float x, y;
	vector2()
	{
		x = 0; y = 0;
	}
	vector2(const float _x, const float _y)
	{
		x = _x; y = _y;
	}
	vector2(const vector2& v)
	{
		x = v.x; y = v.y;
	}


	vector2 operator+(const vector2& v)const
	{
		return{ x + v.x,y + v.y };
	}
	vector2 operator-(const vector2& v)const
	{
		return{ x - v.x,y - v.y };
	}
	vector2 operator*(const float f)const
	{
		return{ x*f,y*f };
	}
	vector2 operator/(const float f)const
	{
		assert(f != 0 && "denominator is zero");
		return{ x / f,y / f };
	}
	vector2 operator+=(const vector2& v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}
	vector2 operator-=(const vector2& v)
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}
	vector2 operator*=(float f)
	{
		x *= f; y *= f;
		return *this;
	}
	vector2 operator/=(const float f)
	{
		assert(f != 0 && "denominator is zero");
		x /= f; y /= f;
		return *this;
	}
	bool operator==(const vector2& v)const
	{
		if (x - 1e-6f > v.x || x + 1e-6f < v.x)return false;
		if (y - 1e-6f > v.y || y + 1e-6f < v.y)return false;
		return true;
	}

	bool operator!=(const vector2& v)const
	{
		if (x - 1e-6f <= v.x && x + 1e-6f >= v.x)return false;
		if (y - 1e-6f <= v.y && y + 1e-6f >= v.y)return false;
		return true;
	}
};

class vector3
{
public:
	float x, y, z;
	vector3()
	{
		x = 0; y = 0; z = 0;
	}
	vector3(float _x, float _y, float _z)
	{
		x = _x; y = _y; z = _z;
	}
	vector3(const vector3& v)
	{
		x = v.x; y = v.y; z = v.z;
	}

	float LengthSq()
	{
		return x*x + y*y + z*z;
	}
	static float LengthSq(const vector3& v)
	{
		return v.x*v.x + v.y*v.y + v.z*v.z;
	}


	float Length()
	{
		return sqrtf(LengthSq());
	}
	static float Length(const vector3& v)
	{
		return sqrtf(LengthSq(v));
	}

	vector3 Scale(float scalar)
	{
		return vector3(x*scalar, y*scalar, z*scalar);
	}
	static vector3 Scale(const vector3& v, float scalar)
	{
		return vector3(v.x*scalar, v.y*scalar, v.z*scalar);
	}

	void Normalize()
	{
		float len = Length();
		assert((int)len != 0);
		*this = Scale(1.0f / len);
	}


	vector3 Add(const vector3& v)
	{
		return vector3(x + v.x, y + v.y, z + v.z);
	}
	static vector3 Add(const vector3& v1, const vector3& v2)
	{
		return vector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
	}


	vector3 Subtract(const vector3& v)
	{
		return vector3(x - v.x, y - v.y, z - v.z);
	}
	static vector3 Subtract(const vector3& v1, const vector3& v2)
	{
		return vector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
	}


	float Dot(const vector3& v)
	{
		return x*v.x + y*v.y + z*v.z;
	}
	static float Dot(const vector3& v1, const vector3& v2)
	{
		return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
	}


	vector3 Cross(const vector3& v)
	{
		return vector3(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
	}
	static vector3 Cross(const vector3& v1, const vector3& v2)
	{
		return vector3(v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x);
	}


	vector3 operator+(const vector3& v)const
	{
		return{ x + v.x,y + v.y,z + v.z };
	}
	vector3 operator-(const vector3& v)const
	{
		return{ x - v.x,y - v.y,z - v.z };
	}
	vector3 operator*(const float f)const
	{
		return{ x*f,y*f,z*f };
	}
	float operator*(const vector3& v)const
	{
		return{ x*v.x + y*v.y + z*v.z };
	}
	vector3 operator/(const float f)const
	{
		assert(f != 0 && "denominator is zero");
		return{ x / f,y / f,z / f };
	}
	vector3 operator+=(const vector3& v)
	{
		x += v.x; y += v.y; z += v.z;
		return *this;
	}
	vector3 operator-=(const vector3& v)
	{
		x -= v.x; y -= v.y; z -= v.z;
		return *this;
	}
	vector3 operator*=(const float f)
	{
		x *= f; y *= f; z *= f;
		return *this;
	}
	bool operator== (const vector3& v) const
	{
		if (x - 1e-6f > v.x || x + 1e-6f < v.x)return false;
		if (y - 1e-6f > v.y || y + 1e-6f < v.y)return false;
		if (z - 1e-6f > v.z || z + 1e-6f < v.z)return false;
		return true;
	}
	bool operator!= (const vector3& v) const
	{
		if (x - 1e-6f <= v.x && x + 1e-6f >= v.x)return false;
		if (y - 1e-6f <= v.y && y + 1e-6f >= v.y)return false;
		if (z - 1e-6f <= v.z && z + 1e-6f >= v.z)return false;
		return true;
	}

};

class vector4
{
public:
	float x, y, z, w;
	vector4()
	{
		x = 0; y = 0; z = 0; w = 0;
	}
	vector4(float _x = 0, float _y = 0, float _z = 0, float _w = 0)
	{
		x = _x; y = _y; z = _z; w = _w;
	}
	vector4(const vector4& v)
	{
		x = v.x; y = v.y; z = v.z; w = v.w;
	}

	float LengthSq()
	{
		return x*x + y*y + z*z;
	}
	static float LengthSq(const vector4& v)
	{
		return v.x*v.x + v.y*v.y + v.z*v.z;
	}


	float Length()
	{
		return sqrtf(LengthSq());
	}
	static float Length(const vector4& v)
	{
		return sqrtf(LengthSq(v));
	}

	vector4 Scale(float scalar)
	{
		return vector4(x*scalar, y*scalar, z*scalar);
	}
	static vector4 Scale(const vector4& v, float scalar)
	{
		return vector4(v.x*scalar, v.y*scalar, v.z*scalar);
	}

	void Normalize()
	{
		float len = Length();
		assert((int)len != 0);
		*this = Scale(1.0f / len);
	}


	vector4 Add(const vector4& v)
	{
		return vector4(x + v.x, y + v.y, z + v.z);
	}
	static vector4 Add(const vector4& v1, const vector4& v2)
	{
		return vector4(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
	}


	vector4 Subtract(const vector4& v)
	{
		return vector4(x - v.x, y - v.y, z - v.z);
	}
	static vector4 Subtract(const vector4& v1, const vector4& v2)
	{
		return vector4(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
	}


	float Dot(const vector4& v)
	{
		return x*v.x + y*v.y + z*v.z;
	}
	static float Dot(const vector4& v1, const vector4& v2)
	{
		return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
	}


	vector4 Cross(const vector4& v)
	{
		return vector4(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
	}
	static vector4 Cross(const vector4& v1, const vector4& v2)
	{
		return vector4(v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x);
	}


	vector4 operator+(const vector4& v)const
	{
		return{ x + v.x,y + v.y,z + v.z };
	}
	vector4 operator-(const vector4& v)const
	{
		return{ x - v.x,y - v.y,z - v.z };
	}
	vector4 operator*(const float f)const
	{
		return{ x*f,y*f,z*f };
	}
	float operator*(const vector4& v)const
	{
		return{ x*v.x + y*v.y + z*v.z };
	}
	vector4 operator/(const float f)const
	{
		assert(f != 0 && "denominator is zero");
		return{ x / f,y / f,z / f };
	}
	vector4 operator+=(const vector4& v)
	{
		x += v.x; y += v.y; z += v.z;
		return *this;
	}
	vector4 operator-=(const vector4& v)
	{
		x -= v.x; y -= v.y; z -= v.z;
		return *this;
	}
	vector4 operator*=(const float f)
	{
		x *= f; y *= f; z *= f;
		return *this;
	}
	bool operator== (const vector4& v) const
	{
		if (x - 1e-6f > v.x || x + 1e-6f < v.x)return false;
		if (y - 1e-6f > v.y || y + 1e-6f < v.y)return false;
		if (z - 1e-6f > v.z || z + 1e-6f < v.z)return false;
		if (w - 1e-6f > v.w || w + 1e-6f < v.w)return false;
		return true;
	}
	bool operator!= (const vector4& v) const
	{
		if (x - 1e-6f <= v.x && x + 1e-6f >= v.x)return false;
		if (y - 1e-6f <= v.y && y + 1e-6f >= v.y)return false;
		if (z - 1e-6f <= v.z && z + 1e-6f >= v.z)return false;
		if (w - 1e-6f <= v.w && w + 1e-6f >= v.w)return false;
		return true;
	}

};

#endif // !VECTOR_H_
