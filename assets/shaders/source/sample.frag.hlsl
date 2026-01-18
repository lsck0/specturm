struct VertOutput
{
    float4 pos   : SV_POSITION;
    float4 color : COLOR0;
};

float4 main(VertOutput input) : SV_TARGET
{
    return input.color;
}
