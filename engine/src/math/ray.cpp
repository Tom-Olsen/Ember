#include "ray.h"
#include "graphics.h"
#include <sstream>



namespace emberMath
{
	// Constructor:
	Ray::Ray(const Float3& origin, const Float3& direction) : origin(origin), direction(direction.Normalize()) {}
	Ray Ray::CameraRay(const Float2& mousePos01)
	{
		// Get camera inverse matrices:
		const emberEngine::Graphics::Camera& camera = emberEngine::Graphics::GetActiveCamera();
		Float4x4 projectionInv = camera.projectionMatrix.Inverse();
		Float4x4 viewInv = camera.viewMatrix.Inverse();

		// Convert mouse position to screen position:
		Float2 screenPos = 2.0f * mousePos01 - Float2::one;

		// Convert screenPos to NDC pos on near and far plane:
		Float4 nearPlaneNDC = Float4(screenPos, 0.0f, 1.0f);
		Float4 farPlaneNDC = Float4(screenPos, 1.0f, 1.0f);

		// Unproject NDC to camera space coordinates:
		Float4 nearCamera = projectionInv * nearPlaneNDC;
		Float4 farCamera = projectionInv * farPlaneNDC;
		nearCamera /= nearCamera.w;
		farCamera /= farCamera.w;

		// Convert camera space to world space coordinates:
		Float3 nearWorld = Float3(viewInv * nearCamera);
		Float3 farWorld = Float3(viewInv * farCamera);

		return Ray(nearWorld, (farWorld - nearWorld).Normalize());
	}



	// Methods:
	Float3 Ray::GetPoint(float distance) const
	{
		return origin + distance * direction;
	}



	// Logging:
	std::string Ray::ToString() const
	{
		std::ostringstream oss;
		oss << "origin: " << origin.ToString() << ", direction: " << direction.ToString();
		return oss.str();
	}
	std::ostream& operator<<(std::ostream& os, const Ray& ray)
	{
		os << ray.ToString();
		return os;
	}
}