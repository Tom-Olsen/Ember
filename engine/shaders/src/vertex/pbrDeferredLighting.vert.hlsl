struct VertexOutput
{
    float4 clipPosition : SV_POSITION;
};



VertexOutput main(uint vertexId : SV_VertexID)
{
    // No VertexInput, derive vertices from vertexId:
	// vertexId = 0: float2((0 << 1) & 2, 0 & 2) = float2(0 & 2, 0 & 2) = float2(0, 0)
	// vertexId = 1: float2((1 << 1) & 2, 1 & 2) = float2(2 & 2, 1 & 2) = float2(2, 0)
	// vertexId = 2: float2((2 << 1) & 2, 2 & 2) = float2(4 & 2, 2 & 2) = float2(0, 2)
    float2 position = float2((vertexId << 1) & 2, vertexId & 2);

    VertexOutput output;
    output.clipPosition = float4(2.0f * position - 1.0f, 0.0f, 1.0f);
    return output;
}