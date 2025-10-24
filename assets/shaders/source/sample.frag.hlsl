struct VertInput {
    float4 pos : SV_POSITION;
};

float4 main(VertInput input) : SV_TARGET {
    return float4(1.0, 0.0, 0.0, 1.0);
}
