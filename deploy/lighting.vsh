#version 410

uniform mat4 u_projection;
uniform mat4 u_modelView;
uniform mat4 u_shadowMVP;

layout(location = 0) in vec3 a_vertex;
//layout(location = 1) in vec2 a_texCoord;
layout(location = 2) in vec3 a_normal;

out vec3 v_shadowPos;
out vec3 v_vertex;
//out vec2 v_texCoord;
out vec3 v_normal;

void main()
{
    // Compute vertex in shadow clip space
    vec4 shadowProj = u_shadowMVP * vec4(a_vertex, 1.0);
    v_shadowPos = ((shadowProj.xyz / shadowProj.w) + vec3(1.0, 1.0, 1.0)) / 2.0;
    
    // Pass texture coordinate through
    //v_texCoord = a_texCoord;

    // Compute vertex and normal in eye space
    v_vertex = (u_modelView * vec4(a_vertex, 1.0)).xyz;
    v_normal = normalize((u_modelView * vec4(a_normal, 0.0)).xyz);
    
    // Compute vertex in projection space
    gl_Position = u_projection * vec4(v_vertex, 1.0);
}
