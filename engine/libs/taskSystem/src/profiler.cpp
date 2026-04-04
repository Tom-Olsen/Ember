#include "profiler.h"
#include "parallelThreadPool.h"
#include <iostream>
#include <sstream>
#include <thread>



// Event struct:
namespace Profiler
{
    double Event::GetTime(TimeUnit unit) const
    {
        switch (unit)
        {
        case TimeUnit::s:
            return time / 1'000'000.0;
        case TimeUnit::ms:
            return time / 1'000.0;
        case TimeUnit::us:
            return time;
        default:
            return -1.0;
        }
    }
    std::string Event::ToString(TimeUnit unit) const
    {
        std::ostringstream os;
        os << "Name: " << name << "\n"
           << "Time: " << GetTime(unit) << "\n";
        return os.str();
    }
}



// Session class:
namespace Profiler
{
    // Singleton getter:
    Session& Session::Get()
    {
        static Session instance;    // only instantiated on first call.
        return instance;
    }
    void Session::Start(const std::string& name, const std::string& filepath)
    {
        m_isActive = true;
        m_sessionName = name;
        std::string path = (filepath == "" ? name : filepath) + ".json";
        m_outputStream.open(path);
        m_events.clear();
        m_events.reserve(1000);     // initial capacity != 0 is essential!
        m_referenceTimePoint = std::chrono::steady_clock::now();

        // Name the main process:
        Profiler::Event mainProcessNamingEvent { "Main", 0, Profiler::EventType::processNaming, 0, 0 };
        PushBack(mainProcessNamingEvent);
        Profiler::Event mainThreadNamingEvent { "Main Thread", 0, Profiler::EventType::threadNaming, 0, 0 };
        PushBack(mainThreadNamingEvent);
    }
    void Session::End()
    {
        // Write to disk:
        WriteHeader();
        LogEvents();
        WriteFooter();
        m_outputStream.close();

        // Reset for next session:
        m_events.shrink_to_fit();
        m_sessionName = "";
        m_isActive = false;
    }

    // Add events to vector:
    void Session::PushBack(Event&& event)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_isActive)
            return;

        // Double capacity once m_events is full (expects m_events.size() != 0):
        if (m_events.size() == m_events.capacity())
            m_events.reserve(2 * m_events.capacity());

        m_events.push_back(std::move(event));
    }
    void Session::PushBack(const Event& event)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_isActive)
            return;

        // Double capacity once m_events is full (expects m_events.size() != 0):
        if (m_events.size() == m_events.capacity())
            m_events.reserve(2 * m_events.capacity());

        m_events.push_back(event);
    }

    // Write data to json:
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
    void Session::LogEvents()
    {
        for (size_t i = 0; i < m_events.size(); i++)
        {
            // Sanitize name to not break json format:
            std::string name = m_events[i].name;
            std::replace(name.begin(), name.end(), '"', '\'');

            // Event seperator (skip on first event):
            if (i > 0) m_outputStream << ",\n";

            switch (m_events[i].eventType)
            {
                case EventType::start:
                    m_outputStream << "\t\t{\n";
                    m_outputStream << "\t\t\t\"cat\":\"function\",\n";
                    m_outputStream << "\t\t\t\"name\":\"" << name << "\",\n";
                    m_outputStream << "\t\t\t\"ph\":\"B\",\n";
                    m_outputStream << "\t\t\t\"pid\":" << m_events[i].processId << ",\n";
                    m_outputStream << "\t\t\t\"tid\":" << m_events[i].threadId << ",\n";
                    m_outputStream << "\t\t\t\"ts\":" << m_events[i].GetTime(TimeUnit::us) << "\n";
                    m_outputStream << "\t\t}";
                    break;

                case EventType::end:
                    m_outputStream << "\t\t{\n";
                    m_outputStream << "\t\t\t\"cat\":\"function\",\n";
                    m_outputStream << "\t\t\t\"name\":\"" << name << "\",\n";
                    m_outputStream << "\t\t\t\"ph\":\"E\",\n";
                    m_outputStream << "\t\t\t\"pid\":" << m_events[i].processId << ",\n";
                    m_outputStream << "\t\t\t\"tid\":" << m_events[i].threadId << ",\n";
                    m_outputStream << "\t\t\t\"ts\":" << m_events[i].GetTime(TimeUnit::us) << "\n";
                    m_outputStream << "\t\t}";
                    break;

                case EventType::processNaming:
                    name = (m_events[i].processId  < 10 ? "0" : "") + std::to_string(m_events[i].processId ) + "_" + name;
                    m_outputStream << "\t\t{\n";
                    m_outputStream << "\t\t\t\"ph\":\"M\",\n";
                    m_outputStream << "\t\t\t\"name\":\"process_name\",\n";
                    m_outputStream << "\t\t\t\"pid\":" << m_events[i].processId << ",\n";
                    m_outputStream << "\t\t\t\"args\":{ \"name\":\"" << name << "\" }\n";
                    m_outputStream << "\t\t}";
                    break;

                case EventType::threadNaming:
                    m_outputStream << "\t\t{\n";
                    m_outputStream << "\t\t\t\"ph\":\"M\",\n";
                    m_outputStream << "\t\t\t\"name\":\"thread_name\",\n";
                    m_outputStream << "\t\t\t\"pid\":" << m_events[i].processId << ",\n";
                    m_outputStream << "\t\t\t\"tid\":" << m_events[i].threadId << ",\n";
                    m_outputStream << "\t\t\t\"args\":{ \"name\":\"" << name << "\" }\n";
                    m_outputStream << "\t\t}";
                    break;
            }
        }
    }


    // Event analysis:
    std::chrono::time_point<std::chrono::steady_clock>& Session::GetReferenceTimePoint()
    {
        return m_referenceTimePoint;
    }
    void Session::PrintEvents(TimeUnit unit)
    {
        for (size_t i = 0; i < m_events.size(); i++)
            std::cout << m_events[i].ToString(unit) << std::endl;
    }
    double Session::GetTotalTime(const std::string& eventName, TimeUnit unit)
    {
        double duration = 0;
        for (size_t i = 0; i < m_events.size(); i++)
        {
            if (m_events[i].name == eventName)
            {
                if (m_events[i].eventType == EventType::start)
                    duration += m_events[i].GetTime(unit);
                else if (m_events[i].eventType == EventType::end)
                    duration -= m_events[i].GetTime(unit);
            }
        }
        return duration != 0 ? duration : -1;
    }
    double Session::GetAverageTime(const std::string& eventName, TimeUnit unit)
    {
        int count = 0;
        double duration = 0;
        for (size_t i = 0; i < m_events.size(); i++)
        {
            if (m_events[i].name == eventName)
            {
                if (m_events[i].eventType == EventType::start)
                    duration += m_events[i].GetTime(unit);
                else if (m_events[i].eventType == EventType::end)
                    duration -= m_events[i].GetTime(unit);
                count++;
            }
        }
        return duration != 0 ? duration / count : -1;
    }
    std::vector<std::string> Session::GetAllEventNames()
    {
        std::vector<std::string> names;
        for (size_t i = 0; i < m_events.size(); i++)
        {
            if (std::find(names.begin(), names.end(), m_events[i].name) != names.end())
                continue;
            else
                names.push_back(m_events[i].name);
        }
        return names;
    }
    void Session::PrintFunctionTotalTime(const std::string& name, TimeUnit unit)
    {
        std::cout << name << ": " << GetTotalTime(name, unit) << TimeUnitNames[static_cast<int>(unit)] << std::endl;
    }
    void Session::PrintFunctionAverageTime(const std::string& name, TimeUnit unit)
    {
        std::cout << name << ": " << GetAverageTime(name, unit) << TimeUnitNames[static_cast<int>(unit)] << std::endl;
    }
}



// Timer class:
namespace Profiler
{
    Timer::Timer(const char* name)
    : m_name(name), m_isStopped(false)
    {
        m_timePoint = std::chrono::steady_clock::now();
        long long startTime = std::chrono::time_point_cast<std::chrono::microseconds>(m_timePoint).time_since_epoch().count()
                   - std::chrono::time_point_cast<std::chrono::microseconds>(Session::Get().GetReferenceTimePoint()).time_since_epoch().count();
        Event event { m_name, startTime, EventType::start, ParallelThreadPool::GetPoolId(), ParallelThreadPool::GetCoreIndex() };
        Session::Get().PushBack(event);
        //std::cout << "Start '" << m_name << "': " << event.GetTime(TimeUnit::ms) << "ms" << std::endl;
    }
    Timer::~Timer()
    {
        if (!m_isStopped)
            Stop();
    }
    void Timer::Stop()
    {
        m_timePoint = std::chrono::steady_clock::now();
        long long endTime = std::chrono::time_point_cast<std::chrono::microseconds>(m_timePoint).time_since_epoch().count()
                   - std::chrono::time_point_cast<std::chrono::microseconds>(Session::Get().GetReferenceTimePoint()).time_since_epoch().count();

        Event event { m_name, endTime, EventType::end, ParallelThreadPool::GetPoolId(), ParallelThreadPool::GetCoreIndex() };
        Session::Get().PushBack(event);
        //std::cout << "  End '" << m_name << "': " << event.GetTime(TimeUnit::ms) << "ms" << std::endl;
        m_isStopped = true;
    }
}



void Marker(std::string name, bool newline)
{
    static size_t i = 0;
    std::cout << name << ": " << i << std::endl;
    if (newline)
        std::cout << std::endl;
    i++;
}