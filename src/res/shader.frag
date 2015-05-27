// ----------------------------------------------
// Informatique Graphique 3D & Réalité Virtuelle.
// Travaux Pratiques
// Shaders
// Copyright (C) 2015 Tamy Boubekeur
// All rights reserved.
// ----------------------------------------------

// Add here all the value you need to describe the light or the material. 
// At first used const values. 
// Then, use uniform variables and set them from the CPU program.

const vec3 lightPos = vec3 (5.0, 5.0, 5.0);
const vec3 matAlbedo = vec3 (0.6, 0.6, 0.6);

varying vec4 P; // fragment-wise position
varying vec3 N; // fragment-wise normal

uniform vec3 camPos;

void main (void) {
    gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0);
    
    vec3 p = vec3 (gl_ModelViewMatrix * P);
    vec3 n = normalize (gl_NormalMatrix * N);
    vec3 l = normalize (lightPos - p);
    vec3 v = normalize (-p);
    
    vec3 lightColor = vec3 (1.0, 1.0, 1.0);
    vec3 wh = normalize (lightPos - vec3(P) + camPos - vec3(P));
    
    vec3 color = lightColor * 0.5 * dot(n, l);
    
    gl_FragColor += vec4(color, 1.0);
}
 
