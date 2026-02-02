
struct DirectionalLight
{
    float4 color;
    float3 direction;
    float intensity;
};

struct PointLight
{
    float4 color;
    float3 position;
    float intensity;
    float radius;
    float decay;
    float2 pad;
};

float PointLightAttenuation(float3 lightPos, float3 objPos, float radius, float decay, float intensity)
{
    float distance = length(lightPos - objPos);
    return pow(saturate(-distance / radius + 1.0), decay) * intensity;
}

float PointLightAttenuationSimply(float3 lightPos, float3 objPos, float intensity)
{
    float distance = length(objPos - lightPos);
    return (1.0f - (distance / 10.0f - intensity)) / 2.0f;
}

float4 HalfLambert(float3 normal, float4 color, float4 lightColor, float3 direction, float intensity)
{
    float NdotL = dot(normalize(normal), -direction);
    float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
    float4 outputColor = color * lightColor * cos * intensity;
    outputColor.w = color.w;
    return outputColor;
}

float4 LambertReflectance(float3 normal, float4 color, float4 lightColor, float3 direction, float intensity)
{
    float NdotL = dot(normalize(normal), -direction);
    float cos = saturate(dot(normalize(normal), -direction));
    float4 outputColor = color * lightColor * cos * intensity;
    outputColor.w = color.w;
    return outputColor;
}


float4 PhongReflection(float3 cameraPos, float3 objPos, float3 lightDir, float3 normal, float shininess, float4 objColor, float4 lightedColor, float4 lightColor)
{
    float3 toEye = normalize(cameraPos - objPos);
    
    float3 reflectLight = reflect(lightDir, normalize(normal));
    
    float RdotE = dot(reflectLight, toEye);
    float specular = pow(saturate(RdotE), shininess);
    
    float3 specularColor = objColor.rgb * specular * lightColor.rgb;
    
    return float4(lightedColor.rgb + specularColor, lightedColor.w);
}


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
    int dirLightCount;
    int pointLightCount;
    int2 padding2;
};

StructuredBuffer<DirectionalLight> dirLights : register(t0);
StructuredBuffer<PointLight> pointLights : register(t1);

Texture2D textures[] : register(t8);
SamplerState gSampler : register(s0);

PSOutput main(PSInput input)
{
    PSOutput output;
    
    float4 textureColor = textures[textureIndex].Sample(gSampler, input.texCoord);
    float4 baseColor = materialColor * textureColor;
    
    output.color = baseColor / 4.0f;
    
    if(baseColor.a < 0.1f)
    {
        discard;
    }
    
    for (int j = 0; j < dirLightCount; j++)
    {
        DirectionalLight light = dirLights[j];
        if (light.intensity <= 0.0f)
            continue;
        output.color += HalfLambert(input.normal, baseColor, light.color, light.direction, light.intensity);
    }
    
    for (int i = 0; i < pointLightCount; i++)
    {
        PointLight light = pointLights[i];
        if (light.intensity <= 0.0f)
            continue;
        float power = PointLightAttenuation(light.position, input.worldPos, light.radius, light.decay, light.intensity);
        output.color += HalfLambert(input.normal, baseColor, light.color, input.worldPos - light.position, power);
    }
    
    output.color.a = baseColor.a;
    
    if(pointLightCount + dirLightCount == 0)
    {
        output.color = baseColor;
    }
    
    return output;
}
