#include "../Core/include/Math/Matrix4.h"
#include "../Core/include/Utils/Logger.h" // Tambah untuk logging
#include <cmath>

namespace Math
{
    Matrix4::Matrix4()
    {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                m[i][j] = 0.0f;
    }

    Matrix4::Matrix4(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33)
    {
        m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
        m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
        m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
        m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
    }

    Matrix4 Matrix4::Identity()
    {
        Matrix4 mat;
        mat.m[0][0] = mat.m[1][1] = mat.m[2][2] = mat.m[3][3] = 1.0f;
        return mat;
    }

    Matrix4 Matrix4::CreatePerspective(float fov, float aspect, float near, float far)
    {
        Matrix4 mat;
        float tanHalfFov = tanf(fov / 2.0f);
        mat.m[0][0] = 1.0f / (aspect * tanHalfFov); // x scale
        mat.m[1][1] = 1.0f / tanHalfFov; // y scale
        mat.m[2][2] = -(far + near) / (far - near); // z scale
        mat.m[2][3] = -1.0f; // perspective
        mat.m[3][2] = -(2.0f * far * near) / (far - near); // z translate

        // Log projection matrix
        Logger::Log(LogLevel::Info, "Projection Matrix: [%.2f, %.2f, %.2f, %.2f]",
            mat.m[0][0], mat.m[0][1], mat.m[0][2], mat.m[0][3]);
        Logger::Log(LogLevel::Info, "                  [%.2f, %.2f, %.2f, %.2f]",
            mat.m[1][0], mat.m[1][1], mat.m[1][2], mat.m[1][3]);
        Logger::Log(LogLevel::Info, "                  [%.2f, %.2f, %.2f, %.2f]",
            mat.m[2][0], mat.m[2][1], mat.m[2][2], mat.m[2][3]);
        Logger::Log(LogLevel::Info, "                  [%.2f, %.2f, %.2f, %.2f]",
            mat.m[3][0], mat.m[3][1], mat.m[3][2], mat.m[3][3]);

        return mat;
    }

    Matrix4 Matrix4::CreateLookAt(const Vector3& eye, const Vector3& target, const Vector3& up)
    {
        Matrix4 mat;
        Vector3 z = (target - eye).Normalize();
        Vector3 x = up.Cross(z).Normalize();
        Vector3 y = z.Cross(x);

        mat.m[0][0] = x.x; mat.m[0][1] = x.y; mat.m[0][2] = x.z; mat.m[0][3] = -x.Dot(eye);
        mat.m[1][0] = y.x; mat.m[1][1] = y.y; mat.m[1][2] = y.z; mat.m[1][3] = -y.Dot(eye);
        mat.m[2][0] = z.x; mat.m[2][1] = z.y; mat.m[2][2] = z.z; mat.m[2][3] = -z.Dot(eye);
        mat.m[3][3] = 1.0f;

        // Log view matrix
        Logger::Log(LogLevel::Info, "View Matrix: [%.2f, %.2f, %.2f, %.2f]",
            mat.m[0][0], mat.m[0][1], mat.m[0][2], mat.m[0][3]);
        Logger::Log(LogLevel::Info, "            [%.2f, %.2f, %.2f, %.2f]",
            mat.m[1][0], mat.m[1][1], mat.m[1][2], mat.m[1][3]);
        Logger::Log(LogLevel::Info, "            [%.2f, %.2f, %.2f, %.2f]",
            mat.m[2][0], mat.m[2][1], mat.m[2][2], mat.m[2][3]);
        Logger::Log(LogLevel::Info, "            [%.2f, %.2f, %.2f, %.2f]",
            mat.m[3][0], mat.m[3][1], mat.m[3][2], mat.m[3][3]);

        return mat;
    }

    Matrix4 Matrix4::CreateRotationZ(float radians)
    {
        float c = cosf(radians);
        float s = sinf(radians);
        return Matrix4(
            c, -s, 0.0f, 0.0f,
            s, c, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    Matrix4 Matrix4::operator*(const Matrix4& other) const
    {
        Matrix4 result;
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                result.m[i][j] = m[i][0] * other.m[0][j] +
                    m[i][1] * other.m[1][j] +
                    m[i][2] * other.m[2][j] +
                    m[i][3] * other.m[3][j];
            }
        }
        return result;
    }

    Matrix4 Matrix4::Transpose() const
    {
        Matrix4 result;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                result.m[i][j] = m[j][i];
        return result;
    }
}