#version 120 core

varying vec4 P;
varying vec3 N;

void main(void) {
    P = gl_Vertex;
    N = gl_Normal;
    gl_Position = ftransform ();
    gl_TexCoord[0] = gl_MultiTexCoord0;
}
