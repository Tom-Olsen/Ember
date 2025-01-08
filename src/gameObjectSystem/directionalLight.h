#ifndef __INCLUDE_GUARD_directionalLight_h__
#define __INCLUDE_GUARD_directionalLight_h__
#include "emberEngine.h"


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
	float m_nearClip;
	float m_farClip;
	float m_viewWidth;
	float m_viewHeight;
	Float4x4 m_projectionMatrix;
	bool m_updateProjectionMatrix;

	// Shadow cascade settings:
	Camera* m_pActiveCamera;
	float m_maxShadowDistance;	// Distance from camera to last shadow cascade.
	ShadowCascadeCount m_shadowCascadeCount;
	float m_shadowCascadeSplits[3];	// Percentile splits for each shadow cascade € [0,1].

	// Visualization:
	bool m_drawFrustum;
	bool m_showShaowCascades;
	bool m_overwriteSceneActiveCamera;
	std::unique_ptr<Mesh> pQuad;

public: // Methods:
	DirectionalLight();
	~DirectionalLight();

	// Setters:
	// Light properties:
	void SetIntensity(float intensity);
	void SetColor(const Float3& color);
	void SetNearClip(float nearClip);
	void SetFarClip(float farClip);
	void SetViewWidth(float viewWidth);
	void SetViewHeight(float viewHeight);

	// Shadow cascade properties:
	void SetActiveCamera(Camera* pCamera);
	void SetActiveCamera(Camera* pCamera, bool overwriteSceneActiveCamera);
	void SetMaxShadowDistance(float maxShadowDistance);
	void SetShadowCascadeCount(ShadowCascadeCount shadowCascadeCount);
	void SetShadowCascadeSplits(float value01, uint32_t index);

	// Visualization bools:
	void SetDrawFrustum(bool drawFrustum);
	void SetShowShadowCascades(bool showShadowCascades);

	// Getters:
	// Light properties:
	Float3 GetDirection() const;
	float GetIntensity() const;
	Float3 GetColor() const;
	Float4 GetColorIntensity() const;
	float GetNearClip() const;
	float GetFarClip() const;
	float GetViewWidth() const;
	float GetViewHeight() const;
	Float4x4 GetViewMatrix() const;
	Float4x4 GetProjectionMatrix();

	// Shadow cascade properties:
	float GetMaxShadowDistance() const;
	ShadowCascadeCount GetShadowCascadeCount() const;
	const float* const GetShadowCascadeSplits() const;

	// Overrides:
	void Start() override;
	void LateUpdate() override;
	const std::string ToString() const override;

private: // Methods:
	void UpdateProjectionMatrix();
};



#endif // __INCLUDE_GUARD_spotLight_h__