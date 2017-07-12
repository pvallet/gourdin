// Version is defined by shader compiler, 130 for compatibility and 330 for core

in vec2 texCoord;
in float layer;

// The vertex shader signals that the element is too close
in float discardFrag;

out vec4 color;

uniform sampler2DArray myTextureSampler;

uniform bool onlyOpaqueParts;

void main() {
	if (discardFrag > 0.f)
		discard;

	color = texture( myTextureSampler, vec3(texCoord, layer));

	if (onlyOpaqueParts) {
		if (color.a < 0.9)
			discard;
	}

	else {
		if (color.a == 0.f)
			discard;
	}
}
