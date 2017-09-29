// Version is defined by shader compiler, 330 for desktop and 300 es for mobile

in vec2 texCoords;

layout (location = 0) out vec4 depthVec;

uniform sampler2D tex;

vec4 pack_depth(const in float depth) {
  const vec4 bit_shift = vec4(256.0*256.0*256.0, 256.0*256.0, 256.0, 1.0);
  const vec4 bit_mask  = vec4(0.0, 1.0/256.0, 1.0/256.0, 1.0/256.0);
  vec4 res = fract(depth * bit_shift);
  res -= res.xxyz * bit_mask;
  return res;
}

void main() {
  depthVec = pack_depth(texture(tex, texCoords).r);
}
