#ifndef _MY_PLANE_H_
#define _MY_PLANE_H_

#include <vgl.h>
#include <vec.h>

struct MyPlaneVertex
{
	vec4 position;
	vec4 color;
};

struct MyPlaneInfo
{
	float width;
	float height;
	float x;
	float y;
};

class MyPlane
{
public:
	int NUM_POINTS;
	int rect_length = 2;
	MyPlaneInfo info;
	MyPlaneVertex* vertices;
	GLuint vao;
	GLuint vbo;
	int vertices_index = 0;

	void init();
	void setRectangle(int a, int b, int c, vec4* vertex_pos, vec4* vertex_color, int row, int col);
	void draw(GLuint prog, float gTime, int gWave);
	void setAttributePointers(GLuint prog);
};

void MyPlane::setRectangle(int a, int b, int c, vec4* vertex_pos, vec4* vertex_color, int row, int col)
{
	int color_flag = (row + col) % 2;
	vec4 plane_default = vec4(info.x, info.y, 0.0, 1.0) + vertex_pos[a] * (float)row + vertex_pos[b] * (float)col;
	vertices[vertices_index].position = plane_default;						vertices[vertices_index].color = vertex_color[color_flag]; vertices_index++;
	vertices[vertices_index].position = plane_default + vertex_pos[a];		vertices[vertices_index].color = vertex_color[color_flag]; vertices_index++;
	vertices[vertices_index].position = plane_default + vertex_pos[b];		vertices[vertices_index].color = vertex_color[color_flag]; vertices_index++;

	vertices[vertices_index].position = plane_default + vertex_pos[a];		vertices[vertices_index].color = vertex_color[color_flag]; vertices_index++;
	vertices[vertices_index].position = plane_default + vertex_pos[b];		vertices[vertices_index].color = vertex_color[color_flag]; vertices_index++;
	vertices[vertices_index].position = plane_default + vertex_pos[c];		vertices[vertices_index].color = vertex_color[color_flag]; vertices_index++;
}

void MyPlane::init()
{
	vertices_index = 0;
	NUM_POINTS = rect_length * rect_length * 6;
	vertices = new MyPlaneVertex[NUM_POINTS];
	
	info.width = 1.0f;		info.height = 1.0f;
	info.x = -0.5f;			info.y = 0.5f;

	vec4 vertex_pos[3] = {
		vec4(info.width / (float)rect_length,	0.0,								0.0, 0.0),
		vec4(0.0,								-info.height / (float)rect_length,	0.0, 0.0),
		vec4(info.width / (float)rect_length,	-info.height / (float)rect_length,	0.0, 0.0),
	};

	vec4 vertex_color[4] = {
		vec4(0.5,0.5,0.5,0.5),
		vec4(0.3,0.3,0.3,0.3)
	};

	for (int i = 0; i < rect_length; i++)
		for (int j = 0; j < rect_length; j++)
			setRectangle(0, 1, 2, vertex_pos, vertex_color, i, j);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MyPlaneVertex) * NUM_POINTS,
		vertices, GL_STATIC_DRAW);

	delete[] vertices;

}

void MyPlane::setAttributePointers(GLuint prog)
{
	GLuint vPosition = glGetAttribLocation(prog, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, FALSE,
		sizeof(MyPlaneVertex), BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(prog, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, FALSE,
		sizeof(MyPlaneVertex), BUFFER_OFFSET(sizeof(vec4)));

}

void MyPlane::draw(GLuint prog, float gTime, int gWave)
{
	glBindVertexArray(vao);

	glUseProgram(prog);
	setAttributePointers(prog);
	GLuint uTime = glGetUniformLocation(prog, "uTime");
	glUniform1f(uTime, gTime);
	GLuint uWave = glGetUniformLocation(prog, "uWave");
	glUniform1f(uWave, (float)gWave);

	glDrawArrays(GL_TRIANGLES, 0, NUM_POINTS);
}

#endif