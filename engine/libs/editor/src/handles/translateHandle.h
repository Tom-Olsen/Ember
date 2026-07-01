#pragma once
#include "emberMath.h"
#include "material.h"
#include "mesh.h"
#include <memory>



// Forward declarations:
namespace emberEcs
{
	class Transform;
}



namespace emberEditor
{
	class TranslateHandle
	{
	public: // Enums:
		enum class SubHandle
		{
			none,
			axisX,
			axisY,
			axisZ,
			planeXY,
			planeYZ,
			planeXZ
		};

	private: // Members:
        float m_handleScale;
		CoordinateSpace m_coordinateSpace;
        // Handle colors:
		static Float4 s_colorX;
		static Float4 s_colorY;
		static Float4 s_colorZ;
        static Float4 s_hoverColor;
        static Float4 s_activeColor;
        // Rotation matrizes:
		static Float4x4 s_rotX;
		static Float4x4 s_rotY;
		static Float4x4 s_rotZ;
        // Interaction capsule geometry:
		static float s_capsuleStart;
		static float s_capsuleEnd;
		static float s_capsuleWidth;
        // Arrow handle geometry:
		static float s_arrowBodyHeight;
		static float s_arrowBodyRadius;
		static float s_arrowHeadHeight;
		static float s_arrowHeadRadius;
		static float s_arrowCornerCount;
		emberEcs::Transform* m_pTransform;

		// State:
		bool m_isDragging;
		SubHandle m_hoveredSubHandle;
		SubHandle m_activeSubHandle;
		Float3 m_dragStartPosition;
		Float3 m_dragStartHitPoint;
		Float3 m_dragAxisDir;
		Float3 m_dragPlaneNormal;

		// Meshes:
		emberCore::Mesh m_arrowMesh;
		emberCore::Mesh m_capsuleMesh;
		emberCore::Mesh m_quadMesh;
        emberCore::Mesh m_quadOutlineMesh;

	public: // Methods:
		// Constructor/Destructor:
		TranslateHandle();
		~TranslateHandle();

		// Target:
		void SetTarget(emberEcs::Transform* pTransform);
		void ClearTarget();
		bool HasTarget() const;

		// Coordinate space:
		void SetCoordinateSpace(CoordinateSpace coordinateSpace);
		CoordinateSpace GetCoordinateSpace() const;

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
		void EndDrag();
		void UpdateHoveredSubHandle();
		
		// Helpers:
		float Size() const;
		Float4x4 HandleRotationMatrix() const;
		Float4x4 LocalToWorldMatrix();
		Float4 SubHandleColor(TranslateHandle::SubHandle subHandle, const Float4& baseColor);
		void TryPickAxisSubHandle(TranslateHandle::SubHandle subHandle, const Float4x4& axisLocalToWorldMatrix, const Ray& ray, float& closestHitDistanceSq);
		void TryPickPlaneSubHandle(TranslateHandle::SubHandle subHandle, const Float4x4& localToWorldMatrix, const Ray& ray, float& closestHitDistanceSq);

        // Static helpers:
		static Float3 SubHandleDirection(TranslateHandle::SubHandle subHandle);
		static bool IsAxisSubHandle(TranslateHandle::SubHandle subHandle);
		static bool IsPlaneSubHandle(TranslateHandle::SubHandle subHandle);
	};
}