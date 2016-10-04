#include <loguru/loguru.hpp>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"

static void error_callback(int error, const char* description)
{
    LOG_F(ERROR, "Error %d: %s\n", error, description);
}

int main(void)
{
    GLFWwindow* window;

    glfwSetErrorCallback(error_callback);

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(800, 600, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (gl3wInit()) {
        LOG_F(FATAL, "Failed to initialize OpenGL\n");
        return -1;
    }

    if (!gl3wIsSupported(3, 2)) {
        LOG_F(FATAL, "OpenGL 3.2 not supported\n");
        return -1;
    }

    LOG_F(INFO, "OpenGL %s, GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));

    ImGui_ImplGlfwGL3_Init(window, true);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        ImGui_ImplGlfwGL3_NewFrame();

        ImGui::Text("Hello, world!");

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);


        ImGui::Render();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}