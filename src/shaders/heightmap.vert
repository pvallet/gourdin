// Version is defined by shader compiler, 330 for desktop and 300 es for mobile

layout (location = 0) in vec3 in_Vertex;
layout (location = 1) in vec3 in_Normal;
layout (location = 2) in vec2 in_TexCoord;

out vec2 uv;
out vec3 normal;

uniform mat4 MVP;

void main(){
	gl_Position =  MVP * vec4(in_Vertex,1);

	uv = in_TexCoord;
	normal = in_Normal;
}
