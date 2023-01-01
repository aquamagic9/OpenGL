#version 330

//in  vec4 color;

//vphong ���� ��Ḹ �Ѱ��ش�. �� �ﰢ������ ������ ������ N���� �ٰ����� �޺��� ���� ���⺤�ʹ� �ٸ��Ƿ� �װ��� ���� ������.
in vec3 N3;
in vec3 L3;
in vec3 V3;

out vec4 fColor;

uniform mat4 uProjMat;
uniform mat4 uModelMat;
uniform vec4 uLPos;
uniform vec4 uLCol;
uniform vec4 uAmb;
uniform vec4 uDif;
uniform vec4 uSpec;
uniform float uShiness;

void main()
{
	vec3 N = normalize(N3);
	vec3 L = normalize(L3);
	vec3 V = normalize(V3);
	vec3 R = 2*dot(L, N)*N  - L;

	vec4 amb = uAmb * uLCol;
	vec4 dif = uDif * uLCol * max(dot(N, L), 0);
	vec4 spec = uSpec*uLCol*pow(max(dot(V, R), 0), uShiness);			
	
	//pow(max(dot(V, R), uShiness) //V, R�� �󸶳� ����Ѱ�??

	fColor = amb + dif + spec;
}
