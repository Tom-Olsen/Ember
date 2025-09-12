#include "taskSystem.h"



namespace emberTaskSystem
{
	// Static members:
	bool TaskSystem::s_isInitialized = false;
	int TaskSystem::s_cores;
	tf::Executor TaskSystem::s_executor;



	// Initialization/Cleanup:
	void TaskSystem::Init()
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		s_cores = static_cast<int>(std::thread::hardware_concurrency());
		tf::Executor s_executor(s_cores);
	}
	void TaskSystem::Clear()
	{

	}



	int TaskSystem::GetCoreCount()
	{
		return s_cores;
	}
	int TaskSystem::GetThreadIndex()
	{
		// thread_local <=> static, but for a thread.
		// This means index will be initialized only once per thread.
		thread_local int index = s_counter++;
		return index;
	}
	tf::Executor& TaskSystem::GetExecutor()
	{
		return s_executor;
	}
	void TaskSystem::RunAndWait(tf::Taskflow& taskflow)
	{
		s_executor.run(taskflow).wait();
	}
	void TaskSystem::ResetThreadIndices()
	{
		// No need to reset except when you expect number of threads in parallel zone to be less.
		s_counter = 0;
	}
}