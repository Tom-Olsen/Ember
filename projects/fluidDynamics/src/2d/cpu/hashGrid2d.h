#pragma once
#include "emberMath.h"
#include <vector>



namespace fluidDynamics
{
	class HashGrid2d
	{
	private: // Members:
		size_t m_particleCount;
		size_t m_size;
		size_t m_prime0 = 15823;
		size_t m_prime1 = 9737333;
		std::vector<uint32_t> m_cellKeys;			// m_cellKeys[particleIndex] = cell key (hash % Count) of given particle.
		std::vector<uint32_t> m_startIndices;		// m_startIndices[cellKey] = first entry in cellKeys that is in that cell, else uint32_t(-1).
		std::vector<std::size_t> m_sortPermutation;	// permutations for sorting.

	public: // Methods:
		HashGrid2d(int particleCount);
		~HashGrid2d();

		// Getters:
		size_t GetSize() const;
		uint32_t GetCellKey(int particleIndex) const;
		uint32_t GetStartIndex(int cellKey) const;

		// Getters by cell:
		uint32_t GetCellKey(Int2 cell) const;
		uint32_t GetStartIndex(Int2 cell) const;

		// Hashing:
		Int2 Cell(Float2 position, float radius) const;
		int64_t CellHash(Int2 cell) const;
		uint32_t CellKey(int cellHash) const;

		// Main hash grid mechanism:
		void UpdateGrid(std::vector<Float2>& positions, float radius);
		void ApplySort(std::vector<Float2>& data) const;
	};
}