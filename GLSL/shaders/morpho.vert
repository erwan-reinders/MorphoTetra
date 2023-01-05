#version 430
layout (location=0) in vec3 i_position;
layout (location=1) in vec3 i_normal;
layout (location=2) in vec3 i_color;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

out vec3 normal;
out vec3 color;

void main(void)
{
    gl_Position = proj_matrix * mv_matrix * vec4(i_position,1.0);
    normal = i_normal;
    color = i_color;
}
