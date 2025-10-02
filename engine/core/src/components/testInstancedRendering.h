#pragma once
#include "emberEngine.h"



namespace emberEngine
{
	// Forward declarations:
	class MeshRenderer;



	class TestInstancedRendering : public Component
	{
	private: // Members:
		ComputeShader* m_pStartCS;
		ComputeShader* m_pUpdateCS;
		std::unique_ptr<Buffer> m_pInstanceBuffer;
		ShaderProperties m_startProperties;
		ShaderProperties m_updateProperties;

	public: // Methods:
		TestInstancedRendering(uint32_t instanceCount);
		~TestInstancedRendering();

		Buffer* GetInstanceBuffer() const;

		// Overrides:
		void Start() override;
		void Update() override;
		const std::string ToString() const override;
	};
}