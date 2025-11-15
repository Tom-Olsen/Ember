#pragma once
#include "mathConstants.hlsli"



// Vector operations:
float LinAlg_VectorToPlaneDistance(float3 vec, float3 planeNormal)
{
    return dot(vec, planeNormal) / dot(planeNormal, planeNormal);
}
float3 LinAlg_VectorToPlaneProjection(float3 vec, float3 planeNormal)
{
    return vec - LinAlg_VectorToPlaneDistance(vec, planeNormal) * planeNormal;
}
bool LinAlg_IsEpsilonEqual(float3 a, float3 b)
{
    float epsilon = 1e-4f;
    return abs(a.x - b.x) < epsilon && abs(a.y - b.y) < epsilon && abs(a.z - b.z) < epsilon;
}

float3 LinAlg_GetOrhtogonalVector(float3 v)
{
    float epsilon = 1e-4f;
    float3 result = cross(v, float3(1.0f, 0.0f, 0.0f));
    if (length(result) < epsilon)
    {
        result = cross(v, float3(0.0f, 1.0f, 0.0f));
        if (length(result) < epsilon)
            result = cross(v, float3(0.0f, 0.0f, 1.0f));
    }
    return result;
}
float LinAlg_Angle(float3 a, float3 b)
{
    float epsilon = 1e-4f;
    float lengths = length(a) * length(b);
    if (lengths <= epsilon)
        return 0.0f;
    return acos(clamp(dot(a, b) / lengths, -1.0f, 1.0f));
}



// Matrix inversion:
float4x4 LinAlg_Invert(float4x4 input)
{
#define minor(a,b,c) determinant(float3x3(input.a, input.b, input.c))
    float4x4 cofactors = float4x4(
         minor(_22_23_24, _32_33_34, _42_43_44),
        -minor(_12_13_14, _32_33_34, _42_43_44),
         minor(_12_13_14, _22_23_24, _42_43_44),
        -minor(_12_13_14, _22_23_24, _32_33_34),

        -minor(_21_23_24, _31_33_34, _41_43_44),
         minor(_11_13_14, _31_33_34, _41_43_44),
        -minor(_11_13_14, _21_23_24, _41_43_44),
         minor(_11_13_14, _21_23_24, _31_33_34),

         minor(_21_22_24, _31_32_34, _41_42_44),
        -minor(_11_12_14, _31_32_34, _41_42_44),
         minor(_11_12_14, _21_22_24, _41_42_44),
        -minor(_11_12_14, _21_22_24, _31_32_34),

        -minor(_21_22_23, _31_32_33, _41_42_43),
         minor(_11_12_13, _31_32_33, _41_42_43),
        -minor(_11_12_13, _21_22_23, _41_42_43),
         minor(_11_12_13, _21_22_23, _31_32_33));
#undef minor
    return cofactors / determinant(input);
}

float4x4 LinAlg_NormalMatrix(float4x4 localToWorldMatrix)
{
    return transpose(LinAlg_Invert(localToWorldMatrix));
}



// Translation/Rotation/Scale matrices:
float4x4 LinAlg_Translate(float3 translation)
{
    return float4x4
    (1.0f, 0.0f, 0.0f, translation.x,
     0.0f, 1.0f, 0.0f, translation.y,
     0.0f, 0.0f, 1.0f, translation.z,
     0.0f, 0.0f, 0.0f,          1.0f);
}
float3x3 LinAlg_RotateX3x3(float angle)
{
    float s, c;
    sincos(angle, s, c);
    return float3x3
	(1.0f, 0.0f, 0.0f,
	 0.0f,    c,   -s,
	 0.0f,    s,    c);
}
float4x4 LinAlg_RotateX4x4(float angle)
{
    float s, c;
    sincos(angle, s, c);
    return float4x4
	(1.0f, 0.0f, 0.0f, 0.0f,
	 0.0f,    c,   -s, 0.0f,
	 0.0f,    s,    c, 0.0f,
	 0.0f, 0.0f, 0.0f, 1.0f);
}
float4x4 LinAlg_Rotate4x4(float3 axis, float angle)
{
    float c = cos(angle);
    float s = sin(angle);
    float t = 1.0f - c;
    float3 normalizedAxis = normalize(axis);
    float x = normalizedAxis.x;
    float y = normalizedAxis.y;
    float z = normalizedAxis.z;
    return float4x4
    (    x * x * t + c, x * y * t - z * s, x * z * t + y * s, 0.0f,
     y * x * t + z * s,     y * y * t + c, y * z * t - x * s, 0.0f,
     z * x * t - y * s, z * y * t + x * s,     z * z * t + c, 0.0f,
                  0.0f,              0.0f,              0.0f, 1.0f);
}
float4x4 LinAlg_RotateFromTo(float3 from, float3 to)
{
    float3 f = normalize(from);
    float3 t = normalize(to);
    float3 diff = abs(t - f);
    float epsilon = 1e-4f;
    if (LinAlg_IsEpsilonEqual(f, t))
        return math_identity4x4;
    if (LinAlg_IsEpsilonEqual(f, -t))
        return LinAlg_Rotate4x4(LinAlg_GetOrhtogonalVector(f), math_PI);
    float3 axis = cross(from, to); // normalization not needed, as Rotate(...) will normalize it
    float angle = LinAlg_Angle(from, to);
    return LinAlg_Rotate4x4(axis, angle);
}
float4x4 LinAlg_Scale(float3 scale)
{
    return float4x4
     (scale.x,    0.0f,    0.0f, 0.0f,
         0.0f, scale.y,    0.0f, 0.0f,
         0.0f,    0.0f, scale.z, 0.0f,
         0.0f,    0.0f,    0.0f, 1.0f);
}
float4x4 LinAlg_Scale(float scale)
{
    return LinAlg_Scale(float3(scale, scale, scale));
}



// Get Translation/Rotation/Scale from matrix:
float3 LinAlg_GetTranslation(float4x4 TRS)
{
    return float3(TRS._12, TRS._13, TRS._14);
}
float3 LinAlg_GetScale(float4x4 TRS)
{
    return float3(length(TRS._11_21_31), length(TRS._12_22_32), length(TRS._13_23_33));
}
float3x3 LinAlg_GetRotation3x3(float4x4 TRS)
{
    float3 scale = LinAlg_GetScale(TRS);
    return float3x3(normalize(TRS._11_21_31 / scale), normalize(TRS._12_22_32 / scale), normalize(TRS._13_23_33 / scale));
}
float4x4 LinAlg_GetRotation4x4(float4x4 TRS)
{
    float3 scale = LinAlg_GetScale(TRS);
    float4 column0 = float4(normalize(TRS._11_21_31 / scale), 0);
    float4 column1 = float4(normalize(TRS._12_22_32 / scale), 0);
    float4 column2 = float4(normalize(TRS._13_23_33 / scale), 0);
    float4 column3 = float4(0, 0, 0, 1);
    return float4x4(column0, column1, column2, column3);
}