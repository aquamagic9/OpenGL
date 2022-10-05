#ifndef _MY_COLOR_CUBE_H_
#define _MY_COLOR_CUBE_H_

#include <vgl.h>
#include <vec.h>

struct MyCubeVertex
{
	vec4 position;
	vec4 color;
};


class MyColorCube
{
public:
	const int		NUM_POINTS = 36;
	MyCubeVertex*	vertices;
	GLuint			vao;
	GLuint			vbo;

	void			init();
	void			setRectangle(int a, int b, int c, int d, vec4 *vertices_pos, vec4 *vertices_color);
	void			draw(GLuint prog, float uTime);
	void			setAttributePointers(GLuint prog);
};

void MyColorCube::setRectangle(int a, int b, int c, int d, vec4* vertices_pos, vec4* vertices_color)
{
	static int index = 0;
	vertices[index].position = vertices_pos[a];		vertices[index].color = vertices_color[a]; index++;
	vertices[index].position = vertices_pos[b];		vertices[index].color = vertices_color[b]; index++;
	vertices[index].position = vertices_pos[c];		vertices[index].color = vertices_color[c]; index++;

	vertices[index].position = vertices_pos[c];		vertices[index].color = vertices_color[c]; index++;
	vertices[index].position = vertices_pos[d];		vertices[index].color = vertices_color[d]; index++;
	vertices[index].position = vertices_pos[a];		vertices[index].color = vertices_color[a]; index++;
}

void MyColorCube::init()
{
	MyCubeVertex	*vertices = new MyCubeVertex[NUM_POINTS];

	vec4 vertices_pos[8] = {
		vec4(-0.5,-0.5,0.5,1.0),
		vec4(-0.5, 0.5,0.5,1.0),
		vec4(0.5, 0.5,0.5,1.0),
		vec4(0.5,-0.5,0.5,1.0),
		vec4(-0.5,-0.5,-0.5,1.0),
		vec4(-0.5, 0.5,-0.5,1.0),
		vec4(0.5, 0.5,-0.5,1.0),
		vec4(0.5,-0.5,-0.5,1.0)
	};

	vec4 vertices_color[8] = {
		vec4(1,1,1,1),
		vec4(1,0,0,1),
		vec4(0,1,0,1),
		vec4(0,0,1,1),
		vec4(1,1,0,1),
		vec4(1,0,1,1),
		vec4(0,1,1,1),
		vec4(0,0,0,1)
	};

	// 1. create data in cpu
	setRectangle(0, 3, 2, 1, vertices_pos, vertices_color);
	setRectangle(4, 5, 6, 7, vertices_pos, vertices_color);
	setRectangle(0, 4, 7, 3, vertices_pos, vertices_color);
	setRectangle(3, 7, 6, 2, vertices_pos, vertices_color);
	setRectangle(2, 6, 5, 1, vertices_pos, vertices_color);
	setRectangle(0, 1, 5, 4, vertices_pos, vertices_color);
	
	// 2. seding the data to gpu
	// 2-1. create vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	// 2-2. create vertex buffer object
	GLuint vbo;
	glGenBuffers(1, & vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MyCubeVertex) + sizeof(NUM_POINTS),
		NULL, GL_STATIC_DRAW);
	delete[] vertices;
}

void	MyColorCube::setAttributePointers(GLuint prog)
{
	// 4. connect the data with the shaders
	GLuint vPosition = glGetAttribLocation(prog, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, FALSE, sizeof(MyCubeVertex),    //4부터 myCubeVertex사이즈까지 라는 의미임.
		BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(prog, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, FALSE, sizeof(vec4),				//color값은 vec4만큼 띄어서
		BUFFER_OFFSET(sizeof(vec4)));
}

void MyColorCube::draw(GLuint prog, float uTime)
{
	glBindVertexArray(vao);

	glUseProgram(prog);
	setAttributePointers(prog);
	GLuint uTime = glGetUniformLocation(prog, "uTime");
	glUniform1f(uTime, gTime);
	
	glDrawArrays(GL_TRIANGLES, 0, NUM_POINTS);
}


#endif
