#ifndef __INCLUDE_GUARD_infinitGrid2d_h__
#define __INCLUDE_GUARD_infinitGrid2d_h__
#include "emberMath.h"
#include <vector>



namespace emberEngine
{
	class InfinitGrid2d
	{
	private: // Members:
		uint32_t m_particleCount;
		uint32_t m_prime0 = 15823;
		uint32_t m_prime1 = 9737333;
		std::vector<uint32_t> m_cellKeys;		// m_cellKeys[particleIndex] = cell key (hash % Count) of given particle.
		std::vector<uint32_t> m_startIndices;	// m_startIndices[cellKey] = first entry in cellKeys that is in that cell, else uint32_t(-1).
		std::vector<std::size_t> m_permutation;	// permutations for sorting.

	public: // Methods:
		InfinitGrid2d(int particleCount);
		~InfinitGrid2d();

		Int2 Cell(Float2 position, float radius);
		uint32_t CellHash(Int2 cell);
		uint32_t CellKey(uint32_t cellHash);
		uint32_t GetCellKey(int particleIndex);
		uint32_t GetStartIndex(int cellKey);
		void UpdateGrid(std::vector<Float2>& positions, float radius);
		void ApplySort(std::vector<Float2>& data);
	};
}



#endif // __INCLUDE_GUARD_infinitGrid2d_h__