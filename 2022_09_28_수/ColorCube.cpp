//#include <vgl.h>
//#include <InitShader.h>
//#include <vec.h>
//#include "MyColorCube.h"
//
//
//float gTime = 0.0f;
//GLuint prog;
//MyColorCube cube;
//
//void init()
//{
//	cube.init();
//	prog = InitShader("vShader.glsl", "fShader.glsl");
//	glUseProgram(prog);
//}
//
//
//void display()
//{
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //�׸������� DEPTHBUFFERBIT�� Ŭ����?
//	glEnable(GL_DEPTH_TEST);			//z-buffer�� �׸������� üũ�ϴ� ����
//	cube.draw(prog, gTime);
//	glFlush();
//}
//
//void myIdle()
//{
//	gTime += 0.001f;
//	glutPostRedisplay();			//�ٽ� �׷��ߵȴٶ�� ���� �˷��ִ� �Լ���.
//}
//
////���ư��°� �̻��ϰ� ���̴µ� �̴� �ﰢ���� �׸��� ���� ������ �ﰢ���� ���� ���߿� �׸��⶧���� �׷���. 
////->�̸� z-buffer�� ���ؼ� �ذᰡ�� color buffer �ڿ� float���� �����ϴ� buffer�� ����� float�����ٰ� z���� ���ؼ� �׸���? �ᱹ sorting �� �ʿ������.
//
//void myIdle()
//{
//	gTime += 0.001f;
//	glutPostRedisplay();
//}
//
////z���� �ݴ���ؾߵȴ�?????
//int main(int argc, char** argv)
//{
//	glutInit(&argc, argv);
//	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);   //���⿡ GLUT_DEPTH�� ������
//	glutInitWindowSize(512, 512);
//	glutCreateWindow("Hello GL");
//
//	glewExperimental = true;
//	glewInit();
//
//	init();
//	glutIdleFunc(myIdle);
//	glutDisplayFunc(display);
//	glutMainLoop();
//
//	return 0;
//}