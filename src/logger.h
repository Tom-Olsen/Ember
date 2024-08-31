#ifndef __INCLUDE_GUARD_Logger_h__
#define __INCLUDE_GUARD_Logger_h__
#include <memory>
#include "spdlog/spdlog.h"



// Custom wrapper for spdlog:
class Logger
{
public:
	// Must be called once before using the logger:
	static void Init();

	inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_coreLogger; }

private:
	static std::shared_ptr<spdlog::logger> s_coreLogger;
};



// Logging macros:
#define LOG_TRACE(...)    ::Logger::GetCoreLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...)     ::Logger::GetCoreLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)     ::Logger::GetCoreLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)    ::Logger::GetCoreLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) ::Logger::GetCoreLogger()->critical(__VA_ARGS__)



#endif // __INCLUDE_GUARD_Logger_h__