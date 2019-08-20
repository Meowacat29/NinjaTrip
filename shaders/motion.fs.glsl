#version 330

// From vertex shader
in vec2 texcoord;

uniform vec3 fcolor;
uniform sampler2D sampler0;
uniform float time;
uniform float division;
uniform bool is_immune;

// Output color
layout(location = 0) out vec4 color;

void main(){
	int m_division = int(division);
	int timeInt = int(time);
	vec2 coord = vec2(texcoord);
	coord.x /= m_division;
	coord.x += (1.0f/m_division) * (timeInt % m_division);	// get the corresponding partial img
	if (is_immune) {
		color = vec4(fcolor, 1.0) * texture(sampler0, coord) + vec4(0.7,0,0,0);
		return;
	}
	color = vec4(fcolor, 1.0) * texture(sampler0, coord);
}






