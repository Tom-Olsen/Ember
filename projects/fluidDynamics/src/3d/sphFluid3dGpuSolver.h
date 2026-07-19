#pragma once
#include "emberEngine.h"
#include <array>
using namespace emberCore;
using namespace emberEcs;



namespace fluidDynamics
{
	class SphFluid3dGpuSolver
	{
	public: // Structs:
		struct ComputeShaders
		{
			ComputeType computeType;
			uint32_t sessionID;
			ComputeShader resetDataComputeShader;
			ComputeShader resetRungeKuttaComputeShader;
			ComputeShader cellKeysComputeShader;
			ComputeShader startIndicesResetComputeShader;
			ComputeShader startIndicesComputeShader;
			ComputeShader densityComputeShader;
			ComputeShader normalAndCurvatureComputeShader;
			ComputeShader forceDensityComputeShader;
			ComputeShader rungeKutta2Step1ComputeShader;
			ComputeShader rungeKutta2Step2ComputeShader;
			ComputeShader boundaryCollisionsComputeShader;
			ComputeShader densityTexture3dComputeShader;
			ComputeShader opticalDepthTexture3dComputeShader;

			ComputeShaders();
			void SetUseHashGridOptimization(bool useHashGridOptimization);
			void SetEffectRadius(float effectRadius);
			void SetHashGridSize(int hashGridSize);
			void SetMass(float mass);
			void SetViscosity(float viscosity);
			void SetSurfaceTension(float surfaceTension);
			void SetTargetDensity(float targetDensity);
			void SetPressureMultiplier(float pressureMultiplier);
			void SetNearPressureRatio(float nearPressureRatio);
			void SetGravity(float gravity);
			void SetMaxVelocity(float maxVelocity);
			void SetAttractorRadius(float attractorRadius);
			void SetAttractorStrength(float attractorStrength);
			void SetAttractorState(int attractorState);
			void SetAttractorPoint(const Float3& attractorPoint);
		};
		struct Settings
		{
			bool useHashGridOptimization;
			bool computeDensityTexture3d;
			float effectRadius;
			float mass;
			float viscosity;
			float surfaceTension;
			float collisionDampening;
			float targetDensity;
			float pressureMultiplier;
			float nearPressureRatio;
			float gravity;
			float maxVelocity;
			RotatedBounds fluidBounds;
		};
		struct ScratchData
		{
			BufferTyped<uint32_t> cellKeyBuffer;
			BufferTyped<uint32_t> startIndexBuffer;
			BufferTyped<uint32_t> sortPermutationBuffer;
			BufferTyped<Float3> forceDensityBuffer;
			BufferTyped<float> nearDensityBuffer;
			BufferTyped<Float3> tempBuffer0;
			BufferTyped<Float3> tempBuffer1;
			BufferTyped<Float3> tempBuffer2;
			BufferTyped<Float3> tempBuffer3;
			BufferTyped<Float3> tempBuffer4;
			BufferTyped<Float3> tempBuffer5;
			BufferTyped<Float3> tempBuffer6;
			BufferTyped<Float3> tempBuffer7;
			BufferTyped<Float3> kp1Buffer;
			BufferTyped<Float3> kv1Buffer;
			BufferTyped<Float3> tempPositionBuffer;
			BufferTyped<Float3> tempVelocityBuffer;

			int ParticleCount();
			void Reallocate(int particleCount);
		};
		struct TripleData
		{
			TripleBuffer<Float3> positionBuffer;
			TripleBuffer<Float3> velocityBuffer;
			TripleBuffer<float> densityBuffer;
			TripleBuffer<Float3> normalBuffer;
			TripleBuffer<float> curvatureBuffer;
			std::array<Texture3d, PhysicsTripleBufferState::bufferCount> densityTexture3d;
			std::array<Texture3d, PhysicsTripleBufferState::bufferCount> opticalDepthTexture3d;
			std::array<RotatedBounds, PhysicsTripleBufferState::bufferCount> fluidBounds;
			std::array<RotatedBounds, PhysicsTripleBufferState::bufferCount> opticalDepthBounds;
			std::array<bool, PhysicsTripleBufferState::bufferCount> hasOpticalDepthTexture3d;
			std::array<float, PhysicsTripleBufferState::bufferCount> extinctionCoefficients;
			Uint3 densityTexture3dResolution;
			Uint3 opticalDepthTexture3dResolution;

			int ParticleCount();
			void Reallocate(int particleCount, Uint3 densityResolution, Uint3 opticalDepthResolution);
			void ReallocateDensityTexture3d(Uint3 resolution);
			void ReallocateOpticalDepthTexture3d(Uint3 resolution);
		};
		struct Attractor
		{
			Float3 point;
			float radius;
			float strength;
			int state;	// -1 = repulse, 0 = off, 1 = attract.
		};

	public: // Methods:
		// Ordinary Differential Equation Solvers:
		//static void TimeStepLeapFrog(float dt, int timeStep, const Settings& settings, Data& data, const Attractor& attractor);
		//static void TimeStepVelocityVerlet(float dt, const Settings& settings, Data& data, const Attractor& attractor);
		static void TimeStepRungeKutta2(float dt, const Settings& settings, ComputeShaders& computeShaders, ScratchData& scratchData, TripleData& tripleData, uint32_t sourceDataIndex, uint32_t destinationDataIndex);

		// Data management:
		static void ResetData(ComputeShaders& computeShaders, ScratchData& scratchData, TripleData& tripleData, uint32_t dataIndex, float initialDistributionRadius);

		// Field computations:
		static void ComputeCellKeys(ComputeShaders& computeShaders, const BufferView<uint32_t>& cellKeyBufferView, const BufferView<Float3>& positionBufferView);
		static void ComputeStartIndices(ComputeShaders& computeShaders, const BufferView<uint32_t>& startIndexBufferView, const BufferView<uint32_t>& cellKeyBufferView);
		static void ComputeDensities(ComputeShaders& computeShaders, const BufferView<float>& densityBufferView, const BufferView<float>& nearDensityBufferView, const BufferView<Float3>& positionBufferView, const BufferView<uint32_t>& startIndexBufferView, const BufferView<uint32_t>& cellKeyBufferView);
		static void ComputeNormalsAndCurvatures(ComputeShaders& computeShaders, const BufferView<Float3>& normalBufferView, const BufferView<float>& curvatureBufferView, const BufferView<float>& densityBufferView, const BufferView<Float3>& positionBufferView, const BufferView<uint32_t>& startIndexBufferView, const BufferView<uint32_t>& cellKeyBufferView);
		static void ComputeForceDensities(ComputeShaders& computeShaders, const BufferView<Float3>& forceDensityBufferView, const BufferView<float>& densityBufferView, const BufferView<float>& nearDensityBufferView, const BufferView<Float3>& positionBufferView, const BufferView<Float3>& velocityBufferView, const BufferView<Float3>& normalBufferView, const BufferView<float>& curvatureBufferView, const BufferView<uint32_t>& startIndexBufferView, const BufferView<uint32_t>& cellKeyBufferView);
		static void ComputeRungeKutta2Step1(ComputeShaders& computeShaders, float dt, const BufferView<Float3>& forceDensityBufferView, const BufferView<float>& densityBufferView, const BufferView<Float3>& positionBufferView, const BufferView<Float3>& velocityBufferView, const BufferView<Float3>& kp1BufferView, const BufferView<Float3>& kv1BufferView, const BufferView<Float3>& tempPositionBufferView, const BufferView<Float3>& tempVelocityBufferView);
		static void ComputeRungeKutta2Step2(ComputeShaders& computeShaders, float dt, const BufferView<Float3>& forceDensityBufferView, const BufferView<float>& densityBufferView, const BufferView<Float3>& kp1BufferView, const BufferView<Float3>& kv1BufferView, const BufferView<Float3>& tempVelocityBufferView, const BufferView<Float3>& sourcePositionBufferView, const BufferView<Float3>& sourceVelocityBufferView, const BufferView<Float3>& positionBufferView, const BufferView<Float3>& velocityBufferView);
		static void ComputeBoundaryCollisions(ComputeShaders& computeShaders, const BufferView<Float3>& positionBufferView, const BufferView<Float3>& velocityBufferView, const RotatedBounds& fluidBounds, float collisionDampening);
		static void ComputeDensityTexture3d(ComputeShaders& computeShaders, ScratchData& scratchData, TripleData& tripleData, uint32_t dataIndex);
		static void ComputeOpticalDepthTexture3d(ComputeShaders& computeShaders, TripleData& tripleData, uint32_t dataIndex);
	};
}