#include "profiler.h"



// Result struct:
namespace emberEngine
{
    namespace Profiler
    {
        double Result::Duration(TimeUnit unit) const
        {
            switch (unit)
            {
            case emberEngine::TimeUnit::s:
                return (end - start) / 1'000'000.0;
            case emberEngine::TimeUnit::ms:
                return (end - start) / 1'000.0;
            case emberEngine::TimeUnit::us:
                return end - start;
            default:
                return -1.0;
            }
        }
        std::string Result::ToString(TimeUnit unit) const
        {
            std::ostringstream os;
            os << "Name:     " << name << "\n"
                << "Start:    " << start << "\n"
                << "End:      " << end << "\n"
                << "Duration: " << Duration(unit) << "\n"
                << "ThreadID: " << threadID << "\n";
            return os.str();
        }
    }
}



// Session class:
namespace emberEngine
{
    namespace Profiler
    {
        Session& Session::Get()
        {
            static Session instance;
            return instance;
        }
        void Session::Start(const std::string& name, const std::string& filepath)
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
        void Session::End()
        {
            WriteFooter();
            results.resize(numResults);
            outputStream.close();
            sessionName = "";
            profileCount = 0;
        }

        // Write data to json:
        void Session::LogResult(const Result& result)
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
        void Session::WriteHeader()
        {
            outputStream << "{\n";
            outputStream << "\t\"otherData\": {},\n";
            outputStream << "\t\"traceEvents\":\n";
            outputStream << "\t[\n";
        }
        void Session::WriteFooter()
        {
            outputStream << "\n\t]\n";
            outputStream << "}";
            outputStream.flush();
        }

        // Result analysis:
        void Session::PrintResults(TimeUnit unit, LogLevel level)
        {
            for (size_t i = 0; i < results.size(); i++)
                LOG(level, "{}", results[i].ToString(unit));
        }
        double Session::GetTotalTime(const std::string& resultName, TimeUnit unit)
        {
            double duration = 0;
            for (size_t i = 0; i < results.size(); i++)
            {
                if (results[i].name == resultName)
                    duration += results[i].Duration(unit);
            }
            return duration != 0 ? duration : -1;
        }
        double Session::GetAverageTime(const std::string& resultName, TimeUnit unit)
        {
            int count = 0;
            double duration = 0;
            for (size_t i = 0; i < results.size(); i++)
            {
                if (results[i].name == resultName)
                {
                    duration += results[i].Duration(unit);
                    count++;
                }
            }
            return duration != 0 ? duration / count : -1;
        }
        std::vector<std::string> Session::GetAllResultNames()
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
        void Session::PrintFunctionTotalTIme(const std::string& name, TimeUnit unit, LogLevel level)
        {
            LOG(level, "{}: {}{}", name, GetTotalTime(name, unit), TimeUnitNames[static_cast<int>(unit)]);
        }
        void Session::PrintFunctionAverageTime(const std::string& name, TimeUnit unit, LogLevel level)
        {
            LOG(level, "{}: {}{}", name, GetAverageTime(name, unit), TimeUnitNames[static_cast<int>(unit)]);
        }
	}
}



// Timer class:
namespace emberEngine
{
    namespace Profiler
    {
        Timer::Timer(const char* name_) : name(name_), isStopped(false)
        {
            startTimepoint = std::chrono::steady_clock::now();
        }
        Timer::~Timer()
        {
            if (!isStopped)
                Stop();
        }
        void Timer::Stop()
        {
            auto endTimepoint = std::chrono::steady_clock::now();

            long long start = std::chrono::time_point_cast<std::chrono::microseconds>(startTimepoint).time_since_epoch().count();
            long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

            //uint32_t threadID = omp_get_thread_num();         // implement proper thread handling via c++ threads 
            Session::Get().LogResult({ name, start, end, 0 });

            isStopped = true;
        }
    }
}