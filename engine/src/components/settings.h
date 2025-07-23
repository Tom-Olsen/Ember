#ifndef __INCLUDE_GUARD_settings_h__
#define __INCLUDE_GUARD_settings_h__
#include "emberEngine.h"



namespace emberEngine
{
	// Forward declarations:
	class DepthBiasEditorWindow;
	class FpsEditorWindow;



	class Settings : public Component
	{
	private: // Members:
		std::unique_ptr<DepthBiasEditorWindow> m_depthBiasEditorWindow;
		std::unique_ptr<FpsEditorWindow> m_fpsEditorWindow;

	public: // Methods:
		Settings();
		~Settings();

		// Overrides:
		const std::string ToString() const override;

	private: // Methods:
		void UpdateProjectionMatrix();
	};
}



#endif // __INCLUDE_GUARD_settings_h__