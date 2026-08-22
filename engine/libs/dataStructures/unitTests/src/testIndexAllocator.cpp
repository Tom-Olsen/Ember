#include "indexAllocator.h"
#include <gtest/gtest.h>
#include <stdexcept>



using namespace emberDataStructures;



TEST(IndexAllocator, AllocatesSequentialIndices)
{
	IndexAllocator indexAllocator;
	EXPECT_EQ(indexAllocator.Allocate(), 0);
	EXPECT_EQ(indexAllocator.Allocate(), 1);
	EXPECT_EQ(indexAllocator.Allocate(), 2);
	EXPECT_TRUE(indexAllocator.IsAllocated(0));
	EXPECT_TRUE(indexAllocator.IsAllocated(1));
	EXPECT_TRUE(indexAllocator.IsAllocated(2));
}



TEST(IndexAllocator, ReusesFreedIndices)
{
	IndexAllocator indexAllocator;
	uint32_t index0 = indexAllocator.Allocate();
	uint32_t index1 = indexAllocator.Allocate();
	uint32_t index2 = indexAllocator.Allocate();

	indexAllocator.Free(index1);
	indexAllocator.Free(index0);
	EXPECT_FALSE(indexAllocator.IsAllocated(index0));
	EXPECT_FALSE(indexAllocator.IsAllocated(index1));
	EXPECT_TRUE(indexAllocator.IsAllocated(index2));
	EXPECT_EQ(indexAllocator.Allocate(), index0);
	EXPECT_EQ(indexAllocator.Allocate(), index1);
	EXPECT_EQ(indexAllocator.Allocate(), 3);
}



TEST(IndexAllocator, RejectsInvalidFree)
{
	IndexAllocator indexAllocator;
	uint32_t index = indexAllocator.Allocate();
	indexAllocator.Free(index);

	EXPECT_THROW(indexAllocator.Free(index), std::runtime_error);
	EXPECT_THROW(indexAllocator.Free(index + 1), std::out_of_range);
}