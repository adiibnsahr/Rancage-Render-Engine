#pragma once
#include "../Math/Vector3.h"
#include "../Math/Matrix4.h"

namespace Graphics
{
    class Camera
    {
    public:
        Camera();
        void Update(float deltaTime);
        Math::Matrix4 GetViewMatrix() const;
        Math::Matrix4 GetProjectionMatrix() const;

    private:
        Math::Vector3 m_Position;
        Math::Vector3 m_Rotation; // Pitch (x), Yaw (y)
        float m_Fov;
        float m_Aspect;
        float m_Near;
        float m_Far;
        float m_MoveSpeed;
        float m_RotateSpeed;
    };
}