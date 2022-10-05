#version 330

in vec4 color;	//in으로 들어오는 값은 못바꿈 상수임
out vec4 fColor;

void main()
{
	//float avg = (colo).r, color.g, color.b) / 3;
	//fColor = vec4(avg, avg, avg, 1);
	//fColor = color;


	fColor = color;
	if (position.x < -0.45 || position.x >0.45)
	{
		if(position.y < -0.45 || position.y > 0.45)
			fColor = vec(0,1,0,1);
		if(position.y < -0.45 || position.z > 0.45)
			fColor = vec(0,1,0,1);
	}
}