#version 410

uniform mat4 u_projection;
uniform mat4 u_modelView;

layout(location = 0) in vec3 a_vertex;
layout(location = 1) in vec2 a_texCoord;
layout(location = 2) in vec3 a_normal;

out vec2 v_texCoord;

void main()
{
    // Pass texture coordinate through
    v_texCoord = a_texCoord;
    
    // Compute vertex in projection space
    gl_Position = u_projection * u_modelView * vec4(a_vertex, 1.0);
}
