#include "utils.h"

#include <GL/glew.h>
#include <GL/GL.h>
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp> 

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>


#include <string>
#include <vector>
#include <limits>


using namespace Assimp;
using namespace glm;

GLFWwindow* init(std :: string name)
{
	if (!glfwInit()) {
		fprintf(stderr, "Failed to init GLFW\n");
		return nullptr;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	GLFWwindow* window = glfwCreateWindow(720, 540, name.c_str(), nullptr, nullptr);

	if (!window) {
		fprintf(stderr, "Failed to open GLFW Window\n");
		glfwTerminate();
		return nullptr;
	}

	glfwMakeContextCurrent(window);

	glewExperimental = true;

	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return nullptr;
	}

	glViewport(0, 0, 720, 540);
	return window;
}

std::pair<std::vector<GLfloat>, std::vector<GLfloat>> loadScene(const std :: string& fileName) {
	Importer importer;

	const aiScene* scene = importer.ReadFile(fileName.c_str(), aiProcessPreset_TargetRealtime_Quality);
	assert(scene != nullptr);
	std::vector<GLfloat> vertices;
	std::vector<GLfloat> normals;
	
	vec3 box_bot(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	vec3 box_top(-box_bot);

	for (size_t i = 0; i < scene->mNumMeshes; ++i)
	{
		const aiMesh* mesh = scene->mMeshes[i];
		assert(mesh->HasNormals());
		for (size_t j = 0; j < mesh->mNumFaces; ++j)
		{
			const aiFace face = mesh->mFaces[j];

			assert(face.mNumIndices <= 3);
			if (face.mNumIndices != 3)
			{
				continue;
			}
			for (size_t k = 0; k < face.mNumIndices; ++k)
			{
				size_t index = face.mIndices[k];
				vertices.push_back(mesh->mVertices[index].x);
				box_bot.x = min(mesh->mVertices[index].x, box_bot.x);
				box_top.x = max(mesh->mVertices[index].x, box_top.x);

				vertices.push_back(mesh->mVertices[index].y);
				box_bot.y = min(mesh->mVertices[index].y, box_bot.y);
				box_top.y = max(mesh->mVertices[index].y, box_top.y);

				vertices.push_back(mesh->mVertices[index].z);
				box_bot.z = min(mesh->mVertices[index].z, box_bot.z);
				box_top.z = max(mesh->mVertices[index].z, box_top.z);

				normals.push_back(mesh->mNormals[index].x);
				normals.push_back(mesh->mNormals[index].y);
				normals.push_back(mesh->mNormals[index].z);
			}
		}
	}

	vec3 center = (box_bot + box_bot) / 2.0f;
	vec3 sizes = box_top - box_bot;
	float mult = 1.0f / max(max(sizes.x, sizes.y), sizes.z);

	for (size_t i = 0; i < vertices.size(); i += 3)
	{
		vertices[i] = (vertices[i] - center.x) * mult;
		vertices[i + 1] = (vertices[i + 1] - center.y) * mult;
		vertices[i + 2] = (vertices[i + 2] - center.z) * mult;
	}

	return std :: make_pair(vertices, normals);
}