#ifndef __INCLUDE_GUARD_Logger_h__
#define __INCLUDE_GUARD_Logger_h__
#include <memory>
#include "spdlog/spdlog.h"



namespace emberEngine
{
	/// <summary>
	/// Custom wrapper for spdlog.
	/// Must call Init() to initialize the static class before using the logger.
	/// Macros for logging are: LOG_TRACE(), LOG_INFO(), LOG_WARN(), LOG_ERROR(), LOG_CRITICAL().
	/// </summary>
	class Logger
	{
	public: // Methods:
		// Must be called once before using the logger:
		static void Init();
		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_coreLogger; }

	private: // Members:
		static bool s_initialized;
		static std::shared_ptr<spdlog::logger> s_coreLogger;
	};



	// Logging macros:
	#define LOG_TRACE(...)    emberEngine::Logger::GetCoreLogger()->trace(__VA_ARGS__)
	#define LOG_INFO(...)     emberEngine::Logger::GetCoreLogger()->info(__VA_ARGS__)
	#define LOG_WARN(...)     emberEngine::Logger::GetCoreLogger()->warn(__VA_ARGS__)
	#define LOG_ERROR(...)    emberEngine::Logger::GetCoreLogger()->error(__VA_ARGS__)
	#define LOG_CRITICAL(...) emberEngine::Logger::GetCoreLogger()->critical(__VA_ARGS__)
}



#endif // __INCLUDE_GUARD_Logger_h__