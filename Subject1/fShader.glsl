#version 330

in vec4 color;
in float height;
out vec4 fColor;

uniform float uTime;
uniform float uWave;

void main()
{
	if (height < 0.0)
		fColor = color + vec4(-height*5, 0.0, 0.0, 1.0);
	else
		fColor = color + vec4(0, 0, height*5, 1);
}