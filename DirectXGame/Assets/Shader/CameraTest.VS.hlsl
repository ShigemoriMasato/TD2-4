
struct TransformationMatrix
{
    float4x4 world;
    float4x4 vp;
};

ConstantBuffer<TransformationMatrix> gTransformMatrix : register(b0);

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

VSOutput main(VertexShaderInput input)
{
    VSOutput output;
    output.position = mul(input.position, mul(gTransformMatrix.world, gTransformMatrix.vp));
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3) gTransformMatrix.world));
    return output;
}
