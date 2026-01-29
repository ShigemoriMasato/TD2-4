#include"Sprite.hlsli"

cbuffer cbuff0 : register(b0)
{
    float32_t4 color;
    float32_t4x4 uvTransform;
    float32_t4x4 WVP;
    uint32_t textureIndex;
};

Texture2D<float32_t4> gTexture[] : register(t8);
SamplerState gSampler : register(s0);

float4 main(VertexShaderOutput input) : SV_TARGET
{
    //float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), uvTransform);
    float4 textureColor = gTexture[textureIndex].Sample(gSampler, input.texcoord);
    //float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    float32_t4 outputColor = textureColor * color;
    
    if (outputColor.a == 0.0f)
    {
        discard;
    }
    return color;
}