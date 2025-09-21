#pragma once
#include <cstdint>
#include <string>



namespace vulkanRendererBackend
{
	using PresentMode = uint32_t;
	struct PresentModes
	{
        static PresentMode immediate;
        static PresentMode mailbox;
        static PresentMode fifo;
        static PresentMode fifo_relaxed;
        static PresentMode shared_demand_refresh;
        static PresentMode shared_continuous_refresh;
        static PresentMode fifo_latest_ready;
        static PresentMode max_enum;
        static std::string ToString(PresentMode presentMode);
	};
}