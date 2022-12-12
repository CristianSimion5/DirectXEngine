cbuffer MatrixBuffer 
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct VertexInputType
{
    float4 position : POSITION;
    //float4 color : COLOR;
    float3 normal : NORMAL;
    float2 texcoord : UV;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PixelInputType VertexShaderMain(VertexInputType input)
{
    PixelInputType output;
    
    input.position.w = 1.0f;
    
    matrix mvp = mul(projectionMatrix, mul(viewMatrix, worldMatrix));
    output.position = mul(mvp, input.position);

    //output.color = input.color;
    output.color = float4(normalize(mul((float3x3) worldMatrix, input.normal)) * 0.5f + 0.5f, 1.0f);
    
    return output;
}