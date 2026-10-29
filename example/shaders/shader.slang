static float2 positions[3] = float2[](
    float2(0.0, -0.5),
    float2(0.5, 0.5),
    float2(-0.5, 0.5)
);

static float3 colours[3] = float3[](
    float3(1.0, 0.0, 0.0),
    float3(0.0, 1.0, 0.0),
    float3(0.0, 0.0, 1.0)
);

struct VSInput {
    float2 inPosition;
    float3 inColour;
};

struct UniformBuffer {
    float4x4 model;
    float4x4 view;
    float4x4 proj;
};
ConstantBuffer<UniformBuffer> ubo;

struct VSOutput {
    float4 sv_position : SV_Position;
    float3 colour;
};

[shader("vertex")]
VSOutput vertMain(VSInput in) {
    float4x4 yflip = float4x4(1,0,0,0, 0,-1,0,0, 0,0,1,0, 0,0,0,1);

    VSOutput out;
    out.sv_position = mul(yflip, mul(ubo.proj, mul(ubo.view, mul(ubo.model, float4(in.inPosition, 0.0, 1.0)))));
    out.colour = in.inColour;
    return out;
}

[shader("fragment")]
float4 fragMain(VSOutput inVert) : SV_Target {
    float3 colour = inVert.colour;
    return float4(inVert.colour, 1.0);
}
