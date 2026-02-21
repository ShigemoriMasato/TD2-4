#include "../LightFunc.hlsli"

struct PSInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct PSOutput
{
    float4 color : SV_TARGET;
};

cbuffer TextureIndex : register(b0)
{
    int textureIndex;
};

cbuffer Color : register(b1)
{
    float4 color;
};

cbuffer Light : register(b2)
{
    DirectionalLight light;
};

Texture2D textures[] : register(t8);
SamplerState gSampler : register(s0);

PSOutput main(PSInput input) : SV_TARGET
{
    PSOutput output;
    
    //テクスチャとカラーの乗算
    float4 texColor = textures[textureIndex].Sample(gSampler, input.texcoord);
    float4 baseColor = texColor * color;
    
    //ライトの照射
    output.color = HalfLambert(input.normal, baseColor, light.color, light.direction, light.intensity);
    
    return output;
}
