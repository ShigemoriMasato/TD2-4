struct Wave
{
    float2 direction;
    float amplitude;
    float wavelength;
    float speed;
    float3 pad;
};

cbuffer CBPerObject : register(b0)
{
    float4x4 world;
    float4x4 vp;
    float3 cameraPos;
    float time;
    int waveCount;
    float3 pad;
    Wave waves[8];
};

struct Position
{
    float3 localPos;
    float pad;
};
StructuredBuffer<Position> positions : register(t0);

struct VSInput
{
    float4 pos : POSITION0;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 worldPos : WORLDPOS0;
};

float kFromWavelength(float wavelength)
{
    return 2.0 * 3.14159265 / wavelength;
}

VSOutput main(VSInput input, uint id : SV_InstanceID)
{
    VSOutput o;
    //scale調整用
    float3 pos = positions[id].localPos;
    float3 displaced = pos;
    float3 tangent = float3(1, 0, 0);
    float3 bitangent = float3(0, 0, 1);

    // accumulate displacement and partial derivatives for normal
    float3 dPos_dx = float3(1, 0, 0);
    float3 dPos_dz = float3(0, 0, 1);

    for (int i = 0; i < waveCount; ++i)
    {
        if(waves[i].amplitude == 0.0f)
            continue;
        
        float2 D = normalize(waves[i].direction);
        float A = waves[i].amplitude;
        float L = waves[i].wavelength;
        float S = waves[i].speed;
        float k = kFromWavelength(L);
        float w = S * k; // angular frequency approximation

        float phase = dot(D, pos.xz) * k - w * time;
        float cosP = cos(phase);
        float sinP = sin(phase);

        // horizontal displacement
        displaced.x += D.x * (A * cosP);
        displaced.z += D.y * (A * cosP);
        displaced.y += A * sinP;

        //ここまじでわからん
        // partial derivatives for normal
        // ∂p/∂x and ∂p/∂z contributions
        // from derivation:
        dPos_dx.x += -D.x * D.x * A * k * sinP;
        dPos_dx.y += D.x * A * k * cosP;
        dPos_dx.z += -D.x * D.y * A * k * sinP;

        dPos_dz.x += -D.y * D.x * A * k * sinP;
        dPos_dz.y += D.y * A * k * cosP;
        dPos_dz.z += -D.y * D.y * A * k * sinP;
    }

    // compute normal from cross product of partials
    float3 normal = normalize(cross(dPos_dz, dPos_dx)); // note order
    pos = mul(input.pos.xyz, (float3x3) world) + positions[id].localPos;
    displaced.y = max(displaced.y, 0.5f);
    pos.y *= displaced.y;
    
    o.worldPos = pos.xyz;
    o.normal = normal; // transform normal correctly
    o.texCoord = input.uv;
    o.position = mul(float4(o.worldPos, 1.0f), vp);
    return o;
}
