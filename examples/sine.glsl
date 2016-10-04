#version 330
in vec2 Frag_UV;
layout(location = 0) out vec4 Out_Color;
void main()
{
   Out_Color = vec4(sin(Frag_UV.x*60), cos(Frag_UV.y*60), 0.0, 1.0);
}
