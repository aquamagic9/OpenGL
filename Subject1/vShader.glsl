#version 330

in vec4 vPosition;
in vec4 vColor;

uniform float uTime;
uniform float uWave;


out vec4 color;
out float height;


void main()
{
	float default_angle1 = 45.0f;
	float rad1 = default_angle1 / 180.0f * 3.141592f;

	float default_angle2 = 5.0f * uTime;
	float rad2 = default_angle2 / 180.0f*3.141592f;

	mat4 rotX = mat4(1.0f);			
	mat4 rotZ = mat4(1.0f);
/*
	mat4[j][i]:			row i, col j
	[0][0]	[1][0]	[2][0]	[3][0]
	[0][1]	[1][1]	[2][1]	[3][1]
	[0][2]	[1][2]	[2][2]	[3][2]
	[0][3]	[1][3]	[2][3]	[3][3]
*/
	rotX[1][1] = cos(rad1);	rotX[2][1] = -sin(rad1);
	rotX[1][2] = sin(rad1);	rotX[2][2] = cos(rad1);

	rotZ[0][0] = cos(rad2);	rotZ[1][0] = -sin(rad2);
	rotZ[0][1] = sin(rad2);	rotZ[1][1] = cos(rad2);

	float distance = (vPosition.x * vPosition.x + vPosition.y * vPosition.y);
	mat4 waveMat = mat4(1.0f);
	if (distance <= 0.25f && uWave >= 0.5f)
	{
		waveMat[3][2] = sin((uTime + distance * 3.141592 * 24));
		waveMat[3][2] = waveMat[3][2] * (0.25 -  distance);	
	}
	vec4 wPos = rotX*rotZ*waveMat*vPosition;
	wPos.z = -wPos.z;
	gl_Position = wPos;
	color = vColor;
	height = waveMat[3][2];
}