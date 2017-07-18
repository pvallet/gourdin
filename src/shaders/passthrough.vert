// Version is defined by shader compiler, 330 for desktop and 300 es for mobile

layout (location = 0) in vec2 in_Vertex;
layout (location = 1) in vec2 in_TexCoord;

out vec2 texCoord;

void main() {
	gl_Position = vec4(in_Vertex,0,1);

	texCoord = in_TexCoord;
}
