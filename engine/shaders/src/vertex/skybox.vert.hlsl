#include "vertexShaderCommon.hlsli"



struct VertexInput
{
    float3 position : POSITION; // position in local/model sapce
};
struct VertexOutput
{
    float4 clipPosition : SV_POSITION;  // position in clip space: x,y€[-1,1] z€[0,1]
    float3 localPos : TEXCOORD1;        // position in local space
};



VertexOutput main(VertexInput input)
{
    float4 pos = float4(input.position, 1.0);
    float4x4 fakeView = camera_viewMatrix;
    fakeView[0][3] = fakeView[1][3] = fakeView[2][3] = 0.0f;
    float4x4 mat = mul(camera_projMatrix, fakeView);
    
    VertexOutput output;
    output.clipPosition = mul(mat, pos);
    output.localPos = mul(LinAlg_RotateX3x3(-math_PI_2), input.position);
    return output;
}