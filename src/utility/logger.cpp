#include "logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"



bool Logger::s_initialized = false;
std::shared_ptr<spdlog::logger> Logger::s_coreLogger;



void Logger::Init()
{
	if (s_initialized)
		return;

	//spdlog::set_pattern("%^[%s:%#] [%T] %n: %v%$"); // this needs some extra stuff to work
	spdlog::set_pattern("%^[%T] %n: %v%$");
	s_coreLogger = spdlog::stdout_color_mt("MyEngine");
	s_coreLogger->set_level(spdlog::level::trace);
}