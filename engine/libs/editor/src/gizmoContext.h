#pragma once
#include "emberMath.h"



// Forward declarations:
namespace emberEcs
{
	class Camera;
}



namespace emberEditor
{
	struct SnapSettings
	{
		bool enabled = false;
		Float3 translationSnap = Float3(0.25f);
		float rotationSnap = math::deg2rad * 15.0f;
		Float3 scaleSnap = Float3(0.1f);
	};



	struct GizmoContext
	{
		emberEcs::Camera* pCamera = nullptr;
		Float2 viewportSize = Float2::zero;
		Float2 viewportMousePos = Float2::zero;
		Float2 viewportMousePos01 = Float2::zero;
		bool isHovered = false;
		CoordinateSpace coordinateSpace = CoordinateSpace::world;
		SnapSettings snap;
	};
}