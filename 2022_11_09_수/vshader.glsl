#version 330

in  vec4 vPosition;
in  vec4 vColor;
out vec4 color;

uniform mat4 uMat;

void main()
{
	gl_Position  = uMat*vPosition;
	//w로 나누어주어야 원래좌표대로 나온다!!
	gl_Position /= gl_Position.w;

	gl_Position.z = -gl_Position.z;

    color = vColor;
}
