#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <gli/gli.hpp>

#include <shader.h>
#include <utils.h>

static const GLfloat skyboxVertices[] = {
	-1.0f, 1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,

	-1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,

	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,

	-1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, 1.0f
};

using namespace glm;

void loadTexture(GLint idx, gli::texture const &texture)
{
	glTexImage2D(idx, 0, GL_RGB,
		texture.extent().x,
		texture.extent().y,
		0, GL_RGB, GL_UNSIGNED_BYTE, texture.data());
}

GLuint loadSkybox(std :: string filepath)
{
	GLuint textureId;
	glGenTextures(1, &textureId);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

	gli::texture texturePlusX = gli::load(filepath + "+x.dds");
	assert(!texturePlusX.empty());
	gli::texture texturePlusY = gli::load(filepath + "+y.dds");
	assert(!texturePlusY.empty());
	gli::texture texturePlusZ = gli::load(filepath + "+z.dds");
	assert(!texturePlusZ.empty());

	gli::texture textureMinusX = gli::load(filepath + "-x.dds");
	assert(!textureMinusX.empty());
	gli::texture textureMinusY = gli::load(filepath + "-y.dds");
	assert(!textureMinusY.empty());
	gli::texture textureMinusZ = gli::load(filepath + "-z.dds");
	assert(!textureMinusZ.empty());

	loadTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_X, texturePlusX);
	loadTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, texturePlusY);
	loadTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, texturePlusZ);
	
	loadTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, textureMinusX);
	loadTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, textureMinusY);
	loadTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, textureMinusZ);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return textureId;
}

int main() 
{
	GLFWwindow* window = init("OpenGL hw2");

	assert(window);

	GLuint skyboxShader = loadShaders("shaders/skybox.vrtx", "shaders/skybox.frag");
	GLuint sceneShader = loadShaders("shaders/scene.vrtx", "shaders/scene.frag");

	GLuint skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);

	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
	glBindVertexArray(0);

	GLuint skyboxTexture = loadSkybox("textures/skybox");
	assert(skyboxTexture);

	std::pair<std::vector<GLfloat>, std::vector<GLfloat>> scene = loadScene("textures/bunny_with_normals.obj");

	std::vector<GLfloat> verts = scene.first;
	std::vector<GLfloat> norms = scene.second;

	GLuint vertexBuffer, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &vertexBuffer);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), verts.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), nullptr);

	GLuint normsBuffer;
	glGenBuffers(1, &normsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normsBuffer);
	glBufferData(GL_ARRAY_BUFFER, norms.size() * sizeof(GLfloat), norms.data(), GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, normsBuffer);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), nullptr);
	
	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
	{
		double time = glfwGetTime();

		glfwPollEvents();
		glClearColor(255.0f, 255.0f, 255.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		mat4 projection = perspective(45.0f, (720.0f / 540.0f), 0.1f, 100.0f);
		vec3 cameraPos(sin(time * 3.145926f * 0.1f), sin(time * 3.145926f * 0.5f * 0.2f), cos(time * 0.2));
		mat4 view(lookAt(cameraPos, vec3(0, 0, 0), vec3(0, 1, 0)));
		
		GLint loc;

		glUseProgram(sceneShader);
		loc = glGetUniformLocation(sceneShader, "viewProjection");
		assert(loc != -1);
		
		glUniformMatrix4fv(loc, 1, GL_FALSE, value_ptr(projection * view));

		loc = glGetUniformLocation(sceneShader, "fromCamera");
		assert(loc != -1);
		
		glUniform3fv(loc, 1, value_ptr(normalize(-cameraPos)));

		glBindVertexArray(VAO);
		
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
		glDrawArrays(GL_TRIANGLES, 0, verts.size() / 3);

		glUseProgram(skyboxShader);
		
		loc = glGetUniformLocation(skyboxShader, "viewProjection");
		assert(loc != -1);

		glUniformMatrix4fv(loc, 1, GL_FALSE, value_ptr(projection * view));
		glBindVertexArray(skyboxVAO);

		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}

	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &normsBuffer);
	glDeleteVertexArrays(1, &VAO);
	glDeleteProgram(sceneShader);
	glDeleteProgram(skyboxShader);

	glfwTerminate();
	return 0;
}