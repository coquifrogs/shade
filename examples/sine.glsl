#version 330
uniform float Time;
in vec2 Frag_UV;
layout(location = 0) out vec4 Out_Color;
void main()
{
   vec2 offset = Frag_UV + Time * 0.1;
   Out_Color = vec4(sin(offset.x*60), cos(offset.y*40), 0.6, 1.0);
}
