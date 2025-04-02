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
		std::vector<uint32_t> m_particleLookup;
		std::vector<uint32_t> m_startIndices;	// maps cellKeys to index of first particle in particleLookUp.

	public: // Methods:
		InfinitGrid2d(int particleCount);
		~InfinitGrid2d();

		void UpdateGrid(std::vector<Float2>& positions, std::vector<Float2>& velocities, float radius);
		Int2 Cell(Float2 position, float radius);
		uint32_t CellHash(Int2 cell);
		uint32_t CellKey(uint32_t cellHash);
	};
}



#endif // __INCLUDE_GUARD_infinitGrid2d_h__