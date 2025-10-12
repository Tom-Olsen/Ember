#include "editor.h"
#include "iEditor.h"



namespace emberEngine
{
	// Static members:
	bool Editor::s_isInitialized = false;
	std::unique_ptr<emberBackendInterface::IEditor> Editor::s_pIEditor;



	void Editor::Init(emberBackendInterface::IEditor* pIEditor)
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;
		s_pIEditor = std::unique_ptr<emberBackendInterface::IEditor>(pIEditor);
	}
	void Editor::Clear()
	{
		s_isInitialized = false;
	}
}