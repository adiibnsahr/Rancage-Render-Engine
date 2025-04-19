#pragma once
#include "Vector3.h"
#include "Matrix4.h"
#include "MathUtils.h"
#include <cmath>

namespace Math
{
	class Transform
	{
	public:
		Transform();

		// Set posisi, rotasi (radian), scale
		void SetPosition(const Vector3& position);
		void SetRotation(const Vector3& rotation);
		void SetScale(const Vector3& scale);

		// Tambah perubahan
		void Translate(const Vector3& delta);
		void Rotate(const Vector3& delta);

		// Ambil model matrix
		Matrix4 GetModelMatrix() const;

	private:
		Vector3 m_Position;
		Vector3 m_Rotation; // Euler angles (radian)
		Vector3 m_Scale;
	};
}
