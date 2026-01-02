#include "PostEffect.hlsli"

Texture2D<float4> gTexture[1024] : register(t8);
SamplerState gSampler : register(s0);

cbuffer TextureIndexBuffer : register(b0)
{
    uint textureIndex;
}

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;
    output.color = gTexture[textureIndex].Sample(gSampler, input.texcoord);
    return output;
}
