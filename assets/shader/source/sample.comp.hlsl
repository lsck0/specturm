struct Particle {
  float2 position;
  float2 velocity;
};

cbuffer Uniforms : register(b0, space2) {
  float delta_time;
  float gravity;
  uint  particle_count;
  float damping;
};

RWStructuredBuffer<Particle> particles : register(u0, space1);

[numthreads(64, 1, 1)]
void main(uint3 id: SV_DispatchThreadID) {
  if (id.x >= particle_count) return;

  Particle p = particles[id.x];

  p.velocity.y += gravity * delta_time;
  p.velocity   *= damping;
  p.position   += p.velocity * delta_time;

  // bounce off floor
  if (p.position.y < -1.0) {
    p.position.y = -1.0;
    p.velocity.y = -p.velocity.y * 0.6;
  }

  // wrap horizontally
  if (p.position.x < -1.0) p.position.x += 2.0;
  if (p.position.x > 1.0) p.position.x -= 2.0;

  particles[id.x] = p;
}
