#include "renderer.h"
#include "buffer.h"
#include "emberTime.h"
#include "iRenderer.h"
#include "mesh.h"
#include "meshManager.h"
#include "material.h"
#include "materialManager.h"
#include "shaderProperties.h"
#include "texture.h"
#include "texture2d.h"
#include "window.h"



namespace emberEngine
{
	// Static members:
	bool Renderer::s_isInitialized = false;
	std::unique_ptr<emberBackendInterface::IRenderer> Renderer::s_pIRenderer;
	std::array<Float4x4, 6> Renderer::s_pointLightRotationMatrices;
	emberBackendInterface::IRenderer* Renderer::GetInterfaceHandle()
	{
		return s_pIRenderer.get();
	}



	// Public Methods:
	// Initialization/Cleanup:
	void Renderer::Init(emberBackendInterface::IRenderer* pIRenderer)
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		s_pointLightRotationMatrices[0] = Float4x4::identity;
		s_pointLightRotationMatrices[1] = Float4x4::RotateY( math::pi2);
		s_pointLightRotationMatrices[2] = Float4x4::RotateY( math::pi );
		s_pointLightRotationMatrices[3] = Float4x4::RotateY(-math::pi2);
		s_pointLightRotationMatrices[4] = Float4x4::RotateX( math::pi2);
		s_pointLightRotationMatrices[5] = Float4x4::RotateX(-math::pi2);

		s_pIRenderer = std::unique_ptr<emberBackendInterface::IRenderer>(pIRenderer);
	}
	void Renderer::Clear()
	{
		s_pIRenderer.reset();
		s_isInitialized = false;
	}



	// Main render loop:
	void Renderer::RenderFrame()
	{
		s_pIRenderer->RenderFrame(Time::GetTime(), Time::GetDeltaTime());
	}



	// Lightsources:
	void Renderer::AddDirectionalLight(const Float3& direction, float intensity, const Float3& color, emberCommon::ShadowType shadowType, const Float4x4& worldToClipMatrix)
	{
		s_pIRenderer->AddDirectionalLight(direction, intensity, color, shadowType, worldToClipMatrix);
	}
	void Renderer::AddPositionalLight(const Float3& position, float intensity, const Float3& color, emberCommon::ShadowType shadowType, float blendStart, float blendEnd, const Float4x4& worldToClipMatrix)
	{
		s_pIRenderer->AddPositionalLight(position, intensity, color, shadowType, blendStart, blendEnd, worldToClipMatrix);
	}



	// Draw mesh:
	void Renderer::DrawMesh(Mesh& mesh, Material& material, ShaderProperties& shaderProperties, const Float4x4& localToWorldMatrix, bool receiveShadows, bool castShadows)
	{
		if (material.GetName() == "errorMaterial")
			receiveShadows = castShadows = false;
		s_pIRenderer->DrawMesh(mesh.GetInterfaceHandle(), material.GetInterfaceHandle(), shaderProperties.GetInterfaceHandle(), localToWorldMatrix, receiveShadows, castShadows);
	}
	void Renderer::DrawMesh(Mesh& mesh, Material& material, ShaderProperties& shaderProperties, const Float3& position, const Float3x3& rotationMatrix, const Float3& scale, bool receiveShadows, bool castShadows)
	{
		Float4x4 localToWorldMatrix = Float4x4::TRS(position, rotationMatrix, scale);
		DrawMesh(mesh, material, shaderProperties, localToWorldMatrix, receiveShadows, castShadows);
	}
	void Renderer::DrawMesh(Mesh& mesh, Material& material, ShaderProperties& shaderProperties, const Float3& position, const Float3x3& rotationMatrix, float scale, bool receiveShadows, bool castShadows)
	{
		DrawMesh(mesh, material, shaderProperties, position, rotationMatrix, Float3(scale), receiveShadows, castShadows);
	}
	ShaderProperties Renderer::DrawMesh(Mesh& mesh, Material& material, const Float4x4& localToWorldMatrix, bool receiveShadows, bool castShadows)
	{
		if (material.GetName() == "errorMaterial")
			receiveShadows = castShadows = false;
		ShaderProperties shaderProperties = ShaderProperties(s_pIRenderer->DrawMesh(mesh.GetInterfaceHandle(), material.GetInterfaceHandle(), localToWorldMatrix, receiveShadows, castShadows));
		shaderProperties.SetOwnerShip(false);
		return shaderProperties;
	}
	ShaderProperties Renderer::DrawMesh(Mesh& mesh, Material& material, const Float3& position, const Float3x3& rotationMatrix, const Float3& scale, bool receiveShadows, bool castShadows)
	{
		Float4x4 localToWorldMatrix = Float4x4::TRS(position, rotationMatrix, scale);
		return DrawMesh(mesh, material, localToWorldMatrix, receiveShadows, castShadows);
	}
	ShaderProperties Renderer::DrawMesh(Mesh& mesh, Material& material, const Float3& position, const Float3x3& rotationMatrix, float scale, bool receiveShadows, bool castShadows)
	{
		return DrawMesh(mesh, material, position, rotationMatrix, Float3(scale), receiveShadows, castShadows);
	}



	// Draw instanced:
	void Renderer::DrawInstanced(uint32_t instanceCount, Buffer& instanceBuffer, Mesh& mesh, Material& material, ShaderProperties& shaderProperties, const Float4x4& localToWorldMatrix, bool receiveShadows, bool castShadows)
	{
		if (material.GetName() == "errorMaterial")
			receiveShadows = castShadows = false;
		s_pIRenderer->DrawInstanced(instanceCount, instanceBuffer.GetInterfaceHandle(), mesh.GetInterfaceHandle(), material.GetInterfaceHandle(), shaderProperties.GetInterfaceHandle(), localToWorldMatrix, receiveShadows, castShadows);
	}
	void Renderer::DrawInstanced(uint32_t instanceCount, Buffer& instanceBuffer, Mesh& mesh, Material& material, ShaderProperties& shaderProperties, const Float3& position, const Float3x3& rotationMatrix, const Float3& scale, bool receiveShadows, bool castShadows)
	{
		Float4x4 localToWorldMatrix = Float4x4::TRS(position, rotationMatrix, scale);
		return DrawInstanced(instanceCount, instanceBuffer, mesh, material, shaderProperties, localToWorldMatrix, receiveShadows, castShadows);
	}
	void Renderer::DrawInstanced(uint32_t instanceCount, Buffer& instanceBuffer, Mesh& mesh, Material& material, ShaderProperties& shaderProperties, const Float3& position, const Float3x3& rotationMatrix, float scale, bool receiveShadows, bool castShadows)
	{
		return DrawInstanced(instanceCount, instanceBuffer, mesh, material, shaderProperties, position, rotationMatrix, Float3(scale), receiveShadows, castShadows);
	}
	ShaderProperties Renderer::DrawInstanced(uint32_t instanceCount, Buffer& instanceBuffer, Mesh& mesh, Material& material, const Float4x4& localToWorldMatrix, bool receiveShadows, bool castShadows)
	{
		if (material.GetName() == "errorMaterial")
			receiveShadows = castShadows = false;
		ShaderProperties shaderProperties = ShaderProperties(s_pIRenderer->DrawInstanced(instanceCount, instanceBuffer.GetInterfaceHandle(), mesh.GetInterfaceHandle(), material.GetInterfaceHandle(), localToWorldMatrix, receiveShadows, castShadows));
		shaderProperties.SetOwnerShip(false);
		return shaderProperties;
	}
	ShaderProperties Renderer::DrawInstanced(uint32_t instanceCount, Buffer& instanceBuffer, Mesh& mesh, Material& material, const Float3& position, const Float3x3& rotationMatrix, const Float3& scale, bool receiveShadows, bool castShadows)
	{
		Float4x4 localToWorldMatrix = Float4x4::TRS(position, rotationMatrix, scale);
		return DrawInstanced(instanceCount, instanceBuffer, mesh, material, localToWorldMatrix, receiveShadows, castShadows);
	}
	ShaderProperties Renderer::DrawInstanced(uint32_t instanceCount, Buffer& instanceBuffer, Mesh& mesh, Material& material, const Float3& position, const Float3x3& rotationMatrix, float scale, bool receiveShadows, bool castShadows)
	{
		return DrawInstanced(instanceCount, instanceBuffer, mesh, material, position, rotationMatrix, Float3(scale), receiveShadows, castShadows);
	}



	// Draw basic geometry:
	void Renderer::DrawLineSegment(const Float3& start, const Float3& end, float width, const Float4& color, bool receiveShadows, bool castShadows)
	{
		Float3 direction = end - start;
		float length = direction.Length();
		Float3 position = start + 0.5f * direction;
		Float3x3 rotationMatrix = Float3x3::RotateFromTo(Float3::up, direction);
		Float3 scale = Float3(width, width, length);
	
		ShaderProperties shaderProperties = DrawMesh(MeshManager::GetMesh("zylinderFlat"), MaterialManager::GetMaterial("defaultMaterial"), position, rotationMatrix, scale, receiveShadows, castShadows);
		shaderProperties.SetValue("SurfaceProperties", "diffuseColor", color);
	}
	void Renderer::DrawCube(const Float3& position, float width, const Float4& color, bool receiveShadows, bool castShadows)
	{
		ShaderProperties shaderProperties = DrawMesh(MeshManager::GetMesh("unitCube"), MaterialManager::GetMaterial("defaultMaterial"), position, Float3x3::identity, width, receiveShadows, castShadows);
		shaderProperties.SetValue("SurfaceProperties", "diffuseColor", color);
	}
	void Renderer::DrawSphere(const Float3& position, float radius, const Float4& color, bool receiveShadows, bool castShadows)
	{
		ShaderProperties shaderProperties = DrawMesh(MeshManager::GetMesh("cubeSphere"), MaterialManager::GetMaterial("defaultMaterial"), position, Float3x3::identity, radius, receiveShadows, castShadows);
		shaderProperties.SetValue("SurfaceProperties", "diffuseColor", color);
	}
	void Renderer::DrawArrow(const Float3& position, const Float3& direction, float size, const Float4& color, bool receiveShadows, bool castShadows)
	{
		Float3x3 rotationMatrix = Float3x3::RotateFromTo(Float3::forward, direction);
		ShaderProperties shaderProperties = DrawMesh(MeshManager::GetMesh("arrowFlat"), MaterialManager::GetMaterial("defaultMaterial"), position, rotationMatrix, Float3(size), receiveShadows, castShadows);
		shaderProperties.SetValue("SurfaceProperties", "diffuseColor", color);
	}
	void Renderer::DrawFrustum(const Float4x4& localToWorldMatrix, const Float4x4& projectionMatrix, float width, const Float4& color, bool receiveShadows, bool castShadows)
	{
		// Corner positions in normalized device coordinates:
		Float4 cornerPoints[8] =
		{
			Float4(-1, -1, 0, 1),
			Float4(-1, -1, 1, 1),
			Float4(-1,  1, 0, 1),
			Float4(-1,  1, 1, 1),
			Float4(1, -1, 0, 1),
			Float4(1, -1, 1, 1),
			Float4(1,  1, 0, 1),
			Float4(1,  1, 1, 1)
		};
	
		float det = projectionMatrix.Determinant();
		if (det == 0)
			return;
	
		Float4x4 invPerspectiveMatrix = projectionMatrix.Inverse(det);
		for (uint32_t i = 0; i < 8; i++)
		{
			// Corner positions in camera/local space:
			cornerPoints[i] = invPerspectiveMatrix * cornerPoints[i];
			cornerPoints[i] /= cornerPoints[i].w;
	
			// Corner positions in world space:
			cornerPoints[i] = localToWorldMatrix * cornerPoints[i];
		}
	
		// Draw corner points:
		for (uint32_t i = 0; i < 8; i++)
			DrawSphere(Float3(cornerPoints[i]), 2.0f * width, Float4::gray, receiveShadows, castShadows);
	
		// Draw horizontal lines:
		DrawLineSegment(Float3(cornerPoints[0]), Float3(cornerPoints[4]), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(cornerPoints[1]), Float3(cornerPoints[5]), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(cornerPoints[2]), Float3(cornerPoints[6]), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(cornerPoints[3]), Float3(cornerPoints[7]), width, color, receiveShadows, castShadows);
		// Draw vertical lines:
		DrawLineSegment(Float3(cornerPoints[0]), Float3(cornerPoints[2]), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(cornerPoints[1]), Float3(cornerPoints[3]), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(cornerPoints[4]), Float3(cornerPoints[6]), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(cornerPoints[5]), Float3(cornerPoints[7]), width, color, receiveShadows, castShadows);
		// Draw depth lines:
		DrawLineSegment(Float3(cornerPoints[0]), Float3(cornerPoints[1]), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(cornerPoints[2]), Float3(cornerPoints[3]), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(cornerPoints[4]), Float3(cornerPoints[5]), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(cornerPoints[6]), Float3(cornerPoints[7]), width, color, receiveShadows, castShadows);
	}
	void Renderer::DrawBounds(const Float4x4& localToWorldMatrix, const Bounds2d& bounds, float width, const Float4& color, bool receiveShadows, bool castShadows)
	{
		std::array<Float2, 4> cornerPoints = bounds.GetCorners();
	
		// Draw corner points:
		for (uint32_t i = 0; i < 4; i++)
		{
			Float3 pos = Float3(localToWorldMatrix * Float4(cornerPoints[i], 0.0f, 1.0f));
			DrawCube(pos, 2.0f * width, Float4::black, receiveShadows, castShadows);
		}
	
		// Draw lines:
		DrawLineSegment(Float3(localToWorldMatrix * Float4(cornerPoints[0], 0.0f, 1.0f)), Float3(localToWorldMatrix * Float4(cornerPoints[1], 0.0f, 1.0f)), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(localToWorldMatrix * Float4(cornerPoints[1], 0.0f, 1.0f)), Float3(localToWorldMatrix * Float4(cornerPoints[3], 0.0f, 1.0f)), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(localToWorldMatrix * Float4(cornerPoints[3], 0.0f, 1.0f)), Float3(localToWorldMatrix * Float4(cornerPoints[2], 0.0f, 1.0f)), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(localToWorldMatrix * Float4(cornerPoints[2], 0.0f, 1.0f)), Float3(localToWorldMatrix * Float4(cornerPoints[0], 0.0f, 1.0f)), width, color, receiveShadows, castShadows);
	}
	void Renderer::DrawBounds(const Float4x4& localToWorldMatrix, const Bounds& bounds, float width, const Float4& color, bool receiveShadows, bool castShadows)
	{
		std::array<Float3, 8> cornerPoints = bounds.GetCorners();
	
		// Draw corner points:
		for (uint32_t i = 0; i < 8; i++)
		{
			Float3 pos = Float3(localToWorldMatrix * Float4(cornerPoints[i], 1.0f));
			DrawCube(pos, 2.0f * width, Float4::black, receiveShadows, castShadows);
		}
	
		// Draw horizontal lines:
		DrawLineSegment(Float3(localToWorldMatrix * Float4(cornerPoints[0], 1.0f)), Float3(localToWorldMatrix * Float4(cornerPoints[4], 1.0f)), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(localToWorldMatrix * Float4(cornerPoints[1], 1.0f)), Float3(localToWorldMatrix * Float4(cornerPoints[5], 1.0f)), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(localToWorldMatrix * Float4(cornerPoints[2], 1.0f)), Float3(localToWorldMatrix * Float4(cornerPoints[6], 1.0f)), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(localToWorldMatrix * Float4(cornerPoints[3], 1.0f)), Float3(localToWorldMatrix * Float4(cornerPoints[7], 1.0f)), width, color, receiveShadows, castShadows);
		// Draw vertical lines:
		DrawLineSegment(Float3(localToWorldMatrix * Float4(cornerPoints[0], 1.0f)), Float3(localToWorldMatrix * Float4(cornerPoints[2], 1.0f)), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(localToWorldMatrix * Float4(cornerPoints[1], 1.0f)), Float3(localToWorldMatrix * Float4(cornerPoints[3], 1.0f)), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(localToWorldMatrix * Float4(cornerPoints[4], 1.0f)), Float3(localToWorldMatrix * Float4(cornerPoints[6], 1.0f)), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(localToWorldMatrix * Float4(cornerPoints[5], 1.0f)), Float3(localToWorldMatrix * Float4(cornerPoints[7], 1.0f)), width, color, receiveShadows, castShadows);
		// Draw depth lines:
		DrawLineSegment(Float3(localToWorldMatrix * Float4(cornerPoints[0], 1.0f)), Float3(localToWorldMatrix * Float4(cornerPoints[1], 1.0f)), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(localToWorldMatrix * Float4(cornerPoints[2], 1.0f)), Float3(localToWorldMatrix * Float4(cornerPoints[3], 1.0f)), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(localToWorldMatrix * Float4(cornerPoints[4], 1.0f)), Float3(localToWorldMatrix * Float4(cornerPoints[5], 1.0f)), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(localToWorldMatrix * Float4(cornerPoints[6], 1.0f)), Float3(localToWorldMatrix * Float4(cornerPoints[7], 1.0f)), width, color, receiveShadows, castShadows);
	}


	// Getters:
	const uint32_t Renderer::GetShadowMapResolution()
	{
		return s_pIRenderer->GetShadowMapResolution();
	}
	const Uint2 Renderer::GetSurfaceExtent()
	{
		return s_pIRenderer->GetSurfaceExtent();
	}
	const Float4x4& Renderer::GetPointLightRotationMatrix(int faceIndex)
	{
		return s_pointLightRotationMatrices[faceIndex];
	}
	Texture2d Renderer::GetRenderTexture()
	{
		return Texture2d(s_pIRenderer->GetRenderTexture(), false);
	}
	float Renderer::GetDepthBiasConstantFactor()
	{
		return s_pIRenderer->GetDepthBiasConstantFactor();
	}
	float Renderer::GetDepthBiasClamp()
	{
		return s_pIRenderer->GetDepthBiasClamp();
	}
	float Renderer::GetDepthBiasSlopeFactor()
	{
		return s_pIRenderer->GetDepthBiasSlopeFactor();
	}



	// Setters:
	void Renderer::SetActiveCamera(const Float3& position, const Float4x4& viewMatrix, const Float4x4& projectionMatrix)
	{
		s_pIRenderer->SetActiveCamera(position, viewMatrix, projectionMatrix);
	}
	void Renderer::SetDepthBiasConstantFactor(float depthBiasConstantFactor)
	{
		s_pIRenderer->SetDepthBiasConstantFactor(depthBiasConstantFactor);
	}
	void Renderer::SetDepthBiasClamp(float depthBiasClamp)
	{
		s_pIRenderer->SetDepthBiasClamp(depthBiasClamp);
	}
	void Renderer::SetDepthBiasSlopeFactor(float depthBiasSlopeFactor)
	{
		s_pIRenderer->SetDepthBiasSlopeFactor(depthBiasSlopeFactor);
	}



	// Functionallity forwarding:
	void Renderer::CollectGarbage()
	{
		s_pIRenderer->CollectGarbage();
	}
	void Renderer::WaitDeviceIdle()
	{
		s_pIRenderer->WaitDeviceIdle();
	}

	// Gpu resource factories:
	emberBackendInterface::IBuffer* Renderer::CreateBuffer(uint32_t count, uint32_t elementSize, const std::string& name, emberCommon::BufferUsage usage)
	{
		return s_pIRenderer->CreateBuffer(count, elementSize, name, usage);
	}
	//static emberBackendInterface::ITexture* Renderer::CreateTexture1d(const std::string& name, int width, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data)
	//{
	//
	//}
	emberBackendInterface::ITexture* Renderer::CreateTexture2d(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data)
	{
		return s_pIRenderer->CreateTexture2d(name, width, height, format, usage, data);
	}
	//static emberBackendInterface::ITexture* Renderer::CreateTexture3d(const std::string& name, int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data)
	//{
	//
	//}
	emberBackendInterface::ITexture* Renderer::CreateTextureCube(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data)
	{
		return s_pIRenderer->CreateTextureCube(name, width, height, format, usage, data);
	}
	emberBackendInterface::IComputeShader* Renderer::CreateComputeShader(const std::string& name, const std::filesystem::path& computeSpv)
	{
		return s_pIRenderer->CreateComputeShader(name, computeSpv);
	}
	emberBackendInterface::IMaterial* Renderer::CreateMaterial(emberCommon::MaterialType type, const std::string& name, uint32_t renderQueue, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv)
	{
		return s_pIRenderer->CreateMaterial(type, name, renderQueue, vertexSpv, fragmentSpv);
	}
	emberBackendInterface::IMesh* Renderer::CreateMesh(const std::string& name)
	{
		return s_pIRenderer->CreateMesh(name);
	}
	emberBackendInterface::IShaderProperties* Renderer::CreateShaderProperties(emberBackendInterface::IComputeShader* pIComputeShader)
	{
		return s_pIRenderer->CreateShaderProperties(pIComputeShader);
	}
	emberBackendInterface::IShaderProperties* Renderer::CreateShaderProperties(emberBackendInterface::IMaterial* pIMaterial)
	{
		return s_pIRenderer->CreateShaderProperties(pIMaterial);
	}
}