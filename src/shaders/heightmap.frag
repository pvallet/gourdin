// Version is defined by shader compiler, 330 for desktop and 300 es for mobile

const vec3 lightDir = normalize(vec3 (1,0,1));

in vec2 uv;
in vec3 normal;

layout (location = 0) out vec3 color;

uniform sampler2D myTextureSampler;

void main(){
	color = texture( myTextureSampler, uv ).rgb *
		(0.5 + 0.5*dot(lightDir,normal));
}
