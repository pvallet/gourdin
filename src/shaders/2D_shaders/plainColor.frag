// Version is defined by shader compiler, 330 for desktop and 300 es for mobile

out vec4 fragColor;

uniform vec4 color;

void main() {
  fragColor = color;
}
