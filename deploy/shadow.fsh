#version 410

out vec4 f_color;
 
void main()
{
    // This shader isn't used; we don't write to a color buffer
    f_color = vec4(gl_FragCoord.z, 0.0, 0.0, 1.0);
}