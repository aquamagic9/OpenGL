#include <vgl.h>
#include <InitShader.h>
#include <mat.h>
#include "MyCube.h"
#include "MyPyramid.h"
#include "MyTarget.h"

MyCube cube;
MyPyramid pyramid;
MyTarget target(&cube);

GLuint program;
GLuint uMat;

mat4 CTM;

bool bPlay = false;
bool bChasingTarget= false;
bool bDrawTarget = false;

float ang1 = 0;
float ang2 = 0;
float ang3 = 0;

void myInit()
{
	cube.Init();
	pyramid.Init();	

	program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);
}

float g_time = 0;

void drawRobotArm(float ang1, float ang2, float ang3)
{
	mat4 temp = CTM;
	
	// BASE
	mat4 M(1.0);
		
	//CTM = Translate(0, -0.4, 0) * RotateY(g_time * 20);

	M = Translate(0, 0, 0.075) * Scale(0.3, 0.2, 0.05);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	pyramid.Draw(program);

	M = Translate(0, 0, -0.075) * Scale(0.3, 0.2, 0.05);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	pyramid.Draw(program);


	//Fixing Pin
	M = Translate(0, 0.00, -0.1) * Scale(0.05, 0.05, 0.03);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	M = Translate(0, 0.00, 0.1) * Scale(0.05, 0.05, -0.03);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	// Upper Arm
	CTM *= RotateZ(-ang1) * Translate(0, 0.15, 0);
	M = Scale(0.1, 0.4, 0.1);
	glUniformMatrix4fv(uMat, 1, true, CTM*M);
	cube.Draw(program);

	// Lower Arm
	CTM *= Translate(0, 0.15, 0) * RotateZ(-ang2) * Translate(0, 0.2, 0);
	M = Translate(0, 0, -0.075) * Scale(0.1, 0.5, 0.05);
	glUniformMatrix4fv(uMat, 1, true, CTM*M);
	cube.Draw(program);

	// Lower Arm2
	M = Translate(0, 0, 0.075) * Scale(0.1, 0.5, 0.05);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	//Fixing Pin
	M = Translate(0, -0.2, 0.1) * Scale(0.05, 0.05, 0.03);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	M = Translate(0, -0.2, -0.1) * Scale(0.05, 0.05, -0.03);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);
	
	// Hand
	CTM *= Translate(0, 0.2, 0) * RotateZ(-ang3);
	M = Scale(0.15, 0.4, 0.1);
	glUniformMatrix4fv(uMat, 1, true, CTM*M);
	cube.Draw(program);

	//Fixing Pin
	M = Translate(0, 0, 0.1) * Scale(0.05, 0.05, 0.03);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	M = Translate(0, 0, -0.1) * Scale(0.05, 0.05, -0.03);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	CTM = temp;
}

float distanceVec3(vec3 a, vec3 b)
{
	return sqrt((a.x - b.x)* (a.x - b.x) + (a.y - b.y)* (a.y - b.y) + (a.z - b.z)*(a.z - b.z));
}

int find_direction(vec3 redboxPos, float upper_length)
{
	float ang_shift = 0.05;
	vec3 upper_offset(0, 0, 0);
	vec3 joint_minus = upper_offset + vec3(sin((ang1 - ang_shift) / 360 * 2 * 3.141592) * upper_length, cos((ang1 - ang_shift) / 360 * 2 * 3.141592) * upper_length, 0);
	vec3 joint_plus  = upper_offset + vec3(sin((ang1 + ang_shift) / 360 * 2 * 3.141592) * upper_length, cos((ang1 + ang_shift) / 360 * 2 * 3.141592) * upper_length, 0);
	double distance1 = distanceVec3(joint_minus, redboxPos);
	double distance2 = distanceVec3(joint_plus, redboxPos);
	if (distance1 > distance2)
		return 1;
	return -1;
}

void myDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	GLuint uColor = glGetUniformLocation(program, "uColor");
	glUniform4f(uColor, -1, -1, -1, -1);


	uMat = glGetUniformLocation(program, "uMat");
	CTM = Translate(0, -0.4, 0) * RotateY(g_time*30);
	//CTM = Translate(0, -0.4, 0);

	drawRobotArm(ang1, ang2, ang3);

	glUniform4f(uColor, 1, 0, 0, 1);
	if (bDrawTarget == true)
		target.Draw(program, CTM, g_time);

	glutSwapBuffers();
}

void computeAngle()
{
	//centerPos: 중심 
	//redboxPos: 타겟좌표 
	//joint: upper와 lower를 이어주는 관절의 좌표
	vec3 centerPos(0, 0, 0);
	vec3 redboxPos = target.currentPos;
	vec3 joint1;
	float upper_length = 0.3;
	float lower_length = 0.4;
	float hand_length = 0.2;

	float ang_shift = 1.5;
	vec3 upper_offset(0, 0, 0);
	ang_shift = find_direction(redboxPos, upper_length) * ang_shift;	

	int n = 0;
	float tmp = ang1;
	while (1)
	{
		n++;
		if (n > 100)
		{
			ang1 = tmp;
			break;
		}
		joint1 = upper_offset + vec3(sin((ang1) / 180.0 * 3.141592) * upper_length, cos((ang1) / 180.0 * 3.141592) * upper_length, 0);
		double distance_joint_to_redbox = distanceVec3(joint1, redboxPos);
		
		if (distance_joint_to_redbox >= lower_length + hand_length || distance_joint_to_redbox + hand_length <= lower_length)
			ang1 = ang1 + ang_shift;
		ang2 = acos((lower_length * lower_length + distance_joint_to_redbox * distance_joint_to_redbox - hand_length * hand_length) / (2 * lower_length * distance_joint_to_redbox)) * 180 / 3.141592;
		if (isnan(ang2))
			continue;
		vec3 joint1_to_center = centerPos - joint1;
		vec3 joint1_to_redbox = redboxPos - joint1;
		float angle_center_joint1_redbox = acos((joint1_to_center.x * joint1_to_redbox.x + joint1_to_center.y * joint1_to_redbox.y) / (sqrt(joint1_to_center.x * joint1_to_center.x + joint1_to_center.y * joint1_to_center.y) * sqrt(joint1_to_redbox.x * joint1_to_redbox.x + joint1_to_redbox.y * joint1_to_redbox.y))) * 180 / 3.141592;
		ang2 = 180 - (ang2 + angle_center_joint1_redbox);
		if(ang_shift < 0)
			ang2 = -ang2;
		ang3 = 180 - acos((lower_length * lower_length + hand_length * hand_length - distance_joint_to_redbox * distance_joint_to_redbox) / (2 * lower_length * hand_length)) * 180 / 3.141592;
		if (ang_shift < 0)
			ang3 = -ang3;
		if (isnan(ang3))
			continue;
		break;
	}
}


void myIdle()
{
	if(bPlay)
	{
		g_time += 1/60.0f;
		Sleep(1/60.0f*1000);

		if(bChasingTarget == false)
		{
			ang1 = 45 * sin(g_time*3.141592);
			ang2 = 60 * sin(g_time*2*3.141592);
			ang3 = 30 * sin(g_time*3.141592);
		}
		else
			computeAngle();

		glutPostRedisplay();
	}
}

void myKeyboard(unsigned char c, int x, int y)
{

	switch(c)
	{
	case '1':
		bChasingTarget = !bChasingTarget;
		break;
	case '2':
		bDrawTarget = !bDrawTarget;
		break;
	case '3':
		target.toggleRandom();
		break;
	case ' ':
		bPlay = !bPlay;
		break;
	default:
		break;
	}
}


int main(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
	glutInitWindowSize(500,500);
	glutCreateWindow("Simple Robot Arm");

	glewExperimental = true;
	glewInit();

	myInit();
	glutDisplayFunc(myDisplay);
	glutKeyboardFunc(myKeyboard);
	glutIdleFunc(myIdle);

	glutMainLoop();

	return 0;
}