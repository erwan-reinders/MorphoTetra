#version 430

in vec3 color;
in float visibility;


out vec4 FragColor;


void main (void) {
    if(visibility > 0.) discard;
    FragColor = vec4(color, 1.0);
} 
