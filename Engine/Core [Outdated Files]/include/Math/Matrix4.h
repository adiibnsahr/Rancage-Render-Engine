#pragma once
#include "Vector3.h"

namespace Math
{
    struct Matrix4
    {
        float m[4][4];

        Matrix4();
        Matrix4(
            float m00, float m01, float m02, float m03,
            float m10, float m11, float m12, float m13,
            float m20, float m21, float m22, float m23,
            float m30, float m31, float m32, float m33
        );

        static Matrix4 Identity();
        static Matrix4 CreatePerspective(float fov, float aspect, float near, float far);
        static Matrix4 CreateLookAt(const Vector3& eye, const Vector3& target, const Vector3& up);
        static Matrix4 CreateRotationZ(float radians); // Tambah untuk 0.0.0.10
        Matrix4 operator*(const Matrix4& other) const;
        Matrix4 Transpose() const;
    };
}