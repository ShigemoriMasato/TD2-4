#include "../LightFuncs.hlsli"

struct PSInput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 worldPos : WORLDPOS0;
};

struct PSOutput
{
    float4 color : SV_TARGET;
};

cbuffer Material : register(b0)
{
    float4 materialColor;
    int textureIndex;
    int3 padding;
};

cbuffer LightCount : register(b1)
{
    int pointLightCount;
    int dirLightCount;
    int2 padding2;
};

StructuredBuffer<PointLight> pointLights : register(t0);
StructuredBuffer<DirectionalLight> dirLights : register(t1);

Texture2D textures[] : register(t8);
SamplerState gSampler : register(s0);

PSOutput main(PSInput input)
{
    PSOutput output;
    
    float4 textureColor = textures[textureIndex].Sample(gSampler, input.texCoord);
    float4 baseColor = materialColor * textureColor;
    
    if(baseColor.a < 0.1f)
    {
        discard;
    }
    
    for(int i = 0; i < pointLightCount; i++)
    {
        PointLight light = pointLights[i];
        float power = PointLightAttenuation(light.position, input.worldPos, light.radius, light.decay, light.intensity);
        output.color += HalfLambert(input.normal, baseColor, light.color, input.worldPos - light.position, power);
    }
    
    for (int j = 0; j < dirLightCount; j++)
    {
        DirectionalLight light = dirLights[j];
        output.color += HalfLambert(input.normal, baseColor, light.color, light.direction, light.intensity);
    }
    
    output.color.a = baseColor.a;
    
    if(pointLightCount + dirLightCount == 0)
    {
        output.color = baseColor;
    }
    
    return output;
}
