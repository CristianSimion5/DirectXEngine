Texture2D tex : register(t0);
sampler texSampler : register(s0);

struct Material
{
    float4 emissive;
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float specularStrength;
    // TODO: padding?
};

cbuffer MaterialProperties : register(b0)
{
    Material material;
}

struct Light
{
    float4 position;
    //float4 direction;
    float4 color;
    float3 attenuation;
    bool enabled;
};

cbuffer LightProperties : register(b1)
{
    float4 eyePosition;
    float4 globalAmbient;
    Light lights[8];
    bool specularType;
}

/*
Naming conventions:
N - surface normal (world space normal)
L - normalised direction from surface point to light
V - normalised direction from surface point to eye
P - world position at the surface point (current pixel location)
*/

float4 computeDiffuse(Light light, float3 L, float3 N)
{
    return light.color * max(0.0f, dot(N, L));
}

float4 computeSpecular(Light light, float3 V, float3 L, float3 N)
{
    float weight;
    if (specularType)
    {
        float3 R = normalize(reflect(-L, N));
        weight = max(0.0f, dot(R, V));
    }
    else
    {
        float3 H = normalize(L + V);
        weight = max(0.0f, dot(N, H));
    }
    
    return light.color * pow(weight, material.specularStrength);
}

float computeAttenuation(Light light, float dist)
{
    return 1.0f / dot(light.attenuation, float3(1, dist, dist * dist));
}

struct LightResult
{
    float4 diffuse;
    float4 specular;
};

LightResult computeLight(Light light, float3 V, float4 P, float3 N)
{
    float3 L = (light.position - P).xyz;
    float dist = length(L);
    L /= dist;
    
    float attenuation = computeAttenuation(light, dist);
    
    LightResult result;
    result.diffuse = attenuation * computeDiffuse(light, L, N);
    result.specular = attenuation * computeSpecular(light, V, L, N);
    
    return result;
}

LightResult computeAllLights(float4 P, float3 N)
{
    float3 V = normalize((eyePosition - P).xyz);
    
    LightResult result =
    {
        { 0.0f, 0.0f, 0.0f, 0.0f},
        { 0.0f, 0.0f, 0.0f, 0.0f}
    };
    
    LightResult localResult;
    
    // Use unroll directive
    [unroll]
    for (int i = 0; i < 8; i++)
    {
        if (!lights[i].enabled)
            continue;
        
        localResult = computeLight(lights[i], V, P, N);
        result.diffuse += localResult.diffuse;
        result.specular += localResult.specular;
    }

    result.diffuse = saturate(result.diffuse);
    result.specular = saturate(result.specular);
    
    return result;
}

struct PixelInputType
{
    float4 positionWorld : TEXCOORD1;
    float3 normalWorld   : TEXCOORD2;
    float2 texCoord      : TEXCOORD0;
};

float4 main(PixelInputType input) : SV_TARGET
{
    LightResult lresult = computeAllLights(input.positionWorld, normalize(input.normalWorld));
    float4 emissive = material.emissive;
    float4 ambient = material.ambient * globalAmbient;
    float4 diffuse = material.diffuse * lresult.diffuse;
    float4 specular = material.specular * lresult.specular;
    
    float4 textureColor = tex.Sample(texSampler, input.texCoord);
    
    return textureColor * (emissive + ambient + diffuse + specular);
}