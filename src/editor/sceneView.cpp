#include "sceneView.h"
#include "mathf.h"
#include <imgui.h>
using namespace emberEngine;



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
		MultiSelect();



		// Specify default position/size:
		const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 350, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

		ImGui::PopItemWidth();
		ImGui::End();
	}


	// Extra functions to add deletion support to ImGuiSelectionBasicStorage
	struct ExampleSelectionWithDeletion : ImGuiSelectionBasicStorage
	{
		// Find which item should be Focused after deletion.
		// Call _before_ item submission. Retunr an index in the before-deletion item list, your item loop should call SetKeyboardFocusHere() on it.
		// The subsequent ApplyDeletionPostLoop() code will use it to apply Selection.
		// - We cannot provide this logic in core Dear ImGui because we don't have access to selection data.
		// - We don't actually manipulate the ImVector<> here, only in ApplyDeletionPostLoop(), but using similar API for consistency and flexibility.
		// - Important: Deletion only works if the underlying ImGuiID for your items are stable: aka not depend on their index, but on e.g. item id/ptr.
		// FIXME-MULTISELECT: Doesn't take account of the possibility focus target will be moved during deletion. Need refocus or scroll offset.
		int ApplyDeletionPreLoop(ImGuiMultiSelectIO* ms_io, int items_count)
		{
			if (Size == 0)
				return -1;

			// If focused item is not selected...
			const int focused_idx = (int)ms_io->NavIdItem;  // Index of currently focused item
			if (ms_io->NavIdSelected == false)  // This is merely a shortcut, == Contains(adapter->IndexToStorage(items, focused_idx))
			{
				ms_io->RangeSrcReset = true;    // Request to recover RangeSrc from NavId next frame. Would be ok to reset even when NavIdSelected==true, but it would take an extra frame to recover RangeSrc when deleting a selected item.
				return focused_idx;             // Request to focus same item after deletion.
			}

			// If focused item is selected: land on first unselected item after focused item.
			for (int idx = focused_idx + 1; idx < items_count; idx++)
				if (!Contains(GetStorageIdFromIndex(idx)))
					return idx;

			// If focused item is selected: otherwise return last unselected item before focused item.
			for (int idx = mathf::Min(focused_idx, items_count) - 1; idx >= 0; idx--)
				if (!Contains(GetStorageIdFromIndex(idx)))
					return idx;

			return -1;
		}

		// Rewrite item list (delete items) + update selection.
		// - Call after EndMultiSelect()
		// - We cannot provide this logic in core Dear ImGui because we don't have access to your items, nor to selection data.
		template<typename ITEM_TYPE>
		void ApplyDeletionPostLoop(ImGuiMultiSelectIO* ms_io, ImVector<ITEM_TYPE>& items, int item_curr_idx_to_select)
		{
			// Rewrite item list (delete items) + convert old selection index (before deletion) to new selection index (after selection).
			// If NavId was not part of selection, we will stay on same item.
			ImVector<ITEM_TYPE> new_items;
			new_items.reserve(items.Size - Size);
			int item_next_idx_to_select = -1;
			for (int idx = 0; idx < items.Size; idx++)
			{
				if (!Contains(GetStorageIdFromIndex(idx)))
					new_items.push_back(items[idx]);
				if (item_curr_idx_to_select == idx)
					item_next_idx_to_select = new_items.Size - 1;
			}
			items.swap(new_items);

			// Update selection
			Clear();
			if (item_next_idx_to_select != -1 && ms_io->NavIdSelected)
				SetItemSelected(GetStorageIdFromIndex(item_next_idx_to_select), true);
		}
	};


	void SceneView::MultiSelect()
	{
		static const char* ExampleNames[] =
		{
			"Artichoke", "Arugula", "Asparagus", "Avocado", "Bamboo Shoots", "Bean Sprouts", "Beans", "Beet", "Belgian Endive", "Bell Pepper",
			"Bitter Gourd", "Bok Choy", "Broccoli", "Brussels Sprouts", "Burdock Root", "Cabbage", "Calabash", "Capers", "Carrot", "Cassava",
			"Cauliflower", "Celery", "Celery Root", "Celcuce", "Chayote", "Chinese Broccoli", "Corn", "Cucumber"
		};

		// See multi select widgets im imgui_demo.cpp (multiple versions).
		// Demonstrate dynamic item list + deletion support using the BeginMultiSelect/EndMultiSelect API.
		// In order to support Deletion without any glitches you need to:
		// - (1) If items are submitted in their own scrolling area, submit contents size SetNextWindowContentSize() ahead of time to prevent one-frame readjustment of scrolling.
		// - (2) Items needs to have persistent ID Stack identifier = ID needs to not depends on their index. PushID(index) = KO. PushID(item_id) = OK. This is in order to focus items reliably after a selection.
		// - (3) BeginXXXX process
		// - (4) Focus process
		// - (5) EndXXXX process
		{
			// Storing items data separately from selection data.
			// (you may decide to store selection data inside your item (aka intrusive storage) if you don't need multiple views over same items)
			// Use a custom selection.Adapter: store item identifier in Selection (instead of index)
			static ImVector<ImGuiID> items;
			static ExampleSelectionWithDeletion selection;
			selection.UserData = (void*)&items;
			selection.AdapterIndexToStorageId = [](ImGuiSelectionBasicStorage* self, int idx) { ImVector<ImGuiID>* p_items = (ImVector<ImGuiID>*)self->UserData; return (*p_items)[idx]; }; // Index -> ID

			// Initialize default list with 50 items + button to add/remove items.
			static ImGuiID items_next_id = 0;
			if (items_next_id == 0)
				for (ImGuiID n = 0; n < 50; n++)
					items.push_back(items_next_id++);

			// (1) Extra to support deletion: Submit scrolling range to avoid glitches on deletion
			const float items_height = ImGui::GetTextLineHeightWithSpacing();
			ImGui::SetNextWindowContentSize(ImVec2(0.0f, items.Size * items_height));

			ImGuiMultiSelectFlags flags = ImGuiMultiSelectFlags_ClearOnEscape | ImGuiMultiSelectFlags_BoxSelect1d;
			ImGuiMultiSelectIO* ms_io = ImGui::BeginMultiSelect(flags, selection.Size, items.Size);
			selection.ApplyRequests(ms_io);

			const bool want_delete = ImGui::Shortcut(ImGuiKey_Delete, ImGuiInputFlags_Repeat) && (selection.Size > 0);
			const int item_curr_idx_to_focus = want_delete ? selection.ApplyDeletionPreLoop(ms_io, items.Size) : -1;

			for (int n = 0; n < items.Size; n++)
			{
				const ImGuiID item_id = items[n];
				char label[64];
				sprintf(label, "%s", ExampleNames[item_id % IM_ARRAYSIZE(ExampleNames)]);

				bool item_is_selected = selection.Contains(item_id);
				ImGui::SetNextItemSelectionUserData(n);
				ImGui::Selectable(label, item_is_selected);
				if (item_curr_idx_to_focus == n)
					ImGui::SetKeyboardFocusHere(-1);
			}

			// Apply multi-select requests
			ms_io = ImGui::EndMultiSelect();
			selection.ApplyRequests(ms_io);
			if (want_delete)
				selection.ApplyDeletionPostLoop(ms_io, items, item_curr_idx_to_focus);
		}
	}
}