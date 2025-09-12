#ifndef __INCLUDE_GUARD_profiler_h__
#define __INCLUDE_GUARD_profiler_h__
#include "emberTime.h"
#include "logger.h"
#include <fstream>
#include <mutex>
#include <string>
#include <vector>



// Usage:
//
// Start session: (e.g. in main routine of your code)
// Profiler::Session::Get().Start("session name", "file name" (optional));
// 
// Scope profiling:
// {
//     PROFILE_SCOPE("Some name to identify this scope");
//     ...
// }
// 
// Function profiling:
// void Foo(...)
// {
//      PROFILE_FUNCTION();
//      ...
// }
// 
// End session: (e.g. on application shutdown)
// Profiler::Session::Get().End();
//
// Simply drag and drop the resulting .json in chrome/chromium under the link chrome://tracing/ for result visualization.



namespace emberEngine
{
    namespace Profiler
    {
        struct Result
        {
            std::string name;
            long long start;
            long long end;
            uint32_t threadID;
            double Duration(TimeUnit unit) const;
            std::string ToString(TimeUnit unit = TimeUnit::s) const;
        };



        class Session
        {
        private: // Members:
            std::string m_sessionName;
            std::ofstream m_outputStream;
            size_t m_profileCount;
            std::mutex m_writeMutex;
            std::vector<Result> m_results;

        private: // Methods:
            // Singleton Pattern: hide these constructors.
            Session();
            Session(Session const&) = delete;
            void operator=(Session const&) = delete;

        public: // Methods:
            static Session& Get();  // Singleton getter.

            // Start/End session before/after profiling:
            void Start(const std::string& name, const std::string& filepath = "");
            void End();

            // Write data to json:
            void LogResult(const Result& result);
            void WriteHeader();
            void WriteFooter();

            // Result analysis:
            void PrintResults(TimeUnit unit = TimeUnit::s, emberLogger::LogLevel level = emberLogger::LogLevel::trace);
            double GetTotalTime(const std::string& resultName, TimeUnit unit = TimeUnit::s);
            double GetAverageTime(const std::string& resultName, TimeUnit unit = TimeUnit::s);
            std::vector<std::string> GetAllResultNames();
            void PrintFunctionTotalTIme(const std::string& name, TimeUnit unit = TimeUnit::s, emberLogger::LogLevel level = emberLogger::LogLevel::trace);
            void PrintFunctionAverageTime(const std::string& name, TimeUnit unit = TimeUnit::s, emberLogger::LogLevel level = emberLogger::LogLevel::trace);
        };



        class Timer
        {
        private:
            const char* m_name;
            std::chrono::time_point<std::chrono::steady_clock> m_startTimepoint;
            bool m_isStopped;
            
        public:
            Timer(const char* name);
            ~Timer();
            void Stop();
        };
    }
}



// Macro for function name extraction with any compiler:
#if defined(_MSC_VER)
    #define FUNCTION_NAME __FUNCTION__
#else
    #define FUNCTION_NAME __PRETTY_FUNCTION__
#endif

// Macros for enabling/disabling chrome://tracing/ profiler:
#define PROFILING
#ifdef PROFILING
#define PROFILE_SCOPE(name) Profiler::Timer timer##__LINE__(name)
#define PROFILE_FUNCTION() PROFILE_SCOPE(FUNCTION_NAME)
#else
#define PROFILE_SCOPE(name)
#define PROFILE_FUNCTION()
#endif



#endif //__INCLUDE_GUARD_profiler_h__