#include <vgl.h>
#include <cstdio>
#include <InitShader.h>


struct vec2
{
	float x;
	float y;
};

const int NumPoints = 5000;

void init()
{
	vec2 points[NumPoints];

	for (int i = 0; i < NumPoints; i++)
	{
		points[i].x = (rand() % 200) / 100.0f - 1.0f;
		points[i].y = (rand() % 200) / 100.0f - 1.0f;
	}

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

	GLuint program
		= InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	int vPosition = 0;
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

}


void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_POINTS, 0, NumPoints);
	glFlush();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(512, 512);
	glutCreateWindow("Hello GL");

	glewExperimental = true;
	glewInit();

	init();
	glutDisplayFunc(display);
	glutMainLoop();

	return 0;
}