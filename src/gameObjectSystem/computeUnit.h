#ifndef __INCLUDE_GUARD_computeUnit_h__
#define __INCLUDE_GUARD_computeUnit_h__
#include "emberEngine.h"



namespace emberEngine
{
	// Forward declarations:
	class ComputeShader;
	class ShaderProperties;



	class ComputeUnit : public Component
	{
	private: // Members:
		Uint3 m_groupCount;
		ComputeShader* m_pComputeShader;
		std::unique_ptr<ShaderProperties> m_pShaderProperties;

	public: // Methods:
		ComputeUnit();
		~ComputeUnit();
		
		// Setter:
		void SetComputeCount(Uint3 groupCount);
		void SetComputeCount(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ);
		void SetComputeShader(ComputeShader* pComputeShader);

		// Getters:
		Uint3 GetGroupCount();
		ComputeShader* GetComputeShader();
		ShaderProperties* GetShaderProperties();
		
		// Overrides:
		const std::string ToString() const override;
	};
}



#endif // __INCLUDE_GUARD_computeUnit_h__