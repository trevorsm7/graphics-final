#version 410

uniform sampler2D u_ambientMap;

in vec2 v_texCoord;

out vec4 f_color;

void main()
{
    f_color = texture(u_ambientMap, v_texCoord);
}
