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
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //그릴때마다 DEPTHBUFFERBIT를 클리어?
//	glEnable(GL_DEPTH_TEST);			//z-buffer를 그릴때마다 체크하는 것임
//	cube.draw(prog, gTime);
//	glFlush();
//}
//
//void myIdle()
//{
//	gTime += 0.001f;
//	glutPostRedisplay();			//다시 그려야된다라는 것을 알려주는 함수임.
//}
//
////돌아가는게 이상하게 보이는데 이는 삼각형을 그릴때 가장 마지막 삼각형을 가장 나중에 그리기때문에 그렇다. 
////->이를 z-buffer를 통해서 해결가능 color buffer 뒤에 float값을 저장하는 buffer를 만들고 float값에다가 z값과 비교해서 그린다? 결국 sorting 이 필요없어짐.
//
//void myIdle()
//{
//	gTime += 0.001f;
//	glutPostRedisplay();
//}
//
////z축은 반대로해야된다?????
//int main(int argc, char** argv)
//{
//	glutInit(&argc, argv);
//	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);   //여기에 GLUT_DEPTH를 설정함
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