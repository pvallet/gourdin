// Version is defined by shader compiler, 330 for desktop and 300 es for mobile

const vec3 lightDir = normalize(vec3 (1,0,1));

in vec2 texCoords;
in vec3 normal;

layout (location = 0) out vec3 fragColor;

uniform sampler2D tex;

void main() {
	fragColor = texture( tex, texCoords ).rgb *
		(0.5 + 0.5*dot(lightDir,normal));
}
