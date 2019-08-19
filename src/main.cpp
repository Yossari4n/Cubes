#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>


/*************
 * Callbacks *
 *************/
void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}


/********************************
 * OpenGL utilities and helpers *
 ********************************/
struct Window {
    GLFWwindow* const handler;
    const int width;
    const int height;
};

GLuint CreateShader(const char* source, GLenum shader_type) {
    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &source, '\0');
    glCompileShader(shader);
    return shader;
}

GLuint CreateProgram(GLuint vertex_shader, GLuint fragment_shader) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    return program;
}


/******************************************
 * Sources of shaders used in the program *
 ******************************************/
const char* VertexShaderSource =
"#version 330 core\n"
"layout(location = 0) in vec3 aPos;\n"
"layout(location = 1) in vec3 aColor;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"out vec4 VertexColor;\n"
"void main() {\n"
"    VertexColor = vec4(aColor, 1.0);\n"
"    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"}\n\0";

const char* FragmentShaderSource =
"#version 330 core\n"
"in vec4 VertexColor;\n"
"out vec4 FragColor;\n"
"void main() {\n"
"    FragColor = VertexColor;\n"
"}\n\0";


/***************************************
 * Visualizations forward declarations *
 ***************************************/
void CubeWave(Window* window, GLuint shader_program);


int main() {
    // Initialize glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create window
    Window window = { glfwCreateWindow(800, 600, "Cubes!", nullptr, nullptr), 800, 600 };
    if (!window.handler) {
        std::cout << "Failed to initialize window\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window.handler);

    // Set callbacks
    glfwSetFramebufferSizeCallback(window.handler, FramebufferSizeCallback);

    // Load glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // Shader program
    GLuint vertex_shader = CreateShader(VertexShaderSource, GL_VERTEX_SHADER);
    GLuint fragment_shader = CreateShader(FragmentShaderSource, GL_FRAGMENT_SHADER);
    GLuint shader_program = CreateProgram(vertex_shader, fragment_shader);    
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    //
    CubeWave(&window, shader_program);

    // End of application
    glfwTerminate();
    return EXIT_SUCCESS;
}


void CubeWave(Window* window, GLuint shader_program) {
    constexpr int ROWS = 15;
    constexpr int COLUMNS = 15;
    constexpr float MIN_CUBE_HEIGHT = 5.0f;
    constexpr float CUBE_HEIGHT_MULTIPLIER = 3.0f;
    constexpr float SIN_MULTIPLIER = 2.0f;

    // Verticies
    constexpr float cube[] = {
        // coords             color
        // back
        -0.5f, -0.5f, -0.5f,  1.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  1.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  1.0f,  1.0f,  1.0f,

        // front
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  0.18f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  0.18f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  0.18f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  0.18f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  0.18f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  0.18f,

        // left
        -0.5f,  0.5f,  0.5f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  1.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  1.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  1.0f,  1.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  1.0f,  1.0f,  1.0f,

        // right
         0.5f,  0.5f,  0.5f,  0.65f,  0.8f,  0.6f,
         0.5f,  0.5f, -0.5f,  0.65f,  0.8f,  0.6f,
         0.5f, -0.5f, -0.5f,  0.65f,  0.8f,  0.6f,
         0.5f, -0.5f, -0.5f,  0.65f,  0.8f,  0.6f,
         0.5f, -0.5f,  0.5f,  0.65f,  0.8f,  0.6f,
         0.5f,  0.5f,  0.5f,  0.65f,  0.8f,  0.6f,

         // down
         -0.5f, -0.5f, -0.5f,  1.0f,  1.0f,  1.0f,
          0.5f, -0.5f, -0.5f,  1.0f,  1.0f,  1.0f,
          0.5f, -0.5f,  0.5f,  1.0f,  1.0f,  1.0f,
          0.5f, -0.5f,  0.5f,  1.0f,  1.0f,  1.0f,
         -0.5f, -0.5f,  0.5f,  1.0f,  1.0f,  1.0f,
         -0.5f, -0.5f, -0.5f,  1.0f,  1.0f,  1.0f,

         // top
         -0.5f,  0.5f, -0.5f,  0.4f,  0.6f,  0.65f,
          0.5f,  0.5f, -0.5f,  0.4f,  0.6f,  0.65f,
          0.5f,  0.5f,  0.5f,  0.4f,  0.6f,  0.65f,
          0.5f,  0.5f,  0.5f,  0.4f,  0.6f,  0.65f,
         -0.5f,  0.5f,  0.5f,  0.4f,  0.6f,  0.65f,
         -0.5f,  0.5f, -0.5f,  0.4f,  0.6f,  0.65f
    };

    // Buffer objects
    GLuint VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Camera
    glm::mat4 view(1.0f);
    view = glm::lookAt(glm::vec3(20.0f, 22.5f, 20.0f),
                       glm::vec3(0.0f, 0.0f, 0.0f),
                       glm::vec3(0.0f, 1.0f, 0.0f));
    int view_loc = glGetUniformLocation(shader_program, "view");

    glm::mat4 projection(1.0f);
    projection = glm::perspective(glm::radians(45.0f), (float)window->width / (float)window->height, 0.1f, 100.0f);
    GLint projection_loc = glGetUniformLocation(shader_program, "projection");

    // Load uniforms
    glUseProgram(shader_program);
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(projection));

    // OpenGL settings
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    // Render loop
    while (!glfwWindowShouldClose(window->handler)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader_program);
        glBindVertexArray(VAO);
        for (int i = -ROWS / 2; i < ROWS / 2; ++i) {
            for (int j = -COLUMNS / 2; j < COLUMNS / 2; ++j) {
                float distance_factor = static_cast<float>(sqrt(pow(i, 2) + pow(j, 2))) * 0.9f;
                float height = CUBE_HEIGHT_MULTIPLIER * static_cast<float>(sin(SIN_MULTIPLIER * glfwGetTime() + distance_factor)) + MIN_CUBE_HEIGHT;

                glm::mat4 model(1.0f);
                model = glm::translate(model, glm::vec3(i, 0.0f, j));
                model = glm::scale(model, glm::vec3(1.0f, height, 1.0f));
                GLint model_loc = glGetUniformLocation(shader_program, "model");
                glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));

                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }

        glfwSwapBuffers(window->handler);
        glfwPollEvents();
    }

    // Free memory
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}
