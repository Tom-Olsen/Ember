#pragma once
#include <memory>



// Forward decleration:
namespace emberBackendInterface
{
	class IRenderer;
}



namespace emberEngine
{
	// Forward declerations:
	struct RendererCreateInfo;



	class Renderer
	{
	private: // Members:
		std::unique_ptr<emberBackendInterface::IRenderer> m_pIRenderer;

	public: // Methods:
		// Initialization/Ceanup:
		Renderer(const RendererCreateInfo& createInfo);
		~Renderer();

		// Non-copyable:
		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		// Non-movable:
		Renderer(Renderer&& other) noexcept = default;
		Renderer& operator=(Renderer&& other) noexcept = default;

		void RenderFrame();
	};
}