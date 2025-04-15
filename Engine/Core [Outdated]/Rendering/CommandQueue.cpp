#include "CommandQueue.h"
#include "../Utils/Logger.h"

bool CommandQueue::Initialize(ComPtr<ID3D12Device> device, ComPtr<ID3D12PipelineState> pipelineState)
{
	if (!device)
	{
		Logger::Log(LogLevel::Error, "Device is null when creating Command Queue");
		return false;
	}

	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0;

	Logger::Log(LogLevel::Info, "Creating command queue...");
	HRESULT hr = device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_Queue));
	if (FAILED(hr))
	{
		Logger::Log(LogLevel::Error, "Failed to create command queue: HRESULT " + std::to_string(hr));
		return false;
	}
	Logger::Log(LogLevel::Info, "Command queue created");

	// Buat fence
	Logger::Log(LogLevel::Info, "Creating fence...");
	hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence));
	if (FAILED(hr))
	{
		Logger::Log(LogLevel::Error, "Failed to create fence: HRESULT " + std::to_string(hr));
		return false;
	}
	Logger::Log(LogLevel::Info, "Fence created");

	// Buat fence event
	m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (m_FenceEvent == nullptr)
	{
		Logger::Log(LogLevel::Error, "Failed to create fence event: Error " + std::to_string(GetLastError()));
		return false;
	}
	Logger::Log(LogLevel::Info, "Fence event created");

	// Buat command allocator
	Logger::Log(LogLevel::Info, "Creating command allocator...");
	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator));
	if (FAILED(hr))
	{
		Logger::Log(LogLevel::Error, "Failed to create command allocator: HRESULT " + std::to_string(hr));
		return false;
	}
	Logger::Log(LogLevel::Info, "Command allocator created");

	// Buat command list
	Logger::Log(LogLevel::Info, "Creating command list...");
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator.Get(), pipelineState.Get(), IID_PPV_ARGS(&m_CommandList));
	if (FAILED(hr))
	{
		Logger::Log(LogLevel::Error, "Failed to create command list: HRESULT " + std::to_string(hr));
		return false;
	}
	Logger::Log(LogLevel::Info, "Command list created");
	
	// Tutup command list (harus ditutup dulu sebelum bisa dieksekusi
	hr = m_CommandList->Close();
	if (FAILED(hr))
	{
		Logger::Log(LogLevel::Error, "Failed to close command list: HRESULT " + std::to_string(hr));
		return false;
	}
	Logger::Log(LogLevel::Info, "Command list initialized");

	return true;
}

void CommandQueue::WaitForFence()
{
	if (!m_Queue || !m_Fence || !m_FenceEvent)\
	{
		Logger::Log(LogLevel::Error, "Cannot wait for fence: Queue, fence, or event is null");
		return;
	}

	// Signal fence dengan fence value berikutnya
	const UINT64 fenceValueToSignal = m_FenceValue + 1;
	HRESULT hr = m_Queue->Signal(m_Fence.Get(), fenceValueToSignal);
	if (FAILED(hr))
	{
		Logger::Log(LogLevel::Error, "Failed to signal fence: HRESULT " + std::to_string(hr));
		return;
	}

	if (m_Fence->GetCompletedValue() < fenceValueToSignal)
	{
		hr = m_Fence->SetEventOnCompletion(fenceValueToSignal, m_FenceEvent);
		if (FAILED(hr))
		{
			Logger::Log(LogLevel::Error, "Failed to set fence event: HRESULT " + std::to_string(hr));
			return;
		}
		WaitForSingleObject(m_FenceEvent, INFINITE);
	}

	// Update fence value
	m_FenceValue = fenceValueToSignal;
}
