#version 130

in vec3 in_Vertex;
in vec3 in_Normal;
in vec2 in_TexCoord;

out vec2 uv;
out vec3 normal;

uniform mat4 MVP;

void main(){
	gl_Position =  MVP * vec4(in_Vertex,1);

	uv = in_TexCoord;
	normal = in_Normal;
}
