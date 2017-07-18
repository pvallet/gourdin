// Version is defined by shader compiler, 330 for desktop and 300 es for mobile

in vec2 texCoord;

out vec3 color;

uniform sampler2D renderedTexture;

void main() {
  color = texture( renderedTexture, texCoord ).rgb;
}
