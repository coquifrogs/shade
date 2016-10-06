#include <loguru/loguru.hpp>
#include "cli.h"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"

#include "shader.h"

struct ShadeConfig {
    Program* program;
    Shader* vertexShader;
    Shader* fragmentShader;
    GLuint vao;
    GLuint vertexBuffer;
    GLuint indexBuffer;
};

static void error_callback(int error, const char* description) {
    LOG_F(ERROR, "Error %d: %s\n", error, description);
}

GLFWwindow* setupGLFW();
bool setupGLObjects(ShadeConfig& config, const char* shaderFile);
bool loadErrorShader(ShadeConfig& config);
bool loadShader(ShadeConfig& config, const char* shaderFile);
void printUsage();

int main(int argc, const char* argv[]) {
    ShadeConfig shadeConfig;
    bool verbose = false;
    const char* shaderFile;

    cli::Parser parser = {
        cli::OptionFlag('v', "verbose", "output logging info", &verbose)
    };

    if(!parser.parse(argc, argv)) {
        printUsage();
        parser.printOptionsUsage();
        return EXIT_FAILURE;
    }

    if(parser.getRemainingArgs().size() < 1) {
        // Use default shader
        shaderFile = NULL;
    } else {
        shaderFile = parser.getRemainingArgs()[0];
    }

    if(verbose) {
        loguru::g_stderr_verbosity = loguru::Verbosity_INFO;
    } else {
        loguru::g_stderr_verbosity = loguru::Verbosity_OFF;
    }

    GLFWwindow* window = setupGLFW();
    if(!window) {
        return EXIT_FAILURE;
    }

    setupGLObjects(shadeConfig, shaderFile);

    GLint timeLocation = glGetUniformLocation(shadeConfig.program->getID(), "Time");

    while (!glfwWindowShouldClose(window)) {
        ImGui_ImplGlfwGL3_NewFrame();

        ////////// BEGIN FRAME ///////////

        glClear(GL_COLOR_BUFFER_BIT);

        ImGui::BeginMainMenuBar();
        if(ImGui::BeginMenu("File")) {
            if(ImGui::MenuItem("Reload", "CTRL+R")) {
                loadShader(shadeConfig, shaderFile);
            }
            ImGui::EndMenu();
        }
        
        ImGui::EndMainMenuBar();

        shadeConfig.program->use();

        if (timeLocation != -1) {
           glUniform1f(timeLocation, glfwGetTime());
        }

        CHECK_GL(glBindVertexArray(shadeConfig.vao));
        CHECK_GL(glEnableVertexAttribArray(0));
        CHECK_GL(glEnableVertexAttribArray(1));
        
        CHECK_GL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
        
        CHECK_GL(glDisableVertexAttribArray(0));
        CHECK_GL(glDisableVertexAttribArray(1));
        CHECK_GL(glBindVertexArray(0));

        //////////// END FRAME ///////////
        ImGui::Render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

GLfloat _quadVerts[] = {
    // X     Y     Z     U    V
      1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
     -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
      1.0f,-1.0f, 0.0f, 1.0f, 0.0f,
     -1.0f,-1.0f, 0.0f, 0.0f, 0.0f
};

GLint _quadIndices[] = {
    0, 1, 2,
    2, 1, 3
};

const GLchar *vertex_shader =
    R"raw(
        #version 330
        layout(location = 0) in vec3 Pos;
        layout(location = 1) in vec2 UV;
        out vec2 Frag_UV;
        void main(){
            Frag_UV = UV;
            gl_Position = vec4(Pos, 1.0);
        }
    )raw";

const GLchar *frag_shader =
    R"raw(
        #version 330
        in vec2 Frag_UV;
        layout(location = 0) out vec4 Out_Color;
        void main()
        {
           Out_Color = vec4(Frag_UV, 0.0, 1.0);
        }
    )raw";

const GLchar *error_frag_shader =
    R"raw(
        #version 330
        in vec2 Frag_UV;
        layout(location = 0) out vec4 Out_Color;
        void main()
        {
           Out_Color = vec4(1.0, 0.0, 1.0, 1.0);
        }
    )raw";

bool setupGLObjects(ShadeConfig& config, const char* shaderFile) {
    glGenVertexArrays(1, &config.vao);
    glBindVertexArray(config.vao);

    CHECK_GL(glGenBuffers(1, &config.vertexBuffer));
    CHECK_GL(glGenBuffers(1, &config.indexBuffer));


    CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, config.vertexBuffer));
    CHECK_GL(glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(GLfloat), _quadVerts, GL_STATIC_DRAW));

    CHECK_GL(glEnableVertexAttribArray(0));
    CHECK_GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (char*)0 + (0 * sizeof(GLfloat))));

    CHECK_GL(glEnableVertexAttribArray(1));
    CHECK_GL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (char*)0 + (3 * sizeof(GLfloat))));

    CHECK_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, config.indexBuffer));
    CHECK_GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLint), _quadIndices, GL_STATIC_DRAW));

    config.vertexShader = config.fragmentShader = nullptr;
    config.program = nullptr;
    bool success = loadShader(config, shaderFile);
    //bool success = loadErrorShader(config);

    CHECK_GL(glDisableVertexAttribArray(0));
    CHECK_GL(glDisableVertexAttribArray(1));
    CHECK_GL(glBindVertexArray(0));
    
    return success;
}

bool loadErrorShader(ShadeConfig& config) {
    Shader* vertexShader = new Shader;
    if(!vertexShader->compile(GL_VERTEX_SHADER, 0, vertex_shader, "<builtin vertex>")) {
        delete vertexShader;
        return false;
    }

    Shader* fragmentShader = new Shader;
    if(!fragmentShader->compile(GL_FRAGMENT_SHADER, 0, error_frag_shader, "<builtin error fragment>")) {
        delete vertexShader;
        delete fragmentShader;
        return false;
    }

    Program* program = new Program(vertexShader, fragmentShader);
    program->bindAttribLocation(0, "Pos");
    program->bindAttribLocation(1, "UV");
    if(!program->link()) {
        delete program;
        delete vertexShader;
        delete fragmentShader;
        return false;
    }

    if(config.program) { delete config.program; }
    if(config.vertexShader) { delete config.vertexShader; }
    if(config.fragmentShader) { delete config.fragmentShader; }
    
    config.vertexShader = vertexShader;
    config.fragmentShader = fragmentShader;
    config.program = program;
    return true;
}

bool loadShader(ShadeConfig& config, const char* shaderFile) {
    Shader* vertexShader = new Shader;
    if(!vertexShader->compile(GL_VERTEX_SHADER, 0, vertex_shader, "<builtin vertex>")) {
        delete vertexShader;
        loadErrorShader(config);
        return false;
    }

    Shader* fragmentShader = new Shader;
    if(shaderFile) {
        if(!fragmentShader->compile(GL_FRAGMENT_SHADER, shaderFile)) {
            delete vertexShader;
            delete fragmentShader;
            loadErrorShader(config);
            return false;
        }
    } else {
        if(!fragmentShader->compile(GL_FRAGMENT_SHADER, 0, frag_shader, "<builtin fragment>")) {
            delete vertexShader;
            delete fragmentShader;
            loadErrorShader(config);
            return false;
        }
    }

    Program* program = new Program(vertexShader, fragmentShader);
    program->bindAttribLocation(0, "Pos");
    program->bindAttribLocation(1, "UV");
    if(!program->link()) {
        delete program;
        delete vertexShader;
        delete fragmentShader;
        loadErrorShader(config);
        return false;
    }

    if(config.program) { delete config.program; }
    if(config.vertexShader) { delete config.vertexShader; }
    if(config.fragmentShader) { delete config.fragmentShader; }
    
    config.vertexShader = vertexShader;
    config.fragmentShader = fragmentShader;
    config.program = program;
    return true;
}

GLFWwindow* setupGLFW() {
    GLFWwindow* window;

    glfwSetErrorCallback(error_callback);

    /* Initialize the library */
    if (!glfwInit())
        return NULL;

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(800, 600, "Shade", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return NULL;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (gl3wInit()) {
        LOG_F(FATAL, "Failed to initialize OpenGL\n");
        return NULL;
    }

    if (!gl3wIsSupported(3, 2)) {
        LOG_F(FATAL, "OpenGL 3.2 not supported\n");
        return NULL;
    }

    LOG_F(INFO, "OpenGL %s, GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));

    ImGui_ImplGlfwGL3_Init(window, true);

    return window;
}

void printUsage() {
    fprintf(stderr, 
        "Usage: shade [options] [SHADER_FILE]\n\n"
        "    Renders the shader in a window.\n\n");
}