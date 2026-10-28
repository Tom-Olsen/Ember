#pragma once
#include "commonCullMode.h"
#include "emberCoreExport.h"
#include "emberMath.h"
#include <cstdint>



namespace emberCore
{
	// Forward declarations:
	class InstanceBuffer;
	class Material;
	class Mesh;



	struct EMBER_CORE_API DrawData
	{
	public: // Members:
		// Mandatory:
		Float4x4 localToWorldMatrix;
		const Mesh& mesh;
		const Material& material;

		// Optional:
		uint32_t instanceCount = 0;			// 0 disables instanced rendering.
		InstanceBuffer* pInstanceBuffer = nullptr;	// nullptr means no call-local instance buffer is bound.
		bool receiveShadows = true;			// true allows the surface to receive shadows.
		bool castShadows = true;			// true records the matching shadow draw call.
		emberCommon::CullMode cullMode = emberCommon::CullMode::materialDefault; // materialDefault uses the material's cull mode.

	public: // Methods:
		// Constructors:
		DrawData(const Float4x4& localToWorldMatrix, const Mesh& mesh, const Material& material);
		DrawData(const Float4x4& localToWorldMatrix, const Mesh& mesh, const Material& material, bool receiveShadows, bool castShadows = true, emberCommon::CullMode cullMode = emberCommon::CullMode::materialDefault);
		DrawData(const Float4x4& localToWorldMatrix, const Mesh& mesh, const Material& material, uint32_t instanceCount, InstanceBuffer* pInstanceBuffer = nullptr);
		DrawData(const Float4x4& localToWorldMatrix, const Mesh& mesh, const Material& material, uint32_t instanceCount, InstanceBuffer* pInstanceBuffer, bool receiveShadows, bool castShadows = true, emberCommon::CullMode cullMode = emberCommon::CullMode::materialDefault);
	};
}