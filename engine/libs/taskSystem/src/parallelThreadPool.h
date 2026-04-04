
#pragma once
#include "profiler.h"
#include <atomic>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>



// Mangeled version to work with c++14...
class ParallelThreadPool
{
private:
    uint32_t m_coreCount;       // logical cores used (clamped to number of physical cores(*2 if hyperthreading supported)).
    uint32_t m_threadCount;     // number of threads to execute (can be bigger than logicalCores).
    bool m_jobReady;            // validats that job has beed registered for execution.
    bool m_stop;                // signals entire pool to stop.
    std::string m_poolName;     // user defined name for profiling.
    uint32_t m_poolId;          // 0 for mainThread, incremented for each created pool.
    std::vector<std::thread> m_workers;
    std::mutex m_mutex;
    std::condition_variable m_cvWorkAvailable;
    std::condition_variable m_cvWorkDone;
    std::function<void()> m_job;
    uint32_t m_nextThread;
    std::atomic<uint32_t> m_threadsDone{0};

    // Thread-local members:
    inline static thread_local ParallelThreadPool* th_currentPool = nullptr; // For static access to information about pool that is currently running via the static Get...() methods. 
    inline static thread_local uint32_t th_coreId = 0;                 // unique identifier for each core € [0, ..., m_coreCount-1] .
    inline static thread_local uint32_t th_threadId = 0;               // unique identifier for tracking which thread is beeing executed in current job.

public:
    // Constructor/Destructor:
    ParallelThreadPool();
    ParallelThreadPool(uint32_t coreCount, const std::string& poolName);
    ~ParallelThreadPool();

    // Non-copyable:
    ParallelThreadPool(const ParallelThreadPool&) = delete;
    ParallelThreadPool& operator=(const ParallelThreadPool&) = delete;

    // Non-movable:
    ParallelThreadPool(ParallelThreadPool&&) noexcept = delete;
    ParallelThreadPool& operator=(ParallelThreadPool&&) noexcept = delete;

    // Invoke any function with arbitrary arguments:
    template<typename Func, typename... Args>
    inline void RunJob(uint32_t numThreads, Func&& f, Args&&... args)
    {
        m_threadCount = numThreads;

        // Asign generic function to m_job:
        { // Only 1 thread at a time can be in this scope:
            std::unique_lock lock(m_mutex); // is this needed? RunJob is only ever called from he thread that owns the threadPool object.
            m_job = [f = std::forward<Func>(f), argsTuple = std::make_tuple(std::forward<Args>(args)...)]() mutable { std::apply(f, argsTuple); };
            m_threadsDone = 0;
            m_nextThread = 0;
            m_jobReady = true;
        }
        m_cvWorkAvailable.notify_all();

        // Wait until all threads have been executed:
        { // Only 1 thread at a time can be in this scope:
            std::unique_lock lock(m_mutex);
            m_cvWorkDone.wait(lock, [this] { return m_threadsDone == m_threadCount; });
        }

        m_threadCount = 1;
    }

    // Getters:
    static uint32_t GetThreadIndex(); // ThreadIndex: e.g. you can run 100 jobs(threads) on only 8 cores.
    static uint32_t GetCoreIndex(); // CoreId = 'logicalCoreId', usually number of physical cores(*2 if hyperthreading supported).
    static uint32_t GetThreadCount();
    static uint32_t GetCoreCount();
    static uint32_t GetPoolId();

private:
    void WorkerLoop(uint32_t coreId);
};