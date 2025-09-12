#ifndef __INCLUDE_GUARD_emberTaskSystem_taskSystem_h__
#define __INCLUDE_GUARD_emberTaskSystem_taskSystem_h__
#include <taskflow/taskflow.hpp>



namespace emberTaskSystem
{
    /// <summary>
    /// Purely static class that allows acces to global tf::Executor.
    /// </summary>
    class TaskSystem
    {
    private: // Members:
        static bool s_isInitialized;
        static int s_cores; // number of logical cores (physical + hyperthreading)
        static tf::Executor s_executor;
        inline static std::atomic<int> s_counter{ 0 };

    public: // Methods:
        static void Init();
        static void Clear();
        static int GetCoreCount();
        static int GetThreadIndex();
        static tf::Executor& GetExecutor();
        static void RunAndWait(tf::Taskflow& taskflow);
        static void ResetThreadIndices();

    private: // Methods:
        // Delete all constructors:
        TaskSystem() = delete;
        TaskSystem(const TaskSystem&) = delete;
        TaskSystem& operator=(const TaskSystem&) = delete;
        TaskSystem(TaskSystem&&) = delete;
        TaskSystem& operator=(TaskSystem&&) = delete;
        ~TaskSystem() = delete;
    };
}
#endif // __INCLUDE_GUARD_emberTaskSystem_taskSystem_h__