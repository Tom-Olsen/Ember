#ifndef __INCLUDE_GUARD_sphFluid2dGpu_h__
#define __INCLUDE_GUARD_sphFluid2dGpu_h__
#include "emberEngine.h"
#include <array>
#include <memory>



namespace emberEngine
{
	// Forward decleration:
	struct SphFluid2dGpuEditorWindow;
	class Sph2dBitonicSort;



	class SphFluid2dGpu : public Component
	{
	private: // Members:
		// Compute shaders:
		Uint3 m_threadCount;
		std::unique_ptr<ComputeShader> cs_pReset;
		std::unique_ptr<ComputeShader> cs_pDensity;
		std::unique_ptr<ComputeShader> cs_pNormalAndCurvature;
		std::unique_ptr<ComputeShader> cs_pForceDensity;
		std::unique_ptr<ComputeShader> cs_pRungeKutta2Step1;
		std::unique_ptr<ComputeShader> cs_pRungeKutta2Step2;
		std::unique_ptr<ComputeShader> cs_pBoundaryCollisions;
		std::unique_ptr<ShaderProperties> m_pResetProperties;
		std::array<std::unique_ptr<ShaderProperties>, 2> m_densityProperties;
		std::array<std::unique_ptr<ShaderProperties>, 2> m_normalAndCurvatureProperties;
		std::array<std::unique_ptr<ShaderProperties>, 2> m_forceDensityProperties;
		std::unique_ptr<ShaderProperties> m_pRungeKutta2Step1Properties;
		std::unique_ptr<ShaderProperties> m_pRungeKutta2Step2Properties;
		std::unique_ptr<ShaderProperties> m_pBoundaryCollisionsProperties;

		// Management:
		bool m_isRunning;
		bool m_reset;
		float m_timeScale;
		bool m_useGridOptimization;
		uint32_t m_timeStep;
		std::unique_ptr<Sph2dBitonicSort> pGpuSort;

		// Data:
		int m_particleCount;
		std::unique_ptr<StorageBuffer> m_pCellKeyBuffer;
		std::unique_ptr<StorageBuffer> m_pStartIndexBuffer;
		std::unique_ptr<StorageBuffer> m_pPositionBuffer;
		std::unique_ptr<StorageBuffer> m_pVelocityBuffer;
		std::unique_ptr<StorageBuffer> m_pDensityBuffer;
		std::unique_ptr<StorageBuffer> m_pNormalBuffer;
		std::unique_ptr<StorageBuffer> m_pCurvatureBuffer;
		std::unique_ptr<StorageBuffer> m_pForceDensityBuffer;

		// Runge Kutta fields:
		std::unique_ptr<StorageBuffer> m_pKp1Buffer;
		std::unique_ptr<StorageBuffer> m_pKv1Buffer;
		std::unique_ptr<StorageBuffer> m_pKp2Buffer;
		std::unique_ptr<StorageBuffer> m_pKv2Buffer;
		std::unique_ptr<StorageBuffer> m_pTempPositionBuffer;
		std::unique_ptr<StorageBuffer> m_pTempVelocityBuffer;

		// Physics:
		float m_effectRadius;
		float m_mass;
		float m_viscosity;
		float m_surfaceTension;
		float m_collisionDampening;
		float m_targetDensity;
		float m_pressureMultiplier;
		float m_gravity;
		float m_maxVelocity;

		// User Interaction/Boundaries:
		int m_attractorState;
		Float2 m_attractorPoint;
		float m_attractorRadius;
		float m_attractorStrength;
		Bounds m_fluidBounds;

		// Visuals:
		int m_colorMode;
		float m_initialDistributionRadius;
		float m_visualRadius;
		std::unique_ptr<Mesh> m_pParticleMesh;
		std::unique_ptr<Mesh> m_pRingMesh;
		Material* m_pParticleMaterial;
		std::unique_ptr<ShaderProperties> m_pShaderProperties;
		std::unique_ptr<SphFluid2dGpuEditorWindow> editorWindow;

	public: // Methods:
		SphFluid2dGpu();
		~SphFluid2dGpu();

		// Physics update:
		void Reset();
		void FixedUpdate() override;
		void TimeStepLeapFrog(float deltaT);
		void TimeStepVelocityVerlet(float deltaT);
		void TimeStepRungeKutta2(float deltaT);

		// Setters:
		void SetIsRunning(bool isRunning);
		void SetTimeScale(float timeScale);
		void SetUseGridOptimization(bool useGridOptimization);
		void SetParticleCount(int particleCount);
		void SetEffectRadius(float effectRadius);
		void SetMass(float mass);
		void SetViscosity(float viscosity);
		void SetSurfaceTension(float surfaceTension);
		void SetCollisionDampening(float collisionDampening);
		void SetTargetDensity(float targetDensity);
		void SetPressureMultiplier(float pressureMultiplier);
		void SetGravity(float gravity);
		void SetMaxVelocity(float maxVelocity);
		void SetAttractorRadius(float attractorRadius);
		void SetAttractorStrength(float attractorStrength);
		void SetAttractorState(int attractorState);
		void SetAttractorPoint(const Float2& attractorPoint);
		void SetFluidBounds(const Bounds& bounds);
		void SetColorMode(int colorMode);
		void SetInitialDistributionRadius(float initialDistributionRadius);
		void SetVisualRadius(float visualRadius);

		// Getters:
		bool GetIsRunning() const;
		float GetTimeScale() const;
		bool GetUseGridOptimization() const;
		uint32_t GetTimeStep() const;
		int GetParticleCount() const;
		float GetEffectRadius() const;
		float GetMass() const;
		float GetViscosity() const;
		float GetSurfaceTension() const;
		float GetCollisionDampening() const;
		float GetTargetDensity() const;
		float GetPressureMultiplier() const;
		float GetGravity() const;
		float GetMaxVelocity() const;
		float GetAttractorRadius() const;
		float GetAttractorStrength() const;
		Bounds GetFluidBounds() const;
		int GetColorMode() const;
		float GetInitialDistributionRadius() const;
		float GetVisualRadius() const;

		// Overrides:
		void Update() override;
		const std::string ToString() const override;
		
	private:
		// Physics:
		void ResetFluid();
		void ComputeDensity(StorageBuffer* positionBuffer, ShaderProperties* pShaderProperties, float gridRadius);
		void ComputeNormalAndCurvature(StorageBuffer* positionBuffer, ShaderProperties* pShaderProperties, float gridRadius);
		void ComputeCurvature();
		void ComputeForceDensity(StorageBuffer* positionBuffer, StorageBuffer* velocityBuffer, ShaderProperties* pShaderProperties, float gridRadius);

		void ComputeRungeKutta2Step1();
		void ComputeRungeKutta2Step2();
		void ComputeBoundaryCollisions();
	};
}



#endif // __INCLUDE_GUARD_sphFluid2dGpu_h__