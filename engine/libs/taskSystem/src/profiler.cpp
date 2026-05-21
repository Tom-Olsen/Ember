#include "profiler.h"
#include "parallelThreadPool.h"
#include <iostream>
#include <sstream>
#include <thread>



namespace emberTaskSystem
{
    // Event struct:
    namespace profiler
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
        bool Event::IsSameProfileScope(const Event& a, const Event& b)
        {
            return a.name == b.name
                && a.processId == b.processId
                && a.threadId == b.threadId;
        }
        bool Event::IsMatchingTracePair(const Event& startEvent, const Event& endEvent)
        {
            return startEvent.eventType == EventType::start
                && endEvent.eventType == EventType::end
                && IsSameProfileScope(startEvent, endEvent);
        }
    }



    // Session class:
    namespace profiler
    {
        // Singleton getter:
        Session& Session::Get()
        {
            static Session instance;    // only instantiated on first call.
            return instance;
        }
        void Session::Start(const std::string& name, const std::string& filepath)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_isActive = true;
            m_sessionName = name;
            std::string path = (filepath == "" ? name : filepath) + ".json";
            m_outputStream.open(path);
            m_events.clear();
            m_openEvents.clear();
            m_maxOpenEventCount = 0;
            m_openEvents.reserve(1000);
            m_referenceTimePoint = std::chrono::steady_clock::now();

            // Name the main process:
            m_events.push_back(Event { "Main", 0, EventType::processNaming, 0, 0 });
            m_events.push_back(Event { "Main Thread", 0, EventType::threadNaming, 0, 0 });
        }
        void Session::End()
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (!m_isActive)
                return;
            m_isActive = false;

            // Write to disk:
            WriteHeader();
            LogEvents();
            WriteFooter();
            m_outputStream.close();

            // Reset for next session:
            m_events.shrink_to_fit();
            m_openEvents.clear();
            m_openEvents.shrink_to_fit();
            m_sessionName = "";
        }

        // Record events:
        void Session::PushBack(Event&& event)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (!m_isActive)
                return;

            // Track starts until a matching end arrives:
            if (event.eventType == EventType::start)
            {
                m_openEvents.push_back(std::move(event));
                m_maxOpenEventCount = std::max(m_maxOpenEventCount, m_openEvents.size());
                return;
            }

            // Commit completed scopes (start+end pairs):
            if (event.eventType == EventType::end)
            {
                auto startIt = FindLastOpenEvent(event);
                if (startIt == m_openEvents.end())
                    return; // invalid end without start.

                Event startEvent = std::move(*startIt);
                m_openEvents.erase(startIt);
                m_events.push_back(std::move(startEvent));
                m_events.push_back(std::move(event));
                TrimEvents();
                return;
            }

            // Metadata events do not need pairing:
            m_events.push_back(std::move(event));
            TrimEvents();
        }
        void Session::PushBack(const Event& event)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (!m_isActive)
                return;

            // Track starts until a matching end arrives:
            if (event.eventType == EventType::start)
            {
                m_openEvents.push_back(event);
                m_maxOpenEventCount = std::max(m_maxOpenEventCount, m_openEvents.size());
                return;
            }

            // Commit completed scopes (start+end pairs):
            if (event.eventType == EventType::end)
            {
                auto startIt = FindLastOpenEvent(event);
                if (startIt == m_openEvents.end())
                    return;

                Event startEvent = std::move(*startIt);
                m_openEvents.erase(startIt);
                m_events.push_back(std::move(startEvent));
                m_events.push_back(event);
                TrimEvents();
                return;
            }

            // Metadata events do not need pairing:
            m_events.push_back(event);
            TrimEvents();
        }
        std::vector<Event>::iterator Session::FindLastOpenEvent(const Event& event)
        {
            for (auto it = m_openEvents.end(); it != m_openEvents.begin();)
            {
                it--;
                if (Event::IsSameProfileScope(*it, event))
                    return it;
            }
            return m_openEvents.end();
        }
        size_t Session::AccumulateCompletedDuration(const std::string& eventName, TimeUnit unit, double& duration)
        {
            std::vector<Event> openEvents;
            size_t count = 0;

            for (const Event& event : m_events)
            {
                if (event.name != eventName)
                    continue;

                // Collect all start events:
                if (event.eventType == EventType::start)
                {
                    openEvents.push_back(event);
                    continue;
                }
                // Match the end against the newest still-open start:
                else if (event.eventType == EventType::end)
                {
                    auto startIt = openEvents.end();
                    for (auto it = openEvents.end(); it != openEvents.begin();)
                    {
                        it--;
                        if (Event::IsSameProfileScope(*it, event))
                        {
                            startIt = it;
                            break;
                        }
                    }
                    if (startIt == openEvents.end())
                        continue;

                    duration += event.GetTime(unit) - startIt->GetTime(unit);
                    openEvents.erase(startIt);
                    count++;
                }
                // Skip naming events:
                else // (event.eventType == EventType::processNaming || event.eventType == EventType::threadNaming)
                    continue;
            }
            return count;
        }
        void Session::TrimEvents()
        {
            while (m_events.size() > s_maxEventCount)
            {
                Event event = m_events.front();
                m_events.pop_front();

                // If the oldest event was a completed scope start, drop its end with it.
                if (!m_events.empty() && Event::IsMatchingTracePair(event, m_events.front()))
                    m_events.pop_front();
            }
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
            // Sort pointers so completed long-running scopes are written by timestamp, not by end time:
            std::vector<const Event*> events;
            events.reserve(m_events.size());
            for (const Event& event : m_events)
                events.push_back(&event);
            std::stable_sort(events.begin(), events.end(), [](const Event* a, const Event* b)
            {
                // Sort by time:
                if (a->time != b->time)
                    return a->time < b->time;
                // Sort by event type:
                return static_cast<int>(a->eventType) < static_cast<int>(b->eventType);
            });

            for (size_t i = 0; i < events.size(); i++)
            {
                // Sanitize name to not break json format:
                const Event& event = *events[i];
                std::string name = event.name;
                std::replace(name.begin(), name.end(), '"', '\'');

                // Event separator (skip on first event):
                if (i > 0) m_outputStream << ",\n";

                switch (event.eventType)
                {
                    case EventType::start:
                        m_outputStream << "\t\t{\n";
                        m_outputStream << "\t\t\t\"cat\":\"function\",\n";
                        m_outputStream << "\t\t\t\"name\":\"" << name << "\",\n";
                        m_outputStream << "\t\t\t\"ph\":\"B\",\n";
                        m_outputStream << "\t\t\t\"pid\":" << event.processId << ",\n";
                        m_outputStream << "\t\t\t\"tid\":" << event.threadId << ",\n";
                        m_outputStream << "\t\t\t\"ts\":" << event.GetTime(TimeUnit::us) << "\n";
                        m_outputStream << "\t\t}";
                        break;

                    case EventType::end:
                        m_outputStream << "\t\t{\n";
                        m_outputStream << "\t\t\t\"cat\":\"function\",\n";
                        m_outputStream << "\t\t\t\"name\":\"" << name << "\",\n";
                        m_outputStream << "\t\t\t\"ph\":\"E\",\n";
                        m_outputStream << "\t\t\t\"pid\":" << event.processId << ",\n";
                        m_outputStream << "\t\t\t\"tid\":" << event.threadId << ",\n";
                        m_outputStream << "\t\t\t\"ts\":" << event.GetTime(TimeUnit::us) << "\n";
                        m_outputStream << "\t\t}";
                        break;

                    case EventType::processNaming:
                        name = (event.processId  < 10 ? "0" : "") + std::to_string(event.processId ) + "_" + name;
                        m_outputStream << "\t\t{\n";
                        m_outputStream << "\t\t\t\"ph\":\"M\",\n";
                        m_outputStream << "\t\t\t\"name\":\"process_name\",\n";
                        m_outputStream << "\t\t\t\"pid\":" << event.processId << ",\n";
                        m_outputStream << "\t\t\t\"args\":{ \"name\":\"" << name << "\" }\n";
                        m_outputStream << "\t\t}";
                        break;

                    case EventType::threadNaming:
                        m_outputStream << "\t\t{\n";
                        m_outputStream << "\t\t\t\"ph\":\"M\",\n";
                        m_outputStream << "\t\t\t\"name\":\"thread_name\",\n";
                        m_outputStream << "\t\t\t\"pid\":" << event.processId << ",\n";
                        m_outputStream << "\t\t\t\"tid\":" << event.threadId << ",\n";
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
        size_t Session::GetMaxOpenEventCount()
        {
            return m_maxOpenEventCount;
        }
        double Session::GetTotalTime(const std::string& eventName, TimeUnit unit)
        {
            double duration = 0;
            size_t count = AccumulateCompletedDuration(eventName, unit, duration);
            return count != 0 ? duration : -1;
        }
        double Session::GetAverageTime(const std::string& eventName, TimeUnit unit)
        {
            double duration = 0;
            size_t count = AccumulateCompletedDuration(eventName, unit, duration);
            return count != 0 ? duration / count : -1;
        }
        std::vector<std::string> Session::GetAllEventNames()
        {
            std::vector<std::string> names;
            for (size_t i = 0; i < m_events.size(); i++)
            {
                if (m_events[i].eventType != EventType::start && m_events[i].eventType != EventType::end)
                    continue;
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
        void Session::PrintSessionStats()
        {
            std::cout << "Max open event count = " << GetMaxOpenEventCount() << std::endl;
	        std::vector<std::string> results = GetAllEventNames();
	        for (std::string& result : results)
	        	PrintFunctionAverageTime(result, emberTaskSystem::profiler::TimeUnit::ms);
        }
    }



    // Timer class:
    namespace profiler
    {
        Timer::Timer(const char* name)
        : m_name(name), m_isStopped(false)
        {
            m_timePoint = std::chrono::steady_clock::now();
            long long startTime = std::chrono::time_point_cast<std::chrono::microseconds>(m_timePoint).time_since_epoch().count()
                       - std::chrono::time_point_cast<std::chrono::microseconds>(Session::Get().GetReferenceTimePoint()).time_since_epoch().count();
            Session::Get().PushBack(Event { m_name, startTime, EventType::start, ParallelThreadPool::GetPoolId(), ParallelThreadPool::GetCoreIndex() });
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
            Session::Get().PushBack(Event { m_name, endTime, EventType::end, ParallelThreadPool::GetPoolId(), ParallelThreadPool::GetCoreIndex() });
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
}