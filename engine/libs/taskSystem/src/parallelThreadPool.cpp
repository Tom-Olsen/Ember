#include "parallelThreadPool.h"



// Public methods:
// Constructor/Destructor:
ParallelThreadPool::ParallelThreadPool()
    : m_coreCount{0}
    , m_threadCount{0}
    , m_jobReady{false}
    , m_stop{true}
    , m_poolName{""}
    , m_poolId{UINT32_MAX -1}
{
    
}
ParallelThreadPool::ParallelThreadPool(uint32_t coreCount, const std::string& poolName)
    : m_threadCount{1}
    , m_jobReady{false}
    , m_stop{false}
    , m_poolName{poolName}
{
    unsigned int logicalCores = std::thread::hardware_concurrency();
    if (logicalCores == 0)
        logicalCores = 1; // fallback as hardware_concurrency can return 0 on weird systems.
    m_coreCount = std::min<uint32_t>(coreCount, logicalCores);

    static uint32_t nextPoolId = 1;  // 0 reserved for mainThread.
    m_poolId = nextPoolId++;

    #ifdef PROFILING
    Profiler::Event processNamingEvent { poolName, 0, Profiler::EventType::processNaming, m_poolId, 0 };
    Profiler::Session::Get().PushBack(processNamingEvent);
    #endif

    // Create logicalCores many threads and assign them the WorkerLoop:
    for (uint32_t coreId = 0; coreId < m_coreCount; coreId++)
    {
        m_workers.emplace_back([this, coreId] { WorkerLoop(coreId); });
        #ifdef PROFILING
        Profiler::Event threadNamingEvent { poolName  + " Core " + (coreId < 10 ? "0" : "") + std::to_string(coreId), 0, Profiler::EventType::threadNaming, m_poolId, coreId };
        Profiler::Session::Get().PushBack(threadNamingEvent);
        #endif
    }
}
ParallelThreadPool::~ParallelThreadPool()
{
    // Set stop signal.
    { // Only 1 thread at a time can be in this scope:
        std::unique_lock lock(m_mutex);
        m_stop = true;
    }
    m_cvWorkAvailable.notify_all();                   // release the idle wait => threads will exit infinit loop.
    for (auto& t : m_workers) t.join();               // join threads (waits for all threads to finish the workLoop).
}



// Getters:
uint32_t ParallelThreadPool::GetThreadIndex()
{
    return th_threadId;
}
uint32_t ParallelThreadPool::GetCoreIndex() 
{
    return th_coreId;
}
uint32_t ParallelThreadPool::GetThreadCount()
{
    return th_currentPool ? th_currentPool->m_threadCount : 1;
}
uint32_t ParallelThreadPool::GetCoreCount()
{
    return th_currentPool ? th_currentPool->m_coreCount : 1;
}
uint32_t ParallelThreadPool::GetPoolId()
{
    return th_currentPool ? th_currentPool->m_poolId : 0;
}



// Public methods:
void ParallelThreadPool::WorkerLoop(uint32_t coreId)
{
    th_coreId = coreId;
    std::function<void()> currentJob; // core local job function.

    while (true)
    {
        // Wait until job is available and assign it to all cores:
        { // Only 1 thread at a time can be in this scope:
            std::unique_lock lock(m_mutex); 
            // Checks condition whenever m_cvWorkAvailable is notified and continues if true:
            m_cvWorkAvailable.wait(lock, [this]{ return m_stop || (m_jobReady && (m_threadCount > m_nextThread)); });
            if (m_stop) break;              // shut down thread pool.
            currentJob = m_job;             // assign job to execute.
            th_threadId = m_nextThread++;   // get id of next thread to work on and increment it.
        }
    
        // Execute job on all threads:
        th_currentPool = this;
        try
        {
            currentJob();
        }
        catch(const std::exception& e)
        {
            std::cerr << "Core " << th_coreId << ", Thread " << th_threadId << "failed with: " << e.what() << '\n';
        }
        catch (...)
        {
            std::cerr << "Core " << th_coreId << ", Thread " << th_threadId << " failed with unknown exception\n";
        }
        th_currentPool = nullptr;

        // Atomic increment to count completed threads:
        if (m_threadCount == ++m_threadsDone)
        {
            m_jobReady = false;
            m_cvWorkDone.notify_one();
        }
    }
}