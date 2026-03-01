struct VertInput {
  uint   id : SV_VertexID;
  float3 pos : POSITION;
  float4 color : COLOR0;
  float3 normals : NORMAL0;
  float2 uv : TEXCOORD0;
};

cbuffer Uniforms : register(b0, space1) {
  float now;
};

struct VertOutput {
  float4 pos : SV_POSITION;
};

VertOutput main(VertInput input) {
  VertOutput output;

  float pulse = sin(now * 5);

  if (input.id == 0) {
    output.pos = float4(pulse, 0.5, 0, 1.0);
  } else if (input.id == 1) {
    output.pos = float4(-0.5, pulse, 0, 1.0);
  } else if (input.id == 2) {
    output.pos = float4(0.5, -0.5, pulse, 1.0);
  }

  return output;
}
