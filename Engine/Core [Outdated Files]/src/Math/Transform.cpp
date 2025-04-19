#include "../Core/include/Math/Transform.h"

namespace Math
{
	Transform::Transform()
		: m_Position(0.0f, 0.0f, 0.0f)
		, m_Rotation(0.0f, 0.0f, 0.0f)
		, m_Scale(1.0f, 1.0f, 1.0f)
	{
	}

	void Transform::SetPosition(const Vector3& position)
	{
		m_Position = position;
	}

	void Transform::SetRotation(const Vector3& rotation)
	{
		m_Rotation = rotation;
	}

	void Transform::SetScale(const Vector3& scale)
	{
		m_Scale = scale;
	}

	void Transform::Translate(const Vector3& delta)
	{
		m_Position += delta;
	}

	void Transform::Rotate(const Vector3& delta)
	{
		m_Rotation += delta;
	}

	Matrix4 Transform::GetModelMatrix() const
	{
        // Scale matrix
        Matrix4 scaleMatrix(
            m_Scale.x, 0.0f, 0.0f, 0.0f,
            0.0f, m_Scale.y, 0.0f, 0.0f,
            0.0f, 0.0f, m_Scale.z, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );

        // Rotation matrices (Y -> X -> Z order)
        float cy = cos(m_Rotation.y);
        float sy = sin(m_Rotation.y);
        float cx = cos(m_Rotation.x);
        float sx = sin(m_Rotation.x);
        float cz = cos(m_Rotation.z);
        float sz = sin(m_Rotation.z);

        Matrix4 rotY(
            cy, 0.0f, sy, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            -sy, 0.0f, cy, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );

        Matrix4 rotX(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, cx, -sx, 0.0f,
            0.0f, sx, cx, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );

        Matrix4 rotZ(
            cz, -sz, 0.0f, 0.0f,
            sz, cz, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );

        Matrix4 rotationMatrix = rotZ * rotX * rotY;

        // Translation matrix
        Matrix4 translationMatrix(
            1.0f, 0.0f, 0.0f, m_Position.x,
            0.0f, 1.0f, 0.0f, m_Position.y,
            0.0f, 0.0f, 1.0f, m_Position.z,
            0.0f, 0.0f, 0.0f, 1.0f
        );

        // Model matrix = T * R * S
        return translationMatrix * rotationMatrix * scaleMatrix;
	}

}