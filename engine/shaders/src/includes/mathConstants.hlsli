#ifndef __INCLUDE_GUARD_mathConstants_hlsli__
#define __INCLUDE_GUARD_mathConstants_hlsli__



// Irrationals:
static const float math_PI = 3.1415927f;
static const float math_PI_2 = 0.500f * math_PI;
static const float math_PI_4 = 0.250f * math_PI;
static const float math_PI_8 = 0.125f * math_PI;
static const float math_PI_INV = 1.0f / math_PI;
static const float math_PHI = 1.618034f;
static const float math_E = 2.7182818f;
static const float math_GOLDEN_ANGLE = 2.3999632f;
static const float math_SQRT2 = 1.41421356f;
static const float math_SQRT2_INV = 0.70710678f;
static const float math_SQRT3 = 1.7320508f;
static const float math_SQRT3_INV = 0.57735027f;

// Zero and One:
static const float2 math_zero2 = float2(0, 0);
static const float3 math_zero3 = float3(0, 0, 0);
static const float4 math_zero4 = float4(0, 0, 0, 0);
static const float2 math_one2 = float2(1, 1);
static const float3 math_one3 = float3(1, 1, 1);
static const float4 math_one4 = float4(1, 1, 1, 1);
static const float2x2 math_identity2x2 = float2x2(1, 0, 0, 1);
static const float3x3 math_identity3x3 = float3x3(1, 0, 0, 0, 1, 0, 0, 0, 1);
static const float4x4 math_identity4x4 = float4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

// Colors:
static const float4 color_white   = float4(1.0f, 1.0f, 1.0f, 1.0f);
static const float4 color_gray    = float4(0.5f, 0.5f, 0.5f, 1.0f);
static const float4 color_black   = float4(0.0f, 0.0f, 0.0f, 1.0f);
static const float4 color_red     = float4(1.0f, 0.0f, 0.0f, 1.0f);
static const float4 color_green   = float4(0.0f, 1.0f, 0.0f, 1.0f);
static const float4 color_blue    = float4(0.0f, 0.0f, 1.0f, 1.0f);
static const float4 color_yellow  = float4(1.0f, 1.0f, 0.0f, 1.0f);
static const float4 color_cyan    = float4(0.0f, 1.0f, 1.0f, 1.0f);
static const float4 color_magenta = float4(1.0f, 0.0f, 1.0f, 1.0f);
static const float4 color_orange  = float4(1.0f, 0.5f, 0.0f, 1.0f);



#endif // __INCLUDE_GUARD_mathConstants_hlsli__