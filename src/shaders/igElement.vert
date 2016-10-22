#version 130

in vec3 in_Vertex;
in vec2 in_TexCoord0;

out vec2 UV;

uniform mat4 MVP;

void main(){
	gl_Position =  MVP * vec4(in_Vertex,1);

	UV = in_TexCoord0;
}
