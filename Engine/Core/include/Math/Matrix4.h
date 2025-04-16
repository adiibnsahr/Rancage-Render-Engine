#pragma once
#include "Vector3.h"

namespace Math
{
    struct Matrix4
    {
        float m[4][4];

        Matrix4();
        static Matrix4 Identity();
        static Matrix4 CreatePerspective(float fov, float aspect, float near, float far);
        static Matrix4 CreateLookAt(const Vector3& eye, const Vector3& target, const Vector3& up);
        Matrix4 operator*(const Matrix4& other) const; // Tambah operator *
    };
}