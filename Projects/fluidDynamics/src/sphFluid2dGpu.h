#ifndef __INCLUDE_GUARD_sphFluid2dGpu_h__
#define __INCLUDE_GUARD_sphFluid2dGpu_h__
#include "emberEngine.h"
#include <array>
#include <memory>



namespace emberEngine
{
	// Forward decleration:
	struct SphFluid2dGpuEditorWindow;



	class SphFluid2dGpu : public Component
	{
	private: // Members:
		// Compute shaders:
		Uint3 m_threadCount;
		std::unique_ptr<ComputeShader> cs_pReset;
		std::unique_ptr<ComputeShader> cs_pDensity;
		std::unique_ptr<ShaderProperties> m_pResetProperties;
		std::unique_ptr<ShaderProperties> m_pDensityProperties;

		// Management:
		bool m_isRunning;
		float m_timeScale;
		bool m_useGridOptimization;
		uint32_t m_timeStep;

		// Data:
		int m_particleCount;
		std::unique_ptr<StorageBuffer> m_pPositionBuffer;
		std::unique_ptr<StorageBuffer> m_pVelocityBuffer;
		std::unique_ptr<StorageBuffer> m_pDensityBuffer;
		//std::vector<Float2> m_normals;
		//std::vector<float> m_curvatures;
		//std::vector<Float2> m_forceDensities;

		// Runge Kutta fields:
		//std::vector<Float2> m_kp1;
		//std::vector<Float2> m_kp2;
		//std::vector<Float2> m_kv1;
		//std::vector<Float2> m_kv2;
		//std::vector<Float2> m_tempPositions;
		//std::vector<Float2> m_tempVelocities;

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
		void SetFluidBounds(const Bounds& bounds);
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
		float GetInitialDistributionRadius() const;
		float GetVisualRadius() const;

		// Overrides:
		void Start() override;
		void Update() override;
		const std::string ToString() const override;
		
	private:
		// Physics:
		void DispatchResetKernal();
		void DispatchDensityKernal();
		void Normal();
		void Curvature();
		void PressureForceDensity();
		void ViscosityForceDensity();
		void SurfaceTensionForceDensity();
		void GravityForceDensity();
		void ExternalForceDensity();
		void BoundaryCollisions();
	};
}



#endif // __INCLUDE_GUARD_sphFluid2dGpu_h__