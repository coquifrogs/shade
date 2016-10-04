#version 330
in vec2 Frag_UV;
layout(location = 0) out vec4 Out_Color;
void main()
{
   Out_Color = vec4(Frag_UV, 0.0, 1.0);
}
