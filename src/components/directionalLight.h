﻿#ifndef __INCLUDE_GUARD_directionalLight_h__
#define __INCLUDE_GUARD_directionalLight_h__
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
	public: // Enums:
		enum class ShadowCascadeCount { one = 1, two = 2, three = 3, four = 4 };

	private: // Members:
		float m_intensity;
		Float3 m_color;
		ShadowType m_shadowType;

		// Shadow cascade settings:
		std::array<ShadowCascade*, 4> m_shadowCascades;
		Camera* m_pActiveCamera;
		float m_distributionFactor;		// 0 = linear, 1 = logarithmic
		ShadowCascadeCount m_shadowCascadeCount;
		float m_shadowCascadeSplits[5];	// Percentile splits for each shadow cascade € [0,1].

		// Visualization:
		bool m_drawFrustum;
		bool m_overwriteSceneActiveCamera;

	public: // Methods:
		DirectionalLight();
		~DirectionalLight();
		void UpdateShadowCascades(float sceneHeight);

		// Setters:
		// Light properties:
		void SetIntensity(float intensity);
		void SetColor(const Float3& color);
		void SetShadowType(ShadowType shadowType);

		// Shadow cascade properties:
		void SetActiveCamera(Camera* pCamera);
		void OverwriteActiveCamera(Camera* pCamera, bool overwriteSceneActiveCamera);
		void SetShadowCascadeCount(ShadowCascadeCount shadowCascadeCount);
		void SetDistributionFactor(float value01);

		// Visualization bools:
		void SetDrawFrustum(bool drawFrustum);

		// Getters:
		// Light properties:
		Float3 GetDirection() const;
		float GetIntensity() const;
		Float3 GetColor() const;
		Float4 GetColorIntensity() const;
		ShadowType GetShadowType() const;
		Float4x4 GetViewMatrix(uint32_t shadowCascadeIndex) const;
		Float4x4 GetProjectionMatrix(uint32_t shadowCascadeIndex);

		// Shadow cascade properties:
		ShadowCascadeCount GetShadowCascadeCount() const;
		float GetDistributionFactor() const;
		float GetShadowCascadeSplit(uint32_t index) const;

		// Overrides:
		void Start() override;
		void LateUpdate() override;
		const std::string ToString() const override;
	};
}



#endif // __INCLUDE_GUARD_spotLight_h__