#version 330

in vec3 N3; 
in vec3 L3; 
in vec3 V3;
in vec3 wV;
in vec3 wP;
in vec3 wN;

out vec4 fColor;

uniform mat4 uModelMat; 
uniform mat4 uProjMat; 
uniform vec4 uLPos; 
uniform vec4 uAmb; 
uniform vec4 uDif; 
uniform vec4 uSpc; 
uniform float uShininess;
uniform samplerCube uTexCube;
uniform vec4 uEPos;

void main()
{
	vec3 N = normalize(N3); 
	vec3 L = normalize(L3); 
	vec3 V = normalize(V3); 
	vec3 H = normalize(V+L); 

    float NL = max(dot(N, L), 0); 
	float VR = pow(max(dot(H, N), 0), uShininess); 

	fColor = uAmb + uDif*NL + uSpc*VR; 
	fColor.w = 1; 
	vec3 viewDir = wP - wV;
	vec3 wNormal = normalize(wN);
	vec3 dir = reflect(viewDir, wNormal);
	/*
	float rfRatio = 1.3f/1.0f;   //  공기중에서 물로 들어가는 ratio임
	vec3 dir = refract(viewDir, wNormal, rfRatio);
	if (length(dir) < 0.1f)
		dir = reflect(viewDir, wNormal);
	*/
	dir.y = -dir.y;
	vec4 rColor = texture(uTexCube, dir);
	float fr = 0 + 0.5f * pow(max(1 - dot(V, N), 0), 1); // offset + scale*(1+dot(V,N))*n; 코드에서 -dot(v,n) 을 한거는 v의 방향이 물체안에서 나오는 방향으로했기 때문이다
	// pow의 제곱수가 많아질수록 좀더 매트해지는 느낌임. 중앙으로 갈수록 원래색, 바깥쪽으로갈수록 반사가 일어나는것을 볼 수 있다.
	//float fr = 1
	fColor = (1 - fr)*fColor + fr*rColor;
}
