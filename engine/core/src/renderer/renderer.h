#pragma once
#include "commonBufferUsage.h"
#include "commonLighting.h"
#include "commonPipelineState.h"
#include "commonRendererCreateInfo.h"
#include "commonTextureFormat.h"
#include "commonTextureUsage.h"
#include "emberCoreExport.h"
#include "emberMath.h"
#include "texture2d.h"
#include <array>
#include <memory>
#include <filesystem>



// Forward decleration:
namespace emberBackendInterface
{
	class IBuffer;
	class IComputeShader;
	class IDescriptorSetBinding;
	class IMaterial;
	class IMesh;
	class IRenderer;
	class ITexture;
}



namespace emberEngine
{
	// Forward declerations:
	class Buffer;
	class Material;
	class Mesh;
	class ShaderProperties;

	

	class EMBER_CORE_API Renderer
	{
		// Friends:
		friend class Buffer;
		friend class ComputeShader;
		friend class Material;
		friend class MaterialManager;
		friend class Mesh;
		friend class ShaderProperties;
		friend class Texture2d;
		friend class TextureCube;

	private: // Members:
		static bool s_isInitialized;
		static std::unique_ptr<emberBackendInterface::IRenderer> s_pIRenderer;
		static std::array<Float4x4, 6> s_pointLightRotationMatrices;
		static emberBackendInterface::IRenderer* GetInterfaceHandle();

	public: // Methods:
		// Initialization/Clear:
		static void Init(emberBackendInterface::IRenderer* pIRenderer);
		static void Clear();

		// Main render loop:
		static void RenderFrame();

		// Lightsources:
		static void AddDirectionalLight(const Float3& direction, float intensity, const Float3& color, emberCommon::ShadowType shadowType, const Float4x4& worldToClipMatrix);
		static void AddPositionalLight(const Float3& position, float intensity, const Float3& color, emberCommon::ShadowType shadowType, float blendStart, float blendEnd, const Float4x4& worldToClipMatrix);

		// Draw mesh:
		static void DrawMesh(const Mesh& mesh, const Material& material, ShaderProperties& shaderProperties, const Float4x4& localToWorldMatrix, bool receiveShadows = true, bool castShadows = true);
		static ShaderProperties DrawMesh(const Mesh& mesh, const Material& material, const Float4x4& localToWorldMatrix, bool receiveShadows = true, bool castShadows = true);
		static void DrawGizmo(const Mesh& mesh, const Material& material, ShaderProperties& shaderProperties, const Float4x4& localToWorldMatrix);
		static ShaderProperties DrawGizmo(const Mesh& mesh, const Material& material, const Float4x4& localToWorldMatrix);

		// Draw instanced:
		static void DrawInstanced(uint32_t instanceCount, Buffer& instanceBuffer, const Mesh& mesh, const Material& material, ShaderProperties& shaderProperties, const Float4x4& localToWorldMatrix, bool receiveShadows = true, bool castShadows = true);
		static ShaderProperties DrawInstanced(uint32_t instanceCount, Buffer& instanceBuffer, const Mesh& mesh, const Material& material, const Float4x4& localToWorldMatrix, bool receiveShadows = true, bool castShadows = true);
		static void DrawInstanced(uint32_t instanceCount, const Mesh& mesh, const Material& material, ShaderProperties& shaderProperties, const Float4x4& localToWorldMatrix, bool receiveShadows = true, bool castShadows = true);
		static ShaderProperties DrawInstanced(uint32_t instanceCount, const Mesh& mesh, const Material& material, const Float4x4& localToWorldMatrix, bool receiveShadows = true, bool castShadows = true);

		// Getters:
		static const uint32_t GetShadowMapResolution();
		static const Uint2 GetSurfaceExtent();
		static const Float4x4& GetPointLightRotationMatrix(int faceIndex);
		static Texture2d GetRenderTexture();
		static Texture2d GetGizmoTexture();
		static float GetDepthBiasConstantFactor();
		static float GetDepthBiasClamp();
		static float GetDepthBiasSlopeFactor();
		static uint32_t GetFrameIndex();
		static bool IsFrameFinished(uint32_t frameIndex);

		// Setters:
		static void SetActiveCamera(const Float3& position, const Float4x4& viewMatrix, const Float4x4& projectionMatrix);
		static void SetDepthBiasConstantFactor(float depthBiasConstantFactor);
		static void SetDepthBiasClamp(float depthBiasClamp);
		static void SetDepthBiasSlopeFactor(float depthBiasSlopeFactor);

		// Functionallity forwarding:
		static void CollectGarbage();
		static void WaitDeviceIdle();
		static void WaitForFrameFinished(uint32_t frameIndex);

		// Debugging:
		static void DumpVmaBufferAllocations();
		static void DumpVmaImageAllocations();

	private: // Methods:
		// Gpu resource factories:
		static emberBackendInterface::IBuffer* CreateBuffer(uint32_t count, uint32_t elementSize, emberCommon::BufferUsage usage);
		//static emberBackendInterface::ITexture* CreateTexture1d(int width, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data);
		static emberBackendInterface::ITexture* CreateTexture2d(int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data);
		//static emberBackendInterface::ITexture* CreateTexture3d(int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data);
		static emberBackendInterface::ITexture* CreateTextureCube(int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data);
		static emberBackendInterface::IComputeShader* CreateComputeShader(const std::string& name, const std::filesystem::path& computeSpv);
		static emberBackendInterface::IMaterial* CreateForwardMaterial(emberCommon::RenderMode renderMode, const std::string& name, uint32_t renderQueue, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv);
		static emberBackendInterface::IMaterial* CreateShadowMaterial(const std::string& name, const std::filesystem::path& vertexSpv);
		static emberBackendInterface::IMesh* CreateMesh(const std::string& name);
		static emberBackendInterface::IDescriptorSetBinding* CreateDrawCallDescriptorSetBinding(emberBackendInterface::IMaterial* pIMaterial);

		// Delete all constructors:
		Renderer() = delete;
		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer(Renderer&&) = delete;
		Renderer& operator=(Renderer&&) = delete;
		~Renderer() = delete;
	};
}