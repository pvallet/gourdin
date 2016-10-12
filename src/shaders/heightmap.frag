#version 120 core 

const vec3 lightDir = vec3 (0.2, 1.0, 0.1);
const vec3 matAlbedo = vec3 (0.6, 0.6, 0.6);

varying vec4 P; // fragment-wise position
varying vec3 N; // fragment-wise normal

uniform vec3 camPos;

uniform sampler2D tex;

void main (void) {
    gl_FragColor = texture2D(tex, gl_TexCoord[0].st);

    vec3 p = vec3 (gl_ModelViewMatrix * P);
    vec3 n = normalize (gl_NormalMatrix * N);
    vec3 l = normalize (lightDir);
    vec3 v = normalize (-p);

    vec3 gna = vec3 (-1.0, -1.0, -1.0);
    vec3 lightColor = vec3 (1.0, 1.0, 1.0);
    vec3 wh = normalize (lightDir + camPos - vec3(P));

    vec3 color = lightColor * 0.1 * dot(n, l);

    gl_FragColor += vec4(color, 1.0);
}
