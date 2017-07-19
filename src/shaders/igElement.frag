// Version is defined by shader compiler, 330 for desktop and 300 es for mobile

in vec2 texCoords;
in float layer;

// The vertex shader signals that the element is too close
in float discardFrag;

layout (location = 0) out vec4 fragColor;

uniform sampler2DArray tex;

uniform bool onlyOpaqueParts;

void main() {
	if (discardFrag > 0.f)
		discard;

	fragColor = texture(tex, vec3(texCoords, layer));

	if (onlyOpaqueParts) {
		if (fragColor.a < 0.9)
			discard;
	}

	else {
		if (fragColor.a == 0.f)
			discard;
	}
}
