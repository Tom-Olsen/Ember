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
		static Float4x4 s_rotX;
		static Float4x4 s_rotY;
		static Float4x4 s_rotZ;
		static float s_capsuleStart;
		static float s_capsuleEnd;
		static float s_capsuleWidth;
		float m_handleScale;
		emberEcs::Transform* m_pTransform;

		// State:
		bool m_isDragging;
		SubHandle m_hoveredSubHandle;
		SubHandle m_activeSubHandle;
		Float3 m_dragStartPosition;
		Float3 m_dragStartHitPoint;
		Float3 m_dragAxisDir;
		Float3 m_dragPlaneNormal;

		// Rendering:
		emberCore::Material m_material;
		emberCore::Mesh m_arrowMesh;
		emberCore::Mesh m_quadMesh;
		emberCore::Mesh m_capsuleMesh;

	public: // Methods:
		// Constructor/Destructor:
		TranslateHandle();
		~TranslateHandle();

		// Target:
		void SetTarget(emberEcs::Transform* pTransform);
		void ClearTarget();
		bool HasTarget() const;

		// Handle:
		SubHandle GetHoveredSubHandle() const;
		SubHandle GetActiveSubHandle() const;
		bool IsDragging() const;

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
		Float4x4 LocalToWorldMatrix();
		Float4 SubHandleColor(TranslateHandle::SubHandle subHandle, const Float4& baseColor);
		void TryPickSubHandle(TranslateHandle::SubHandle subHandle, const Float4x4& axisLocalToWorldMatrix, const Ray& ray, float& closestHitDistanceSq);

        // Static helpers:
		static Float3 SubHandleAxisDirection(TranslateHandle::SubHandle subHandle);
	};
}