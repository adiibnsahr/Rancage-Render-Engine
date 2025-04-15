#pragma once
#include "Vector.h"
#include <cmath> // Buat tan()

struct Matrix4x4
{
    float m[4][4];

    Matrix4x4()
    {
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                m[i][j] = (i == j) ? 1.0f : 0.0f;
            }
        }
    }

    static Matrix4x4 Identity()
    {
        return Matrix4x4();
    }

    Matrix4x4 operator*(const Matrix4x4& other) const
    {
        Matrix4x4 result;
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                result.m[i][j] = 0.0f;
                for (int k = 0; k < 4; ++k)
                {
                    result.m[i][j] += m[i][k] * other.m[k][j];
                }
            }
        }
        return result;
    }

    Vector4 Transform(const Vector4& v) const
    {
        Vector4 result;
        result.x = v.x * m[0][0] + v.y * m[0][1] + v.z * m[0][2] + v.w * m[0][3];
        result.y = v.x * m[1][0] + v.y * m[1][1] + v.z * m[1][2] + v.w * m[1][3];
        result.z = v.x * m[2][0] + v.y * m[2][1] + v.z * m[2][2] + v.w * m[2][3];
        result.w = v.x * m[3][0] + v.y * m[3][1] + v.z * m[3][2] + v.w * m[3][3];
        return result;
    }

    static Matrix4x4 Perspective(float fov, float aspect, float nearPlane, float farPlane)
    {
        Matrix4x4 result;
        float tanHalfFov = tan(fov * 0.5f);
        float zRange = farPlane - nearPlane;

        result.m[0][0] = 1.0f / (aspect * tanHalfFov);
        result.m[0][1] = 0.0f;
        result.m[0][2] = 0.0f;
        result.m[0][3] = 0.0f;

        result.m[1][0] = 0.0f;
        result.m[1][1] = 1.0f / tanHalfFov;
        result.m[1][2] = 0.0f;
        result.m[1][3] = 0.0f;

        result.m[2][0] = 0.0f;
        result.m[2][1] = 0.0f;
        result.m[2][2] = -(farPlane + nearPlane) / zRange;
        result.m[2][3] = -2.0f * farPlane * nearPlane / zRange;

        result.m[3][0] = 0.0f;
        result.m[3][1] = 0.0f;
        result.m[3][2] = -1.0f;
        result.m[3][3] = 0.0f;

        return result;
    }
};