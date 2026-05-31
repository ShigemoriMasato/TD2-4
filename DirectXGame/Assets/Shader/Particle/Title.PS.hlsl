
struct PSInput
{
    float4 position : SV_POSITION;
    uint instanceID : INSTANCE0;
};

struct PSOutput
{
    float4 color : SV_TARGET;
};

StructuredBuffer<float4> colors : register(t0);

PSOutput main(PSInput input)
{
    PSOutput output;
    output.color = colors[input.instanceID];
    if (output.color.a <= 0.1f)
    {
        discard;
    }
    return output;
}
