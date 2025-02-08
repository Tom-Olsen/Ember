#include "sceneView.h"
#include <imgui.h>



namespace emberEditor
{
	// Constructor/Destructor:
	SceneView::SceneView()
	{

	}
	SceneView::~SceneView()
	{

	}



	void SceneView::DrawWindow(bool* pOpen)
	{
		IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing Dear ImGui context. Refer to examples app!");
		IMGUI_CHECKVERSION();

		// Demonstrate the various window flags. Typically you would just use the default.
		static bool noTitlebar = false;
		static bool noScrollbar = false;
		static bool noMenu = false;
		static bool noMove = false;
		static bool noResize = false;
		static bool noCollapse = false;
		static bool noClose = false;
		static bool noNav = false;
		static bool noBackground = false;
		static bool noBring_to_front = false;
		static bool noDocking = false;
		static bool unsavedDocument = false;

		// Some window flags to play around with and test:
		ImGuiWindowFlags window_flags = 0;
		if (noTitlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
		if (noScrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
		if (!noMenu)           window_flags |= ImGuiWindowFlags_MenuBar;
		if (noMove)            window_flags |= ImGuiWindowFlags_NoMove;
		if (noResize)          window_flags |= ImGuiWindowFlags_NoResize;
		if (noCollapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
		if (noNav)             window_flags |= ImGuiWindowFlags_NoNav;
		if (noBackground)      window_flags |= ImGuiWindowFlags_NoBackground;
		if (noBring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
		if (noDocking)         window_flags |= ImGuiWindowFlags_NoDocking;
		if (unsavedDocument)   window_flags |= ImGuiWindowFlags_UnsavedDocument;
		if (noClose)           pOpen = NULL; // Don't pass our bool* to Begin

		// Main body of window starts here:
		if (!ImGui::Begin("Scene View", pOpen, window_flags))
		{
			// Early out if the window is collapsed, as an optimization.
			ImGui::End();
			return;
		}

		ImGui::PushItemWidth(ImGui::GetFontSize() * -12);
		ImGui::Text("Here is some example text! (%s) (%d)", IMGUI_VERSION, IMGUI_VERSION_NUM);
		ImGui::Spacing();
		MultiSelect();



		// Specify default position/size:
		const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 350, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

		ImGui::PopItemWidth();
		ImGui::End();
	}

	void SceneView::MultiSelect()
	{
		// See multi select widgets im imgui_demo.cpp (multiple versions).
	}
}