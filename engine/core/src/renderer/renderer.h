#pragma once
#include "commonBufferUsage.h"
#include "commonLighting.h"
#include "commonMaterialType.h"
#include "commonRendererCreateInfo.h"
#include "commonTextureFormat.h"
#include "commonTextureUsage.h"
#include "emberMath.h"
#include <array>
#include <memory>
#include <filesystem>



// Forward decleration:
namespace emberBackendInterface
{
	class IBuffer;
	class IComputeShader;
	class IMaterial;
	class IMesh;
	class IRenderer;
	class IShaderProperties;
	class ITexture;
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
		friend class Buffer;
		friend class ComputeShader;
		friend class Material;
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
		static void SetActiveCamera(const Float3& position, const Float4x4& viewMatrix, const Float4x4& projectionMatrix);

		// Functionallity forwarding:
		static void CollectGarbage();
		static void WaitDeviceIdle();

	private: // Methods
		// Gpu resource factories:
		static emberBackendInterface::IBuffer* CreateBuffer(uint32_t count, uint32_t elementSize, const std::string& name, emberCommon::BufferUsage usage);
		//static emberBackendInterface::ITexture* CreateTexture1d(const std::string& name, int width, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data);
		static emberBackendInterface::ITexture* CreateTexture2d(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data);
		//static emberBackendInterface::ITexture* CreateTexture3d(const std::string& name, int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data);
		static emberBackendInterface::ITexture* CreateTextureCube(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data);
		static emberBackendInterface::IComputeShader* CreateComputeShader(const std::string& name, const std::filesystem::path& computeSpv);
		static emberBackendInterface::IMaterial* CreateMaterial(emberCommon::MaterialType type, const std::string& name, uint32_t renderQueue, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv);
		static emberBackendInterface::IMesh* CreateMesh(const std::string& name);
		static emberBackendInterface::IShaderProperties* CreateShaderProperties(emberBackendInterface::IComputeShader* pIComputeShader);
		static emberBackendInterface::IShaderProperties* CreateShaderProperties(emberBackendInterface::IMaterial* pIMaterial);

		// Delete all constructors:
		Renderer() = delete;
		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer(Renderer&&) = delete;
		Renderer& operator=(Renderer&&) = delete;
		~Renderer() = delete;
	};
}