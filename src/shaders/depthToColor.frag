// Version is defined by shader compiler, 330 for desktop and 300 es for mobile

in vec2 texCoords;

layout (location = 0) out float depth;

uniform sampler2D tex;

void main() {
  depth = texture( tex, texCoords ).r;
}
