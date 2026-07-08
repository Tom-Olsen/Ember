#pragma once
#include "emberMath.h"
#include "mesh.h"



// Forward declarations:
namespace emberEcs
{
	class Transform;
}



namespace emberEditor
{
	class ScaleHandle
	{
	public: // Enums:
		enum class SubHandle
		{
			none,
			axisX,
			axisY,
			axisZ,
			center
		};

	private: // Members:
		float m_handleScale;
		emberEcs::Transform* m_pTransform;
		// Handle colors:
		static Float4 s_colorX;
		static Float4 s_colorY;
		static Float4 s_colorZ;
		static Float4 s_colorCenter;
		static Float4 s_hoverColor;
		static Float4 s_activeColor;
		// Rotation matrizes:
		static Float4x4 s_rotX;
		static Float4x4 s_rotY;
		static Float4x4 s_rotZ;
		// Geometry:
		static float s_axisLength;
		static float s_axisWidth;
		static float s_axisCornerCount;
		static float s_cubeWidth;
		static float s_minScaleMagnitude;

		// State:
		bool m_isDragging;
		SubHandle m_hoveredSubHandle;
		SubHandle m_activeSubHandle;
		Float3 m_dragStartPosition;
		Float3 m_dragStartScale;
		Float3 m_dragStartHitPoint;
		Float3 m_dragAxisDir;
		Float3 m_dragPlaneNormal;
		Float2 m_dragStartMousePos;
		float m_dragStartHandleSize;
		float m_grabAxisFraction;
		float m_dragAxisLengthFactor;

		// Meshes:
		emberCore::Mesh m_zylinderMesh;
		emberCore::Mesh m_capsuleMesh;
		emberCore::Mesh m_cubeMesh;
		emberCore::Mesh m_sphereMesh;

	public: // Methods:
		// Constructor/Destructor:
		ScaleHandle();
		~ScaleHandle();

		// Target:
		void SetTarget(emberEcs::Transform* pTransform);
		void ClearTarget();
		bool HasTarget() const;

		// Handle:
		SubHandle GetHoveredSubHandle() const;
		SubHandle GetActiveSubHandle() const;
		bool GetIsDragging() const;

		// Update/Draw:
		void Update();
		void Draw();

	private: // Methods:
		void ResetInteractionState();
		void CreateMeshes();
		void TryBeginDrag();
		void UpdateDrag();
		void UpdateHoveredSubHandle();

		// Helpers:
		float Size() const;
		float SizeAtPosition(const Float3& position) const;
		Float4x4 HandleRotationMatrix() const;
		Float4x4 LocalToWorldMatrix();
		Float4 SubHandleColor(ScaleHandle::SubHandle subHandle, const Float4& baseColor);
		float AxisLengthFactor(ScaleHandle::SubHandle subHandle) const;
		Float4x4 AxisLengthMatrix(ScaleHandle::SubHandle subHandle) const;
		void SetScale(ScaleHandle::SubHandle subHandle, float amount);
		void TryPickAxisSubHandle(ScaleHandle::SubHandle subHandle, const Float4x4& localToWorldMatrix, const Ray& ray, float& closestHitDistanceSq);
		void TryPickCubeSubHandle(ScaleHandle::SubHandle subHandle, const Float3& cubeWorldPosition, const Ray& ray, float& closestHitDistanceSq);

		// Static helpers:
		static Float3 SubHandleDirection(ScaleHandle::SubHandle subHandle);
		static Float4 SubHandleColor(ScaleHandle::SubHandle subHandle);
		static bool IsAxisSubHandle(ScaleHandle::SubHandle subHandle);
		static bool IsCenterSubHandle(ScaleHandle::SubHandle subHandle);
	};
}