#include <gtest/gtest.h>
#include <iostream>



int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	::testing::FLAGS_gtest_catch_exceptions = false;
	::testing::FLAGS_gtest_output = "console";
	return RUN_ALL_TESTS();
}