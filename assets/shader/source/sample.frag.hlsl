struct VertOutput {
  float4 pos : SV_POSITION;
};

cbuffer Uniforms : register(b0, space3) {
  float now;
};

float4 main(VertOutput input) : SV_TARGET {
  float pulse = sin(now) * 0.5 + 0.5;

  return float4(pulse, 1.0, 1.0 - pulse, 1.0);
}
