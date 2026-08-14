#pragma once
#include "commonBufferUsage.h"
#include "commonForwardRenderMode.h"
#include "commonGizmoRenderMode.h"
#include "commonLighting.h"
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
	class IMaterialShader;
	class IMesh;
	class IRenderer;
	class ITexture;
}



namespace emberCore
{
	// Forward declarations:
	class Buffer;
	class Material;
	class MaterialShaderManager;
	class Mesh;
	class ShaderProperties;

	

	class EMBER_CORE_API Renderer
	{
		// Friends:
		friend class Buffer;
		friend class ComputeShader;
		friend class MaterialManager;
		friend class MaterialShaderManager;
		friend class Mesh;
		friend class ShaderProperties;
		friend class Texture2d;
		friend class Texture3d;
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
        static void DrawOutline(const Mesh& mesh, const Float4x4& localToWorldMatrix);
		static void DrawMesh(const Mesh& mesh, const Material& material, ShaderProperties& shaderProperties, const Float4x4& localToWorldMatrix, bool receiveShadows = true, bool castShadows = true);
		static ShaderProperties DrawMesh(const Mesh& mesh, const Material& material, const Float4x4& localToWorldMatrix, bool receiveShadows = true, bool castShadows = true);
		static void DrawGizmo(const Mesh& mesh, const Material& material, ShaderProperties& shaderProperties, const Float4x4& localToWorldMatrix);
		static ShaderProperties DrawGizmo(const Mesh& mesh, const Material& material, const Float4x4& localToWorldMatrix);

		// Draw instanced:
        static void DrawOutlineInstanced(const Mesh& mesh, const Float4x4& localToWorldMatrix, uint32_t instanceCount);
		static void DrawMeshInstanced(uint32_t instanceCount, Buffer& instanceBuffer, const Mesh& mesh, const Material& material, ShaderProperties& shaderProperties, const Float4x4& localToWorldMatrix, bool receiveShadows = true, bool castShadows = true);
		static ShaderProperties DrawMeshInstanced(uint32_t instanceCount, Buffer& instanceBuffer, const Mesh& mesh, const Material& material, const Float4x4& localToWorldMatrix, bool receiveShadows = true, bool castShadows = true);
		static void DrawMeshInstanced(uint32_t instanceCount, const Mesh& mesh, const Material& material, ShaderProperties& shaderProperties, const Float4x4& localToWorldMatrix, bool receiveShadows = true, bool castShadows = true);
		static ShaderProperties DrawMeshInstanced(uint32_t instanceCount, const Mesh& mesh, const Material& material, const Float4x4& localToWorldMatrix, bool receiveShadows = true, bool castShadows = true);

		// Getters:
		static bool TryGetDirectionalLight(emberCommon::DirectionalLight& directionalLight, uint32_t index);
		static bool TryGetPositionalLight(emberCommon::PositionalLight& positionalLight, uint32_t index);
		static const uint32_t GetShadowMapResolution();
		static const Uint2 GetSurfaceExtent();
		static const Float4x4& GetPointLightRotationMatrix(int faceIndex);
		static Texture2d GetRenderTexture();
		static Texture2d GetGizmoTexture();
		static float GetDepthBiasConstantFactor();
		static float GetDepthBiasClamp();
		static float GetDepthBiasSlopeFactor();
		static const Float4& GetOutlineColor();
		static int GetOutlineThickness();
		static uint32_t GetFrameIndex();
		static bool IsFrameFinished(uint32_t frameIndex);

		// Setters:
		static void SetActiveCamera(const Float3& position, const Float4x4& viewMatrix, const Float4x4& projectionMatrix);
		static void SetDepthBiasConstantFactor(float depthBiasConstantFactor);
		static void SetDepthBiasClamp(float depthBiasClamp);
		static void SetDepthBiasSlopeFactor(float depthBiasSlopeFactor);
		static void SetOutlineColor(const Float4& outlineColor);
		static void SetOutlineThickness(int outlineThickness);

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
		static emberBackendInterface::ITexture* CreateTexture3d(int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data);
		static emberBackendInterface::ITexture* CreateTextureCube(int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data);
		static emberBackendInterface::IComputeShader* CreateComputeShader(const std::filesystem::path& computeSpv, const std::string& name);
		static emberBackendInterface::IMaterialShader* CreateOutlineMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name);
		static emberBackendInterface::IMaterialShader* CreateForwardMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name);
		static emberBackendInterface::IMaterialShader* CreateGizmoMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name);
		static emberBackendInterface::IMaterialShader* CreateShadowMaterialShader(const std::filesystem::path& vertexSpv, const std::string& name);
		static emberBackendInterface::IMaterialShader* CreatePresentMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name);
		static emberBackendInterface::IMaterial* CreateOutlineMaterial(emberBackendInterface::IMaterialShader* pMaterialShader, const std::string& name);
		static emberBackendInterface::IMaterial* CreateForwardMaterial(emberCommon::ForwardRenderMode renderMode, emberBackendInterface::IMaterialShader* pMaterialShader, const std::string& name);
		static emberBackendInterface::IMaterial* CreateGizmoMaterial(emberCommon::GizmoRenderMode renderMode, emberBackendInterface::IMaterialShader* pMaterialShader, const std::string& name);
		static emberBackendInterface::IMaterial* CreateShadowMaterial(emberBackendInterface::IMaterialShader* pMaterialShader, const std::string& name);
		static emberBackendInterface::IMaterial* CreatePresentMaterial(emberBackendInterface::IMaterialShader* pMaterialShader, const std::string& name);
		static emberBackendInterface::IMaterial* CloneForwardMaterial(emberBackendInterface::IMaterial* pSourceMaterial, const std::string& name);
		static emberBackendInterface::IMaterial* CloneGizmoMaterial(emberBackendInterface::IMaterial* pSourceMaterial, const std::string& name);
		static emberBackendInterface::IMaterial* CloneShadowMaterial(emberBackendInterface::IMaterial* pSourceMaterial, const std::string& name);
		static emberBackendInterface::IMesh* CreateMesh(const std::string& name);
		static emberBackendInterface::IDescriptorSetBinding* CreateDrawCallDescriptorSetBinding(emberBackendInterface::IMaterial* pIMaterial);
		
		// Gpu resource destruction:
		static void DestroyMaterial(emberBackendInterface::IMaterial* pMaterial);
		static void DestroyMaterialShader(emberBackendInterface::IMaterialShader* pMaterialShader);
		static void SetDefaultMaterials(const Material& outlineMaterial, const Material& defaultShadowMaterial, const Material& presentMaterial);

		// Delete all constructors:
		Renderer() = delete;
		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer(Renderer&&) = delete;
		Renderer& operator=(Renderer&&) = delete;
		~Renderer() = delete;
	};
}