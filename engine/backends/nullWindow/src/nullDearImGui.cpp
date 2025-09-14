#include "nullDearImGui.h"



namespace nullWindowBackend
{
	// Constructor/Destructor:
	NullDearImGui::NullDearImGui()
	{

	}
	NullDearImGui::~NullDearImGui()
	{

	}



	// Move semantics:
	NullDearImGui::NullDearImGui(NullDearImGui&& other) noexcept
	{

	}
	NullDearImGui& NullDearImGui::operator=(NullDearImGui&& other) noexcept
	{
		return *this;
	}



	// Public methods:
	void NullDearImGui::Update()
	{

	}
	void NullDearImGui::ProcessEvent(void* pEvent)
	{

	}
	void NullDearImGui::Render(VkCommandBuffer vkCommandBuffer)
	{

	}



	// Getters:
	bool NullDearImGui::WantCaptureKeyboard()
	{
		return false;
	}
	bool NullDearImGui::WantCaptureMouse()
	{
		return false;
	}
	uintptr_t NullDearImGui::GetTextureID(VkImageView vkImageView, VkSampler vkSampler)
	{
		return 0;
	}



	// Wrappers:
	bool NullDearImGui::IsWindowFocused(emberEngine::DearImGuiFocusedFlags flags)
	{
		return false;
	}
	bool NullDearImGui::Begin(const char* name, bool* pOpen, emberEngine::DearImGuiWindowFlags flags)
	{
		return true;
	}
	void NullDearImGui::End()
	{

	}
	void NullDearImGui::PushID(const char* str_id)
	{

	}
	void NullDearImGui::PopID()
	{

	}
	Float2 NullDearImGui::GetWindowSize()
	{
		return Float2{ 0, 0 };
	}
	Float2  NullDearImGui::GetContentRegionalAvail()
	{
		return Float2{ 0, 0 };
	}
	Float2  NullDearImGui::GetCursorPos()
	{
		return Float2{ 0, 0 };
	}
	void NullDearImGui::SetCursorPos(float localPosX, float localPosY)
	{

	}
	void NullDearImGui::Image(uintptr_t textureID, float imageWidth, float imageHeight, float u0, float v0, float u1, float v1)
	{

	}
}