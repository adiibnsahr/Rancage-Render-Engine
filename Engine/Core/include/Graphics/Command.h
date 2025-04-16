#pragma once
#include <d3d12.h>
#include <wrl/client.h>

namespace Graphics
{
    class Command
    {
    public:
        Command();
        ~Command();

        bool Initialize(ID3D12Device* device);
        ID3D12CommandQueue* GetQueue() const { return m_Queue.Get(); }
        ID3D12CommandAllocator* GetAllocator() const { return m_Allocator.Get(); }
        ID3D12GraphicsCommandList* GetList() const { return m_List.Get(); }

    private:
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_Queue;
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_Allocator;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_List;
    };
}