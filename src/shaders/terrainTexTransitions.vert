#version 130

in vec3 in_Vertex;
in vec2 in_TexCoord0;
in vec3 in_Normal;
in float in_Alpha_value;

out vec2 uv;
out vec3 normal;
out float alpha;

uniform mat4 MVP;

void main(){
	gl_Position =  MVP * vec4(in_Vertex,1);

	uv = in_TexCoord0;
	normal = in_Normal;
  alpha = in_Alpha_value;
}
