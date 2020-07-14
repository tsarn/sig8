#include "sig8_internal.h"

SDL_Window *window;

const char *vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec2 pos;\n"
    "layout (location = 1) in vec4 aColor;\n"
    "out vec4 color;\n"
    "uniform vec2 size;\n"
    "uniform vec2 off;\n"
    "void main()\n"
    "{\n"
    "  color = aColor;\n"
    "  gl_Position = vec4(2.0 * (pos / size * (1 - 2.0 * off) + off - vec2(0.5)), 0.0, 1.0);\n"
    "}";

const char *fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 fragColor;\n"
    "in vec4 color;\n"
    "void main()\n"
    "{\n"
    "  fragColor = color;\n"
    "}\n";


int screenBuffer[SCREEN_WIDTH * SCREEN_HEIGHT];
FloatColor colorMap[N_COLORS];
static int width, height, pixelScale;
static float offsetX, offsetY;

static unsigned int screenVBO, screenVAO;
static unsigned int shader;
static int sizeLoc, offLoc;
static int isBufferCreated = 0;

typedef struct {
    float x, y;
    FloatColor color;
} VertexData;

VertexData vertexData[SCREEN_WIDTH * SCREEN_HEIGHT * 6];

void InitializeOpenGL(void)
{
    glGenBuffers(1, &screenVBO);
    glGenVertexArrays(1, &screenVAO);

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

    sizeLoc = glGetUniformLocation(shader, "size");
    offLoc = glGetUniformLocation(shader, "off");

    for (int i = 0; i < SCREEN_WIDTH; ++i) {
        for (int j = 0; j < SCREEN_HEIGHT; ++j) {
            int idx = (i * SCREEN_HEIGHT + j) * 6;

            float x = (float)i;
            float y = (float)(SCREEN_HEIGHT - j - 1);

            vertexData[idx + 0] = (VertexData){ .x = x, .y = y };
            vertexData[idx + 1] = (VertexData){ .x = x + 1.0f, .y = y };
            vertexData[idx + 2] = (VertexData){ .x = x, .y = y + 1.0f };

            vertexData[idx + 3] = (VertexData){ .x = x, .y = y + 1.0f };
            vertexData[idx + 4] = (VertexData){ .x = x + 1.0f, .y = y };
            vertexData[idx + 5] = (VertexData){ .x = x + 1.0f, .y = y + 1.0f };
        }
    }

    OnResize();
    UpdateVertexData();
}

void OnResize(void)
{
    SDL_GetWindowSize(window, &width, &height);

    int pixelScaleX = width / SCREEN_WIDTH;
    int pixelScaleY = height / SCREEN_HEIGHT;

    if (pixelScaleX < pixelScaleY) {
        pixelScale = pixelScaleX;
    } else {
        pixelScale = pixelScaleY;
    }

    offsetX = (1.0f - SCREEN_WIDTH * pixelScale * 1.0f / width) / 2.0f;
    offsetY = (1.0f - SCREEN_HEIGHT * pixelScale * 1.0f / height) / 2.0f;
}

void UpdateVertexData(void)
{
    for (int i = 0; i < SCREEN_WIDTH; ++i) {
        for (int j = 0; j < SCREEN_HEIGHT; ++j) {
            int idx = i * SCREEN_HEIGHT + j;
            for (int k = 0; k < 6; ++k) {
                vertexData[6 * idx + k].color = colorMap[screenBuffer[idx]];
            }
        }
    }

    glBindVertexArray(screenVAO);
    glBindBuffer(GL_ARRAY_BUFFER, screenVBO);

    if (isBufferCreated) {
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof vertexData, vertexData);
    } else {
        glBufferData(GL_ARRAY_BUFFER, sizeof vertexData, vertexData, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        isBufferCreated = 1;
    }
}

void DrawScreen(void)
{
    UpdateVertexData();
    glViewport(0, 0, width, height);
    glClearColor(
            colorMap[0].r,
            colorMap[0].g,
            colorMap[0].b,
            colorMap[0].a
    );
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader);
    glBindVertexArray(screenVAO);

    glUniform2f(sizeLoc, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT);
    glUniform2f(offLoc, offsetX, offsetY);

    glDrawArrays(GL_TRIANGLES, 0, SCREEN_WIDTH * SCREEN_HEIGHT * 6);
}
