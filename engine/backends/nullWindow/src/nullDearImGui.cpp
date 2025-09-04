#include "nullDearImGui.h"



namespace emberBackend
{
	void NullDearImGuiInstanceExtensionsLoader::AddExtensions(std::vector<const char*>& instanceExtensions) const
	{

	}



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
	std::tuple<float, float> NullDearImGui::GetWindowSize()
	{
		return std::tuple(0, 0);
	}
	std::tuple<float, float> NullDearImGui::GetContentRegionalAvail()
	{
		return std::tuple(0, 0);
	}
	std::tuple<float, float> NullDearImGui::GetCursorPos()
	{
		return std::tuple(0, 0);
	}
	void NullDearImGui::SetCursorPos(float localPosX, float localPosY)
	{

	}
	void NullDearImGui::Image(uintptr_t textureID, float imageWidth, float imageHeight, float u0, float v0, float u1, float v1)
	{

	}
}