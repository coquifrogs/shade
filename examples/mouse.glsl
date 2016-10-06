#version 330

uniform float iTime;
uniform vec2 iMouse;
uniform vec2 iResolution;

in vec2 Frag_UV;
layout(location = 0) out vec4 Out_Color;

void main() {
	vec2 mouseUV = iMouse / iResolution;
	mouseUV.y = 1 - mouseUV.y;
	
	float dist = (0.5 + 0.5 * sin(length(Frag_UV - mouseUV) * 20 - iTime * 5));
	Out_Color = vec4(dist, 0.0, 0.0, 1.0);
}
