#pragma once
#include "commonBufferUsage.h"
#include "commonLighting.h"
#include "commonRendererCreateInfo.h"
#include "commonTextureFormat.h"
#include "commonTextureUsage.h"
#include "drawData.h"
#include "emberCoreExport.h"
#include "emberMath.h"
#include "texture2d.h"
#include <array>
#include <filesystem>
#include <memory>



// Forward decleration:
namespace emberBackendInterface
{
	class IBuffer;
	class IComputeShader;
	class IDescriptorSetBinding;
	class IGpuResourceFactory;
	class IMaterial;
	class IMesh;
	class IRenderer;
	class ITexture;
}



namespace emberCore
{
	// Forward declarations:
	class Buffer;
	class ComputeShaderManager;
	class Material;
	class Mesh;
	class CallProperties;

	

	class EMBER_CORE_API Renderer
	{
		// Friends:
		friend class Buffer;
		friend class ComputeShaderManager;
		friend class MaterialManager;
		friend class Mesh;
		friend class CallProperties;
		friend class Texture2d;
		friend class Texture3d;
		friend class TextureCube;

	private: // Members:
		static bool s_isInitialized;
		static std::unique_ptr<emberBackendInterface::IGpuResourceFactory> s_pIGpuResourceFactory;
		static std::unique_ptr<emberBackendInterface::IRenderer> s_pIRenderer;
		static std::array<Float4x4, 6> s_pointLightRotationMatrices;
		static emberBackendInterface::IRenderer* GetInterfaceHandle();

	public: // Methods:
		// Initialization/Clear:
		static void Init(emberBackendInterface::IRenderer* pIRenderer, emberBackendInterface::IGpuResourceFactory* pIGpuResourceFactory);
		static void Clear();

		// Main render loop:
		static void RenderFrame();

		// Lightsources:
		static void AddDirectionalLight(const Float3& direction, float intensity, const Float3& color, emberCommon::ShadowType shadowType, const Float4x4& worldToClipMatrix);
		static void AddPositionalLight(const Float3& position, float intensity, const Float3& color, emberCommon::ShadowType shadowType, float blendStart, float blendEnd, const Float4x4& worldToClipMatrix);

		// Draw calls:
		static void DrawOutline(const Float4x4& localToWorldMatrix, const Mesh& mesh);
		static CallProperties DrawMesh(const DrawData& drawData);
		static void DrawMesh(const DrawData& drawData, CallProperties& callProperties);
		static CallProperties DrawGizmo(const DrawData& drawData);
		static void DrawGizmo(const DrawData& drawData, CallProperties& callProperties);

		// Getters:
		static bool TryGetDirectionalLight(emberCommon::DirectionalLight& directionalLight, uint32_t index);
		static bool TryGetPositionalLight(emberCommon::PositionalLight& positionalLight, uint32_t index);
		static const uint32_t GetShadowMapResolution();
		static const Uint2 GetSurfaceExtent();
		static const Float4x4& GetPointLightRotationMatrix(int faceIndex);
		static Texture2d GetFinalRenderTexture();
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
		// Draw mesh helpers:
		static void ValidateDrawData(const DrawData& drawData);
		static emberCommon::CullMode ResolveCullMode(const DrawData& drawData, emberBackendInterface::IMaterial* pIMaterial);
		
		// Set default bindings:
		static void SetModelData(const Float4x4& localToWorldMatrix, CallProperties& callProperties);
		static void SetInstanceBuffer(const DrawData& drawData, CallProperties& callProperties);

		// Shadow draws calls:
		static CallProperties DrawMeshShadow(const DrawData& drawData);
		static void DrawMeshShadow(const DrawData& drawData, CallProperties& callProperties);

		// Material resolution:
		static emberBackendInterface::IMaterial* ResolveSurfaceMaterial(const Material& material);
		static emberBackendInterface::IMaterial* ResolveGizmoMaterial(const Material& material);

		// Gpu resource factories:
		static emberBackendInterface::IBuffer* CreateBuffer(uint32_t count, uint32_t elementSize, emberCommon::BufferUsage usage);
		//static emberBackendInterface::ITexture* CreateTexture1d(int width, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data);
		static emberBackendInterface::ITexture* CreateTexture2d(int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data);
		static emberBackendInterface::ITexture* CreateTexture3d(int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data);
		static emberBackendInterface::ITexture* CreateTextureCube(int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data);
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