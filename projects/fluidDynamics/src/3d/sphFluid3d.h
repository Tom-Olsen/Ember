#pragma once
#include "emberEngine.h"
using namespace emberEngine;



// Forward decleration:
namespace emberEditor
{
	struct SphFluid3dEditorWindow;
}



namespace fluidDynamics
{
	// Forward decleration:
	class SphBitonicSort3d;



	class SphFluid3d : public Component
	{
	private: // Members:
		// Compute shaders:
		Uint3 m_threadCount;
		ComputeShader cs_reset;
		ComputeShader cs_density;
		ComputeShader cs_normalAndCurvature;
		ComputeShader cs_forceDensity;
		ComputeShader cs_rungeKutta2Step1;
		ComputeShader cs_rungeKutta2Step2;
		ComputeShader cs_boundaryCollisions;
		ShaderProperties m_resetProperties;
		std::array<ShaderProperties, 2> m_densityProperties;
		std::array<ShaderProperties, 2> m_normalAndCurvatureProperties;
		std::array<ShaderProperties, 2> m_forceDensityProperties;
		ShaderProperties m_rungeKutta2Step1Properties;
		ShaderProperties m_rungeKutta2Step2Properties;
		ShaderProperties m_boundaryCollisionsProperties;

		// Management:
		bool m_isRunning;
		bool m_reset;
		float m_timeScale;
		bool m_useGridOptimization;
		uint32_t m_timeStep;
		std::unique_ptr<SphBitonicSort3d> pGpuSort;

		// Data:
		int m_particleCount;
		Buffer m_cellKeyBuffer;
		Buffer m_startIndexBuffer;
		Buffer m_positionBuffer;
		Buffer m_velocityBuffer;
		Buffer m_densityBuffer;
		Buffer m_normalBuffer;
		Buffer m_curvatureBuffer;
		Buffer m_forceDensityBuffer;

		// Runge Kutta fields:
		Buffer m_kp1Buffer;
		Buffer m_kv1Buffer;
		Buffer m_kp2Buffer;
		Buffer m_kv2Buffer;
		Buffer m_tempPositionBuffer;
		Buffer m_tempVelocityBuffer;

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

		// Boundaries:
		Bounds m_fluidBounds;

		// Visuals:
		int m_colorMode;
		int m_initialDistributionShellCount;
		float m_initialDistributionRadius;
		float m_visualRadius;
		Mesh m_particleMesh;
		Material m_particleMaterial;
		ShaderProperties m_shaderProperties;
		std::unique_ptr<emberEditor::SphFluid3dEditorWindow> editorWindow;

	public: // Methods:
		SphFluid3d();
		~SphFluid3d();

		// Physics update:GpuSort
		void Reset();
		void FixedUpdate() override;
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
		void SetFluidBounds(const Bounds& bounds);
		void SetColorMode(int colorMode);
		void SetInitialDistributionShellCount(int initialDistributionShellCount);
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
		Bounds GetFluidBounds() const;
		int GetColorMode() const;
		int GetInitialDistributionShellCount() const;
		float GetInitialDistributionRadius() const;
		float GetVisualRadius() const;

		// Overrides:
		void Update() override;

	private:
		// Physics:
		void ResetFluid();
		void ComputeDensity(Buffer& positionBuffer, ShaderProperties& shaderProperties, float gridRadius);
		void ComputeNormalAndCurvature(Buffer& positionBuffer, ShaderProperties& shaderProperties, float gridRadius);
		void ComputeCurvature();
		void ComputeForceDensity(Buffer& positionBuffer, Buffer& velocityBuffer, ShaderProperties& shaderProperties, float gridRadius);

		void ComputeRungeKutta2Step1();
		void ComputeRungeKutta2Step2();
		void ComputeBoundaryCollisions();
	};
}