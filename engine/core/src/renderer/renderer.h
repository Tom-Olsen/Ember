#pragma once
#include "commonLighting.h"
#include "commonRendererCreateInfo.h"
#include "emberMath.h"
#include <array>
#include <memory>



// Forward decleration:
namespace emberBackendInterface
{
	class IDearImGui;
	class IRenderer;
}



namespace emberEngine
{
	// Forward declerations:
	class Buffer;
	class Material;
	class Mesh;
	class ShaderProperties;



	class Renderer
	{
		// Friends:
		friend class DearImGui;

	private: // Members:
		static std::unique_ptr<emberBackendInterface::IRenderer> s_pIRenderer;
		static emberBackendInterface::IRenderer* GetInterfaceHandle();
		static std::array<Float4x4, 6> s_pointLightRotationMatrices;

	public: // Methods:
		// Initialization/Clear:
		static void Init(const emberCommon::RendererCreateInfo& createInfo);
		static void Clear();
		static void WaitDeviceIdle();

		// Main render loop:
		static void RenderFrame();
		static void CollectGarbage();

		// Lightsources:
		static void AddDirectionalLight(const Float3& direction, float intensity, const Float3& color, emberCommon::ShadowType shadowType, const Float4x4& worldToClipMatrix);
		static void AddPositionalLight(const Float3& position, float intensity, const Float3& color, emberCommon::ShadowType shadowType, float blendStart, float blendEnd, const Float4x4& worldToClipMatrix);

		// Draw mesh:
		static void DrawMesh(Mesh& mesh, Material& material, ShaderProperties& shaderProperties, const Float4x4& localToWorldMatrix, bool receiveShadows = true, bool castShadows = true);
		static void DrawMesh(Mesh& mesh, Material& material, ShaderProperties& shaderProperties, const Float3& position, const Float3x3& rotationMatrix = Float3x3::identity, const Float3& scale = Float3::one, bool receiveShadows = true, bool castShadows = true);
		static void DrawMesh(Mesh& mesh, Material& material, ShaderProperties& shaderProperties, const Float3& position, const Float3x3& rotationMatrix = Float3x3::identity, float scale = 1.0f, bool receiveShadows = true, bool castShadows = true);
		static ShaderProperties DrawMesh(Mesh& mesh, Material& material, const Float4x4& localToWorldMatrix, bool receiveShadows = true, bool castShadows = true);
		static ShaderProperties DrawMesh(Mesh& mesh, Material& material, const Float3& position, const Float3x3& rotationMatrix = Float3x3::identity, const Float3& scale = Float3::one, bool receiveShadows = true, bool castShadows = true);
		static ShaderProperties DrawMesh(Mesh& mesh, Material& material, const Float3& position, const Float3x3& rotationMatrix = Float3x3::identity, float scale = 1.0f, bool receiveShadows = true, bool castShadows = true);

		// Draw instanced:
		static void DrawInstanced(uint32_t instanceCount, Buffer& instanceBuffer, Mesh& mesh, Material& material, ShaderProperties& shaderProperties, const Float4x4& localToWorldMatrix, bool receiveShadows = true, bool castShadows = true);
		static void DrawInstanced(uint32_t instanceCount, Buffer& instanceBuffer, Mesh& mesh, Material& material, ShaderProperties& shaderProperties, const Float3& position, const Float3x3& rotationMatrix = Float3x3::identity, const Float3& scale = Float3::one, bool receiveShadows = true, bool castShadows = true);
		static void DrawInstanced(uint32_t instanceCount, Buffer& instanceBuffer, Mesh& mesh, Material& material, ShaderProperties& shaderProperties, const Float3& position, const Float3x3& rotationMatrix = Float3x3::identity, float scale = 1.0f, bool receiveShadows = true, bool castShadows = true);
		static ShaderProperties DrawInstanced(uint32_t instanceCount, Buffer& instanceBuffer, Mesh& mesh, Material& material, const Float4x4& localToWorldMatrix, bool receiveShadows = true, bool castShadows = true);
		static ShaderProperties DrawInstanced(uint32_t instanceCount, Buffer& instanceBuffer, Mesh& mesh, Material& material, const Float3& position, const Float3x3& rotationMatrix = Float3x3::identity, const Float3& scale = Float3::one, bool receiveShadows = true, bool castShadows = true);
		static ShaderProperties DrawInstanced(uint32_t instanceCount, Buffer& instanceBuffer, Mesh& mesh, Material& material, const Float3& position, const Float3x3& rotationMatrix = Float3x3::identity, float scale = 1.0f, bool receiveShadows = true, bool castShadows = true);

		// Draw basic geometry:
		static void DrawLineSegment(const Float3& start, const Float3& end, float width, const Float4& color = Float4::one, bool receiveShadows = true, bool castShadows = true);
		static void DrawCube(const Float3& position, float width, const Float4& color = Float4::white, bool receiveShadows = true, bool castShadows = true);
		static void DrawSphere(const Float3& position, float radius, const Float4& color = Float4::white, bool receiveShadows = true, bool castShadows = true);
		static void DrawArrow(const Float3& position, const Float3& direction, float size = 1.0f, const Float4& color = Float4::white, bool receiveShadows = true, bool castShadows = true);
		static void DrawFrustum(const Float4x4& localToWorldMatrix, const Float4x4& projectionMatrix, float width = 0.1f, const Float4& color = Float4::white, bool receiveShadows = false, bool castShadows = false);
		static void DrawBounds(const Float4x4& localToWorldMatrix, const Bounds2d& bounds, float width = 0.1f, const Float4& color = Float4::white, bool receiveShadows = false, bool castShadows = false);
		static void DrawBounds(const Float4x4& localToWorldMatrix, const Bounds& bounds, float width = 0.1f, const Float4& color = Float4::white, bool receiveShadows = false, bool castShadows = false);

		// Getters:
		static const uint32_t GetShadowMapResolution();
		static const Uint2 GetSurfaceExtent();
		static const Float4x4& GetPointLightRotationMatrix(int faceIndex);

		// Setters:
		static void SetIComputeHandle();
		static void SetIDearImGuiHandle();
		static void SetActiveCamera(const Float3& position, const Float4x4& viewMatrix, const Float4x4& projectionMatrix);

	private: // Methods
		// Delete all constructors:
		Renderer() = delete;
		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer(Renderer&&) = delete;
		Renderer& operator=(Renderer&&) = delete;
		~Renderer() = delete;
	};
}