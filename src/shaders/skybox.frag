#version 130

in vec2 texCoord;
in float layer;

out vec3 color;

uniform sampler2DArray myTextureSampler;

void main() {
	color = texture( myTextureSampler, vec3(texCoord, layer)).rgb;
}
