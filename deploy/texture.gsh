#version 410

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 g_shadowPos[];
in vec3 g_vertex[];
in vec2 g_texCoord[];
in vec3 g_normal[];

out vec3 v_shadowPos;
out vec3 v_vertex;
out vec2 v_texCoord;
out vec3 v_normal;
out vec3 v_tangent;
out vec3 v_bitangent;

void main()
{
    // Compute per-triangle tangent space vectors
    vec3 dv1 = g_vertex[1] - g_vertex[0];
    vec3 dv2 = g_vertex[2] - g_vertex[0];
    vec2 dt1 = g_texCoord[1] - g_texCoord[0];
    vec2 dt2 = g_texCoord[2] - g_texCoord[0];
    float r = 1.0f / (dt1.x * dt2.y - dt1.y * dt2.x);
    v_tangent = (dv1 * dt2.y - dv2 * dt1.y) * r;
    v_bitangent = (dv2 * dt2.x - dv1 * dt2.x) * r;

    // Pass on attributes from vertex shader
    for (int i = 0; i < gl_in.length(); ++i)
    {
        v_shadowPos = g_shadowPos[i];
        v_vertex = g_vertex[i];
        v_texCoord = g_texCoord[i];
        v_normal = g_normal[i];
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }

    EndPrimitive();
}
