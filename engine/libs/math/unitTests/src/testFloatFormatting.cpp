#include "emberMath.h"
#include <gtest/gtest.h>
#include <sstream>
#include <string>



TEST(FloatFormatting, Vectors)
{
	EXPECT_EQ(Float2(1.25f, -2.5f).ToString("%.3f"), "(1.250, -2.500)");
	EXPECT_EQ(Float3(-5.12264e-06f, -6.83019e-06f, 3.41509e-06f).ToString("%.6f"), "(-0.000005, -0.000007, 0.000003)");
	EXPECT_EQ(Float4(1.0f, -2.0f, 3.5f, 4.0f).ToString("%+.2f"), "(+1.00, -2.00, +3.50, +4.00)");
	EXPECT_EQ(Float2(1.0f, 2.0f).ToString(), "(1, 2)");
	EXPECT_EQ(Float4(1.0f, 2.0f, 3.0f, 4.0f).ToString(), "(1, 2, 3, 4)");
}

TEST(FloatFormatting, Float2x2)
{
	Float2x2 matrix = Float2x2::Columns(1.0f, 2.0f, 3.0f, 4.0f);
	EXPECT_EQ(matrix.ToString("%.1f"), "(1.0, 2.0 | 3.0, 4.0)");
	EXPECT_EQ(matrix.ToStringMatrixForm("%.1f"), "\n(1.0, 3.0)\n(2.0, 4.0)");
	EXPECT_EQ(matrix.ToString(), "(1, 2 | 3, 4)");
	EXPECT_EQ(matrix.ToStringMatrixForm(), "\n(1, 3)\n(2, 4)");
}

TEST(FloatFormatting, Float2x3)
{
	Float2x3 matrix = Float2x3::Columns(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f);
	EXPECT_EQ(matrix.ToString("%.1f"), "(1.0, 2.0 | 3.0, 4.0 | 5.0, 6.0)");
	EXPECT_EQ(matrix.ToStringMatrixForm("%.1f"), "\n(1.0, 3.0, 5.0)\n(2.0, 4.0, 6.0)");
	EXPECT_EQ(matrix.ToString(), "(1, 2 | 3, 4 | 5, 6)");
	EXPECT_EQ(matrix.ToStringMatrixForm(), "\n(1, 3, 5)\n(2, 4, 6)");
}

TEST(FloatFormatting, Float3x2)
{
	Float3x2 matrix = Float3x2::Columns(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f);
	EXPECT_EQ(matrix.ToString("%.1f"), "(1.0, 2.0, 3.0 | 4.0, 5.0, 6.0)");
	EXPECT_EQ(matrix.ToStringMatrixForm("%.1f"), "\n(1.0, 4.0)\n(2.0, 5.0)\n(3.0, 6.0)");
	EXPECT_EQ(matrix.ToString(), "(1, 2, 3 | 4, 5, 6)");
	EXPECT_EQ(matrix.ToStringMatrixForm(), "\n(1, 4)\n(2, 5)\n(3, 6)");
}

TEST(FloatFormatting, Float3x3)
{
	Float3x3 matrix = Float3x3::Columns(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
	EXPECT_EQ(matrix.ToString("%.1f"), "(1.0, 2.0, 3.0 | 4.0, 5.0, 6.0 | 7.0, 8.0, 9.0)");
	EXPECT_EQ(matrix.ToStringMatrixForm("%.1f"), "\n(1.0, 4.0, 7.0)\n(2.0, 5.0, 8.0)\n(3.0, 6.0, 9.0)");
	EXPECT_EQ(matrix.ToString(), "(1, 2, 3 | 4, 5, 6 | 7, 8, 9)");
	EXPECT_EQ(matrix.ToStringMatrixForm(), "\n(1, 4, 7)\n(2, 5, 8)\n(3, 6, 9)");
}

TEST(FloatFormatting, Float4x4)
{
	Float4x4 matrix = Float4x4::Columns
	(
		1.0f, 2.0f, 3.0f, 4.0f,
		5.0f, 6.0f, 7.0f, 8.0f,
		9.0f, 10.0f, 11.0f, 12.0f,
		13.0f, 14.0f, 15.0f, 16.0f
	);
	EXPECT_EQ(matrix.ToString("%.1f"), "(1.0, 2.0, 3.0, 4.0 | 5.0, 6.0, 7.0, 8.0 | 9.0, 10.0, 11.0, 12.0 | 13.0, 14.0, 15.0, 16.0)");
	EXPECT_EQ(matrix.ToStringMatrixForm("%.1f"), "\n(1.0, 5.0, 9.0, 13.0)\n(2.0, 6.0, 10.0, 14.0)\n(3.0, 7.0, 11.0, 15.0)\n(4.0, 8.0, 12.0, 16.0)");
	EXPECT_EQ(matrix.ToString(), "(1, 2, 3, 4 | 5, 6, 7, 8 | 9, 10, 11, 12 | 13, 14, 15, 16)");
	EXPECT_EQ(matrix.ToStringMatrixForm(), "\n(1, 5, 9, 13)\n(2, 6, 10, 14)\n(3, 7, 11, 15)\n(4, 8, 12, 16)");
}

TEST(FloatFormatting, Angles)
{
	EXPECT_EQ(Radians(1.25f).ToString("%08.2f"), "00001.25 rad");
	EXPECT_EQ(Degrees(15.0f).ToString("%+.2e"), "+1.50e+01 deg");
	EXPECT_EQ(EulerRadians(0.25f, -0.5f, 1.0f).ToString("%.3f"), "(0.250, -0.500, 1.000) rad");
	EXPECT_EQ(EulerDegrees(15.0f, -30.0f, 45.0f).ToString("%.2f"), "(15.00, -30.00, 45.00) deg");
}

TEST(FloatFormatting, LargeWidth)
{
	std::string expectedValue = std::string(196, ' ') + "1.00";
	EXPECT_EQ(Float2(1.0f).ToString("%200.2f"), "(" + expectedValue + ", " + expectedValue + ")");
	EXPECT_EQ(Radians(1.0f).ToString("%200.2f"), expectedValue + " rad");
	EXPECT_EQ(Degrees(1.0f).ToString("%200.2f"), expectedValue + " deg");
	EXPECT_EQ(Float2x2(1.0f).ToString("%200.2f"), "(" + expectedValue + ", " + expectedValue + " | " + expectedValue + ", " + expectedValue + ")");
}

TEST(FloatFormatting, StreamDefaults)
{
	std::ostringstream stream;
	stream << Float2(1.0f, 2.0f) << ' ' << Float2x2::identity << ' ' << Radians(1.0f) << ' ' << EulerRadians(0.0f, 1.0f, 2.0f);
	EXPECT_EQ(stream.str(), "(1, 2) (1, 0 | 0, 1) 1 rad (0, 1, 2) rad");
}

TEST(FloatFormatting, GeometryVectors)
{
	EXPECT_EQ(Bounds(Float3(1.25f), Float3(2.5f)).ToString("%.2f"), "Bounds(center: (1.25, 1.25, 1.25), extent: (2.50, 2.50, 2.50))");
	EXPECT_EQ(Bounds2d(Float2(1.25f), Float2(2.5f)).ToString("%.2f"), "Bounds2d(center: (1.25, 1.25), extents: (2.50, 2.50))");
	EXPECT_EQ(Plane(Float3::zero, Float3::up).ToString("%.1f"), "Plane(support: (0.0, 0.0, 0.0), normal: (0.0, 0.0, 1.0))");
	EXPECT_EQ(Quad(Float3::zero, Float3::right, Float3::forward).ToString("%.1f"), "Quad(origin: (0.0, 0.0, 0.0), uAxis: (1.0, 0.0, 0.0), vAxis: (0.0, 1.0, 0.0))");
	EXPECT_EQ(Ray(Float3(-5.12264e-06f, 0.0f, 1.25f), Float3::forward).ToString("%.6f"), "origin: (-0.000005, 0.000000, 1.250000), direction: (0.000000, 1.000000, 0.000000)");
	EXPECT_EQ(Triangle(Float3::zero, Float3::right, Float3::forward).ToString("%.1f"), "Triangle(a: (0.0, 0.0, 0.0), b: (1.0, 0.0, 0.0), c: (0.0, 1.0, 0.0))");
}

TEST(FloatFormatting, GeometryRadii)
{
	EXPECT_EQ(Capsule(Float3::zero, Float3::up, 1.25f).ToString("%.3f"), "Capsule(point0: (0.000, 0.000, 0.000), point1: (0.000, 0.000, 1.000), radius: 1.250)");
	EXPECT_EQ(Cone(Float3::zero, Float3::up, 0.126f).ToString("%.2f"), "Cone(apex: (0.00, 0.00, 0.00), baseCenter: (0.00, 0.00, 1.00), radius: 0.13)");
	EXPECT_EQ(Sphere(Float3::zero, 1.234567e-05f).ToString("%.6f"), "Sphere(center: (0.000000, 0.000000, 0.000000), radius: 0.000012)");
	std::string expectedValue = std::string(196, ' ') + "1.00";
	EXPECT_EQ(Sphere(Float3(1.0f), 1.0f).ToString("%200.2f"), "Sphere(center: (" + expectedValue + ", " + expectedValue + ", " + expectedValue + "), radius: " + expectedValue + ")");
}

TEST(FloatFormatting, GeometryDefaults)
{
	EXPECT_EQ(Bounds(Float3::zero, Float3::one).ToString(), "Bounds(center: (0, 0, 0), extent: (1, 1, 1))");
	EXPECT_EQ(Sphere(Float3::zero, 0.5f).ToString(), "Sphere(center: (0, 0, 0), radius: 0.5)");
	std::ostringstream stream;
	stream << Sphere(Float3::zero, 0.5f);
	EXPECT_EQ(stream.str(), "Sphere(center: (0, 0, 0), radius: 0.5)");
}