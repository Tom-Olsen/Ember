#pragma once
#include "commonLighting.h"
#include "emberEngine.h"



namespace emberEngine
{
	// Forward declarations:
	class ShadowCascade;



	/// <summary>
	/// View direction is down = (0,0,-1).
	/// </summary>
	class DirectionalLight : public Component
	{
	private: // Members:
		float m_intensity;
		Float3 m_color;
		emberCommon::ShadowType m_shadowType;
		Camera* m_pActiveCamera;
		int m_shadowCascadeCount;
		float m_distributionFactor;		// €[0,1], 0 = linear, 1 = logarithmic
		float m_shadowCascadeSplits[5];	// Percentile splits for each shadow cascade € [0,1].
		std::array<ShadowCascade*, 4> m_shadowCascades;
		bool m_drawFrustum;

	public: // Methods:
		DirectionalLight();
		~DirectionalLight();

		// Setters:
		void SetIntensity(float intensity);
		void SetColor(const Float3& color);
		void SetShadowType(emberCommon::ShadowType shadowType);
		void SetActiveCamera(Camera* pCamera);
		void SetShadowCascadeCount(int shadowCascadeCount);
		void SetDistributionFactor(float distributionFactor);
		void SetDrawFrustum(bool drawFrustum);

		// Getters:
		float GetIntensity() const;
		Float3 GetColor() const;
		emberCommon::ShadowType GetShadowType() const;
		Camera* GetActiveCamera() const;
		int GetShadowCascadeCount() const;
		float GetDistributionFactor() const;
		float GetShadowCascadeSplit(int index) const;
		Float4x4 GetViewMatrix(int shadowCascadeIndex) const;
		Float4x4 GetProjectionMatrix(int shadowCascadeIndex);

		// Overrides:
		void LateUpdate() override;
		const std::string ToString() const override;
	};
}