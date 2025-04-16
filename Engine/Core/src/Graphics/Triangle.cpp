#include "../Core/include/Graphics/Triangle.h"
#include "../Core/include/Utils/Logger.h"
#include <d3dx12.h>

namespace Graphics
{
    Triangle::Triangle(ID3D12Device* device)
    {
        // Hardcoded vertices
        Math::Vector3 vertices[] = {
            Math::Vector3(0.0f,  0.5f, 0.5f), // Top
            Math::Vector3(0.5f, -0.5f, 0.5f), // Bottom right
            Math::Vector3(-0.5f, -0.5f, 0.5f) // Bottom left
        };

        // Create vertex buffer
        {
            CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_UPLOAD);
            CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices));
            HRESULT hr = device->CreateCommittedResource(
                &heapProperties,
                D3D12_HEAP_FLAG_NONE,
                &bufferDesc,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&m_VertexBuffer));
            if (FAILED(hr))
            {
                Logger::Log(LogLevel::Error, "Failed to create triangle vertex buffer");
                return;
            }

            void* mappedData;
            CD3DX12_RANGE readRange(0, 0);
            hr = m_VertexBuffer->Map(0, &readRange, &mappedData);
            if (FAILED(hr))
            {
                Logger::Log(LogLevel::Error, "Failed to map triangle vertex buffer");
                return;
            }
            memcpy(mappedData, vertices, sizeof(vertices));
            m_VertexBuffer->Unmap(0, nullptr);

            m_VBV.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
            m_VBV.SizeInBytes = sizeof(vertices);
            m_VBV.StrideInBytes = sizeof(Math::Vector3);
        }

        // Hardcoded indices
        UINT indices[] = {
            0, 1, 2
        };

        // Create index buffer
        {
            CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_UPLOAD);
            CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(indices));
            HRESULT hr = device->CreateCommittedResource(
                &heapProperties,
                D3D12_HEAP_FLAG_NONE,
                &bufferDesc,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&m_IndexBuffer));
            if (FAILED(hr))
            {
                Logger::Log(LogLevel::Error, "Failed to create triangle index buffer");
                return;
            }

            void* mappedData;
            CD3DX12_RANGE readRange(0, 0);
            hr = m_IndexBuffer->Map(0, &readRange, &mappedData);
            if (FAILED(hr))
            {
                Logger::Log(LogLevel::Error, "Failed to map triangle index buffer");
                return;
            }
            memcpy(mappedData, indices, sizeof(indices));
            m_IndexBuffer->Unmap(0, nullptr);

            m_IBV.BufferLocation = m_IndexBuffer->GetGPUVirtualAddress();
            m_IBV.SizeInBytes = sizeof(indices);
            m_IBV.Format = DXGI_FORMAT_R32_UINT;
        }

        // Inisialisasi transform
        m_Transform.SetPosition(Math::Vector3(0.0f, 0.0f, 0.0f));
        m_Transform.SetScale(Math::Vector3(1.0f, 1.0f, 1.0f));

        Logger::Log(LogLevel::Info, "Triangle initialized");
    }

    void Triangle::Render(ID3D12GraphicsCommandList* commandList) const
    {
        commandList->IASetVertexBuffers(0, 1, &m_VBV);
        commandList->IASetIndexBuffer(&m_IBV);
        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList->DrawIndexedInstanced(3, 1, 0, 0, 0);
        Logger::Log(LogLevel::Info, "Rendering triangle");
    }

    void Triangle::Update(float deltaTime)
    {
        // Rotasi di sumbu Y (90 derajat/detik = 1.5708 radian/detik)
        m_Transform.Rotate(Math::Vector3(0.0f, 1.5708f * deltaTime, 0.0f));
        Logger::Log(LogLevel::Info, "Updating triangle transform");
    }

    Math::Matrix4 Triangle::GetModelMatrix() const
    {
        return m_Transform.GetModelMatrix();
    }
}