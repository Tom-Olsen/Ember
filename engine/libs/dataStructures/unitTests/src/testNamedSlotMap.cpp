#include "namedSlotMap.h"
#include <gtest/gtest.h>
#include <cstdint>
#include <limits>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>



namespace
{
	struct TestId
	{
		uint32_t index;
		uint32_t generation;
	};
	struct WrongIndexId
	{
		uint64_t index;
		uint32_t generation;
	};
	struct MissingGenerationId
	{
		uint32_t index;
	};
	struct ThrowOnCopy
	{
		ThrowOnCopy() = default;
		ThrowOnCopy(const ThrowOnCopy&)
		{
			throw std::runtime_error("Copy failed.");
		}
		ThrowOnCopy(ThrowOnCopy&&) noexcept = default;
	};
}

static_assert(emberDataStructures::SlotMapId<TestId>);
static_assert(!emberDataStructures::SlotMapId<WrongIndexId>);
static_assert(!emberDataStructures::SlotMapId<MissingGenerationId>);

template<typename TypeInput>
concept CanAddStringValue = requires(emberDataStructures::NamedSlotMap<TestId, std::string>& store, TypeInput&& value)
{
	store.Add("value", std::forward<TypeInput>(value));
};

static_assert(CanAddStringValue<std::string&>);
static_assert(CanAddStringValue<std::string>);
static_assert(!CanAddStringValue<const char*>);



using emberDataStructures::NamedSlotMap;

TEST(NamedSlotMap, InvalidatesRemovedIdsAndReusesIndices)
{
	NamedSlotMap<TestId, std::unique_ptr<int>> store;
	TestId first = store.Add("first", std::make_unique<int>(7));
	TestId firstCopy = first;

	ASSERT_NE(store.TryGetValue(first), nullptr);
	EXPECT_EQ(**store.TryGetValue(first), 7);
	ASSERT_TRUE(store.TryGetName(first).has_value());
	EXPECT_EQ(*store.TryGetName(first), "first");
	EXPECT_EQ(store.Find("first").generation, first.generation);

	std::optional<std::unique_ptr<int>> removed = store.Remove(first);
	ASSERT_TRUE(removed.has_value());
	EXPECT_EQ(**removed, 7);
	EXPECT_EQ(store.TryGetValue(firstCopy), nullptr);
	EXPECT_FALSE(store.TryGetName(firstCopy).has_value());
	EXPECT_EQ(store.Remove(firstCopy), std::nullopt);

	TestId second = store.Add("second", std::make_unique<int>(9));
	EXPECT_EQ(second.index, first.index);
	EXPECT_NE(second.generation, first.generation);
	EXPECT_EQ(store.TryGetValue(firstCopy), nullptr);
	EXPECT_EQ(**store.TryGetValue(second), 9);
}

TEST(NamedSlotMap, RejectsDuplicateNamesWithoutChangingTheStore)
{
	NamedSlotMap<TestId, int> store;
	TestId first = store.Add("same", 1);

	TestId duplicate = store.Add("same", 2);
	EXPECT_EQ(duplicate.index, std::numeric_limits<uint32_t>::max());
	EXPECT_EQ(duplicate.generation, std::numeric_limits<uint32_t>::max());
	EXPECT_EQ(*store.TryGetValue(first), 1);
	EXPECT_EQ(store.GetActiveIds().size(), 1);
	TestId next = store.Add("next", 3);
	EXPECT_EQ(next.index, 1);
	EXPECT_EQ(*store.TryGetValue(next), 3);
}

TEST(NamedSlotMap, KeepsOldIdsInvalidAcrossRemovalAndNewInsertions)
{
	NamedSlotMap<TestId, int> store;
	TestId oldId = store.Add("old", 1);
	std::string name = *store.TryGetName(oldId);
	store.Remove(oldId);

	for (int i = 0; i < 128; ++i)
		store.Add("new" + std::to_string(i), i);

	EXPECT_EQ(name, "old");
	EXPECT_EQ(store.TryGetValue(oldId), nullptr);
	EXPECT_FALSE(store.TryGetName(oldId).has_value());
	EXPECT_EQ(store.GetActiveIds().size(), 128);
}

TEST(NamedSlotMap, RestoresTheSlotAfterValueConstructionFails)
{
	NamedSlotMap<TestId, ThrowOnCopy> store;
	ThrowOnCopy value;
	TestId failed = store.Add("first", value);

	EXPECT_EQ(failed.index, std::numeric_limits<uint32_t>::max());
	EXPECT_EQ(failed.generation, std::numeric_limits<uint32_t>::max());
	EXPECT_TRUE(store.GetActiveIds().empty());

	TestId valid = store.Add("first", ThrowOnCopy());
	EXPECT_EQ(valid.index, 0);
	EXPECT_EQ(valid.generation, 1);
	EXPECT_NE(store.TryGetValue(valid), nullptr);
}

TEST(NamedSlotMap, CopiesAStoredValueBeforeGrowingSlots)
{
	NamedSlotMap<TestId, std::string> store;
	TestId source = store.Add("source", std::string(1000, 'x'));

	for (int i = 0; i < 128; ++i)
	{
		const std::string* pSourceValue = store.TryGetValue(source);
		ASSERT_NE(pSourceValue, nullptr);
		TestId copy = store.Add("copy" + std::to_string(i), *pSourceValue);
		ASSERT_NE(store.TryGetValue(copy), nullptr);
		EXPECT_EQ(*store.TryGetValue(copy), std::string(1000, 'x'));
	}
}