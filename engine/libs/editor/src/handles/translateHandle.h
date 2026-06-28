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
		emberEcs::Transform* m_pTransform;

		// State:
		bool m_isDragging;
		SubHandle m_hoveredSubHandle;
		SubHandle m_activeSubHandle;
        Float3 m_dragStartPosition;
		Float3 m_dragAxisDir;
		Float3 m_dragPlaneNormal;

        // Rendering:
        emberCore::Material m_material;
        emberCore::Mesh m_arrowMesh;
        emberCore::Mesh m_quadMesh;

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
        void CreateArrowMesh();
	};
}