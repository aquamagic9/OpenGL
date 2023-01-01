#define _CRT_SECURE_NO_WARNINGS

#include <vgl.h>
#include <vec.h>
#include <mat.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <InitShader.h>
#include <stdlib.h>
#include "MyCube.h"
#include "MySphere.h"

using namespace std;

struct MyVertecies
{
	vec4 position;
	vec4 color;
	vec3 normal;
};

GLuint program;
GLuint prog_phong;

MyCube cube;
MySphere sphere;

GLuint uMat;
GLuint uColor;
mat4 g_Mat = mat4(1.0f);

int winWidth = 500;
int winHeight = 500;

char fileName[255];
ifstream openFile;

int index = 0;
int numPoint = 0;

GLuint vao;
GLuint buffer;

float turnX_Axis = 0.0f;
float turnY_Axis = 0.0f;
float turnZ_Axis = 0.0f;
bool isRotate = false;
bool turnY = true;
bool turnZ = true;

float shiness = 3.0;
vec4 mSpec = vec4(0.3, 0.3, 0.3, 1);

vector<vec4> my_vertex;
vector<vec4> fragment;

MyVertecies* Vertices;

float maxX = -10000, maxY = -10000, maxZ = -10000;
float avgX = 0, avgY = 0, avgZ = 0;
float sumX = 0, sumY = 0, sumZ = 0;
float scaleAll = 1;


mat4 myLookAt(vec3 eye, vec3 at, vec3 up)
{
	// Implement your own look-at function
	mat4 V(1.0f);
	vec3 n = at - eye;
	n /= length(n);

	float a = dot(up, n);
	vec3 v = up - a * n;
	v /= length(v);

	vec3 w = cross(n, v);

	mat4 Rw(1.0f);

	Rw[0][0] = w.x;	Rw[0][1] = v.x; Rw[0][2] = -n.x;
	Rw[1][0] = w.y;	Rw[1][1] = v.y; Rw[1][2] = -n.y;
	Rw[2][0] = w.z;	Rw[2][1] = v.z; Rw[2][2] = -n.z;

	mat4 Rc(1.0f);
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			Rc[i][j] = Rw[j][i];

	mat4 Tc = Translate(-eye.x, -eye.y, -eye.z);

	V = Rc * Tc;

	return V;
}

vec3 computeNormal(vec4 p0, vec4 p1, vec4 p2)
{
	vec4 a4 = p1 - p0;
	vec4 b4 = p2 - p0;
	vec3 a = vec3(a4.x, a4.y, a4.z);
	vec3 b = vec3(b4.x, b4.y, b4.z);

	vec3 n = normalize((cross(a, b)));
	return n;
}

void putInfo(int a, int b, int c) {
	Vertices[index].position = my_vertex[a]; Vertices[index].color = vec4(0.5, 0.5, 0.5, 1); Vertices[index].normal = computeNormal(my_vertex[a], my_vertex[b], my_vertex[c]); index++;
	Vertices[index].position = my_vertex[b]; Vertices[index].color = vec4(0.5, 0.5, 0.5, 1); Vertices[index].normal = computeNormal(my_vertex[a], my_vertex[b], my_vertex[c]); index++;
	Vertices[index].position = my_vertex[c]; Vertices[index].color = vec4(0.5, 0.5, 0.5, 1); Vertices[index].normal = computeNormal(my_vertex[a], my_vertex[b], my_vertex[c]); index++;
}

vec3 vec4ToVec3(vec4 in)
{
	vec3 result;
	result.x = in.x;
	result.y = in.y;
	result.z = in.z;
	return result;
}

void setting() 
{
	for (int i = 0; i < fragment.size(); i++)
		putInfo(fragment[i].x, fragment[i].y, fragment[i].z);
}

void calScale() {
	int size = my_vertex.size();
	for (int i = 0; i < size; i++)
	{
		if (my_vertex[i].x > maxX) 
			maxX = my_vertex[i].x;
		if (my_vertex[i].y > maxY) 
			maxY = my_vertex[i].y;
		if (my_vertex[i].z > maxZ) 
			maxZ = my_vertex[i].z;
		sumX += my_vertex[i].x;
		sumY += my_vertex[i].y;
		sumZ += my_vertex[i].z;
	}
	avgX = sumX / (float)size;
	avgY = sumY / (float)size;
	avgZ = sumZ / (float)size;

	float scaleX = (1.0 - maxX) * 10 + 1;
	float scaleY = (1.0 - maxY) * 10 + 1;
	float scaleZ = (1.0 - maxZ) * 10 + 1;

	if (scaleX > scaleY)
	{
		if (scaleY > scaleZ)
			scaleAll = scaleZ;
		else
			scaleAll = scaleY;
	}
	else if (scaleX < scaleY)
	{
		if (scaleX < scaleZ)
			scaleAll = scaleX;
		else
			scaleAll = scaleZ;
	}
}

mat4 myOrtho(float l, float r, float b, float t, float zNear, float zFar)
{
	// Implement your own Ortho function
	mat4 V(1.0f);

	V[0][0] = 2 / (r - l);
	V[1][1] = 2 / (t - b);
	V[2][2] = 2 / (zFar - zNear);
	V[0][3] = -(r + l) / (r - l);
	V[1][3] = -(t + b) / (t - b);
	V[2][3] = (zNear) / (zFar - zNear);

	return V;
}

void saveInfo(string str) {
	if (!(str[0] == 'f' || str[0] == 'v'))
		return;
	istringstream ss(str);
	string buf;
	vector<string> pos;
	if (str[0] == 'v')
	{
		while (getline(ss, buf, ' ')) 
		{
			pos.push_back(buf);
		}
		vec3 v;
		v.x = stof(pos[1]); v.y = stof(pos[2]); v.z = stof(pos[3]);
		my_vertex.push_back(v);
	}
	vector<string> vertex_indexes;
	if (str[0] == 'f') 
	{
		while (getline(ss, buf, ' ')) 
		{
			vertex_indexes.push_back(buf);
		}
		vec3 v;
		v.x = stoi(vertex_indexes[1]); v.y = stoi(vertex_indexes[2]); v.z = stoi(vertex_indexes[3]);
		fragment.push_back(v);
	}
}

void readFile() {
	string str;
	while (1) 
	{
		cout << "Input file_path: ";
		cin >> fileName;
		openFile.open(fileName);
		if (!openFile) 
		{
			cout << "Can't find the file\n";
			continue;
		}
		break;
	}
	if (openFile.is_open()) 
	{
		while (getline(openFile, str)) 
			saveInfo(str);
		openFile.close();
	}
}


mat4 myPerspective(float fovy, float aspectRatio, float zNear, float zFar)
{
	mat4 P(1.0f);

	float rad = fovy * 3.141592 / 180.0;

	float sz = 1 / zFar;
	float h = zFar * tan(rad / 2);

	float sy = 1 / h;
	float w = h * aspectRatio;
	float sx = 1 / w;

	mat4 S = Scale(sx, sy, sz);
	mat4 M(1.0f);

	float c = -zNear / zFar;
	M[2][2] = 1 / (c + 1);
	M[2][3] = -c / (c + 1);
	M[3][2] = -1;
	M[3][3] = 0;

	P = M * S;

	return P;
}


void myInit()
{
	cube.Init();

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MyVertecies) * numPoint, Vertices, GL_STATIC_DRAW);

	program = InitShader("vshader.glsl", "fshader.glsl");
	prog_phong = InitShader("vphong.glsl", "fphong.glsl");
}

void processMouse(int button, int state, int x, int y) {

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		turnY = true;
		turnZ = false;
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		turnY = false;
		turnZ = false;
	}
	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
		turnZ = true;
		turnY = false;
	}
}

void DrawAxis()
{
	glUseProgram(program);
	mat4 x_a = Translate(1., 0, 0) * Scale(2, 0.01, 0.01);
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * x_a);
	glUniform4f(uColor, 1, 0, 0, 1);
	cube.Draw(program);

	mat4 y_a = Translate(0, 1., 0) * Scale(0.01, 2, 0.01);
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * y_a);
	glUniform4f(uColor, 0, 1, 0, 1);
	cube.Draw(program);

	mat4 z_a = Translate(0, 0, 1.) * Scale(0.01, 0.01, 2);
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * z_a);
	glUniform4f(uColor, 0, 0, 1, 1);
	cube.Draw(program);
}

void DrawGrid()
{
	glUseProgram(program);
	float n = 40;
	float w = 10;
	float h = 10;

	for (int i = 0; i < n; i++)
	{
		mat4 m = Translate(0, -0.5, -h + 2 * h / n * i) * Scale(w * 2, 0.02, 0.02);
		glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * m);
		glUniform4f(uColor, 1, 1, 1, 1);
		cube.Draw(program);
	}
	for (int i = 0; i < n; i++)
	{
		mat4 m = Translate(-w + 2 * w / n * i, -0.5, 0) * Scale(0.02, 0.02, h * 2);
		glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * m);
		glUniform4f(uColor, 1, 1, 1, 1);
		cube.Draw(program);
	}
}

float g_Time = 0;

void display()
{
	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	mat4 ViewMat = myLookAt(vec3(0,0,3), vec3(0, 0, 0), vec3(0, 1, 0));

	float aspect = winWidth / (float)winHeight;
	float h = 1;

	mat4 ProjMat = myPerspective(40, aspect, 0.1, 100);

	mat4 ModelMat = 1;

	if (isRotate) {
		if (turnY)
			turnY_Axis += 0.01;
		else if (turnZ)
			turnZ_Axis += 0.01;
		else
			turnX_Axis += 0.01;
	}
	ModelMat *= RotateZ(turnZ_Axis * 90) * RotateY(turnY_Axis * 90) * RotateX(turnX_Axis * 90);

	mat4 TransMat = Translate(-avgX, -avgY, -avgZ);
	g_Mat = ProjMat * ViewMat * RotateZ(turnZ_Axis * 90) * RotateY(turnY_Axis * 90) * RotateX(turnX_Axis * 90) * TransMat;

	glUseProgram(program);
	uMat = glGetUniformLocation(program, "uMat");
	uColor = glGetUniformLocation(program, "uColor");

	DrawAxis();

	glUseProgram(prog_phong);
	GLuint uProjMat = glGetUniformLocation(prog_phong, "uProjMat");
	GLuint uModelMat = glGetUniformLocation(prog_phong, "uModelMat");

	mat4 ScaleMat = Scale(scaleAll, scaleAll, scaleAll);

	glUniformMatrix4fv(uProjMat, 1, GL_TRUE, ProjMat);
	glUniformMatrix4fv(uModelMat, 1, GL_TRUE, ViewMat * ScaleMat * ModelMat * TransMat);

	// 1. define lights : Position, Color(Intensity)
	vec4 lpos = vec4(3, 3, 0, 1);	// in World Coord.
	vec4 lcol = vec4(1, 1, 1, 1);	// white 

	// 2. material properties (phong coeff.)
	vec4 mAmb = vec4(0.2, 0.2, 0.2, 1);
	vec4 mDif = vec4(0.6, 0.3, 0.3, 1);

	GLuint uLPos = glGetUniformLocation(prog_phong, "uLPos");
	GLuint uLCol = glGetUniformLocation(prog_phong, "uLCol");
	GLuint uAmb = glGetUniformLocation(prog_phong, "uAmb");
	GLuint uDif = glGetUniformLocation(prog_phong, "uDif");
	GLuint uSpec = glGetUniformLocation(prog_phong, "uSpec");
	GLuint uShiness = glGetUniformLocation(prog_phong, "uShiness");

	glUniform4f(uLPos, lpos[0], lpos[1], lpos[2], lpos[3]);
	glUniform4f(uLCol, lcol[0], lcol[1], lcol[2], lcol[3]);
	glUniform4f(uAmb, mAmb[0], mAmb[1], mAmb[2], mAmb[3]);
	glUniform4f(uDif, mDif[0], mDif[1], mDif[2], mDif[3]);
	glUniform4f(uSpec, mSpec[0], mSpec[1], mSpec[2], mSpec[3]);
	glUniform1f(uShiness, shiness);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	GLuint vPosition = glGetAttribLocation(prog_phong, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, sizeof(MyVertecies), BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(prog_phong, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, sizeof(MyVertecies), BUFFER_OFFSET(sizeof(vec4)));

	GLuint vNormal = glGetAttribLocation(prog_phong, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_TRUE, sizeof(MyVertecies), BUFFER_OFFSET(sizeof(vec4)*2));

	glDrawArrays(GL_TRIANGLES, 0, numPoint);
	glutSwapBuffers();
}


void idle()
{
	Sleep(16);
	glutPostRedisplay();
}

void reshape(int w, int h)
{
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
	if (key == ' ')
		isRotate = !isRotate;
	if (key == '3')
	{
		if (mSpec.x < 1.0)
		{
			mSpec.x += 0.05;
			mSpec.y += 0.05;
			mSpec.z += 0.05;
		}
		cout << "Increase Specular Effect!!" << endl;
	}
	if (key == '4')
	{
		if (mSpec.x > 0.1)
		{
			mSpec.x -= 0.05;
			mSpec.y -= 0.05;
			mSpec.z -= 0.05;
		}
		cout << "Decrease Specular Effect!!" << endl;
	}
	if (key == '5')
	{
		if (shiness < 30)
			shiness++;
		cout << "Increase Shiness!!" << endl;
	}
	if (key == '6') {
		if (shiness > 1)
			shiness--;
		cout << "Decrease Shiness!!" << endl;
	}
}

int main(int argc, char** argv)
{
	vec3 tmp;

	my_vertex.push_back(tmp);  //index 하나 높여주기
	readFile();
	numPoint = fragment.size() * 3;
	Vertices = (MyVertecies*)malloc(sizeof(MyVertecies) * numPoint);
	setting();
	calScale();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(winWidth, winHeight);
	glutCreateWindow("OpenGL");
	glewExperimental = true;
	glewInit();

	myInit();
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(processMouse);
	glutMainLoop();

	return 0;
}