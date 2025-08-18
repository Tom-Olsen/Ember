#ifndef __INCLUDE_GUARD_Logger_h__
#define __INCLUDE_GUARD_Logger_h__
#include <memory>
#include "spdlog/spdlog.h"



namespace emberEngine
{
	enum class LogLevel
	{
		trace = 0,
		info = 1,
		warn = 2,
		error = 3,
		critical = 4
	};



	/// <summary>
	/// Custom wrapper for spdlog.
	/// Must call Init() to initialize the static class before using the logger.
	/// Macros for logging are: LOG_TRACE(), LOG_INFO(), LOG_WARN(), LOG_ERROR(), LOG_CRITICAL().
	/// </summary>
	class Logger
	{
	private: // Members:
		static bool s_initialized;
		static std::shared_ptr<spdlog::logger> s_coreLogger;

	public: // Methods:
		static void Init();	// Must be called once before using the logger.
		static std::shared_ptr<spdlog::logger>& GetCoreLogger();
	};



	// Helper macro, internal use only:
	#define INTERNAL_LOG(level, ...)                                       \
	((level) == emberEngine::LogLevel::trace    ? LOG_TRACE(__VA_ARGS__) : \
	 (level) == emberEngine::LogLevel::info     ? LOG_INFO(__VA_ARGS__) :  \
	 (level) == emberEngine::LogLevel::warn     ? LOG_WARN(__VA_ARGS__) :  \
	 (level) == emberEngine::LogLevel::error    ? LOG_ERROR(__VA_ARGS__) : \
	 (level) == emberEngine::LogLevel::critical ? LOG_CRITICAL(__VA_ARGS__) : \
	 (void)0)

	// Logging macros:
	#define LOG(level, ...)   INTERNAL_LOG(level, __VA_ARGS__)
	#define LOG_TRACE(...)    emberEngine::Logger::GetCoreLogger()->trace(__VA_ARGS__)
	#define LOG_INFO(...)     emberEngine::Logger::GetCoreLogger()->info(__VA_ARGS__)
	#define LOG_WARN(...)     emberEngine::Logger::GetCoreLogger()->warn(__VA_ARGS__)
	#define LOG_ERROR(...)    emberEngine::Logger::GetCoreLogger()->error(__VA_ARGS__)
	#define LOG_CRITICAL(...) emberEngine::Logger::GetCoreLogger()->critical(__VA_ARGS__)

	// Debug logging macros:
    //#define ENABLE_DEBUG_LOG
    #ifdef ENABLE_DEBUG_LOG
	#define DEBUG_LOG(level, ...)   INTERNAL_DEBUG_LOG(level, __VA_ARGS__)
	#define DEBUG_LOG(level, ...)   emberEngine::Logger::Log(level, __VA_ARGS__)
    #define DEBUG_LOG_TRACE(...)    emberEngine::Logger::GetCoreLogger()->trace(__VA_ARGS__)
    #define DEBUG_LOG_INFO(...)     emberEngine::Logger::GetCoreLogger()->info(__VA_ARGS__)
    #define DEBUG_LOG_WARN(...)     emberEngine::Logger::GetCoreLogger()->warn(__VA_ARGS__)
    #define DEBUG_LOG_ERROR(...)    emberEngine::Logger::GetCoreLogger()->error(__VA_ARGS__)
    #define DEBUG_LOG_CRITICAL(...) emberEngine::Logger::GetCoreLogger()->critical(__VA_ARGS__)
    #else
	#define DEBUG_LOG(level, ...)   ((void)0)
    #define DEBUG_LOG_TRACE(...)    ((void)0)
    #define DEBUG_LOG_INFO(...)     ((void)0)
    #define DEBUG_LOG_WARN(...)     ((void)0)
    #define DEBUG_LOG_ERROR(...)    ((void)0)
    #define DEBUG_LOG_CRITICAL(...) ((void)0)
    #endif
}



#endif // __INCLUDE_GUARD_Logger_h__