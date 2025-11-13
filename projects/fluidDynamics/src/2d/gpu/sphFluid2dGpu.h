#pragma once
#include "emberEngine.h"
using namespace emberEngine;



// Forward decleration:
namespace emberEditor
{
	struct SphFluid2dGpuEditorWindow;
}



namespace fluidDynamics
{
	class SphFluid2dGpu : public Component
	{
	private: // Members:
		// Compute shaders:
		Uint3 m_threadCount;
		ComputeShader m_resetComputeShader;
		ComputeShader m_cellKeysComputeShader;
		ComputeShader m_startIndicesComputeShader;
		ComputeShader m_densityComputeShader;
		ComputeShader m_normalAndCurvatureComputeShader;
		ComputeShader m_forceDensityComputeShader;
		ComputeShader m_rungeKutta2Step1ComputeShader;
		ComputeShader m_rungeKutta2Step2ComputeShader;
		ComputeShader m_boundaryCollisionsComputeShader;

		// Shader properties:
		ShaderProperties m_cellKeysProperties;
		ShaderProperties m_startIndicesProperties;
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

		// Data buffers:
		int m_particleCount;
		int m_hashGridSize;
		Buffer m_cellKeyBuffer;
		Buffer m_startIndexBuffer;
		Buffer m_positionBuffer;
		Buffer m_velocityBuffer;
		Buffer m_densityBuffer;
		Buffer m_normalBuffer;
		Buffer m_curvatureBuffer;
		Buffer m_forceDensityBuffer;

		// Runge Kutta buffers:
		Buffer m_kp1Buffer;
		Buffer m_kv1Buffer;
		Buffer m_kp2Buffer;
		Buffer m_kv2Buffer;
		Buffer m_tempPositionBuffer;
		Buffer m_tempVelocityBuffer;

		// Temp buffers:
		Buffer m_sortPermutationBuffer;
		Buffer m_reorderBuffer0;
		Buffer m_reorderBuffer1;
		Buffer m_reorderBuffer2;
		Buffer m_reorderBuffer3;
		Buffer m_reorderBuffer4;

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
		Mesh m_particleMesh;
		Mesh m_ringMesh;
		Material m_particleMaterial;
		ShaderProperties m_shaderProperties;
		std::unique_ptr<emberEditor::SphFluid2dGpuEditorWindow> editorWindow;

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
		
	private:
		// Async compute shaders dispatches:
		void ResetFluidAsync(uint32_t sessionID);
		void ComputeCellKeysAsync(uint32_t sessionID);
		void ComputeStartIndicesAsync(uint32_t sessionID);
		void ComputeDensityAsync(uint32_t sessionID, Buffer& positionBuffer, ShaderProperties& shaderProperties, float gridRadius);
		void ComputeNormalAndCurvatureAsync(uint32_t sessionID, Buffer& positionBuffer, ShaderProperties& shaderProperties, float gridRadius);
		void ComputeCurvatureAsync(uint32_t sessionID);
		void ComputeForceDensityAsync(uint32_t sessionID, Buffer& positionBuffer, Buffer& velocityBuffer, ShaderProperties& shaderProperties, float gridRadius);
		void ComputeRungeKutta2Step1Async(uint32_t sessionID);
		void ComputeRungeKutta2Step2Async(uint32_t sessionID);
		void ComputeBoundaryCollisionsAsync(uint32_t sessionID);

		// Pre render compute shaders dispatches:
		void ComputeCellKeysPreRender();
		void ComputeStartIndicesPreRender();
		void ComputeDensityPreRender(Buffer& positionBuffer, ShaderProperties& shaderProperties, float gridRadius);
		void ComputeNormalAndCurvaturePreRender(Buffer& positionBuffer, ShaderProperties& shaderProperties, float gridRadius);
		void ComputeCurvaturePreRender();
		void ComputeForceDensityPreRender(Buffer& positionBuffer, Buffer& velocityBuffer, ShaderProperties& shaderProperties, float gridRadius);
		void ComputeRungeKutta2Step1PreRender();
		void ComputeRungeKutta2Step2PreRender();
		void ComputeBoundaryCollisionsPreRender();
	};
}