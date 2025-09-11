#ifndef __INCLUDE_GUARD_taskflowManager_h__
#define __INCLUDE_GUARD_taskflowManager_h__
#include <atomic>



// Forward declerations:
namespace tf
{
  class Executor;
  class Taskflow;
}



namespace emberEngine
{


    /// <summary>
    /// Purely static class that allows acces to global tf::Executor.
    /// </summary>
    class TaskflowManager
    {
    private: // Members:
        static bool s_isInitialized;
        static int s_cores; // number of logical cores (physical + hyperthreading)
        static tf::Executor* s_pExecutor;
        inline static std::atomic<int> s_counter{ 0 };

    public: // Methods:
        static void Init();
        static void Clear();
        static int GetCoreCount();
        static int GetThreadIndex();
        static tf::Executor* GetExecutor();
        static void RunAndWait(tf::Taskflow& taskflow);
        static void ResetThreadIndices();

    private: // Methods:
        // Delete all constructors:
        TaskflowManager() = delete;
        TaskflowManager(const TaskflowManager&) = delete;
        TaskflowManager& operator=(const TaskflowManager&) = delete;
        TaskflowManager(TaskflowManager&&) = delete;
        TaskflowManager& operator=(TaskflowManager&&) = delete;
        ~TaskflowManager() = delete;
    };
}



#endif // __INCLUDE_GUARD_taskflowManager_h__