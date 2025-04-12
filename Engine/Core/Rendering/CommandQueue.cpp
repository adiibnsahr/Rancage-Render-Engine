#include "CommandQueue.h"
#include "../Utils/Logger.h"

bool CommandQueue::Initialize(ComPtr<ID3D12Device> device)
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

	return true;
}
