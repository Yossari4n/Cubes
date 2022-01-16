#include <Windows.h>
#include <gl/GL.h>
#include "glext.h"
#include "wglext.h"

#include <chrono>
#include <iostream>
#include <array>

#undef near
#undef far

/********************************
 * OpenGL utilities and helpers *
 ********************************/
 template <class T>
 void LoadOpenGLProc(T& procPointer, const char* name) {
    procPointer = reinterpret_cast<T>(wglGetProcAddress(name));
    if(!procPointer) {
        OutputDebugString("Failed to load function:\n\t");
        OutputDebugString(name);
        OutputDebugString("\n");
        PostQuitMessage(0);
    }
 }

PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
PFNGLCREATEPROGRAMPROC wglCreateProgram = nullptr;
PFNGLATTACHSHADERPROC wglAttachShader = nullptr;
PFNGLLINKPROGRAMPROC wglLinkProgram = nullptr;
PFNGLCREATESHADERPROC wglCreateShader = nullptr;
PFNGLSHADERSOURCEPROC wglShaderSource = nullptr;
PFNGLCOMPILESHADERPROC wglCompileShader = nullptr;
PFNGLDELETESHADERPROC wglDeleteShader = nullptr;
PFNGLGENBUFFERSARBPROC wglGenBuffers = nullptr;
PFNGLGENVERTEXARRAYSPROC wglGenVertexArrays = nullptr;
PFNGLBINDVERTEXARRAYPROC wglBindVertexArray = nullptr;
PFNGLBUFFERDATAPROC wglBufferData = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC wglEnableVertexAttribArray = nullptr;
PFNGLBINDBUFFERPROC wglBindBuffer = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC wglVertexAttribPointer = nullptr;
PFNGLGETUNIFORMLOCATIONPROC wglGetUniformLocation = nullptr;
PFNGLUSEPROGRAMPROC wglUseProgram = nullptr;
PFNGLUNIFORMMATRIX4FVPROC wglUniformMatrix4fv = nullptr;
PFNGLDELETEVERTEXARRAYSPROC wglDeleteVertexArrays = nullptr;
PFNGLDELETEBUFFERSPROC wglDeleteBuffers = nullptr;

LRESULT CALLBACK WindowCallback(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    switch(Msg) {
        case WM_CLOSE:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, Msg, wParam, lParam);
    }

    return 0;
}

GLuint CreateShader(const char* source, GLenum shader_type) {
    const GLuint shader = wglCreateShader(shader_type);
    wglShaderSource(shader, 1, &source, nullptr);
    wglCompileShader(shader);
    return shader;
}

GLuint CreateProgram(GLuint vertex_shader, GLuint fragment_shader) {
    const GLuint program = wglCreateProgram();
    wglAttachShader(program, vertex_shader);
    wglAttachShader(program, fragment_shader);
    wglLinkProgram(program);
    return program;
}

static const auto startTime = std::chrono::high_resolution_clock::now();
float GetTime() {
    const auto currentTime = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count() / 1000.0f;
}

/******************************
 * Math utilities and helpers *
 ******************************/
constexpr float PI = 3.1415926535897931f;

using vec3 = std::array<float, 3>;
using vec4 = std::array<float, 4>;
using mat4 = std::array<vec4, 4>;

bool AlmostEqual(float a, float b) {
    return std::abs(a - b) < std::numeric_limits<float>::epsilon();
}

constexpr float ToRadians(float degrees) {
    return degrees * PI / 180.0f;
}

constexpr float ToDegrees(float radians) {
    return radians * 180.0f / PI;
}

vec3 Normalize(const vec3& vec) {
    float mag = static_cast<float>(sqrt(pow(vec[0], 2) + pow(vec[1], 2) + pow(vec[2], 2)));

    if (!AlmostEqual(mag, 1.0f)) {
        return { vec[0] / mag, vec[1] / mag, vec[2] / mag };
    } else {
        return vec;
    }
}

constexpr vec3 Cross(const vec3& first, const vec3& second) {
    return {
        first[1] * second[2] - first[2] * second[1],
        first[2] * second[0] - first[0] * second[2],
        first[0] * second[1] - first[1] * second[0]
    };
}

constexpr mat4 Mul(const mat4& first, const mat4& second) {
    mat4 result{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 4; k++) {
                result[i][j] += static_cast<float>(first[i][k] * second[k][j]);
            }
        }
    }

    return result;
}

mat4 Perspective(float fov, float aspect, float near, float far) {
    const float top = tan(ToRadians(fov) / 2.0f) * near;
    const float right = top * aspect;

    return {
        near / right, 0.0f,       0.0f,                           0.0f,
        0.0f,         near / top, 0.0f,                           0.0f,
        0.0f,         0.0f,       -(far + near) / (far - near),   -1.0f,
        0.0f,         0.0f,       -2 * far * near / (far - near), 0.0f
    };
}

mat4 LookAt(const vec3& pos, const vec3& target, const vec3& up) {
    const vec3 z_axis = Normalize({ pos[0] - target[0], pos[1] - target[1], pos[2] - target[2] });
    const vec3 x_axis = Normalize(Cross(Normalize(up), z_axis));
    const vec3 y_axis = Cross(z_axis, x_axis);

    mat4 translation{
        1.0f,    0.0f,    0.0f,    0.0f,
        0.0f,    1.0f,    0.0f,    0.0f,
        0.0f,    0.0f,    1.0f,    0.0f,
        -pos[0], -pos[1], -pos[2], 1.0f
    };

    mat4 rotation{
        x_axis[0], y_axis[0], z_axis[0], 0.0f,
        x_axis[1], y_axis[1], z_axis[1], 0.0f,
        x_axis[2], y_axis[2], z_axis[2], 0.0f,
        0.0f,      0.0f,      0.0f,      1.0f
    };

    return Mul(translation, rotation);
}

constexpr mat4 Translate(const mat4& matrix, const vec3& vec) {
    return {
        matrix[0][0],          matrix[0][1],          matrix[0][2],          matrix[0][3],
        matrix[1][0],          matrix[1][1],          matrix[1][2],          matrix[1][3],
        matrix[2][0],          matrix[2][1],          matrix[2][2],          matrix[2][3],
        matrix[3][0] + vec[0], matrix[3][1] + vec[1], matrix[3][2] + vec[2], matrix[3][3]
    };
}

constexpr mat4 Scale(const mat4& matrix, const vec3& vec) {
    return {
        matrix[0][0] * vec[0], matrix[0][1],          matrix[0][2],          matrix[0][3],
        matrix[1][0],          matrix[1][1] * vec[1], matrix[1][2],          matrix[1][3],
        matrix[2][0],          matrix[2][1],          matrix[2][2] * vec[2], matrix[2][3],
        matrix[3][0],          matrix[3][1],          matrix[3][2],          matrix[3][3]
    };
}


/******************************************
 * Sources of shaders used in the program *
 ******************************************/
const char* VertexShaderSource =
"#version 330 core\n"
"layout(location = 0) in vec3 aPos;\n"
"layout(location = 1) in vec3 aColor;\n"
"uniform mat4 model;\n"
"uniform mat4 pv;\n"
"out vec4 VertexColor;\n"
"void main() {\n"
"    VertexColor = vec4(aColor, 1.0);\n"
"    gl_Position = pv * model * vec4(aPos, 1.0);\n"
"}\n\0";

const char* FragmentShaderSource =
"#version 330 core\n"
"in vec4 VertexColor;\n"
"out vec4 FragColor;\n"
"void main() {\n"
"    FragColor = VertexColor;\n"
"}\n\0";


/*************************
 * Constants and globals *
 *************************/
constexpr int WindowWidth = 800;
constexpr int WindowHeight = 600;
constexpr GLfloat CubeVertices[] = {
    // back
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    // front
    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    // left
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

    // right
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,

     // down
     -0.5f, -0.5f, -0.5f,
      0.5f, -0.5f, -0.5f,
      0.5f, -0.5f,  0.5f,
      0.5f, -0.5f,  0.5f,
     -0.5f, -0.5f,  0.5f,
     -0.5f, -0.5f, -0.5f,

     // top
     -0.5f,  0.5f, -0.5f,
      0.5f,  0.5f, -0.5f,
      0.5f,  0.5f,  0.5f,
      0.5f,  0.5f,  0.5f,
     -0.5f,  0.5f,  0.5f,
     -0.5f,  0.5f, -0.5f
};


/***************************************
 * Visualizations forward declarations *
 ***************************************/
void CubeWave(HDC deviceContext, GLuint shader_program);
// void PenroseStairs(const Window* window, GLuint shader_program);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
    WNDCLASSEX wcex;
    ZeroMemory(&wcex, sizeof(wcex));
    wcex.cbSize = sizeof(wcex);
    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wcex.lpfnWndProc = WindowCallback;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.lpszClassName = "CubesWindowClass";

    LPTSTR windowClass = MAKEINTATOM(RegisterClassEx(&wcex));
    if(!windowClass) {
        OutputDebugString("Failed to register window class");
        PostQuitMessage(0);
    }

    // Fake ViewPort
    HWND fakeWindow = CreateWindowEx(
        0,                              // Optional window styles.
        windowClass,                    // Window class
        "Fake Viewport",                // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    HDC fakeDeviceContext = GetDC(fakeWindow);

    PIXELFORMATDESCRIPTOR fakePixelFormatDescriptor{};
    ZeroMemory(&fakePixelFormatDescriptor, sizeof(fakePixelFormatDescriptor));
    fakePixelFormatDescriptor.nSize = sizeof(fakePixelFormatDescriptor);
    fakePixelFormatDescriptor.nVersion = 1;
    fakePixelFormatDescriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    fakePixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
    fakePixelFormatDescriptor.cColorBits = 32;
    fakePixelFormatDescriptor.cAlphaBits = 8;
    fakePixelFormatDescriptor.cDepthBits = 24;

    const int fakePixelFormatDescriptorID = ChoosePixelFormat(fakeDeviceContext, &fakePixelFormatDescriptor);
    if(fakePixelFormatDescriptorID == 0) {
        OutputDebugString("Failed to choose fake pixel format");
        PostQuitMessage(0);
    }

    if(!SetPixelFormat(fakeDeviceContext, fakePixelFormatDescriptorID, &fakePixelFormatDescriptor)) {
        OutputDebugString("Failed to set fake pixel format");
        PostQuitMessage(0);
    }

    HGLRC fakeRenderContext = wglCreateContext(fakeDeviceContext);
    if(fakeRenderContext == 0) {
        OutputDebugString("Failed to set fake pixel format");
        PostQuitMessage(0);
    }

    if(!wglMakeCurrent(fakeDeviceContext, fakeRenderContext)) {
        OutputDebugString("Failed to set fake pixel format");
        PostQuitMessage(0);
    }

    LoadOpenGLProc<PFNWGLCHOOSEPIXELFORMATARBPROC>(wglChoosePixelFormatARB, "wglChoosePixelFormatARB");
    LoadOpenGLProc<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglCreateContextAttribsARB, "wglCreateContextAttribsARB");
    LoadOpenGLProc<PFNGLCREATEPROGRAMPROC>(wglCreateProgram, "glCreateProgram");
    LoadOpenGLProc<PFNGLATTACHSHADERPROC>(wglAttachShader, "glAttachShader");
    LoadOpenGLProc<PFNGLLINKPROGRAMPROC>(wglLinkProgram, "glLinkProgram");
    LoadOpenGLProc<PFNGLCREATESHADERPROC>(wglCreateShader, "glCreateShader");
    LoadOpenGLProc<PFNGLSHADERSOURCEPROC>(wglShaderSource, "glShaderSource");
    LoadOpenGLProc<PFNGLCOMPILESHADERPROC>(wglCompileShader, "glCompileShader");
    LoadOpenGLProc<PFNGLDELETESHADERPROC>(wglDeleteShader, "glDeleteShader");
    LoadOpenGLProc<PFNGLGENBUFFERSARBPROC>(wglGenBuffers, "glGenBuffers");
    LoadOpenGLProc<PFNGLGENVERTEXARRAYSPROC>(wglGenVertexArrays, "glGenVertexArrays");
    LoadOpenGLProc<PFNGLBINDVERTEXARRAYPROC>(wglBindVertexArray, "glBindVertexArray");
    LoadOpenGLProc<PFNGLBUFFERDATAPROC>(wglBufferData, "glBufferData");
    LoadOpenGLProc<PFNGLENABLEVERTEXATTRIBARRAYPROC>(wglEnableVertexAttribArray, "glEnableVertexAttribArray");
    LoadOpenGLProc<PFNGLBINDBUFFERPROC>(wglBindBuffer, "glBindBuffer");
    LoadOpenGLProc<PFNGLVERTEXATTRIBPOINTERPROC>(wglVertexAttribPointer, "glVertexAttribPointer");
    LoadOpenGLProc<PFNGLGETUNIFORMLOCATIONPROC>(wglGetUniformLocation, "glGetUniformLocation");
    LoadOpenGLProc<PFNGLUSEPROGRAMPROC>(wglUseProgram, "glUseProgram");
    LoadOpenGLProc<PFNGLUNIFORMMATRIX4FVPROC>(wglUniformMatrix4fv, "glUniformMatrix4fv");
    LoadOpenGLProc<PFNGLDELETEVERTEXARRAYSPROC>(wglDeleteVertexArrays, "glDeleteVertexArrays");
    LoadOpenGLProc<PFNGLDELETEBUFFERSPROC>(wglDeleteBuffers, "glDeleteBuffers");

    // Real viewport
    HWND window = CreateWindowEx(
        0,                              // Optional window styles.
        windowClass,                    // Window class
        "Cubes!",                       // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window
        NULL,       // Menu
        hInstance,      // Instance handle
        NULL        // Additional application data
    );

    HDC deviceContext = GetDC(window);
    constexpr int pixelAttribs[] = {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
        WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
        WGL_COLOR_BITS_ARB, 32,
        WGL_ALPHA_BITS_ARB, 8,
        WGL_DEPTH_BITS_ARB, 24,
        WGL_STENCIL_BITS_ARB, 8,
        WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
        WGL_SAMPLES_ARB, 4,
        0
    };

    int pixelFormatID;
    UINT numFormats;
    const bool status = wglChoosePixelFormatARB(deviceContext, pixelAttribs, NULL, 1, &pixelFormatID, &numFormats);
    if(status == false || numFormats == 0) {
        OutputDebugString("Failed to choose pixel format ARB");
        PostQuitMessage(0);
    }

    PIXELFORMATDESCRIPTOR pixelFormatDescriptor{};
    DescribePixelFormat(deviceContext, pixelFormatID, sizeof(pixelFormatDescriptor), &pixelFormatDescriptor);
    SetPixelFormat(deviceContext, pixelFormatID, &pixelFormatDescriptor);

    const int majorMin = 4;
    const int minorMin = 0;
    const int contextAttribs[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, majorMin,
            WGL_CONTEXT_MINOR_VERSION_ARB, minorMin,
            WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            0
    };

    HGLRC renderContext = wglCreateContextAttribsARB(deviceContext, 0, contextAttribs);
    if(renderContext == NULL) {
        OutputDebugString("Failed to create context");
        PostQuitMessage(0);
    }

    SetWindowText(window, "Cubes!");
    ShowWindow(window, nCmdShow);

    // Delete fake view
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(fakeRenderContext);
    ReleaseDC(fakeWindow, fakeDeviceContext);
    DestroyWindow(fakeWindow);
    if(wglMakeCurrent(deviceContext, renderContext) == false) {
        OutputDebugString("Failed to context current");
        PostQuitMessage(0);
    }

    // Shader program
    const GLuint vertex_shader = CreateShader(VertexShaderSource, GL_VERTEX_SHADER);
    const GLuint fragment_shader = CreateShader(FragmentShaderSource, GL_FRAGMENT_SHADER);
    const GLuint shader_program = CreateProgram(vertex_shader, fragment_shader);
    wglDeleteShader(vertex_shader);
    wglDeleteShader(fragment_shader);

    // Different scenes
    switch(0) {
        case 0:
            CubeWave(deviceContext, shader_program);
            break;

        case 1:
            // PenroseStairs(&window, shader_program);
            break;

        default:
            break;
    }

    // End of application
    wglMakeCurrent(NULL, NULL);
    if(renderContext) {
        wglDeleteContext(renderContext);
    }

    if(deviceContext) {
        ReleaseDC(window, deviceContext);
    }

    if(window) {
        DestroyWindow(window);
    }

    return EXIT_SUCCESS;
}

void CubeWave(HDC deviceContext, GLuint shader_program) {
    constexpr int ROWS = 15;
    constexpr int COLUMNS = 15;
    constexpr float MIN_CUBE_HEIGHT = 5.0f;
    constexpr float CUBE_HEIGHT_MULTIPLIER = 3.0f;
    constexpr float SIN_MULTIPLIER = 2.0f;

    // Verticies
    constexpr GLfloat colors[] = {
        // back
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,

        // front
        0.0f,  0.0f,  0.18f,
        0.0f,  0.0f,  0.18f,
        0.0f,  0.0f,  0.18f,
        0.0f,  0.0f,  0.18f,
        0.0f,  0.0f,  0.18f,
        0.0f,  0.0f,  0.18f,

        // left
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,

        // right
        0.65f,  0.8f,  0.6f,
        0.65f,  0.8f,  0.6f,
        0.65f,  0.8f,  0.6f,
        0.65f,  0.8f,  0.6f,
        0.65f,  0.8f,  0.6f,
        0.65f,  0.8f,  0.6f,

        // down
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,

        // top
        0.4f,  0.6f,  0.65f,
        0.4f,  0.6f,  0.65f,
        0.4f,  0.6f,  0.65f,
        0.4f,  0.6f,  0.65f,
        0.4f,  0.6f,  0.65f,
        0.4f,  0.6f,  0.65f
    };

    // Buffer objects
    GLuint vertex_buffer, color_buffer, vao;
    wglGenBuffers(1, &vertex_buffer);
    wglGenBuffers(1, &color_buffer);
    wglGenVertexArrays(1, &vao);
    
    wglBindVertexArray(vao);
    wglEnableVertexAttribArray(0);
    wglEnableVertexAttribArray(1);
    
    wglBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    wglBufferData(GL_ARRAY_BUFFER, sizeof(CubeVertices), CubeVertices, GL_STATIC_DRAW);
    wglVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 0, (void*)0);
    
    wglBindBuffer(GL_ARRAY_BUFFER, color_buffer);
    wglBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    wglVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, (void*)0);
    
    wglBindBuffer(GL_ARRAY_BUFFER, 0);
    wglBindVertexArray(0);

    // Camera
    const mat4 projection = Perspective(45.0f, static_cast<float>(WindowWidth / WindowHeight), 0.1f, 100.0f);
    const mat4 view = LookAt(
        { 20.0f, 22.5f, 20.0f },
        { 0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f }
    );

    const mat4 pv = Mul(view, projection);
    const GLint pv_loc = wglGetUniformLocation(shader_program, "pv");

    // Load uniforms
    wglUseProgram(shader_program);
    wglUniformMatrix4fv(pv_loc, 1, GL_FALSE, &pv[0][0]);

    // OpenGL settings
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    MSG msg;
    bool shouldCloseWindow = false;
    while(shouldCloseWindow == false) {
        while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if(msg.message == WM_QUIT) {
                shouldCloseWindow = true;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Rendering
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        wglUseProgram(shader_program);
        wglBindVertexArray(vao);

        const float time = GetTime(); // static_cast<float>(glfwGetTime());
        for(int i = -ROWS / 2; i < ROWS / 2; ++i) {
            for(int j = -COLUMNS / 2; j < COLUMNS / 2; ++j) {
                const float distance_factor = static_cast<float>(sqrt(pow(i, 2) + pow(j, 2))) * 0.9f;
                const float height = CUBE_HEIGHT_MULTIPLIER * sin(SIN_MULTIPLIER * time + distance_factor) + MIN_CUBE_HEIGHT;

                mat4 model{
                    1.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 1.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 1.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f
                };
                model = Translate(model, { static_cast<float>(i), 0.0f, static_cast<float>(j) });
                model = Scale(model, { 1.0f, height, 1.0f });
                const GLint model_loc = wglGetUniformLocation(shader_program, "model");
                wglUniformMatrix4fv(model_loc, 1, GL_FALSE, &model[0][0]);

                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }

        // Swap buffers
        SwapBuffers(deviceContext);
    }

    // Free memory
    wglDeleteVertexArrays(1, &vao);
    wglDeleteBuffers(1, &vertex_buffer);
    wglDeleteBuffers(1, &color_buffer);
}


/*void PenroseStairs(const Window* window, GLuint shader_program) {
    constexpr GLfloat colors[] = {
        // back
        0.37f, 0.0f,  0.73f,
        0.37f, 0.0f,  0.73f,
        0.37f, 0.0f,  0.73f,
        0.37f, 0.0f,  0.73f,
        0.37f, 0.0f,  0.73f,
        0.37f, 0.0f,  0.73f,

        // front
        0.37f, 0.0f,  0.73f,
        0.37f, 0.0f,  0.73f,
        0.37f, 0.0f,  0.73f,
        0.37f, 0.0f,  0.73f,
        0.37f, 0.0f,  0.73f,
        0.37f, 0.0f,  0.73f,

        // left
        0.37f, 0.0f,  0.73f,
        0.37f, 0.0f,  0.73f,
        0.37f, 0.0f,  0.73f,
        0.37f, 0.0f,  0.73f,
        0.37f, 0.0f,  0.73f,
        0.37f, 0.0f,  0.73f,

        // right
        0.37f, 0.0f,  0.73f,
        0.37f, 0.0f,  0.73f,
        0.37f, 0.0f,  0.73f,
        0.37f, 0.0f,  0.73f,
        0.37f, 0.0f,  0.73f,
        0.37f, 0.0f,  0.73f,

        // down
        0.63f, 0.61f,  0.91f,
        0.63f, 0.61f,  0.91f,
        0.63f, 0.61f,  0.91f,
        0.63f, 0.61f,  0.91f,
        0.63f, 0.61f,  0.91f,
        0.63f, 0.61f,  0.91f,

        // top
        0.63f, 0.61f,  0.91f,
        0.63f, 0.61f,  0.91f,
        0.63f, 0.61f,  0.91f,
        0.63f, 0.61f,  0.91f,
        0.63f, 0.61f,  0.91f,
        0.63f, 0.61f,  0.91f
    };

    // Buffer objects
    GLuint vertex_buffer, color_buffer, vao;
    glGenBuffers(1, &vertex_buffer);
    glGenBuffers(1, &color_buffer);
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CubeVertices), CubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 0, (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Camera
    const mat4 projection = Perspective(45.0f, static_cast<float>(window->width / window->height), 0.1f, 100.0f);
    const mat4 view = LookAt(
        { 10.9f, 7.8f, 4.2f },
        { 0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f }
    );

    const mat4 pv = Mul(view, projection);

    // Scene
    const vec3 positions[] = {
        {2.0f, 0.0f, 0.0f},
        {2.0f, 0.0f, 1.0f},
        {2.0f, 0.0f, 2.0f},
        {1.0f, 0.0f, 2.0f},
        {0.0f, 0.0f, 2.0f},
        {-1.0f, 0.0f, 2.0f},
        {-2.0f, 0.0f, 2.0f},
        {-2.0f, 0.0f, 1.0f},
        {-2.0f, 0.0f, 0.0f},
        {-2.0f, 0.0f, -1.0f},
        {-1.0f, 0.0f, -1.0f},
    };
    const float height_modifier = -0.1f;

    // Load uniforms
    glUseProgram(shader_program);
    glUniformMatrix4fv(glGetUniformLocation(shader_program, "pv"), 1, GL_FALSE, &pv[0][0]);

    // OpenGL settings
    glClearColor(0.87f, 0.95f, 1.0f, 0.9f);
    glEnable(GL_DEPTH_TEST);

    // Render loop
    while (!glfwWindowShouldClose(window->handler)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader_program);
        glBindVertexArray(vao);

        for (int i = 0; i < sizeof(positions) / sizeof(positions[0]); i++) {
            vec3 position = positions[i];
            position[1] = height_modifier * i / 2.0f;

            vec3 scale{1.0f, 5.0f + height_modifier * i, 1.0f};

            mat4 model{
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            };
            model = Translate(model, position);
            model = Scale(model, scale);

            glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, &model[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window->handler);
        glfwPollEvents();
    }

    // Free memory
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vertex_buffer);
    glDeleteBuffers(1, &color_buffer);
}*/
