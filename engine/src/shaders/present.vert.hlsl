struct VertexInput
{
    float3 position : POSITION;         // position in local/model sapce
    float4 uv : TEXCOORD0;
};

struct VertexOutput
{
    float4 clipPosition : SV_POSITION;  // position in clip space: x,y€[-1,1] z€[0,1]
    float4 uv : TEXCOORD0;              // texture coordinates
};



VertexOutput main(VertexInput input)
{
    VertexOutput output;
    output.clipPosition = float4(input.position, 1);
    output.uv = input.uv;
    return output;
}