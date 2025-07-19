#include "settings.h"
#include "depthBiasEditorWindow.h"
#include "fpsEditorWindow.h"



namespace emberEngine
{
	// Constructor/Destructor:
	Settings::Settings()
	{
		m_depthBiasEditorWindow = std::make_unique<DepthBiasEditorWindow>();
		m_fpsEditorWindow = std::make_unique<FpsEditorWindow>();
	}
	Settings::~Settings()
	{

	}



	// Overrides:
	const std::string Settings::ToString() const
	{
		return "Settings";
	}
}