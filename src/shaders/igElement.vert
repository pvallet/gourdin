#version 130

in vec3 in_Vertex;
in vec2 in_TexCoord;
in float in_Layer;

out vec2 texCoord;
out float layer;

void main(){
	gl_Position = vec4(in_Vertex,1);

	texCoord = in_TexCoord;
	layer = in_Layer;
}
