#include "../Core/include/Graphics/Camera.h"
#include "../Core/include/Utils/Logger.h"
#include <Windows.h>
#include <cmath>
#include <algorithm>

namespace Graphics
{
    Camera::Camera()
        : m_Position(0.0f, 0.0f, 5.0f) // Mulai di Z=5
        , m_Rotation(0.0f, 0.0f, 0.0f)
        , m_Fov(60.0f * 3.1415926535f / 180.0f)
        , m_Aspect(1280.0f / 720.0f)
        , m_Near(0.1f)
        , m_Far(100.0f)
        , m_MoveSpeed(5.0f)
        , m_RotateSpeed(0.001f)
    {
        Logger::Log(LogLevel::Info, "Camera initialized at pos: (%.2f, %.2f, %.2f)",
            m_Position.x, m_Position.y, m_Position.z);
    }

    void Camera::Update(float deltaTime)
    {
        // WASD movement
        Math::Vector3 moveDir(0.0f, 0.0f, 0.0f);
        if (GetAsyncKeyState('W') & 0x8000) moveDir.z += 1.0f;
        if (GetAsyncKeyState('S') & 0x8000) moveDir.z -= 1.0f;
        if (GetAsyncKeyState('A') & 0x8000) moveDir.x -= 1.0f;
        if (GetAsyncKeyState('D') & 0x8000) moveDir.x += 1.0f;

        // Log input
        POINT mousePos;
        GetCursorPos(&mousePos);
        static POINT lastMousePos = mousePos;
        float dx = static_cast<float>(mousePos.x - lastMousePos.x);
        float dy = static_cast<float>(mousePos.y - lastMousePos.y);
        Logger::Log(LogLevel::Info, "Input: W=%d, S=%d, A=%d, D=%d, dx=%.2f, dy=%.2f",
            (GetAsyncKeyState('W') & 0x8000) ? 1 : 0, (GetAsyncKeyState('S') & 0x8000) ? 1 : 0,
            (GetAsyncKeyState('A') & 0x8000) ? 1 : 0, (GetAsyncKeyState('D') & 0x8000) ? 1 : 0,
            dx, dy);

        // Rotate moveDir berdasarkan yaw (Y-axis)
        float cosYaw = cosf(m_Rotation.y);
        float sinYaw = sinf(m_Rotation.y);
        Math::Vector3 forward(cosYaw, 0.0f, -sinYaw);
        Math::Vector3 right(sinYaw, 0.0f, cosYaw);
        Math::Vector3 move = forward * moveDir.z + right * moveDir.x;
        m_Position += move * m_MoveSpeed * deltaTime;

        // Mouse look
        m_Rotation.y -= dx * m_RotateSpeed; // Yaw
        m_Rotation.x -= dy * m_RotateSpeed; // Pitch
        m_Rotation.x = std::clamp(m_Rotation.x, -1.5708f, 1.5708f); // Clamp pitch ±90°

        lastMousePos = mousePos;

        Logger::Log(LogLevel::Info, "Camera updated, pos: (%.2f, %.2f, %.2f), yaw: %.2f, pitch: %.2f",
            m_Position.x, m_Position.y, m_Position.z, m_Rotation.y, m_Rotation.x);
    }

    Math::Matrix4 Camera::GetViewMatrix() const
    {
        // Forward vector berdasarkan pitch dan yaw
        float cosPitch = cosf(m_Rotation.x);
        float sinPitch = sinf(m_Rotation.x);
        float cosYaw = cosf(m_Rotation.y);
        float sinYaw = sinf(m_Rotation.y);
        Math::Vector3 forward(cosYaw * cosPitch, sinPitch, -sinYaw * cosPitch);
        Math::Vector3 target = m_Position + forward;
        Math::Vector3 up(0.0f, 1.0f, 0.0f);
        return Math::Matrix4::CreateLookAt(m_Position, target, up);
    }

    Math::Matrix4 Camera::GetProjectionMatrix() const
    {
        return Math::Matrix4::CreatePerspective(m_Fov, m_Aspect, m_Near, m_Far);
    }
}