struct VertInput
{
    float3 pos   : POSITION;
    float4 color : COLOR0;
};

struct VertOutput
{
    float4 pos   : SV_POSITION;
    float4 color : COLOR0;
};

cbuffer Uniforms
{
    float4 pos   : POSITION;
}

VertOutput main(VertInput input)
{
    VertOutput output;

    output.pos = float4(input.pos, 1.0);
    output.color = input.color;

    return output;
}
