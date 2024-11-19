cbuffer LightMatrizes : register(b0)
{
    float4x4 localToClipMatrixTest[2];  // local to camera clip space matrix: (projection * view * model)
};



struct VertexInput
{
    float3 position : POSITION;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
};



VertexOutput main(VertexInput input)
{
    float4 pos = float4(input.position, 1.0);
    pos.w = 1.0f;
    
    VertexOutput output;
    output.position = mul(localToClipMatrixTest[0], pos);
    return output;
}