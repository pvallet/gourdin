// Version is defined by shader compiler, 330 for desktop and 300 es for mobile

in vec2 texCoord;

layout (location = 0) out float depth;

uniform sampler2D renderedTexture;

void main() {
  depth = texture( renderedTexture, texCoord ).r;
}
