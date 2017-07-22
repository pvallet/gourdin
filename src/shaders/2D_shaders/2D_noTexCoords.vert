// Version is defined by shader compiler, 330 for desktop and 300 es for mobile

layout (location = 0) in vec2 in_Vertex;

void main() {
	gl_Position = vec4(in_Vertex.xy,0,1);
}
