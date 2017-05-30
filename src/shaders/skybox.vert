// Version is defined by shader compiler, 130 for compatibility and 330 for core

in vec3 in_Vertex;
in vec2 in_TexCoord;
in float in_Layer;

out vec2 texCoord;
out float layer;

uniform mat4 MVP;

void main(){
	gl_Position =  MVP * vec4(in_Vertex,1);

	texCoord = in_TexCoord;
	layer = in_Layer;
}
