#include "taskflowManager.h"



namespace emberEngine
{
	// Static members:
	bool TaskflowManager::s_isInitialized = false;
	int TaskflowManager::s_cores;
	tf::Executor TaskflowManager::s_executor;



	// Initialization and cleanup:
	void TaskflowManager::Init()
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		s_cores = static_cast<int>(std::thread::hardware_concurrency());
		tf::Executor s_executor(s_cores);
	}
	void TaskflowManager::Clear()
	{

	}



	int TaskflowManager::GetCoreCount()
	{
		return s_cores;
	}
	int TaskflowManager::GetThreadIndex()
	{
		// thread_local <=> static, but for a thread.
		// This means index will be initialized only once per thread.
		thread_local int index = s_counter++;
		return index;
	}
	tf::Executor& TaskflowManager::GetExecutor()
	{
		return s_executor;
	}
	void TaskflowManager::RunAndWait(tf::Taskflow& taskflow)
	{
		s_executor.run(taskflow).wait();
	}
	void TaskflowManager::ResetThreadIndices()
	{
		// No need to reset except when you expect number of threads in parallel zone to be less.
		s_counter = 0;
	}
}