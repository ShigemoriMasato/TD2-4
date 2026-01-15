
struct VertexShaderOutput
{
    float32_t4 position : SV_POSITION;
    float32_t2 texcoord : TEXCOORD1;
    float32_t3 normal : NORMAL1;
    float32_t3 worldPosition : POSITION1;
};

struct PSOutput
{
    float4 color : SV_TARGET0;
};

PSOutput main(VertexShaderOutput input)
{
    PSOutput output;
    output.color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    return output;
}
