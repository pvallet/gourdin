// Version is defined by shader compiler, 330 for desktop and 300 es for mobile

layout (location = 0) in vec3 in_Vertex;
layout (location = 1) in vec3 in_Pos;
layout (location = 2) in vec2 in_TexCoords;
layout (location = 3) in float in_Layer;

out vec2 texCoords;
out float layer;

// If the element is too close
out float discardFrag;

uniform mat4 VP;
uniform mat4 MODEL;

uniform float elementNearPlane;
uniform vec3 camPos;

void main(){
	if (length(in_Pos-camPos) < elementNearPlane)
		discardFrag = 1.f;
	else
		discardFrag = 0.f;

	gl_Position = VP * (vec4(in_Pos,0) + MODEL * vec4(in_Vertex,1));

	texCoords = in_TexCoords;
	layer = in_Layer;
}
