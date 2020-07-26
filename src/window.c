#include "sig8_internal.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

static const char *vertexShaderSource =
        "#version 300 es\n"
        "layout (location = 0) in vec2 pos;\n"
        "out mediump vec2 uv;\n"
        "uniform vec2 off;\n"
        "void main()\n"
        "{\n"
        "  uv = vec2(pos.x, 1.0 - pos.y);\n"
        "  gl_Position = vec4(2.0 * (pos * (1.0 - 2.0 * off) + off - vec2(0.5)), 0.0, 1.0);\n"
        "}";

static const char *fragmentShaderSource =
        "#version 300 es\n"
        "out mediump vec4 fragColor;\n"
        "in mediump vec2 uv;\n"
        "uniform sampler2D tex;\n"
        "void main()\n"
        "{\n"
        "  fragColor = texture(tex, uv);\n"
        "}\n";

static const float screenRect[] = {
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
};

static float lastTime = 0.0f;
static float curDelta = 1.0f / 60.0f;

static unsigned int screenVBO;
static unsigned int screenVAO;
static unsigned int screenPBO;
static unsigned int screenTexture;
static unsigned int shader;
static int offLoc;

// window size in physical pixels
static int windowWidth, windowHeight;

// screen size in virtual pixels (e.g. 128x128)
static int screenWidth, screenHeight;

static Palette palette;

static float offsetX, offsetY;
static int pixelScale;

static Color *screenBuffer;
static int screenBufferSize;

static SDL_Window *window = NULL;
static SDL_GLContext glContext = NULL;
static bool shouldQuit = false;
static bool initialized = false;
static bool anyEventsHappened = false;
static void (*mainLoop)(void);

static SDL_Cursor *cachedCursors[SDL_NUM_SYSTEM_CURSORS];

void sig8_InitializeEx(Configuration configuration)
{
    if (initialized) {
        puts("Repeat initialization is not supported.");
        puts("Do not call Initialize() more than once.");
        Finalize();
        exit(EXIT_FAILURE);
    }

    screenWidth = configuration.width ? configuration.width : 128;
    screenHeight = configuration.height ? configuration.height : 128;
    palette = configuration.palette.size ? configuration.palette : PALETTE_DEFAULT;
    
    screenBufferSize = screenWidth * screenHeight * sizeof(Color);
    screenBuffer = malloc(screenBufferSize);

    sig8_InitAlloc();
    sig8_InitScreen(screenBuffer);
    sig8_InitWindow(configuration.windowName);
    sig8_InitGLES();
    sig8_InitAudio();
    sig8_InitMusic();
    sig8_InitInput();
    initialized = true;
}

void sig8_Initialize(const char *windowName)
{
    sig8_InitializeEx((Configuration){
        .windowName = windowName
    });
}

void Finalize(void)
{
    if (screenBuffer) {
        free(screenBuffer);
        screenBuffer = NULL;
    }
    
    if (glContext) {
        SDL_GL_DeleteContext(glContext);
    }

    if (window) {
        SDL_DestroyWindow(window);
    }

    SDL_Quit();
}

int GetScreenWidth(void)
{
    return screenWidth;
}

int GetScreenHeight(void)
{
    return screenHeight;
}

Palette GetPalette(void)
{
    return palette;
}

static void UpdateDelta(void)
{
    float curTime = SDL_GetTicks() / 1000.0f;
    curDelta = curTime - lastTime;
    lastTime = curTime;
}

static void OnQuit(SDL_Event *event)
{
    (void)event;
    Quit();
}

static void OnResize(void)
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

static void OnWindowEvent(SDL_Event *event)
{
    if (event->window.event == SDL_WINDOWEVENT_RESIZED) {
        OnResize();
    }
}

bool AnyEventsHappened(void)
{
    return anyEventsHappened;
}

static void ReportSDLError(void)
{
    printf("SDL Error: %s\n", SDL_GetError());
    Finalize();
    exit(EXIT_FAILURE);
}

void sig8_InitWindow(const char *name)
{
    sig8_RegisterFrameCallback(UpdateDelta);
    sig8_RegisterEventCallback(SDL_QUIT, OnQuit);
    sig8_RegisterEventCallback(SDL_WINDOWEVENT, OnWindowEvent);

    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) != 0) {
        ReportSDLError();
    }

    window = SDL_CreateWindow(
            name,
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT,
            SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (!window) {
        ReportSDLError();
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

    glContext = SDL_GL_CreateContext(window);

    if (!glContext) {
        ReportSDLError();
    }

    for (int i = 0; i < SDL_NUM_SYSTEM_CURSORS; ++i) {
        cachedCursors[i] = SDL_CreateSystemCursor(i);
    }
}

static void UpdateBufferData(void)
{
    glBindTexture(GL_TEXTURE_2D, screenTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
            SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, screenPBO);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, screenBufferSize, NULL, GL_DYNAMIC_DRAW);
    uint8_t *ptr = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, screenBufferSize,
            GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    memcpy(ptr, screenBuffer, screenBufferSize);
    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
}

void sig8_InitGLES(void)
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
    glBufferData(GL_PIXEL_UNPACK_BUFFER, screenBufferSize, screenBuffer, GL_DYNAMIC_DRAW);

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
        puts("Vertex shader compilation failed.");
        glGetShaderInfoLog(vertexShader, sizeof infoLog, NULL, infoLog);
        puts(infoLog);
        exit(EXIT_FAILURE);
    }

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        puts("Fragment shader compilation failed.");
        glGetShaderInfoLog(fragmentShader, sizeof infoLog, NULL, infoLog);
        puts(infoLog);
        exit(EXIT_FAILURE);
    }

    shader = glCreateProgram();
    glAttachShader(shader, vertexShader);
    glAttachShader(shader, fragmentShader);
    glLinkProgram(shader);

    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
        puts("Shader linkage failed.");
        glGetProgramInfoLog(shader, sizeof infoLog, NULL, infoLog);
        puts(infoLog);
        exit(EXIT_FAILURE);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    offLoc = glGetUniformLocation(shader, "off");

    OnResize();
    UpdateBufferData();
}

static void HandleEvents(void)
{
    anyEventsHappened = false;
    sig8_HandleEvent(FRAME_EVENT, NULL);
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        float x, y;
        if (event.type == SDL_MOUSEMOTION) {
            // this is a quick hack, but it works
            x = (event.motion.x * 1.0f / windowWidth - offsetX) / (1.0f - 2.0f * offsetX);
            y = (event.motion.y * 1.0f / windowHeight - offsetY) / (1.0f - 2.0f * offsetY);

            event.user.data1 = &x;
            event.user.data2 = &y;
        }

        if (sig8_HandleEvent(event.type, &event)) {
            anyEventsHappened = true;
        }
    }
}

static void RedrawScreen(void)
{
    UpdateBufferData();
    glViewport(0, 0, windowWidth, windowHeight);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader);
    glBindVertexArray(screenVAO);

    glUniform2f(offLoc, offsetX, offsetY);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    SDL_GL_SwapWindow(window);
}

float GetDelta(void)
{
    return curDelta;
}

bool Tick(void)
{
    RedrawScreen();

    // previous frame ends here

    HandleEvents();

    return !shouldQuit;
}

#ifdef __EMSCRIPTEN__
static void EmscriptenAnimationFrame(void* data) {
    if (!Tick()) {
        emscripten_cancel_main_loop();
        Finalize();
        return;
    }

    mainLoop();
}
#endif

void RunMainLoop(void (*function)(void))
{
    mainLoop = function;

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(EmscriptenAnimationFrame, NULL, -1, 1);
#else
    while (Tick()) {
        mainLoop();
    }
    Finalize();
#endif
}

void Quit(void)
{
    shouldQuit = true;
}

bool ShouldQuit(void)
{
    return shouldQuit;
}

void SetCursorShape(CursorShape cursor)
{
    SDL_SetCursor(cachedCursors[cursor]);
}
