#include "logger.h"
#include <gtest/gtest.h>



struct TestEnvironment : testing::Environment
{
    void SetUp() override
    {
        emberLogger::Logger::Init();
    }

    void TearDown() override
    {

    }
};



int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    testing::AddGlobalTestEnvironment(new TestEnvironment);
    testing::FLAGS_gtest_catch_exceptions = false;
    testing::FLAGS_gtest_output = "console";
    return RUN_ALL_TESTS();
}
