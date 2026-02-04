#include "LightFuncs.hlsli"

struct PSInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 worldPos : WORLDPOS0;
    uint instanceID : INSTANCEID0;
};

struct PSOutput
{
    float4 color : SV_TARGET;
};

Texture2D<float4> gTexture[] : register(t8);
SamplerState gSampler : register(s0);

struct Color
{
    float4 color;
};
StructuredBuffer<Color> colors : register(t0);
struct TextureIndex
{
    int index;
};
StructuredBuffer<TextureIndex> textureIndices : register(t1);
StructuredBuffer<PointLight> pointLights : register(t2);

cbuffer LightNum : register(b0)
{
    int pointLightNum;
    int3 pad;
};

PSOutput main(PSInput input)
{
    PSOutput output;
    float4 color = colors[input.instanceID].color;
    int textureIndex = textureIndices[input.instanceID].index;
    color = gTexture[textureIndex].Sample(gSampler, input.texcoord) * color;
    output.color = color / 4.0f;
    output.color.a = 1.0f;
    
    for (int i = 0; i < pointLightNum; i++)
    {
        PointLight light = pointLights[i];
        if (light.intensity <= 0.0f)
            continue;
        float power = PointLightAttenuation(light.position, input.worldPos, light.radius, light.decay, light.intensity);
        output.color += HalfLambert(input.normal, color, light.color, input.worldPos - light.position, power);
    }
    
    return output;
}
