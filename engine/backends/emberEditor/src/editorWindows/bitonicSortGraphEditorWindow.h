#pragma once
//#include "editorWindow.h"
//#include "dearImGuiFlags.h"
//#include "editor.h"
//#include "bitonicSortGraph.h"
//
//
//
//namespace emberEngine
//{
//	struct BitonicSortGraphEditorWindow : EditorWindow
//	{
//	private:
//		BitonicSortGraph* m_pScript;
//		int count;
//		float width;
//		float height;
//		float lineWidth;
//		float lineGap;
//		float blockGap;
//
//	public:
//		BitonicSortGraphEditorWindow(BitonicSortGraph* pScript)
//		{
//			m_name = "Bitonic Sort Graph";
//			m_ID = 0;
//			m_windowFlags = DearImGuiWindowFlag_None;
//			m_wantCaptureEvents = true;
//			m_nameID = m_name + "##" + std::to_string(m_ID);
//			m_pScript = pScript;
//			GetData();
//		}
//
//		void Render() override
//		{
//			GetData();
//			Editor::InputInt("Count:", &count);
//			Editor::InputFloat("Width:", &width, 0.1f, 1.0f, "%.8f");
//			Editor::InputFloat("Height:", &height, 0.1f, 1.0f, "%.8f");
//			Editor::InputFloat("Line Width:", &lineWidth, 0.1f, 1.0f, "%.8f");
//			Editor::InputFloat("Line Gap:", &lineGap, 0.1f, 1.0f, "%.8f");
//			Editor::InputFloat("Block Gap:", &blockGap, 0.1f, 1.0f, "%.8f");
//			SetData();
//		}
//
//	private:
//		void GetData()
//		{
//			count = m_pScript->count;
//			width = m_pScript->width;
//			height = m_pScript->height;
//			lineWidth = m_pScript->lineWidth;
//			lineGap = m_pScript->lineGap;
//			blockGap = m_pScript->blockGap;
//		}
//		void SetData()
//		{
//			m_pScript->count = count;
//			m_pScript->width = width;
//			m_pScript->height = height;
//			m_pScript->lineWidth = lineWidth;
//			m_pScript->lineGap = lineGap;
//			m_pScript->blockGap = blockGap;
//		}
//	};
//}