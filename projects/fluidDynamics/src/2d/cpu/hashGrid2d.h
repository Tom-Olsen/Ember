#pragma once
#include "emberMath.h"
#include <vector>



namespace fluidDynamics
{
	class HashGrid2d
	{
	private: // Members:
		int m_particleCount;
		int m_prime0 = 15823;
		int m_prime1 = 9737333;
		std::vector<uint32_t> m_cellKeys;		// m_cellKeys[particleIndex] = cell key (hash % Count) of given particle.
		std::vector<uint32_t> m_startIndices;	// m_startIndices[cellKey] = first entry in cellKeys that is in that cell, else uint32_t(-1).
		std::vector<std::size_t> m_permutation;	// permutations for sorting.

	public: // Methods:
		HashGrid2d(int particleCount);
		~HashGrid2d();

		Int2 Cell(Float2 position, float radius);
		int CellHash(Int2 cell);
		uint32_t CellKey(int cellHash);
		uint32_t GetCellKey(int particleIndex);
		uint32_t GetStartIndex(int cellKey);
		void UpdateGrid(std::vector<Float2>& positions, float radius);
		void ApplySort(std::vector<Float2>& data);
	};
}