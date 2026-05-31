#pragma once
#include "emberMath.h"
#include <vector>



namespace fluidDynamics
{
	class HashGrid3d
	{
	private: // Members:
		size_t m_particleCount;
		size_t m_size;
		bool m_inverseValid;
		const size_t m_prime0 = 19349663;   // Not actually prime, but commonly used in spatial hashing.
		const size_t m_prime1 = 73856093;
		const size_t m_prime2 = 83492791;
		std::vector<uint32_t> m_cellKeys;				// m_cellKeys[particleIndex] = cell key (hash % Count) of given particle.
		std::vector<uint32_t> m_startIndices;			// m_startIndices[cellKey] = first entry in cellKeys that is in that cell, else uint32_t(-1).
		std::vector<size_t> m_sortPermutation;			// permutations for sorting.
		std::vector<size_t> m_inverseSortPermutation;	// permutation for unsorting.

	public: // Methods:
		HashGrid3d();
		HashGrid3d(int particleCount);
		~HashGrid3d();
		void Reallocate(int particleCount);

		// Getters:
		size_t GetSize() const;
		uint32_t GetCellKey(int particleIndex) const;
		uint32_t GetStartIndex(int cellKey) const;

		// Getters by cell:
		uint32_t GetCellKey(Int3 cell) const;
		uint32_t GetStartIndex(Int3 cell) const;

		// Hashing:
		Int3 Cell(Float3 position, float radius) const;
		uint32_t CellHash(Int3 cell) const;
		uint32_t CellKey(uint32_t cellHash) const;

		// Main hash grid mechanism:
		void UpdateGrid(std::vector<Float3>& positions, float radius);
		void ApplySort(std::vector<float>& data) const;
		void ApplySort(std::vector<Float3>& data) const;
		void UndoSort(std::vector<float>& data);
		void UndoSort(std::vector<Float3>& data);
	};
}