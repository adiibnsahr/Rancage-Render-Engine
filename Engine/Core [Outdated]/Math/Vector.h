#pragma once
#include <cmath>

struct Vector2
{
	float x, y;

	Vector2() : x(0.0f), y(0.0f) {}
	Vector2(float x, float y) : x(x), y(y) {}

	Vector2 operator+(const Vector2& other) const
	{
		return Vector2(x + other.x, y + other.y);
	}

	Vector2 operator-(const Vector2& other) const
	{
		return Vector2(x - other.x, y - other.y);
	}

	Vector2 operator*(float scalar) const
	{
		return Vector2(x * scalar, y * scalar);
	}
	
	float Dot(const Vector2& other) const
	{
		return x * other.x + y * other.y;
	}

	float Length() const
	{
		return sqrt(Dot(*this));
	}

	Vector2 Normalize() const
	{
		float length = Length();
		if (length > 0.0f)
		{
			return Vector2(x / length, y / length);
		}
		return*this;
	}
};

struct Vector3
{
	float x, y, z;
	Vector3() : x(0.0f), y(0.0f), z(0.0) {}
	Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

	Vector3 operator+(const Vector3& other) const
	{
		return Vector3(x + other.x, y + other.y, z + other.z);
	}

	Vector3 operator-(const Vector3& other) const
	{
		return Vector3(x - other.x, y - other.y, z - other.z);
	}

	Vector3 operator*(float scalar) const
	{
		return Vector3(x * scalar, y * scalar, z * scalar);
	}

	float Dot(const Vector3& other) const
	{
		return x * other.x + y * other.y + z * other.z;
	}

	Vector3 Normalize() const
	{
		float length = sqrt(Dot(*this));
		if (length > 0.0f)
		{
			return Vector3(x / length, y / length, z / length);
		}
		return*this;
	}
};

struct Vector4
{
	float x, y, z, w;

	Vector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
	Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	Vector4(const Vector3& v, float w) : x(v.x), y(v.y), z(v.z), w(w) {}
};
