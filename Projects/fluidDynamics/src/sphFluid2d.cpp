#include "sphFluid2d.h"



namespace emberEngine
{
	// Constructor/Destructor:
	SphFluid2d::SphFluid2d()
	{
		m_particleCount = 1;
		m_isRunning = false;
		m_fluidBounds = Bounds2d(Float2::zero, Float2(16.0f / 9.0f, 1.0f));
		m_particleRadius = 0.05f;
		pQuad = MeshManager::GetMesh("unitQuad");
		pParticleMaterial = MaterialManager::GetMaterial("defaultMaterial");
	}
	SphFluid2d::~SphFluid2d()
	{

	}



	// Setters:
	void SphFluid2d::SetParticleCount(uint32_t particleCount)
	{
		m_particleCount = particleCount;
		Reset();
	}
	void SphFluid2d::SetFluidBounds(const Bounds2d& bounds)
	{
		m_fluidBounds = bounds;
	}
	void SphFluid2d::SetParticleRadius(float radius)
	{
		m_particleRadius = radius;
	}



	// Getters:
	uint32_t SphFluid2d::GetParticleCount() const
	{
		return m_particleCount;
	}
	Bounds2d SphFluid2d::GetFluidBounds() const
	{
		return m_fluidBounds;
	}
	float SphFluid2d::GetParticleRadius() const
	{
		return m_particleRadius;
	}



	// Overrides:
	void SphFluid2d::Start()
	{

	}
	void SphFluid2d::Update()
	{
		if (EventSystem::KeyDown(SDLK_SPACE))
			m_isRunning = !m_isRunning;

		Float4x4 localToWorld = GetTransform()->GetLocalToWorldMatrix();
		Graphics::DrawBounds(localToWorld, m_fluidBounds, 0.01f, Float4::white, false, false);
		for (uint32_t i = 0; i < m_particleCount; i++)
		{
			Float4x4 matrix = localToWorld * Float4x4::Translate(Float3(m_positions[i], 0)) * Float4x4::Scale(m_particleRadius);
			ShaderProperties* pShaderProperties = Graphics::DrawMesh(pQuad, pParticleMaterial, matrix, false, false);
			pShaderProperties->SetValue("SurfaceProperties", "diffuseColor", Float4::blue);
		}
	}
	const std::string SphFluid2d::ToString() const
	{
		return "SphFluid2d";
	}



	// Private methods:
	void SphFluid2d::Reset()
	{
		m_positions.clear();
		m_positions.resize(m_particleCount);

		Float2 min = m_fluidBounds.GetMin();
		Float2 max = m_fluidBounds.GetMax();
		float border = 0.05f * math::Min(m_fluidBounds.GetSize().x, m_fluidBounds.GetSize().y);
		for (uint32_t i = 0; i < m_particleCount; i++)
		{
			m_positions[i].x = math::Random::Uniform(min.x + border, max.x - border);
			m_positions[i].y = math::Random::Uniform(min.y + border, max.y - border);
		}
	}
}