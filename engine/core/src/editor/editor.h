#pragma once
#include <memory>


// Forward declerations:
namespace emberBackendInterface
{
	class IEditor;
}



namespace emberEngine
{
	class Editor
	{
	private: // Members:
		static bool s_isInitialized;
		static std::unique_ptr<emberBackendInterface::IEditor> s_pIEditor;

	public: // Methods:
		//Initialization/Clear:
		static void Init(emberBackendInterface::IEditor* pIEditor);
		static void Clear();

	private: // Methods:
		// Delete all constructors:
		Editor() = delete;
		Editor(const Editor&) = delete;
		Editor& operator=(const Editor&) = delete;
		Editor(Editor&&) = delete;
		Editor& operator=(Editor&&) = delete;
		~Editor() = delete;
	};
}