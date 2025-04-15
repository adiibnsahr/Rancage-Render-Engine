#pragma once
#include "../Core/include/Graphics/Device.h"

namespace Core
{
	class Application
	{
	public:
		Application();
		~Application();

		bool Initialize();
		void Run();

	private:
		Graphics::Device m_Device;
	};
}
