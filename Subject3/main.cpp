
#define _CRT_SECURE_NO_WARNINGS

#include <vgl.h>
#include <InitShader.h>
#include "MyCube.h"
#include "MyUtil.h"
#include <algorithm>
#include <list>
#include <tuple>
#include <vec.h>
#include <mat.h>

#define MAZE_FILE	"maze.txt"
using namespace std;

MyCube cube;
GLuint program;

mat4 g_Mat = mat4(1.0f);
GLuint uMat;
GLuint uColor;

float wWidth = 1000;
float wHeight = 500;

vec3 cameraPos = vec3(0, 0, 0);
vec3 viewDirection = vec3(0, 0, -1);
vec3 goalPos = vec3(0, 0, 0);

int MazeSize;
char maze[255][255] = { 0 };

float cameraSpeed = 0.1;

float g_time = 0;

int direction = 1;

bool collision = false;
bool drawDirection = false;
bool followPath = false;

int moveCnt = 0;
int angleCnt = 0;
float rotateAngle = 0;
bool changeDirection = false;

//

class Node {
public:
	Node* connect;
	int row, col; //index
	int g, h, f;
	char nodeName;
	char value;

public:
	Node(int _y, int _x, int _i)
	{
		row = _y;
		col = _x;
		nodeName = _i;
		g = 0;
		h = 0;
		f = 0;
	}
};

list<Node*> openList;
list<Node*> closeList;
Node* path = nullptr;
Node* cameraPath = nullptr;

tuple<int, int> getIndexFromPosition(vec3 position)
{
	float unit = 1;
	vec3 leftTopPosition = vec3(-MazeSize / 2.0, 0, -MazeSize / 2.0);
	vec3 xDir = vec3(1, 0, 0);
	vec3 zDir = vec3(0, 0, 1);
	vec3 diff = position - leftTopPosition;
	diff.x = floor(diff.x);
	diff.z = floor(diff.z);

	return make_tuple(diff.x, diff.z);
}

tuple<float, float> getFloatPosition(vec3 position)
{
	vec3 leftTopPosition = vec3(-MazeSize / 2.0, 0, -MazeSize / 2.0);
	vec3 xDir = vec3(1, 0, 0);
	vec3 zDir = vec3(0, 0, 1);
	vec3 diff = position - leftTopPosition;

	return make_tuple(diff.x, diff.z);
}


Node* CreateNodeByIndex(int rowIndex, int colIndex, Node* parentNode)
{
	char val = maze[rowIndex][colIndex];

	if (val == '*')
		return nullptr;

	Node* node = nullptr;
	if (val == 'G')
	{
		node = new Node(rowIndex, colIndex, 'G');
		node->g = parentNode->g + 1;
		node->h = 0;
		node->f = node->g;
		node->connect = parentNode;
	}
	else
	{
		node = new Node(rowIndex, colIndex, val);
		node->g = parentNode->g + 1;

		auto inds = getIndexFromPosition(cameraPos);
		int goalRowInd = get<0>(inds);
		int goalColInd = get<1>(inds);

		int h = abs(goalRowInd - rowIndex) + abs(goalColInd - colIndex);
		node->h = h;
		node->f = node->g + h;
		node->connect = parentNode;
	}

	return node;
}

Node* GetChildNodes(int childIndRow, int childIndCol, Node* parentNode)
{
	auto it_open = find_if(openList.begin(), openList.end(), [&](Node* node)
		{
			if (node->row == childIndRow && node->col == childIndCol)
			{
				return true;
			}
			else
			{
				return false;
			}
		});

	auto it_close = find_if(closeList.begin(), closeList.end(), [&](Node* node)
		{
			if (node->row == childIndRow && node->col == childIndCol)
			{
				return true;
			}
			else
			{
				return false;
			}
		});

	if (it_open != openList.end())
	{
		if ((*it_open)->g < parentNode->g + 1)
		{
			(*it_open)->g = parentNode->g + 1;
			parentNode->connect = (*it_open);
			(*it_open)->f = (*it_open)->g + (*it_open)->h;
		}

		return *it_open;
	}
	else if (it_close != closeList.end())
	{
		if ((*it_close)->g < parentNode->g + 1)
		{
			(*it_close)->g = parentNode->g + 1;
			parentNode->connect = (*it_close);
			(*it_close)->f = (*it_close)->g + (*it_close)->h;
		}
		return *it_close;
	}
	else
	{
		Node* newNode = CreateNodeByIndex(childIndRow, childIndCol, parentNode);
		if (newNode != nullptr)
		{
			openList.push_back(newNode);
		}
		return newNode;
	}

	return nullptr;
}

void FindPath()
{
	if (openList.size() == 0)
	{
		cout << "invalid" << endl;
		return;
	}

	Node* openNode = nullptr;

	int smallest_f = 10000;
	for (auto& op : openList)
	{
		if (op->f < smallest_f)
		{
			smallest_f = op->f;
			openNode = op;
		}
	}

	if (openNode != nullptr)
	{
		if (openNode->nodeName == 'G')
		{
			path = openNode;
			while (openNode != nullptr)
			{
				openNode = openNode->connect;
			}
		}
		else
		{
			int rowInd = openNode->row;
			int colInd = openNode->col;

			Node* childNode;
			if (openNode->row - 1 >= 0)
			{
				int childIndRow = openNode->row - 1;
				int childIndCol = openNode->col;

				childNode = GetChildNodes(childIndRow, childIndCol, openNode);
			}

			if (openNode->row + 1 < MazeSize)
			{
				int childIndRow = openNode->row + 1;
				int childIndCol = openNode->col;

				childNode = GetChildNodes(childIndRow, childIndCol, openNode);
			}

			if (openNode->col + 1 < MazeSize)
			{
				int childIndRow = openNode->row;
				int childIndCol = openNode->col + 1;

				childNode = GetChildNodes(childIndRow, childIndCol, openNode);
			}

			if (openNode->col - 1 >= 0)
			{
				int childIndRow = openNode->row;
				int childIndCol = openNode->col - 1;

				childNode = GetChildNodes(childIndRow, childIndCol, openNode);
			}

			openList.remove_if([&](Node* node)
				{
					if (node->row == rowInd && node->col == colInd)
					{
						return true;
					}
					else
					{
						return false;
					}
				});
			closeList.push_back(openNode);

			FindPath();
		}
	}
}

inline vec3 getPositionFromIndex(int i, int j)
{
	float unit = 1;
	vec3 leftTopPosition = vec3(-MazeSize / 2.0 + unit / 2, 0, -MazeSize / 2.0 + unit / 2);
	vec3 xDir = vec3(1, 0, 0);
	vec3 zDir = vec3(0, 0, 1);
	return leftTopPosition + i * xDir + j * zDir;
}

void LoadMaze()
{
	FILE* file = fopen(MAZE_FILE, "r");
	char buf[255];
	fgets(buf, 255, file);
	sscanf(buf, "%d", &MazeSize);
	for (int j = 0; j < MazeSize; j++)
	{
		fgets(buf, 255, file);
		for (int i = 0; i < MazeSize; i++)
		{
			maze[i][j] = buf[i];
			if (maze[i][j] == 'C')				// Setup Camera Position
				cameraPos = getPositionFromIndex(i, j);
			if (maze[i][j] == 'G')				// Setup Goal Position
				goalPos = getPositionFromIndex(i, j);
		}
	}
	fclose(file);
}



float getDistance(vec3 a, vec3 b)
{
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z));
}

void DrawMaze()
{
	for (int j = 0; j < MazeSize; j++)
		for (int i = 0; i < MazeSize; i++)
			if (maze[i][j] == '*')
			{
				vec3 color = vec3(i / (float)MazeSize, j / (float)MazeSize, 1);
				mat4 ModelMat = Translate(getPositionFromIndex(i, j));
				glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * ModelMat);
				glUniform4f(uColor, color.x, color.y, color.z, 1);
				if(collision = true && getDistance(getPositionFromIndex(i, j), cameraPos) < (1.5) / 2.0f)
					glUniform4f(uColor, 1,0, 0, 1);
				cube.Draw(program);
			}
}

bool checkValidPosition()
{
	float cameraSize = 0.5;
	float boxSize = 1;

	for (int j = 0; j < MazeSize; j++)
		for (int i = 0; i < MazeSize; i++)
		{
			if (maze[i][j] == '*' && getDistance(getPositionFromIndex(i, j), cameraPos) < (cameraSize + boxSize) / 2.0f)
			{
				collision = true;
				return false;
			}
		}
	collision = false;
	return true;
}

void myInit()
{
	LoadMaze();
	cube.Init();
	program = InitShader("vshader.glsl", "fshader.glsl");

}

void DrawDirection()
{
	float w = MazeSize;
	float h = MazeSize;

	Node* parent;
	Node* child;
	Node* origin = path;
	while(path != nullptr && path->connect != nullptr)
	{
		parent = path->connect;
		child = path;
		mat4 ModelMat;
		if (parent->row > child->row)
			ModelMat = Translate(getPositionFromIndex(child->row, child->col) + vec3(0.5, -0.4,0)) * Scale(1, 0.02, 0.02);
		else if(parent->row < child->row)
			ModelMat = Translate(getPositionFromIndex(child->row, child->col) + vec3(-0.5, -0.4, 0)) * Scale(1, 0.02, 0.02);
		else if(parent->col > child->col)
			ModelMat = Translate(getPositionFromIndex(child->row, child->col) + vec3(0, -0.4, 0.5)) * Scale(0.02, 0.02, 1);
		else if(parent->col < child->col)
			ModelMat = Translate(getPositionFromIndex(child->row, child->col) + vec3(0, -0.4, -0.5)) * Scale(0.02, 0.02, 1);
		glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * ModelMat);
		glUniform4f(uColor, 1, 0, 0, 1);
		cube.Draw(program);
		path = path->connect;
	}
	path = origin;
}

float findAngleBetweenTwoVec(vec3 v1, vec3 v2)
{
	int direction = 1;
	if (cross(v1, v2).y > 0)
		direction = -1;
	return (direction * acos((v1.x * v2.x + v1.y * v2.y + v1.z * v2.z)
		/ (sqrt(v1.x * v1.x + v1.y * v1.y + v1.z * v1.z) * sqrt(v2.x * v2.x + v2.y * v2.y + v2.z * v2.z))) / 3.141592 * 180);
}

void ReversePath()
{
	Node* p, *q, *r;
	p = path;
	q = NULL;
	while (p != nullptr)
	{
		r = q;
		q = p;
		p = p->connect;
		q->connect = r;
	}
	path = q;
	Node* openNode = path;
}

void FollowPath()
{
	if (cameraPath == nullptr || (cameraPath != nullptr && cameraPath->connect == nullptr))
	{
		followPath = false;
		return;
	}
	float distance = getPositionFromIndex(1, 1).x - getPositionFromIndex(0, 1).x;
	tuple<float, float> current = getFloatPosition(cameraPos);
	float current_y = get<0>(current);
	float current_x = get<1>(current);
	vec3 moveDirection = vec3(cameraPath->connect->row, 0, cameraPath->connect->col) - vec3(cameraPath->row, 0, cameraPath->col);
	if (angleCnt == 0 && abs(findAngleBetweenTwoVec(viewDirection, moveDirection)) > 1)
	{
		rotateAngle = findAngleBetweenTwoVec(moveDirection, viewDirection) * 3.141592 / 180;
		changeDirection = true;
	}
	if (changeDirection)
	{
		if (angleCnt < 30)
		{
			viewDirection = mat3(cos(rotateAngle / 30), 0, -sin(rotateAngle / 30), 0, 1, 0, sin(rotateAngle / 30), 0, cos(rotateAngle / 30)) * viewDirection;
			angleCnt++;
		}
		else
		{
			angleCnt = 0;
			changeDirection = false;
		}
	}
	else
	{
		if (moveCnt < 20)
		{
			cameraPos += normalize(moveDirection) * distance * 0.05f;
			moveCnt++;
		}
		else
		{
			moveCnt = 0;
			cameraPath = cameraPath->connect;
		}
	}
}

void DrawGrid()
{
	float n = 40;
	float w = MazeSize;
	float h = MazeSize;

	for (int i = 0; i < n; i++)
	{
		mat4 m = Translate(0, -0.5, -h / 2 + h / n * i) * Scale(w, 0.02, 0.02);
		glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * m);
		glUniform4f(uColor, 1, 1, 1, 1);
		cube.Draw(program);
	}
	for (int i = 0; i < n; i++)
	{
		mat4 m = Translate(-w / 2 + w / n * i, -0.5, 0) * Scale(0.02, 0.02, h);
		glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * m);
		glUniform4f(uColor, 1, 1, 1, 1);
		cube.Draw(program);
	}
}

void drawCamera()
{
	float cameraSize = 0.5;

	viewDirection, vec3(0, 0, -1);
	mat4 ModelMat = Translate(cameraPos) * Scale(vec3(cameraSize)) * RotateY(findAngleBetweenTwoVec(viewDirection, vec3(0,0,-1)));
	glUseProgram(program);
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * ModelMat);
	glUniform4f(uColor, 0, 1, 0, 1);
	cube.Draw(program);

	ModelMat = Translate(cameraPos + viewDirection * cameraSize / 2) * Scale(vec3(cameraSize / 2)) * RotateY(findAngleBetweenTwoVec(viewDirection, vec3(0, 0, -1)));
	glUseProgram(program);
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * ModelMat);
	glUniform4f(uColor, 0, 1, 0, 1);
	cube.Draw(program);
}

void drawGoal()
{
	glUseProgram(program);
	float GoalSize = 0.7;

	mat4 ModelMat = Translate(goalPos) * RotateY(g_time * 3) * Scale(vec3(GoalSize));
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * ModelMat);
	glUniform4f(uColor, 0, 0, 0, 0);
	cube.Draw(program);

	ModelMat = Translate(goalPos) * RotateY(g_time * 3 + 45) * Scale(vec3(GoalSize));
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * ModelMat);
	glUniform4f(uColor, 0, 0, 0, 0);
	cube.Draw(program);
}


void drawScene(bool bDrawCamera = true)
{
	glUseProgram(program);
	uMat = glGetUniformLocation(program, "uMat");
	uColor = glGetUniformLocation(program, "uColor");

	DrawGrid();
	DrawMaze();
	drawGoal();

	if (followPath)
		FollowPath();
	if (bDrawCamera)
		drawCamera();
	if (drawDirection)
		DrawDirection();
}

void display()
{
	glEnable(GL_DEPTH_TEST);

	float vWidth = wWidth / 2;
	float vHeight = wHeight;

	// LEFT SCREEN : View From Camera (Perspective Projection)
	glViewport(0, 0, vWidth, vHeight);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	float h = 4;
	float aspectRatio = vWidth / vHeight;
	float w = aspectRatio * h;
	mat4 ViewMat = myLookAt(cameraPos, cameraPos + viewDirection, vec3(0, 1, 0));
	mat4 ProjMat = myPerspective(45, aspectRatio, 0.01, 20);

	g_Mat = ProjMat * ViewMat;
	drawScene(false);							// drawing scene except the camera


	// RIGHT SCREEN : View from above (Orthographic parallel projection)
	glViewport(vWidth, 0, vWidth, vHeight);
	h = MazeSize;
	w = aspectRatio * h;
	ViewMat = myLookAt(vec3(0, 5, 0), vec3(0, 0, 0), vec3(0, 0, -1));
	ProjMat = myOrtho(-w / 2, w / 2, -h / 2, h / 2, 0, 20);

	g_Mat = ProjMat * ViewMat;
	drawScene(true);


	glutSwapBuffers();
}

void idle()
{
	g_time += 1;
	if (!checkValidPosition())
	{
		if (direction == 1)
			cameraPos -= cameraSpeed * 2 * viewDirection;
		else if (direction == -1)
			cameraPos += cameraSpeed * 2 * viewDirection;
	}
	if ((GetAsyncKeyState('A') & 0x8000) == 0x8000)		// if "A" key is pressed	: Go Left
	{
		followPath = false;
		viewDirection = mat3(cos(cameraSpeed), 0, -sin(cameraSpeed), 0, 1, 0, sin(cameraSpeed), 0, cos(cameraSpeed)) * viewDirection;
	}
	if ((GetAsyncKeyState('D') & 0x8000) == 0x8000)		// if "D" key is pressed	: Go Right
	{
		followPath = false;
		viewDirection = mat3(cos(-cameraSpeed), 0, -sin(-cameraSpeed), 0, 1, 0, sin(-cameraSpeed), 0, cos(-cameraSpeed)) * viewDirection;
	}
	if ((GetAsyncKeyState('W') & 0x8000) == 0x8000)		// if "W" key is pressed	: Go Forward
	{
		followPath = false;
		direction = 1;
		cameraPos += cameraSpeed * viewDirection;
	}
	if ((GetAsyncKeyState('S') & 0x8000) == 0x8000)		// if "S" key is pressed	: Go Backward
	{
		followPath = false;
		direction = -1;
		cameraPos -= cameraSpeed * viewDirection;
	}
	if ((GetAsyncKeyState('Q') & 0x8000) == 0x8000)
	{
		followPath = false;
		tuple<int, int> pos = getIndexFromPosition(cameraPos);
		Node* startNode = new Node(get<0>(pos), get<1>(pos), 'C');
		openList.clear();
		closeList.clear();
		path = nullptr;
		openList.push_back(startNode);
		FindPath();
		ReversePath();
		drawDirection = true;
	}
	if ((GetAsyncKeyState(' ') & 0x8000) == 0x8000)
	{
		followPath = true;
		cameraPath = path;
		cameraPos = getPositionFromIndex(cameraPath->row, cameraPath->col);
		moveCnt = 0;
		angleCnt = 0;
		
		if (cameraPath != nullptr && cameraPath->connect != nullptr)
		{
			vec3 moveDirection = vec3(cameraPath->connect->row, 0, cameraPath->connect->col) - vec3(cameraPath->row, 0, cameraPath->col);
			viewDirection = moveDirection;
			changeDirection = false;
		}
	}
	Sleep(16);											// for vSync
	glutPostRedisplay();
}

void reshape(int wx, int wy)
{
	printf("%d %d \n", wx, wy);
	wWidth = wx;
	wHeight = wy;
	glutPostRedisplay();
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(wWidth, wHeight);

	glutCreateWindow("Homework3 (Maze Navigator)");

	glewExperimental = true;
	glewInit();

	printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION),
		glGetString(GL_SHADING_LANGUAGE_VERSION));

	myInit();
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	glutMainLoop();

	return 0;
}