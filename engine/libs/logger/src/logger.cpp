#include "logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"



namespace emberLogger
{
	// Public methods:
	// Getters:
	std::shared_ptr<spdlog::logger>& Logger::GetCoreLogger()
	{
		static std::shared_ptr<spdlog::logger> pCoreLogger = CreateCoreLogger();
		return pCoreLogger;
	}



	// Private methods:
	// Creation:
	std::shared_ptr<spdlog::logger> Logger::CreateCoreLogger()
	{
		//spdlog::set_pattern("%^[%s:%#] [%T] %n: %v%$"); // this needs some extra stuff to work
		spdlog::set_pattern("%^[%T] %n: %v%$");
		std::shared_ptr<spdlog::logger> pCoreLogger = spdlog::stdout_color_mt("Ember");
		pCoreLogger->set_level(spdlog::level::trace);
		return pCoreLogger;
	}
}