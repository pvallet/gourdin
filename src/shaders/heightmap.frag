#version 130

const vec3 lightDir = normalize(vec3 (1,0,1));

in vec2 uv;
in vec3 normal;

out vec3 color;

uniform sampler2D myTextureSampler;

void main(){
	color = texture2D( myTextureSampler, uv ).rgb * 
		(0.5 + 0.5*dot(lightDir,normal));
}
