#include "vulkanVertexInputRate.h"
#include <unordered_map>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	VertexInputRate VertexInputRates::vertex = VK_VERTEX_INPUT_RATE_VERTEX;
	VertexInputRate VertexInputRates::instance = VK_VERTEX_INPUT_RATE_INSTANCE;
	VertexInputRate VertexInputRates::max_enum = VK_VERTEX_INPUT_RATE_MAX_ENUM;

	std::string VertexInputRates::ToString(VertexInputRate vertexInputRate)
	{
		const static std::unordered_map<VertexInputRate, std::string> vertexInputRateNames =
		{
			{ VertexInputRates::vertex, "vertex" },
			{ VertexInputRates::instance, "instance" },
			{ VertexInputRates::max_enum, "max_enum" }
		};

		auto it = vertexInputRateNames.find(vertexInputRate);
		return it != vertexInputRateNames.end() ? it->second : "unknown";
	}
}