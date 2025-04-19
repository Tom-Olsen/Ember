#include "vulkanComputePushConstant.h"



namespace emberEngine
{
	namespace vulkanBackend
	{
		// Constructor:
		ComputePushConstant::ComputePushConstant(Uint3 threadCount, float time, float deltaTime)
		{
			this->threadCount = threadCount;
			this->time = time;
			this->deltaTime = deltaTime;
		}

		// Public methods:
		std::string ComputePushConstant::ToString()
		{
			std::string output = "ComputePushConstant:\n";
			output += "ThreadCount: " + threadCount.ToString() + "\n";
			output += "Time: " + std::to_string(time) + "\n";
			output += "Delta Time: " + std::to_string(deltaTime);
			return output;
		}
	}
}