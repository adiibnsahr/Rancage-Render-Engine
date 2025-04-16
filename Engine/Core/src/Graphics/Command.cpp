#include "../Core/include/Graphics/Command.h"
#include "../Core/include/Utils/Logger.h"
#include <comdef.h>

namespace Graphics
{
    bool Command::Initialize(ID3D12Device* device)
    {
        // Create command queue
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

        HRESULT hr = device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_Queue));
        if (FAILED(hr))
        {
            _com_error err(hr);
            Logger::Log(LogLevel::Error, "Failed to create command queue: %s", err.ErrorMessage());
            return false;
        }
        Logger::Log(LogLevel::Info, "Command queue created");

        // Create command allocator
        hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_Allocator));
        if (FAILED(hr))
        {
            _com_error err(hr);
            Logger::Log(LogLevel::Error, "Failed to create command allocator: %s", err.ErrorMessage());
            return false;
        }
        Logger::Log(LogLevel::Info, "Command allocator created");

        // Create command list
        hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_Allocator.Get(), nullptr, IID_PPV_ARGS(&m_CommandList));
        if (FAILED(hr))
        {
            _com_error err(hr);
            Logger::Log(LogLevel::Error, "Failed to create command list: %s", err.ErrorMessage());
            return false;
        }
        m_CommandList->Close();
        Logger::Log(LogLevel::Info, "Command list created");

        return true;
    }
}