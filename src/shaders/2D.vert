// Version is defined by shader compiler, 330 for desktop and 300 es for mobile

uniform vec2 offset;

layout (location = 0) in vec4 in_VertexAndTexCoords;

out vec2 texCoords;

void main() {
	gl_Position = vec4(offset + in_VertexAndTexCoords.xy,0,1);

	texCoords = in_VertexAndTexCoords.zw;
}
