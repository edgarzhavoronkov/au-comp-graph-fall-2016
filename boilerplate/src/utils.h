#pragma once

#include <GL/glew.h>
#include <GL/GL.h>
#include <GLFW/glfw3.h> 

#include <vector>
#include <utility>
#include <string>

GLFWwindow* init(std::string name);

std::pair<std::vector<GLfloat>, std::vector<GLfloat>> loadScene(const std::string& fileName);
