#pragma once
#include "bufferMember.h"
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>



namespace emberBufferLayout
{
	// Represents a bX binding.
	struct BufferLayout
	{
	private: // Members:
		std::string m_name;
		uint32_t m_size;	// in bytes, derived from added members.

	private: // Members:
		std::vector<BufferMember> m_members;
		std::unordered_map<std::string, uint32_t> m_indexMap;

	public: // Methods:
		// Constructor:
		BufferLayout(const std::string& name);

		// Copyable:
		BufferLayout(const BufferLayout& other) = default;
		BufferLayout& operator=(const BufferLayout& other) = default;

		// Movable:
		BufferLayout(BufferLayout&& other) noexcept = default;
		BufferLayout& operator=(BufferLayout&& other) noexcept = default;

		// Getters:
		uint32_t GetSize() const;
		[[nodiscard]] const BufferMember* GetMember(uint32_t index) const;
		[[nodiscard]] const BufferMember* GetMember(const std::string& name) const;
		[[nodiscard]] bool IsLayoutCompatible(const BufferLayout& other) const;

		// Setters:
		void AddMember(const BufferMember& member);
		void ReserveMembers(size_t count);

		// Debugging:
		std::string ToString(int indent = 0) const;
	};

	// Example:
	// struct Foo { int a; float b; };
	// cbuffer DataBuffer : register(b1)
	// {
	//     Foo foo;
	//     float4 c;
	//     float4 d[2];
	// };
	// BufferLayout bufferLayout(...);
	// bufferLayout.m_members = {foo, c, d}
	// foo.m_members = {a,b}
	// d.m_members = {d0,d1}
}