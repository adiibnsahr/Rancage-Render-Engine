#pragma once
#include <vector>
#include "../Rendering/Model.h"
#include "../Math/Vector.h"

inline std::vector<Vertex> GetTriangleVertices() {
    std::vector<Vertex> vertices = {
        { Vector3(0.0f, 0.5f, 0.0f), Vector3(0.0f, 0.0f, -1.0f) },  // Atas
        { Vector3(0.5f, -0.5f, 0.0f), Vector3(0.0f, 0.0f, -1.0f) }, // Kanan bawah
        { Vector3(-0.0f, -0.5f, 0.0f), Vector3(0.0f, 0.0f, -1.0f) } // Kiri bawah
    };
    return vertices;
}