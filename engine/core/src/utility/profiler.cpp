#include "profiler.h"
#include "taskSystem.h"
#include <sstream>
#include <thread>



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
        Session::Session() : m_sessionName(""), m_profileCount(0)
        {

        }

        Session& Session::Get()
        {
            static Session instance;
            return instance;
        }
        void Session::Start(const std::string& name, const std::string& filepath)
        {
            m_sessionName = name;
            std::string path = (filepath == "" ? name : filepath) + ".json";
            m_outputStream.open(path);
            m_profileCount = 0;
            m_results.clear();
            m_results.reserve(1000);
            WriteHeader();
        }
        void Session::End()
        {
            WriteFooter();
            m_results.shrink_to_fit();
            m_outputStream.close();
            m_sessionName = "";
            m_profileCount = 0;
        }

        // Write data to json:
        void Session::LogResult(const Result& result)
        {
            std::lock_guard<std::mutex> lock(m_writeMutex);

            // Double capacity once m_results is full (expects m_results.size() != 0):
            if (m_results.size() == m_results.capacity())
                m_results.reserve(2 * m_results.capacity());
            m_results.push_back(result);

            if (m_profileCount++ > 0)
                m_outputStream << ",\n";

            std::string name = result.name;
            std::replace(name.begin(), name.end(), '"', '\'');

            m_outputStream << "\t\t{\n";
            m_outputStream << "\t\t\t\"cat\":\"function\",\n";
            m_outputStream << "\t\t\t\"dur\":" << (result.end - result.start) << ",\n";
            m_outputStream << "\t\t\t\"name\":\"" << name << "\",\n";
            m_outputStream << "\t\t\t\"ph\":\"X\",\n";
            m_outputStream << "\t\t\t\"pid\":0,\n";
            m_outputStream << "\t\t\t\"tid\":" << result.threadID << ",\n";
            m_outputStream << "\t\t\t\"ts\":" << result.start << "\n";
            m_outputStream << "\t\t}";
        }
        void Session::WriteHeader()
        {
            m_outputStream << "{\n";
            m_outputStream << "\t\"otherData\": {},\n";
            m_outputStream << "\t\"traceEvents\":\n";
            m_outputStream << "\t[\n";
        }
        void Session::WriteFooter()
        {
            m_outputStream << "\n\t]\n";
            m_outputStream << "}";
            m_outputStream.flush();
        }

        // Result analysis:
        void Session::PrintResults(TimeUnit unit, emberLogger::LogLevel level)
        {
            for (size_t i = 0; i < m_results.size(); i++)
                LOG(level, "{}", m_results[i].ToString(unit));
        }
        double Session::GetTotalTime(const std::string& resultName, TimeUnit unit)
        {
            double duration = 0;
            for (size_t i = 0; i < m_results.size(); i++)
            {
                if (m_results[i].name == resultName)
                    duration += m_results[i].Duration(unit);
            }
            return duration != 0 ? duration : -1;
        }
        double Session::GetAverageTime(const std::string& resultName, TimeUnit unit)
        {
            int count = 0;
            double duration = 0;
            for (size_t i = 0; i < m_results.size(); i++)
            {
                if (m_results[i].name == resultName)
                {
                    duration += m_results[i].Duration(unit);
                    count++;
                }
            }
            return duration != 0 ? duration / count : -1;
        }
        std::vector<std::string> Session::GetAllResultNames()
        {
            std::vector<std::string> names;
            for (size_t i = 0; i < m_results.size(); i++)
            {
                if (std::find(names.begin(), names.end(), m_results[i].name) != names.end())
                    continue;
                else
                    names.push_back(m_results[i].name);
            }
            return names;
        }
        void Session::PrintFunctionTotalTIme(const std::string& name, TimeUnit unit, emberLogger::LogLevel level)
        {
            LOG(level, "{}: {}{}", name, GetTotalTime(name, unit), TimeUnitNames[static_cast<int>(unit)]);
        }
        void Session::PrintFunctionAverageTime(const std::string& name, TimeUnit unit, emberLogger::LogLevel level)
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
        Timer::Timer(const char* name) : m_name(name), m_isStopped(false)
        {
            m_startTimepoint = std::chrono::steady_clock::now();
        }
        Timer::~Timer()
        {
            if (!m_isStopped)
                Stop();
        }
        void Timer::Stop()
        {
            auto endTimepoint = std::chrono::steady_clock::now();

            long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_startTimepoint).time_since_epoch().count();
            long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

            uint32_t threadID = emberTaskSystem::TaskSystem::GetThreadIndex();
            Session::Get().LogResult({ m_name, start, end, threadID });

            m_isStopped = true;
        }
    }
}