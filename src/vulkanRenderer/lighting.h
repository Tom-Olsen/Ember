#ifndef __INCLUDE_GUARD_lighting_h__
#define __INCLUDE_GUARD_lighting_h__



namespace emberEngine
{
	// Forward declarations:

	class Lighting
	{
	private: // Members
		static bool s_isInitialized;

	public: // Methods:
		static void Init();
		static void Clear();
		
	private: // Methods:
		// Delete all constructors:
		Lighting() = delete;
		Lighting(const Lighting&) = delete;
		Lighting& operator=(const Lighting&) = delete;
		~Lighting() = delete;
	};
}



#endif // __INCLUDE_GUARD_lighting_h__