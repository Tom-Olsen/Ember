#pragma once
#include "emberMath.h"
#include "mesh.h"
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
	class RotateHandle
	{
	public: // Enums:
		enum class SubHandle
		{
			none,
			axisX,
			axisY,
			axisZ
		};

	private: // Members:
        float m_handleScale;
		CoordinateSpace m_coordinateSpace;
		emberEcs::Transform* m_pTransform;
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
        // Geometry:
		static float s_arcStart;
		static float s_arcEnd;
		static float s_arcWidth;
		static float s_arcCornerCount;

		// State:
		bool m_isDragging;
		SubHandle m_hoveredSubHandle;
		SubHandle m_activeSubHandle;
		uint32_t m_octantIndex;

		// Meshes:
		emberCore::Mesh m_arcMesh;

	public: // Methods:
		// Constructor/Destructor:
		RotateHandle();
		~RotateHandle();

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
		Float4 SubHandleStateColor(RotateHandle::SubHandle subHandle);
		void TryPickAxisSubHandle(RotateHandle::SubHandle subHandle, const Float4x4& localToWorldMatrix, const Ray& ray, float& closestHitDistanceSq);

        // Static helpers:
		static Float3 SubHandleDirection(RotateHandle::SubHandle subHandle);
		static Float4 SubHandleBaseColor(RotateHandle::SubHandle subHandle);
		static Float3 OctantSigns(uint32_t octantIndex);
		static Float4x4 OctantMatrix(RotateHandle::SubHandle subHandle, uint32_t octantIndex);
	};
}