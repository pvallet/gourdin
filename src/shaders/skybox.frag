// Version is defined by shader compiler, 330 for desktop and 300 es for mobile

in vec2 texCoord;
in float layer;

layout (location = 0) out vec3 color;

uniform sampler2DArray myTextureSampler;

void main() {
	color = texture( myTextureSampler, vec3(texCoord, layer)).rgb;
}
