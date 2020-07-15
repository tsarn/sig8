#include "sig8_internal.h"

const char *vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec2 pos;\n"
    "out vec2 uv;\n"
    "uniform vec2 off;\n"
    "void main()\n"
    "{\n"
    "  uv = vec2(pos.x, 1.0 - pos.y);\n"
    "  gl_Position = vec4(2.0 * (pos * (1 - 2.0 * off) + off - vec2(0.5)), 0.0, 1.0);\n"
    "}";

const char *fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 fragColor;\n"
    "in vec2 uv;\n"
    "uniform sampler2D tex;\n"
    "void main()\n"
    "{\n"
    "  fragColor = texture(tex, uv);\n"
    "}\n";

Color screenBuffer[SCREEN_WIDTH * SCREEN_HEIGHT];
Color colorMap[N_COLORS];

static unsigned int screenVBO;
static unsigned int screenVAO;
static unsigned int screenPBO;
static unsigned int screenTexture;
static unsigned int shader;
static int offLoc;

static const float screenRect[] = {
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
};

void ReportSDLError(void)
{
    fprintf(stderr, "SDL Error: %s\n", SDL_GetError());
    Finalize();
    exit(EXIT_FAILURE);
}

void InitializeWindow(const char *name)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        ReportSDLError();
    }

    window = SDL_CreateWindow(
            name,
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (!window) {
        ReportSDLError();
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    glContext = SDL_GL_CreateContext(window);

    if (!glContext) {
        ReportSDLError();
    }

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        fprintf(stderr, "gladLoadGLLoader failed");
        Finalize();
        exit(EXIT_FAILURE);
    }

}

void InitializeOpenGL(void)
{
    glGenBuffers(1, &screenVBO);
    glGenBuffers(1, &screenPBO);
    glGenTextures(1, &screenTexture);
    glGenVertexArrays(1, &screenVAO);

    // Configure buffer objects

    glBindVertexArray(screenVAO);

    glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof screenRect, screenRect, GL_STATIC_DRAW);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, screenPBO);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, sizeof screenBuffer, screenBuffer, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Configure texture

    glBindTexture(GL_TEXTURE_2D, screenTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    // Compile shaders

    int success;
    char infoLog[4096];

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        fprintf(stderr, "Vertex shader compilation failed.\n");
        glGetShaderInfoLog(vertexShader, sizeof infoLog, NULL, infoLog);
        fputs(infoLog, stderr);
        exit(EXIT_FAILURE);
    }

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        fprintf(stderr, "Fragment shader compilation failed.\n");
        glGetShaderInfoLog(fragmentShader, sizeof infoLog, NULL, infoLog);
        fputs(infoLog, stderr);
        exit(EXIT_FAILURE);
    }

    shader = glCreateProgram();
    glAttachShader(shader, vertexShader);
    glAttachShader(shader, fragmentShader);
    glLinkProgram(shader);

    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
        fprintf(stderr, "Shader linkage failed.\n");
        glGetProgramInfoLog(shader, sizeof infoLog, NULL, infoLog);
        fputs(infoLog, stderr);
        exit(EXIT_FAILURE);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    offLoc = glGetUniformLocation(shader, "off");

    OnResize();
    UpdateBufferData();
}

void OnResize(void)
{
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);

    int pixelScaleX = windowWidth / SCREEN_WIDTH;
    int pixelScaleY = windowHeight / SCREEN_HEIGHT;

    if (pixelScaleX < pixelScaleY) {
        pixelScale = pixelScaleX;
    } else {
        pixelScale = pixelScaleY;
    }

    offsetX = (1.0f - SCREEN_WIDTH * pixelScale * 1.0f / windowWidth) / 2.0f;
    offsetY = (1.0f - SCREEN_HEIGHT * pixelScale * 1.0f / windowHeight) / 2.0f;
}

void UpdateBufferData(void)
{
    glBindTexture(GL_TEXTURE_2D, screenTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, screenPBO);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, sizeof screenBuffer, NULL, GL_DYNAMIC_DRAW);
    uint8_t *ptr = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
    memcpy(ptr, screenBuffer, sizeof screenBuffer);
    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
}

void RedrawScreen(void)
{
    UpdateBufferData();
    glViewport(0, 0, windowWidth, windowHeight);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader);
    glBindVertexArray(screenVAO);

    glUniform2f(offLoc, offsetX, offsetY);

    glDrawArrays(GL_TRIANGLES, 0, SCREEN_WIDTH * SCREEN_HEIGHT * 6);
    SDL_GL_SwapWindow(window);
}
