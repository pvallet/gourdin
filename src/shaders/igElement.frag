#version 130

in vec2 texCoord;
in float layer;

uniform sampler2DArray myTextureSampler;

uniform bool onlyOpaqueParts;

void main() {
	vec4 color = texture( myTextureSampler, vec3(texCoord, layer));

	if (onlyOpaqueParts) {
		if (color.a < 0.9)
			discard;
	}

	else {
		if (color.a == 0)
			discard;
	}

	gl_FragColor = color;
}
