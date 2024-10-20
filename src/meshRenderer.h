#pragma once
#ifndef __INCLUDE_GUARD_meshRenderer_h__
#define __INCLUDE_GUARD_meshRenderer_h__
#include "component.h"
#include "mesh.h"
#include "material.h"
#include "materialProperties.h"



class MeshRenderer : public Component
{
public: // Members:
	Mesh* mesh;
	std::unique_ptr<MaterialProperties> materialProperties;

private: // Members:
	Material* material;

public: // Methods:
	MeshRenderer();
	~MeshRenderer();

	void SetMaterial(Material* material);

	VkDescriptorSet* GetDescriptorSets(uint32_t frameIndex);
	VkPipeline& GetPipeline();
	VkPipelineLayout& GetPipelineLayout();

	// Overrides:
	void PrintType() const override;

private: // Methods:
};



#endif // __INCLUDE_GUARD_meshRenderer_h__