#pragma once
#include <d3d12.h>

namespace Graphics
{
	class IPipelineComponent
	{
	public:
		virtual ~IPipelineComponent() = default;
		virtual bool Configure(ID3D12Device* device) = 0;
	};
}