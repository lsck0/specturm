struct VertInput {
    float4 pos : SV_POSITION;
};

float4 main(VertInput input) : SV_POSITION {
    return input.pos;
}
