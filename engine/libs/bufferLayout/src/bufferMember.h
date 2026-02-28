#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>



namespace emberBufferLayout
{
	// Represents a member of a UniformBufferBlock, which may have sub-members (structs/arrays):
	struct BufferMember
	{
	public: // Members:
		std::string m_name;
		uint32_t m_offset;	// in bytes.
		uint32_t m_size;	// in bytes.

	private: // Members:
		std::vector<BufferMember> m_subMembers;
		std::unordered_map<std::string, uint32_t> m_indexMap;

	public: // Methods:
		// Constructor:
		BufferMember(const std::string& name, uint32_t offset, uint32_t size);

		// Copyable:
		BufferMember(const BufferMember& other) = default;
		BufferMember& operator=(const BufferMember& other) = default;

		// Movable:
		BufferMember(BufferMember&& other) noexcept = default;
		BufferMember& operator=(BufferMember&& other) noexcept = default;

		// Getters:
		const std::string& GetName() const;
		uint32_t GetOffset() const;
		uint32_t GetSize() const;
		[[nodiscard]] const BufferMember* GetSubMember(uint32_t index) const;
		[[nodiscard]] const BufferMember* GetSubMember(const std::string& name) const;
		[[nodiscard]] bool IsLayoutCompatible(const BufferMember& other) const;

		// Setters:
		void AddSubMember(const BufferMember& subMember);

		// Debugging:
		std::string ToString(int indent = 0) const;
	};
}