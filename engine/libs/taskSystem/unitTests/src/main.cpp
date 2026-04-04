#include "parallelThreadPool.h"
#include "profiler.h"
#include <chrono>
#include <iostream>
#include <random>
#include <thread>



// This is not a real unit test setup.
// Instead it is a simple example that creates a test.json output for chrome://tracing/ 
// which should reflect the exact structure of this program.



void HeavyWork()
{
    PROFILE_FUNCTION();
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> floatDist(0.0, 1.0);
    
    double sum = 0;
    int count = 1'000'000 * (floatDist(gen) + 1.0);
    for (int i = 0; i < count; i++)
        sum += floatDist(gen);
    sum /= count;

    std::cout << "Thread " << ParallelThreadPool::GetThreadIndex()
              << "/" << ParallelThreadPool::GetThreadCount()
              << " of pool " << ParallelThreadPool::GetPoolId()
              << " sum = " << sum << std::endl;
}
void HeavyHeavyWork()
{
    PROFILE_FUNCTION();
    ParallelThreadPool innerPool(2, "InnerPool" + std::to_string(ParallelThreadPool::GetThreadIndex()));
    innerPool.RunJob(2, &HeavyWork);
}



int main()
{
    Profiler::Session::Get().Start("test");
    {
        PROFILE_FUNCTION();
        
        ParallelThreadPool poolA(4, "PoolA");
        ParallelThreadPool poolB(4, "PoolB");
        
        // Launch both pools concurrently in separate std::threads:
        std::thread t1([&] { poolA.RunJob(2, &HeavyWork); });
        std::thread t2([&] { poolB.RunJob(3, &HeavyWork); });
        
        // Wait for both to finish:
        t1.join();
        t2.join();
        
        // You can still use pools sequentially if needed:
        poolA.RunJob(16, &HeavyWork);
        
        // Or run the function stand alone:
        HeavyWork();
        
        poolA.RunJob(2, &HeavyHeavyWork);
    }
    Profiler::Session::Get().End();
}