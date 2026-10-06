#pragma once
#include "indexAllocator.h"
#include <concepts>
#include <cstdint>
#include <limits>
#include <optional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>



namespace emberDataStructures
{
	template<typename TypeId>
	concept SlotMapId = requires(TypeId id)
	{
		requires std::same_as<decltype(id.index), uint32_t>;
		requires std::same_as<decltype(id.generation), uint32_t>;
		{ TypeId{ uint32_t{}, uint32_t{} } } noexcept;
	};

	// TypeId has an index and generation. Their maximum values mean invalid.
	template<SlotMapId TypeId, typename TypeValue>
	class NamedSlotMap
	{
	// Static assertions:
		static_assert(std::is_nothrow_move_constructible_v<TypeValue>);
		static_assert(std::is_nothrow_destructible_v<TypeValue>);

	private: // Structs:
		// Reusable entry
		struct Slot
		{
			uint32_t generation = 1;
			std::string name;
			std::optional<TypeValue> value;	// allows decommissioned slot to stay in vector for reuse, without an asigned resource.
		};

	private: // Members:
		IndexAllocator m_indices;	// allocates slot indices and recycles them when slots are removed.
		std::vector<Slot> m_slots;	// active and retired slots.
		std::unordered_map<std::string, uint32_t> m_nameToIndex;	// for retrieving a slot by name.

	public: // Methods:
		// Insertion/removal:
		template<typename TypeName, typename TypeNewValue>	// accept TypeValue lvalues and rvalues, copying or moving when supported.
		requires std::same_as<std::remove_cvref_t<TypeNewValue>, TypeValue> && std::constructible_from<TypeValue, TypeNewValue&&>
		TypeId Add(TypeName&& name, TypeNewValue&& value)
		{
			constexpr uint32_t invalidIndex = std::numeric_limits<uint32_t>::max();
			constexpr uint32_t invalidGeneration = std::numeric_limits<uint32_t>::max();
			uint32_t index = invalidIndex;
			try
			{
				// Slot with given name already exists:
				std::string entryName(std::forward<TypeName>(name));
				if (m_nameToIndex.contains(entryName))
					return TypeId{ invalidIndex, invalidGeneration };

				// Get first free index:
				TypeValue entryValue(std::forward<TypeNewValue>(value));
				index = m_indices.Allocate();

				// Create new slot entry:
				if (index == m_slots.size())
					m_slots.emplace_back();

				// Reuse existing slot entry:
				Slot& slot = m_slots[index];
				slot.value.emplace(std::move(entryValue));
				slot.name = std::move(entryName);
				m_nameToIndex.emplace(slot.name, index);
				return TypeId{ index, slot.generation };
			}
			catch (...)
			{
				// Adding failed:
				if (index != invalidIndex)
				{
					if (index < m_slots.size())
					{
						m_slots[index].value.reset();
						m_slots[index].name.clear();
					}
					m_indices.Free(index);
				}
				return TypeId{ invalidIndex, invalidGeneration };
			}
		}
		std::optional<TypeValue> Remove(TypeId id)
		{
			// Invalid entry:
			TypeValue* pValue = TryGetValue(id);
			if (pValue == nullptr)
				return std::nullopt;

			// Valid entry:
			Slot& slot = m_slots[id.index];
			std::optional<TypeValue> removedValue(std::move(*pValue));
			m_nameToIndex.erase(slot.name);
			slot.value.reset();
			slot.name.clear();
			++slot.generation;
			if (slot.generation != std::numeric_limits<uint32_t>::max())
				m_indices.Free(id.index);
			return removedValue;
		}

		// Getters:
		TypeId Find(const std::string& name) const
		{
			auto iterator = m_nameToIndex.find(name);
			if (iterator == m_nameToIndex.end())
				return TypeId{ std::numeric_limits<uint32_t>::max(), std::numeric_limits<uint32_t>::max() };
			uint32_t index = iterator->second;
			return TypeId{ index, m_slots[index].generation };
		}
		TypeValue* TryGetValue(TypeId id)
		{
			if (id.index >= m_slots.size())
				return nullptr;
			Slot& slot = m_slots[id.index];
			return slot.generation == id.generation && slot.value ? &*slot.value : nullptr;
		}
		const TypeValue* TryGetValue(TypeId id) const
		{
			if (id.index >= m_slots.size())
				return nullptr;
			const Slot& slot = m_slots[id.index];
			return slot.generation == id.generation && slot.value ? &*slot.value : nullptr;
		}
		std::optional<std::string> TryGetName(TypeId id) const
		{
			if (TryGetValue(id) == nullptr)
				return std::nullopt;
			return m_slots[id.index].name;
		}
		std::vector<TypeId> GetActiveIds() const
		{
			std::vector<TypeId> ids;
			ids.reserve(m_nameToIndex.size());
			for (uint32_t index = 0; index < m_slots.size(); ++index)
			{
				const Slot& slot = m_slots[index];
				if (slot.value)
					ids.push_back(TypeId{ index, slot.generation });
			}
			return ids;
		}
	};
}