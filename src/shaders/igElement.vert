#version 130

in vec3 in_Vertex;
in vec3 in_Pos;
in vec2 in_TexCoord;
in float in_Layer;

out vec2 texCoord;
out float layer;

uniform mat4 VP;
uniform mat4 MODEL;

void main(){
	gl_Position = VP * (vec4(in_Pos,0) + MODEL * vec4(in_Vertex,1));
	
	texCoord = in_TexCoord;
	layer = in_Layer;
}
