#include "lighting.h"



namespace emberEngine
{
	// Static members:
	bool Lighting::s_isInitialized = false;



	// Initialization/Cleanup:
	void Lighting::Init()
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;
	}
	void Lighting::Clear()
	{

	}
}