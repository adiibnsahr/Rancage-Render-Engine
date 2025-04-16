#include "../Core/include/Math/Matrix4.h"
#include <cmath>

namespace Math
{
    Matrix4::Matrix4()
    {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                m[i][j] = 0.0f;
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
        return mat;
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
}