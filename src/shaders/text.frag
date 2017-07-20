// Version is defined by shader compiler, 330 for desktop and 300 es for mobile

in vec2 texCoords;

layout (location = 0) out vec4 fragColor;

uniform sampler2D tex;

void main() {
  fragColor = vec4(1, 1, 1, texture(tex, texCoords).r);
}
