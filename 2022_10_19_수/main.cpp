#include <vgl.h>
#include <InitShader.h>
#include <mat.h>
#include "MyCube.h"
#include "MyPyramid.h"

MyCube cube;
MyPyramid pyramid;

GLuint program;
GLuint uMat;

mat4 g_Mat = mat4(1.0);

float g_Time = 0;

void myInit()
{
	cube.Init();
	pyramid.Init();

	program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);
}



void myDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	GLuint uMat = glGetUniformLocation(program, "uMat");
	
	/*
	mat4 CTM(1.0);		//current transform matrix //이것은 배치시키는것
	CTM = RotateY(90 * g_Time);
	
	mat4 M(1.0);		//단위행렬을 만듬  M  은 모양을 만드는 것
	M = Scale(0.5, 1, 0.4);
		//M = Translate(0.2, 0.4, 0) * RotateZ(90 * g_Time) * RotateY(30) * Scale(2, 2, 2);	//scale->rotate->translate순
	
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	pyramid.Draw(program);

	CTM *= Translate(0,0.25,0.25) * RotateZ(180 * g_Time) * Scale(0.8, 0.1, 0.1);
	M = Scale(0.8, 0.1, 0.1);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	CTM = Translate(0, 0.25, 0.25) * RotateZ(180 * g_Time) * RotateZ(90) * Scale(0.8, 0.1, 0.1);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	//pyramid.Draw(program);
	cube.Draw(program);
	*/

	//로봇팔 만들기
	mat4 CTM(1.0);
	CTM = RotateY(90 * g_Time);

	mat4 M(1.0);
	M = Scale(0.4, 0.3, 0.2);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	pyramid.Draw(program);

	CTM *= Translate(0, 0.2, 0);
	M = Scale(0.1, 0.4, 0.1);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);
	
	CTM *= Translate(0, 0.4, 0);
	M = Scale(0.15, 0.4, 0.15);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);


	glutSwapBuffers();
}

void myIdle()
{
	g_Time += 0.01f;
	Sleep(16);
	glutPostRedisplay();
}



int main(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
	glutInitWindowSize(500,500);
	glutCreateWindow("Cube and Pyramid");

	glewExperimental = true;
	glewInit();

	myInit();
	glutDisplayFunc(myDisplay);
	glutIdleFunc(myIdle);

	glutMainLoop();

	return 0;
}