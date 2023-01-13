#version 430

layout (location=0) in vec3 i_position;
layout (location=1) in vec3 i_normal;
layout (location=2) in int i_dimension;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

uniform bool visibility_checkX;
uniform bool visibility_checkY;
uniform bool visibility_checkZ;
uniform vec3 clippingPoint;
uniform vec3 clippingNormal;
uniform vec3 cut;
uniform vec3 cutDirection;

out vec3 normal;
out vec3 position;
out float visibility;

float ComputeVisibility(vec3 point){

        float xVis = visibility_checkX ? (point.x - cut.x)*cutDirection.x : 1.0f;
        float yVis = visibility_checkY ? (point.y - cut.y)*cutDirection.y : 1.0f;
        float zVis = visibility_checkZ ? (point.z - cut.z)*cutDirection.z : 1.0f;

        vec3 pos = point - clippingPoint;
        float vis = dot( clippingNormal, pos );
        if( vis < 0. || xVis < 0.|| yVis < 0.|| zVis < 0. )
                return 1000.;
        else return 0.;
}

void main(void){

    normal = i_normal;
    position = i_position;
    gl_Position = proj_matrix * mv_matrix * vec4(position,1.0);
    visibility = ComputeVisibility(i_position);
}
