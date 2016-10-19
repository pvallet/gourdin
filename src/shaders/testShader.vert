#version 130

// Input vertex data, different for all executions of this shader.
in vec3 in_Vertex;
in vec3 in_Normal;
in vec2 in_TexCoord0;

// Output data ; will be interpolated for each fragment.
out vec2 UV;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;

void main(){

	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(in_Vertex,1);

	// UV of the vertex. No special space for this one.
	UV = in_TexCoord0;
}
