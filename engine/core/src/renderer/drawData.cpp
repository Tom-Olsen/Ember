#include "drawData.h"



namespace emberCore
{
	// Constructors:
	DrawData::DrawData(const Float4x4& localToWorldMatrix, const Mesh& mesh, const Material& material)
		: localToWorldMatrix(localToWorldMatrix)
		, mesh(mesh)
		, material(material)
	{

	}
	DrawData::DrawData(const Float4x4& localToWorldMatrix, const Mesh& mesh, const Material& material, bool receiveShadows, bool castShadows, emberCommon::CullMode cullMode)
		: localToWorldMatrix(localToWorldMatrix)
		, mesh(mesh)
		, material(material)
		, receiveShadows(receiveShadows)
		, castShadows(castShadows)
		, cullMode(cullMode)
	{

	}
	DrawData::DrawData(const Float4x4& localToWorldMatrix, const Mesh& mesh, const Material& material, uint32_t instanceCount, Buffer* pInstanceBuffer)
		: localToWorldMatrix(localToWorldMatrix)
		, mesh(mesh)
		, material(material)
		, instanceCount(instanceCount)
		, pInstanceBuffer(pInstanceBuffer)
	{
		
	}
	DrawData::DrawData(const Float4x4& localToWorldMatrix, const Mesh& mesh, const Material& material, uint32_t instanceCount, Buffer* pInstanceBuffer, bool receiveShadows, bool castShadows, emberCommon::CullMode cullMode)
		: localToWorldMatrix(localToWorldMatrix)
		, mesh(mesh)
		, material(material)
		, instanceCount(instanceCount)
		, pInstanceBuffer(pInstanceBuffer)
		, receiveShadows(receiveShadows)
		, castShadows(castShadows)
		, cullMode(cullMode)
	{

	}
}