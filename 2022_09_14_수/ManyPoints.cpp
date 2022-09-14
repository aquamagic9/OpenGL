#include <vgl.h>

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_POINTS);
	for (int i = 0; i < 5000; i++)
	{
		float x = (rand() % 200) / 100.0f - 1.0f;
		float y = (rand() % 200) / 100.0f - 1.0f;
		glVertex2f(x, y);
	}
	glEnd();
	glFlush();
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(512, 512);
	glutCreateWindow("Many Points");

	glutDisplayFunc(display);
	glutMainLoop();

	return 0;
}