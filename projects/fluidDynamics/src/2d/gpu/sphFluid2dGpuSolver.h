#pragma once
#include "emberEngine.h"
using namespace emberEngine;



namespace fluidDynamics
{
	class SphFluid2dGpuSolver
	{
	public: // Structs:
		struct Settings
		{
			bool useHashGridOptimization = true;
			float effectRadius = 0.5f;
			float mass = 1.0f;
			float viscosity = 0.5f;
			float surfaceTension = 0.07f;
			float collisionDampening = 0.95f;
			float targetDensity = 15.0f;
			float pressureMultiplier = 20.0f;
			float gravity = 0.5f;
			float maxVelocity = 5.0f;
			Bounds fluidBounds = Bounds(Float3::zero, Float3(16.0f, 9.0f, 0.01f));
		};
		struct Data
		{
			Buffer cellKeyBuffer;
			Buffer startIndexBuffer;
			Buffer sortPermutationBuffer;
			Buffer positionBuffer;
			Buffer velocityBuffer;
			Buffer densityBuffer;
			Buffer normalBuffer;
			Buffer curvatureBuffer;
			Buffer forceDensityBuffer;
			Buffer tempBuffer0;
			Buffer tempBuffer1;
			Buffer tempBuffer2;
			Buffer tempBuffer3;
			Buffer tempBuffer4;
			Buffer tempBuffer5;
			BufferView<int> cellKeyBufferView;
			BufferView<int> startIndexBufferView;
			BufferView<int> sortPermutationBufferView;
			BufferView<Float2> positionBufferView;
			BufferView<Float2> velocityBufferView;
			BufferView<float> densityBufferView;
			BufferView<Float2> normalBufferView;
			BufferView<Float2> curvatureBufferView;
			BufferView<Float2> forceDensityBufferView;
			BufferView<Float2> tempBufferView0;
			BufferView<Float2> tempBufferView1;
			BufferView<Float2> tempBufferView2;
			BufferView<Float2> tempBufferView3;
			BufferView<Float2> tempBufferView4;
			BufferView<Float2> tempBufferView5;
			ComputeShader resetComputeShader;
			ShaderProperties resetProperties;

			Data();
			int ParticleCount();
			void Reallocate(int particleCount, float initialDistributionRadius);
		};
		struct RungeKutta	// only needed for RungeKutta2 solver.
		{
			Buffer kp1Buffer;
			Buffer kv1Buffer;
			Buffer tempPositionBuffer;
			Buffer tempVelocityBuffer;
			BufferView<Float2> kp1BufferView;
			BufferView<Float2> kv1BufferView;
			BufferView<Float2> tempPositionBufferView;
			BufferView<Float2> tempVelocityBufferView;
			ComputeShader resetComputeShader;
			ShaderProperties resetProperties;

			RungeKutta();
			int ParticleCount();
			void Reallocate(int particleCount);
		};
		struct Attractor
		{
			Float2 point;
			float radius;
			float strength;
			int state;	// -1 = repulse, 0 = off, 1 = attract.
		};
		struct ComputeShaders
		{
			ComputeType computeType = ComputeType::preRender;
			ComputeShader cellKeysComputeShader;
			ComputeShader startIndicesComputeShader;
			ComputeShader densityComputeShader;
			ComputeShader normalAndCurvatureComputeShader;
			ComputeShader forceDensityComputeShader;
			ComputeShader rungeKutta2Step1ComputeShader;
			ComputeShader rungeKutta2Step2ComputeShader;
			ComputeShader boundaryCollisionsComputeShader;
			ShaderProperties cellKeysProperties;
			ShaderProperties startIndicesProperties;
			ShaderProperties densityProperties;
			ShaderProperties normalAndCurvatureProperties;
			ShaderProperties forceDensityProperties;
			ShaderProperties rungeKutta2Step1Properties;
			ShaderProperties rungeKutta2Step2Properties;
			ShaderProperties boundaryCollisionsProperties;

			ComputeShaders();
			void SetUseHashGridOptimization(bool useHashGridOptimization);
			void SetEffectRadius(float effectRadius);
			void SetHashGridSize(int hashGridSize);
			void SetMass(float mass);
			void SetViscosity(float viscosity);
			void SetSurfaceTension(float surfaceTension);
			void SetCollisionDampening(float collisionDampening);
			void SetTargetDensity(float targetDensity);
			void SetPressureMultiplier(float pressureMultiplier);
			void SetGravity(float gravity);
			void SetMaxVelocity(float maxVelocity);
			void SetFluidBounds(const Bounds& bounds);
			void SetAttractorRadius(float attractorRadius);
			void SetAttractorStrength(float attractorStrength);
			void SetAttractorState(int attractorState);
			void SetAttractorPoint(const Float2& attractorPoint);
		};

	public: // Methods:
		// Ordinary Differential Equation Solvers:
		//static void TimeStepLeapFrog(float dt, int timeStep, const Settings& settings, Data& data, const Attractor& attractor);
		//static void TimeStepVelocityVerlet(float dt, const Settings& settings, Data& data, const Attractor& attractor);
		static void TimeStepRungeKutta2(float dt, const Settings& settings, Data& data, ComputeShaders& computeShaders, const Attractor& attractor, RungeKutta& rungeKutta);

		// Field computations:
		static void ComputeCellKeys(ComputeShaders& computeShaders, const BufferView<int>& cellKeyBufferView, const BufferView<Float2>& positionBufferView);
		static void ComputeStartIndices(ComputeShaders& computeShaders, const BufferView<int> startIndexBufferView, const BufferView<int>& cellKeyBufferView);
		static void ComputeDensities(ComputeShaders& computeShaders, const BufferView<float>& densityBufferView, const BufferView<Float2>& positionBufferView, const BufferView<int> startIndexBufferView, const BufferView<int>& cellKeyBufferView);
		static void ComputeNormalsAndCurvatures(ComputeShaders& computeShaders, const BufferView<Float2>& normalBufferView, const BufferView<Float2>& curvatureBufferView, const BufferView<float>& densityBufferView, const BufferView<Float2>& positionBufferView, const BufferView<int> startIndexBufferView, const BufferView<int>& cellKeyBufferView);
		static void ComputeForceDensities(ComputeShaders& computeShaders, const BufferView<Float2>& forceDensityBufferView, const BufferView<float>& densityBufferView, const BufferView<Float2>& positionBufferView, const BufferView<Float2>& velocityBufferView, const BufferView<Float2>& normalBufferView, const BufferView<Float2>& curvatureBufferView, const BufferView<int> startIndexBufferView, const BufferView<int>& cellKeyBufferView);
		static void ComputeRungeKutta2Step1(ComputeShaders& computeShaders, const BufferView<Float2>& forceDensityBufferView, const BufferView<float>& densityBufferView, const BufferView<Float2>& kp1BufferView, const BufferView<Float2>& kv1BufferView, const BufferView<Float2>& tempPositionBufferView, const BufferView<Float2>& tempVelocityBufferView, const BufferView<Float2>& positionBufferView, const BufferView<Float2>& velocityBufferView);
		static void ComputeRungeKutta2Step2(ComputeShaders& computeShaders, const BufferView<Float2>& forceDensityBufferView, const BufferView<float>& densityBufferView, const BufferView<Float2>& kp1BufferView, const BufferView<Float2>& kv1BufferView, const BufferView<Float2>& tempPositionBufferView, const BufferView<Float2>& tempVelocityBufferView, const BufferView<Float2>& positionBufferView, const BufferView<Float2>& velocityBufferView);
		static void ComputeBoundaryCollisions(ComputeShaders& computeShaders, const BufferView<Float2>& positionBufferView, const BufferView<Float2>& velocityBufferView);
	};
}