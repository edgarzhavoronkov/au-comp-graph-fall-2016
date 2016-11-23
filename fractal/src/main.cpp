#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <stdio.h>
#include <algorithm>
#include <vector>

#include <shader.h>
#include <utils.h>

float scale = 2.f;
float offset_x = -1.6f;
float offset_y = -1.0f;
unsigned long max_iter = 50;
bool lbutton_down;

double x_begin, y_begin;
double x_end, y_end;

void scrollCallback(GLFWwindow* window, double x_offset, double y_offset)
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	xpos /= width;
	ypos /= height;
	double scale_mult = pow(1.1, -y_offset);
	offset_x = static_cast<float>(offset_x - xpos * scale * (scale_mult - 1.0));
	offset_y = static_cast<float>(offset_y - ypos * scale * (scale_mult - 1.0));
	scale *= static_cast<float>(scale_mult);
}


//TODO: implement
void mouseCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS) lbutton_down = true;
		if (action == GLFW_RELEASE) lbutton_down = false;
	}

	if (lbutton_down)
	{
		glfwGetCursorPos(window, &x_begin, &y_begin);
	}
	else
	{
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		glfwGetCursorPos(window, &x_end, &y_end);

		x_end -= x_begin;
		y_end -= y_begin;

		offset_x -= static_cast<float>(scale * x_end / width);
		offset_y -= static_cast<float>(scale * y_end / height);
	}
}


int main(int argc, char** argv)
{
	GLFWwindow* window = init("OpenGL hw1");

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	GLuint vertex_array_id;
	glGenVertexArrays(1, &vertex_array_id);
	glBindVertexArray(vertex_array_id);


	GLuint shader_programme = loadShaders(
		"shaders/mdbrt.vert",
		"shaders/mdbrt.frag");

	static const GLfloat g_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f

	};

	static const GLfloat uv_buffer_data[] = {
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
	};

	GLuint vertex_buffer;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	GLuint uv_buffer;
	glGenBuffers(1, &uv_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uv_buffer_data), uv_buffer_data, GL_STATIC_DRAW);

	glfwSetScrollCallback(window, scrollCallback);
	glfwSetMouseButtonCallback(window, mouseCallback);

	static const size_t TEXTURE_WIDTH = 128;
	GLuint texture_id = 0;
	glBindTexture(GL_TEXTURE_1D, texture_id);

	std::vector<glm::vec3> texture;
	texture.reserve(TEXTURE_WIDTH);
	for (size_t i = 0; i < TEXTURE_WIDTH; ++i)
	{
		float r = i / (TEXTURE_WIDTH * 1.0f);
		texture.push_back(glm::vec3(r, 1.0 - r, r * r));
	}
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, TEXTURE_WIDTH, 0, GL_RGB, GL_FLOAT, &texture.front());


	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	do
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(shader_programme);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

		GLint loc;

		loc = glGetUniformLocation(shader_programme, "max_iter");
		assert(loc != -1);
		glUniform1i(loc, max_iter);

		loc = glGetUniformLocation(shader_programme, "texture_sampler");
		assert(loc != -1);
		glUniform1i(loc, texture_id);

		loc = glGetUniformLocation(shader_programme, "scale");
		assert(loc != -1);
		glUniform1f(loc, scale);

		loc = glGetUniformLocation(shader_programme, "offset");
		assert(loc != -1);
		glUniform2f(loc, offset_x, offset_y);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(0);
		glfwSwapBuffers(window);
		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		{
			max_iter += 1;
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		{
			max_iter = std::max(static_cast<unsigned long>(1), max_iter - 1);
		}
	}
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	glDeleteBuffers(1, &vertex_buffer);
	glDeleteVertexArrays(1, &vertex_array_id);
	glDeleteProgram(shader_programme);

	glfwTerminate();
	return 0;
}
