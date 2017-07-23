// Version is defined by shader compiler, 330 for desktop and 300 es for mobile

in vec2 texCoords;

out vec3 fragColor;

uniform sampler2D tex;

void main() {
  fragColor = texture( tex, texCoords ).rgb;
}
