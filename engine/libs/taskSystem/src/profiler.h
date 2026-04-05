#pragma once
#include <algorithm>
#include <atomic>
#include <chrono>
#include <fstream>
#include <mutex>
#include <string>
#include <vector>



// Usage:
//
// Start session: (e.g. in main routine of your code)
// mdeLib::Profiler::Session::Get().Start("session name", "file name" (optional));
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
// mdeLib::Profiler::Session::Get().End();
//
// Simply drag and drop the resulting .json in chrome/chromium under the link chrome://tracing/ for result visualization.



namespace emberTaskSystem
{
    namespace profiler
    {
        enum class TimeUnit
        {
            s = 0,  // seconds       = 10^0
            ms = 1, // milliseconds = 10^-3
            us = 2  // microseconds = 10^-6
        };
        static const char* TimeUnitNames[] = { "s", "ms", "us" };



        enum class EventType
        {
            start,
            end,
            processNaming,
            threadNaming
        };
        struct Event
        {
            std::string name;       // either: functionName, scopeName, processName, threadName.
            long long time;         // in microseconds (10-6).
            EventType eventType;    // one start(true) and one end(false) event per time measurement.
            uint32_t processId;     // uint64_t due to hashing.
            uint32_t threadId;      // allows up to 65.536 threads.

            double GetTime(TimeUnit unit) const;
            std::string ToString(TimeUnit unit = TimeUnit::s) const;
        };



        class Session
        {
        private: // Members:
            bool m_isActive = false;
            std::string m_sessionName;
            std::ofstream m_outputStream;
            std::mutex m_mutex;
            std::vector<Event> m_events;
            std::chrono::time_point<std::chrono::steady_clock> m_referenceTimePoint;

        private: // Methods:
            // Singleton Pattern: hide these constructors.
            Session() : m_sessionName("") {}

            // Non-copyable:
    		Session(const Session&) = delete;
    		Session& operator=(const Session&) = delete;

    		// Movable:
    		Session(Session&& other) noexcept = default;
    		Session& operator=(Session&& other) noexcept = default;

        public: // Methods:
            static Session& Get();  // Singleton getter.

            // Start/End session before/after profiling:
            void Start(const std::string& name, const std::string& filepath = "");
            void End();

            void PushBack(Event&& event);
            void PushBack(const Event& event);

            // Write data to json:
            void WriteHeader();
            void WriteFooter();
            void LogEvents();

            // Event analysis:
            std::chrono::time_point<std::chrono::steady_clock>& GetReferenceTimePoint();
            void PrintEvents(TimeUnit unit = TimeUnit::s);
            double GetTotalTime(const std::string& eventName, TimeUnit unit = TimeUnit::s);
            double GetAverageTime(const std::string& eventName, TimeUnit unit = TimeUnit::s);
            std::vector<std::string> GetAllEventNames();
            void PrintFunctionTotalTime(const std::string& name, TimeUnit unit = TimeUnit::s);
            void PrintFunctionAverageTime(const std::string& name, TimeUnit unit = TimeUnit::s);
        };



        class Timer
        {
        private:
            const char* m_name;
            bool m_isStopped = true;
            std::chrono::time_point<std::chrono::steady_clock> m_timePoint;

        public:
            Timer(const char* name);
            ~Timer();

            // Non-copyable:
    		Timer(const Timer&) = delete;
    		Timer& operator=(const Timer&) = delete;

    		// Movable:
    		Timer(Timer&& other) noexcept = default;
    		Timer& operator=(Timer&& other) noexcept = default;

            void Stop();
        };
    }

    void Marker(std::string name = "", bool newline = true);
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
    #define PROFILE_SCOPE(name) emberTaskSystem::profiler::Timer timer##__LINE__(name)
    #define PROFILE_FUNCTION() PROFILE_SCOPE(FUNCTION_NAME)
#else
    #define PROFILE_SCOPE(name)
    #define PROFILE_FUNCTION()
#endif