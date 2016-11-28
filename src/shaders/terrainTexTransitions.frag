#version 130

const vec3 lightDir = normalize(vec3 (1,0,1));

in vec2 uv;
in vec3 normal;
in float alpha;

out vec4 color;

uniform sampler2D myTextureSampler;

void main(){
	color.rgb = texture2D( myTextureSampler, uv ).rgb *
		(0.5 + 0.5*dot(lightDir,normal));

	color.a = alpha;

  if (color.a < 0)
    color.a = 0;
  else if (color.a > 1)
    color.a = 1;
}
