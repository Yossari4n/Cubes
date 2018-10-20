#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// window settings
const int SCR_WIDTH = 800;
const int SCR_HEIGHT = 600;

// cube constants
const int ROWS = 15;
const int COLUMNS = 15;
const float MIN_CUBE_HEIGHT = 5.0f;
const float CUBE_HEIGHT_MULTIPLIER = 3.0f;
const float SIN_MULTIPLIER = 2.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
unsigned int create_shader(const char* file_path, GLenum shader_type);

int main(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    // create window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Cube waves", nullptr, nullptr);
    if (!window) {
        std::cout << "Failed to initialize window\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);
    
    // set callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // load glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD\n";
        return EXIT_FAILURE;
    }
    
    // data
    float vertices[] = {
        // coords            // color
        // back
        -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
        
        // front
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.18f,
         0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.18f,
         0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 0.18f,
         0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 0.18f,
        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 0.18f,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.18f,
        
        // left
        -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f,
        
        // right
         0.5f,  0.5f,  0.5f, 0.65f, 0.8f, 0.6f,
         0.5f,  0.5f, -0.5f, 0.65f, 0.8f, 0.6f,
         0.5f, -0.5f, -0.5f, 0.65f, 0.8f, 0.6f,
         0.5f, -0.5f, -0.5f, 0.65f, 0.8f, 0.6f,
         0.5f, -0.5f,  0.5f, 0.65f, 0.8f, 0.6f,
         0.5f,  0.5f,  0.5f, 0.65f, 0.8f, 0.6f,
        
        // down
        -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
         0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f,
         0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
        
        // top
        -0.5f,  0.5f, -0.5f, 0.4f, 0.6f, 0.65f,
         0.5f,  0.5f, -0.5f, 0.4f, 0.6f, 0.65f,
         0.5f,  0.5f,  0.5f, 0.4f, 0.6f, 0.65f,
         0.5f,  0.5f,  0.5f, 0.4f, 0.6f, 0.65f,
        -0.5f,  0.5f,  0.5f, 0.4f, 0.6f, 0.65f,
        -0.5f,  0.5f, -0.5f, 0.4f, 0.6f, 0.65f,
    };
    
    // buffer objects
    unsigned int VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    // shaders
    unsigned int vertex_shader = create_shader("VertexShader.vs", GL_VERTEX_SHADER);
    unsigned int fragment_shader = create_shader("FramgentShader.fs", GL_FRAGMENT_SHADER);
    
    // shader program
    unsigned int shader_program;
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    
    // free memory
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    
    // camera
    glUseProgram(shader_program);
    glm::mat4 view;
    view = glm::lookAt(glm::vec3(20.0f, 22.5f, 20.0f),
                       glm::vec3(0.0f, 0.0f, 0.0f),
                       glm::vec3(0.0f, 1.0f, 0.0f));
    int view_loc = glGetUniformLocation(shader_program, "view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
    
    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    int projection_loc = glGetUniformLocation(shader_program, "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(projection));
    
    // settings
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    
    // render loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // render each object
        glUseProgram(shader_program);
        glBindVertexArray(VAO);
        for (int i = -ROWS / 2; i < ROWS / 2; ++i) {
            for (int j = -COLUMNS / 2; j < COLUMNS / 2; ++j) {
                float distance_factor = sqrt(pow(i, 2) + pow(j, 2)) * 0.9f;
                float height = CUBE_HEIGHT_MULTIPLIER * sin(SIN_MULTIPLIER * glfwGetTime() + distance_factor) + MIN_CUBE_HEIGHT;
                
                glm::mat4 model;
                model = glm::translate(model, glm::vec3(i, 0.0f, j));
                model = glm::scale(model, glm::vec3(1.0f, height, 1.0f));
                int model_loc = glGetUniformLocation(shader_program, "model");
                glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
                
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // free memory
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    
    glfwTerminate();
    return EXIT_SUCCESS;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

unsigned int create_shader(const char* file_path, GLenum shader_type) {
    std::string shader_code;
    std::fstream shader_file;
    
    shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    
    try {
        shader_file.open(file_path);
        std::stringstream shader_stream;
        shader_stream << shader_file.rdbuf();
        shader_file.close();
        
        shader_code = shader_stream.str();
    } catch(const std::ifstream::failure &e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ\n";
    }
    
    unsigned int shader = glCreateShader(shader_type);
    const char *shader_code_ptr = shader_code.c_str();
    glShaderSource(shader, 1, &shader_code_ptr, nullptr);
    glCompileShader(shader);
    
    // check compile errors
    GLint success;
    GLchar info_log[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 1024, NULL, info_log);
        std::cout << "ERROR::SHADER_COMPILATION_ERROR\n" << info_log << '\n';
    }
    
    return shader;
}
