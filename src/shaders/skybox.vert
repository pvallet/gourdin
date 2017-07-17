// Version is defined by shader compiler, 130 for compatibility and 330 for core

layout (location = 0) in vec3 in_Vertex;
layout (location = 1) in vec2 in_TexCoord;
layout (location = 2) in float in_Layer;

out vec2 texCoord;
out float layer;

uniform mat4 MVP;

void main(){
	gl_Position =  MVP * vec4(in_Vertex,1);

	texCoord = in_TexCoord;
	layer = in_Layer;
}
