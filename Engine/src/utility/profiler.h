// Basic instrumentation profiler by Cherno
// Usage:
//
// Initialize Profiler (e.g. in main routine of your code):
// Profiler::Session::Get().Start("session name", "file name" (optional));
// 
// {
//     PROFILE_SCOPE("Some name to identify this scope");
//     ...
// }
// 
// void Foo(...)
// {
//      PROFILE_FUNCTION();
//      ...
// }
// 
// End Session:
// Profiler::Session::Get().End();
//
// Simply drag and drop the resulting .json in chrome/chromium uder the link chrome://tracing/ for result visualization.
#ifndef __INCLUDE_GUARD_profiler_h__
#define __INCLUDE_GUARD_profiler_h__
#include <chrono>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <vector>



// Ember::TODO: multithreading support via c++ thread management, not openmp



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



namespace Profiler
{
    struct Result
    {
        std::string name;
        long long start, end;
        uint32_t threadID;
        double DurationNS() const
        {
            return end - start;
        }
        double DurationMS() const
        {
            return (end - start) / 1000.0;
        }
        double DurationS() const
        {
            return (end - start) / 1000000.0;
        }
        inline friend std::ostream& operator<<(std::ostream& os, const Result& result);
    };
    inline std::ostream& operator<<(std::ostream& os, const Result& result)
    {
        os << "Name:     " << result.name << "\n"
            << "Start:    " << result.start << "\n"
            << "End:      " << result.end << "\n"
            << "Duration: " << result.DurationS() << "\n"
            << "ThreadID: " << result.threadID << "\n";
        return os;
    }



    class Session
    {
    private:
        std::string sessionName;
        std::ofstream outputStream;
        size_t profileCount;
        std::mutex writeMutex;
    public:
        std::vector<Result> results;
        size_t numResults;

    private:
        // Singleton Pattern: hide these constructors from user!
        Session() : sessionName(""), profileCount(0), numResults(0) {}
        Session(Session const&) = delete;
        void operator=(Session const&) = delete;

    public:
        // Only way to access the Session is via Session::Get().
        // All constructors are either hidden and/or deleted.
        // You can still save a reference to the singleton:
        // Profiler::Session& session = Profiler::Session::Get();
        static Session& Get()
        {
            static Session instance;
            return instance;
        }

        // These two functions need to embrace the code you want to benchmark:
        void Start(const std::string& name, const std::string& filepath = "")
        {
            sessionName = name;
            std::string path = (filepath == "" ? name : filepath) + ".json";
            outputStream.open(path);
            profileCount = 0;
            results.clear();
            results.resize(1000);
            numResults = 0;
            WriteHeader();
        }
        void End()
        {
            WriteFooter();
            results.resize(numResults);
            outputStream.close();
            sessionName = "";
            profileCount = 0;
        }

        // Write data to json:
        void LogResult(const Result& result)
        {
            std::lock_guard<std::mutex> lock(writeMutex);

            numResults++;
            if (results.size() < numResults)
                results.resize(results.size() + 1000);
            results[numResults - 1] = result;

            if (profileCount++ > 0)
                outputStream << ",\n";

            std::string name = result.name;
            std::replace(name.begin(), name.end(), '"', '\'');

            outputStream << "\t\t{\n";
            outputStream << "\t\t\t\"cat\":\"function\",\n";
            outputStream << "\t\t\t\"dur\":" << (result.end - result.start) << ",\n";
            outputStream << "\t\t\t\"name\":\"" << name << "\",\n";
            outputStream << "\t\t\t\"ph\":\"X\",\n";
            outputStream << "\t\t\t\"pid\":0,\n";
            outputStream << "\t\t\t\"tid\":" << result.threadID << ",\n";
            outputStream << "\t\t\t\"ts\":" << result.start << "\n";
            outputStream << "\t\t}";
        }
        void WriteHeader()
        {
            outputStream << "{\n";
            outputStream << "\t\"otherData\": {},\n";
            outputStream << "\t\"traceEvents\":\n";
            outputStream << "\t[\n";
        }
        void WriteFooter()
        {
            outputStream << "\n\t]\n";
            outputStream << "}";
            outputStream.flush();
        }

        // Result analysis:
        void PrintResults()
        {
            for (size_t i = 0; i < results.size(); i++)
                std::cout << results[i] << std::endl;
        }
        double GetTotalTime(std::string functionName)
        {
            double duration = 0;
            for (size_t i = 0; i < results.size(); i++)
            {
                if (results[i].name == functionName)
                    duration += results[i].DurationS();
            }
            return duration;
        }
        std::vector<std::string> GetAllFunctionNames()
        {
            std::vector<std::string> names;
            for (size_t i = 0; i < results.size(); i++)
            {
                if (std::find(names.begin(), names.end(), results[i].name) != names.end())
                    continue;
                else
                    names.push_back(results[i].name);
            }
            return names;
        }
        void PrintFunctionDuration(std::string name)
        {
            std::cout << name << ": " << GetTotalTime(name) << "s\n";
        }
    };



    class Timer
    {
    private:
        const char* name;
        std::chrono::time_point<std::chrono::steady_clock> startTimepoint;
        bool isStopped;

    public:
        Timer(const char* name_)
            : name(name_), isStopped(false)
        {
            startTimepoint = std::chrono::steady_clock::now();
        }

        ~Timer()
        {
            if (!isStopped)
                Stop();
        }

        void Stop()
        {
            auto endTimepoint = std::chrono::steady_clock::now();

            long long start = std::chrono::time_point_cast<std::chrono::microseconds>(startTimepoint).time_since_epoch().count();
            long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

            //uint32_t threadID = omp_get_thread_num();         // implement proper thread handling via c++ threads 
            Session::Get().LogResult({ name, start, end, 0 });

            isStopped = true;
        }
    };
}



#endif //__INCLUDE_GUARD_profiler_h__