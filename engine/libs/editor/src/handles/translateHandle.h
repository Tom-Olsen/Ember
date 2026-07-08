#pragma once
#include "emberMath.h"
#include "mesh.h"
#include <array>
#include <cstdint>



// Forward declarations:
namespace emberCore
{
	class ShaderProperties;
}
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
		emberEcs::Transform* m_pTransform;
        // Geometry:
		static float s_arrowHeight;
        static float s_quadSize;
		static float s_arrowBodyHeight;
		static float s_arrowBodyRadius;
		static float s_arrowHeadHeight;
		static float s_arrowHeadRadius;
		static float s_arrowCornerCount;

		// State:
		bool m_isDragging;
		SubHandle m_hoveredSubHandle;
		SubHandle m_activeSubHandle;
		Float3 m_dragStartPosition;
		Float3 m_dragStartHitPoint;
		Float3 m_dragAxisDir;
		Float3 m_dragPlaneNormal;
		Float3 m_dragGrabOffset;
		uint32_t m_octantIndex;

		// Meshes:
		emberCore::Mesh m_arrowMesh;
		emberCore::Mesh m_capsuleMesh;
		emberCore::Mesh m_arrowHeadCapsuleMesh;
		emberCore::Mesh m_quadMesh;
        std::array<emberCore::Mesh, 8> m_frameMeshes;

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
		void UpdateOctant();
		void UpdateHoveredSubHandle();
		
		// Helpers:
		float Size() const;
		float SizeAtPosition(const Float3& position) const;
		Float4x4 HandleRotationMatrix() const;
		Float4x4 LocalToWorldMatrix();
		Float4 SubHandleStateColor(TranslateHandle::SubHandle subHandle);
		void TryPickAxisSubHandle(TranslateHandle::SubHandle subHandle, const Float4x4& localToWorldMatrix, const Ray& ray, float& closestHitDistanceSq);
		void TryPickPlaneSubHandle(TranslateHandle::SubHandle subHandle, const Float4x4& localToWorldMatrix, const Ray& ray, float& closestHitDistanceSq);

        // Static helpers:
		static Float3 SubHandleDirection(TranslateHandle::SubHandle subHandle);
		static Float4 SubHandleBaseColor(TranslateHandle::SubHandle subHandle);
		static Float3 OctantSigns(uint32_t octantIndex);
		static Float4x4 PlaneQuadTranslation(TranslateHandle::SubHandle subHandle, uint32_t octantIndex, float size);
		static bool IsAxisSubHandle(TranslateHandle::SubHandle subHandle);
		static bool IsPlaneSubHandle(TranslateHandle::SubHandle subHandle);
	};
}