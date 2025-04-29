#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <exception>
#include "viewer.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

const std::string vertexPath = "../shaders/vertex.glsl";
const std::string fragmentPath = "../shaders/fragment.glsl";

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

GLuint createShaderProgram() {
    std::string paths[2] { vertexPath, fragmentPath };
    GLenum types[2] { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER };
    unsigned int ids[2];
    int success;

    for(size_t i = 0; i < 2; ++i) {
        std::ifstream s;
        s.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        s.open(paths[i]);
        std::stringstream buffer;
        buffer << s.rdbuf();
        std::string content = buffer.str();
        const char* contentRaw = content.c_str();

        ids[i] = glCreateShader(types[i]);
        glShaderSource(ids[i], 1, &contentRaw, NULL);
        glCompileShader(ids[i]);

        glGetShaderiv(ids[i], GL_COMPILE_STATUS, &success);
        if(!success) {
            std::cerr << "compilation of shader at path " << paths[i] << " failed" << std::endl;
            for(size_t j = 0; j < i; ++j) glDeleteShader(ids[j]);
            throw std::runtime_error {""};
        }
    }

    GLuint id = glCreateProgram();
    glAttachShader(id, ids[0]);
    glAttachShader(id, ids[1]);
    glLinkProgram(id);

    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if(!success) {
        std::cerr << "linking of shader program failed" << std::endl;
        throw std::runtime_error {""};
    }

    glDeleteShader(ids[0]);
    glDeleteShader(ids[1]);

    return id;
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "objviewer", NULL, NULL);
    if(window == NULL) {
        std::cerr << "window creation failed" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cerr << "failed to initialize GLAD" << std::endl;
        return -1;
    }

    GLuint shaderId = createShaderProgram();

    Viewer viewer {shaderId};
    viewer.init("../file.obj");

    while(!glfwWindowShouldClose(window)) {
        viewer.update();
        viewer.render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shaderId);
    glfwTerminate();
    return 0;
}
