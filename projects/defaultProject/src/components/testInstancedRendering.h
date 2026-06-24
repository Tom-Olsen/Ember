#pragma once
#include "emberEngine.h"



namespace emberEngine
{
	class TestInstancedRendering : public emberEcs::Component
	{
	private: // Members:
		emberCore::ComputeShader* m_pStartCS;
		emberCore::ComputeShader* m_pUpdateCS;
		std::unique_ptr<emberCore::Buffer> m_pInstanceBuffer;

	public: // Methods:
		TestInstancedRendering(uint32_t instanceCount);
		~TestInstancedRendering();

		emberCore::Buffer* GetInstanceBuffer() const;

		// Overrides:
		void Start() override;
		void Update() override;
	};
}