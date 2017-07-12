// Version is defined by shader compiler, 130 for compatibility and 330 for core

in vec3 in_Vertex;
in vec3 in_Pos;
in vec2 in_TexCoord;
in float in_Layer;

out vec2 texCoord;
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

	texCoord = in_TexCoord;
	layer = in_Layer;
}
