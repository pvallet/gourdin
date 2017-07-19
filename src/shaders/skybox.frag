// Version is defined by shader compiler, 330 for desktop and 300 es for mobile

in vec2 texCoords;

in float layer;

layout (location = 0) out vec3 fragColor;

uniform sampler2DArray tex;

void main() {
	fragColor = texture( tex, vec3(texCoords, layer)).rgb;
}
