#pragma once
#include "emberEngine.h"



namespace emberEngine
{
	/// <summary>
	/// View direction is down = (0,0,-1).
	/// </summary>
	class SpotLight : public Component
	{
	private: // Members:
		float m_intensity;
		Float3 m_color;
		ShadowType m_shadowType;
		float m_fov;
		float m_nearClip;
		float m_farClip;
		float m_blendStart;
		float m_blendEnd;
		Float4x4 m_projectionMatrix;
		bool m_updateProjectionMatrix;
		bool m_drawFrustum;

	public: // Methods:
		SpotLight();
		~SpotLight();

		// Setters:
		void SetIntensity(const float& intensity);
		void SetColor(const Float3& color);
		void SetShadowType(ShadowType shadowType);
		void SetFov(const float& fov);
		void SetNearClip(const float& nearClip);
		void SetFarClip(const float& farClip);
		void SetBlendStart(const float& blendStart);
		void SetBlendEnd(const float& blendEnd);
		void SetDrawFrustum(bool drawFrustum);

		// Getters:
		float GetIntensity() const;
		Float3 GetColor() const;
		ShadowType GetShadowType() const;
		float GetFov() const;
		float GetNearClip() const;
		float GetFarClip() const;
		float GetBlendStart() const;
		float GetBlendEnd() const;
		Float4x4 GetViewMatrix() const;
		Float4x4 GetProjectionMatrix();

		// Overrides:
		void LateUpdate() override;
		const std::string ToString() const override;

	private: // Methods:
		void UpdateProjectionMatrix();
	};
}