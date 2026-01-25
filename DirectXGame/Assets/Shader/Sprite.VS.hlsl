#include"Sprite.hlsli"

cbuffer cbuff0 : register(b0)
{
    float32_t4 color;
    float32_t4x4 uvTransform;
    float32_t4x4 WVP;
};

VertexShaderOutput main(float4 position : POSITION, float2 texcoord : TEXCOORD, float3 normal : NORMAL0)
{
    VertexShaderOutput output;
    output.position = mul(position, WVP);
    output.texcoord = texcoord;
    return output;
}