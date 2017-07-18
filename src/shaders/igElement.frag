// Version is defined by shader compiler, 330 for desktop and 300 es for mobile

in vec2 texCoord;
in float layer;

// The vertex shader signals that the element is too close
in float discardFrag;

layout (location = 0) out vec4 color;

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
