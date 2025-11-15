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

		// Data buffer views:
		BufferView<int> m_cellKeyBufferView;
		BufferView<int> m_startIndexBufferView;
		BufferView<Float2> m_positionBufferView;
		BufferView<Float2> m_velocityBufferView;
		BufferView<float> m_densityBufferView;
		BufferView<Float2> m_normalBufferView;
		BufferView<Float2> m_curvatureBufferView;
		BufferView<Float2> m_forceDensityBufferView;

		// Runge Kutta buffer views:
		BufferView<Float2> m_kp1BufferView;
		BufferView<Float2> m_kv1BufferView;
		BufferView<Float2> m_kp2BufferView;
		BufferView<Float2> m_kv2BufferView;
		BufferView<Float2> m_tempPositionBufferView;
		BufferView<Float2> m_tempVelocityBufferView;

		// Temp buffer views:
		BufferView<int> m_sortPermutationBufferView;
		BufferView<Float2> m_reorderBufferView0;
		BufferView<Float2> m_reorderBufferView1;
		BufferView<Float2> m_reorderBufferView2;
		BufferView<Float2> m_reorderBufferView3;
		BufferView<Float2> m_reorderBufferView4;

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
		// Compute shader recording:
		void ResetFluid();
		void ComputeCellKeys(BufferView<Float2>& positionBufferView);
		void ComputeStartIndices();
		void ComputeDensity(BufferView<Float2>& positionBufferView, ShaderProperties& shaderProperties);
		void ComputeNormalAndCurvature(BufferView<Float2>& positionBufferView, ShaderProperties& shaderProperties);
		void ComputeCurvature();
		void ComputeForceDensity(BufferView<Float2>& positionBufferView, BufferView<Float2>& velocityBufferView, ShaderProperties& shaderProperties);
		void ComputeRungeKutta2Step1();
		void ComputeRungeKutta2Step2();
		void ComputeBoundaryCollisions();
	};
}