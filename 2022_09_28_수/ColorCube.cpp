#include <vgl.h>
#include <InitShader.h>
#include <vec.h>

const int NUM_POINTS = 36;

GLuint prog;
float gTime = 0.0f;

vec4 points[NUM_POINTS];
vec4 colors[NUM_POINTS];

vec4 vertex_pos[8] = {
	vec4(-0.5,-0.5,0.5,1.0),
	vec4(-0.5, 0.5,0.5,1.0),
	vec4(0.5, 0.5,0.5,1.0),
	vec4(0.5,-0.5,0.5,1.0),
	vec4(-0.5,-0.5,-0.5,1.0),
	vec4(-0.5, 0.5,-0.5,1.0),
	vec4(0.5, 0.5,-0.5,1.0),
	vec4(0.5,-0.5,-0.5,1.0)
};

vec4 vertex_color[8] = {
	vec4(1,1,1,1),
	vec4(1,0,0,1),
	vec4(0,1,0,1),
	vec4(0,0,1,1),
	vec4(1,1,0,1),
	vec4(1,0,1,1),
	vec4(0,1,1,1),
	vec4(0,0,0,1)
};


void setRectangle(int a, int b, int c, int d)
{
	static int index = 0;
	points[index] = vertex_pos[a];		colors[index] = vertex_color[a]; index++;
	points[index] = vertex_pos[b];		colors[index] = vertex_color[b]; index++;
	points[index] = vertex_pos[c];		colors[index] = vertex_color[c]; index++;

	points[index] = vertex_pos[c];		colors[index] = vertex_color[c]; index++;
	points[index] = vertex_pos[d];		colors[index] = vertex_color[d]; index++;
	points[index] = vertex_pos[a];		colors[index] = vertex_color[a]; index++;
}

void init()
{
	// 1. create data in cpu
	setRectangle(0, 3, 2, 1);
	setRectangle(4, 5, 6, 7);
	setRectangle(0, 4, 7, 3);
	setRectangle(3, 7, 6, 2);
	setRectangle(2, 6, 5, 1);
	setRectangle(0, 1, 5, 4);

	// 2. seding the data to gpu
	// 2-1. create vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	// 2-2. create vertex buffer object
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// 2-3. sending the data to the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0,
		sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points),
		sizeof(colors), colors);

	// 3. loading the shaders
	prog = InitShader("vShader.glsl", "fShader.glsl");
	glUseProgram(prog);
	// 4. connect the data with the shaders
	GLuint vPosition = glGetAttribLocation(prog, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(prog, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, FALSE, 0,
		BUFFER_OFFSET(sizeof(points)));
}


void display()
{
	//gTime++;
	GLuint uTime = glGetUniformLocation(prog, "uTime");
	glUniform1f(uTime, 0.1f * gTime);	// 하나의 float이다 uTime의 값을 0.1f로 설정. 즉 외부에서 shader에 있는 변수값을 조절할 수 잇다는 것.

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //그릴때마다 DEPTHBUFFERBIT를 클리어?
	glEnable(GL_DEPTH_TEST);			//z-buffer를 그릴때마다 체크하는 것임
	glDrawArrays(GL_TRIANGLES, 0, NUM_POINTS);
	glFlush();
}

void myIdle()
{
	gTime += 0.001f;
	glutPostRedisplay();			//다시 그려야된다라는 것을 알려주는 함수임.
}

//돌아가는게 이상하게 보이는데 이는 삼각형을 그릴때 가장 마지막 삼각형을 가장 나중에 그리기때문에 그렇다. 
//->이를 z-buffer를 통해서 해결가능 color buffer 뒤에 float값을 저장하는 buffer를 만들고 float값에다가 z값과 비교해서 그린다? 결국 sorting 이 필요없어짐.

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);   //여기에 GLUT_DEPTH를 설정함
	glutInitWindowSize(512, 512);
	glutCreateWindow("Hello GL");

	glewExperimental = true;
	glewInit();

	init();
	glutIdleFunc(myIdle);
	glutDisplayFunc(display);
	glutMainLoop();

	return 0;
}