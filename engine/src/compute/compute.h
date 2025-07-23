#ifndef __INCLUDE_GUARD_compute_h__
#define __INCLUDE_GUARD_compute_h__
#include "asyncCompute.h"
#include "immediateCompute.h"
#include "postRenderCompute.h"
#include "preRenderCompute.h"



namespace emberEngine
{
	class Compute
	{
	private: // Members
		static bool s_isInitialized;

	public: // Methods
		static void Init();
		static void Clear();

	private: // Methods
		// Delete all constructors:
		Compute() = delete;
		Compute(const Compute&) = delete;
		Compute& operator=(const Compute&) = delete;
		~Compute() = delete;
	};
}



#endif // __INCLUDE_GUARD_compute_h__