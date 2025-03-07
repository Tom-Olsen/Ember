#ifndef __INCLUDE_GUARD_pointLight_h__
#define __INCLUDE_GUARD_pointLight_h__
#include "emberEngine.h"



namespace emberEngine
{
	class PointLight : public Component
	{
	public: // Members:

	private: // Members:
		float m_intensity;
		Float3 m_color;
		Lighting::ShadowType m_shadowType;
		float m_nearClip;
		float m_farClip;
		bool m_updateProjectionMatrix;
		Float4x4 m_projectionMatrix;
		bool m_drawFrustum;

	public: // Methods:
		PointLight();
		~PointLight();

		// Setters:
		void SetIntensity(const float& intensity);
		void SetColor(const Float3& color = Float3::one);
		void SetShadowType(Lighting::ShadowType shadowType);
		void SetNearClip(const float& nearClip);
		void SetFarClip(const float& farClip);
		void SetDrawFrustum(bool drawFrustum);

		// Getters:
		float GetIntensity() const;
		Float3 GetColor() const;
		Lighting::ShadowType GetShadowType() const;
		float GetNearClip() const;
		float GetFarClip() const;
		Float4x4 GetViewMatrix() const;
		Float4x4 GetProjectionMatrix();

		// Overrides:
		void LateUpdate() override;
		const std::string ToString() const override;

	private: // Methods:
		void UpdateProjectionMatrix();
	};
}



#endif // __INCLUDE_GUARD_pointLight_h__