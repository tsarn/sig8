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
        "  fragColor = texture(tex, uv); fragColor.a = 1.0;\n"
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
static int screenWidth = 128;
static int screenHeight = 128;

static float offsetX, offsetY;
static float pixelScale;

static Color *screenBuffer;
static int screenBufferSize;

static SDL_Window *window = NULL;
static SDL_GLContext glContext = NULL;
static bool shouldQuit = false;
static bool initialized = false;
static bool anyEventsHappened = false;
static void (*mainLoop)(void);

static SDL_Cursor *cachedCursors[SDL_NUM_SYSTEM_CURSORS];

#ifdef SIG8_COMPILE_EDITORS
static bool pendingEditorEnter = false;
static bool pendingEditorLeave = false;
static bool editorsEnabled = false;
static void (*editorLoop)(void);

// saved user screen size, so that we can restore it when we leave an editor
static int userScreenWidth;
static int userScreenHeight;
#endif

void sig8_Initialize(const char *windowName)
{
    if (initialized) {
        puts("Repeat initialization is not supported.");
        puts("Do not call Initialize() more than once.");
        Finalize();
        exit(EXIT_FAILURE);
    }

    screenBufferSize = screenWidth * screenHeight * sizeof(Color);
    screenBuffer = malloc(screenBufferSize);

    sig8_InitAlloc();
    sig8_InitScreen(screenBuffer);
    sig8_InitWindow(windowName);
    sig8_InitGLES();
    sig8_InitAudio();
    sig8_InitMusic();
    sig8_InitInput();
#ifdef SIG8_COMPILE_EDITORS
    const uint8_t *old = GetResourceBundle();
    UseResourceBundle(SIG8_EDITORS_BUNDLE);

    sig8_EDITORS_SPRITESHEET = LoadSpriteSheet("res://editors/spritesheet.png");
    UseResourceBundle(old);
#endif
    initialized = true;
}

void Finalize(void)
{
    if (glContext) {
        SDL_GL_DeleteContext(glContext);
    }

    if (window) {
        SDL_DestroyWindow(window);
    }

    SDL_Quit();
}

static void OnResize(void)
{
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);

    float pixelScaleX = (float)windowWidth / (float)screenWidth;
    float pixelScaleY = (float)windowHeight / (float)screenHeight;

    if (pixelScaleX < pixelScaleY) {
        pixelScale = pixelScaleX;
    } else {
        pixelScale = pixelScaleY;
    }

    offsetX = (1.0f - (float)screenWidth * pixelScale / (float)windowWidth) / 2.0f;
    offsetY = (1.0f - (float)screenHeight * pixelScale  / (float)windowHeight) / 2.0f;
}

void ResizeScreen(int newWidth, int newHeight)
{
    screenWidth = newWidth;
    screenHeight = newHeight;

    if (initialized) {
        free(screenBuffer);
        screenBufferSize = screenWidth * screenHeight * sizeof(Color);
        screenBuffer = malloc(screenBufferSize);
        sig8_InitScreen(screenBuffer);
        sig8_InitGLESPixelBuffer();
        OnResize();
    }
}

int GetScreenWidth(void)
{
    return screenWidth;
}

int GetScreenHeight(void)
{
    return screenHeight;
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
    printf("ERROR: %s\n", SDL_GetError());
    Finalize();
    exit(EXIT_FAILURE);
}

void sig8_InitWindow(const char *name)
{
    sig8_RegisterCallback(FRAME_EVENT, UpdateDelta);
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

    SetVSyncEnabled(true);
}

static void UpdateBufferData(void)
{
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, screenPBO);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, screenBufferSize, NULL, GL_DYNAMIC_DRAW);
    uint8_t *ptr = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, screenBufferSize,
            GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    memcpy(ptr, screenBuffer, screenBufferSize);
    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    glBindTexture(GL_TEXTURE_2D, screenTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                    SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, 0);
}

void sig8_InitGLESPixelBuffer(void)
{
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, screenPBO);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, screenBufferSize, screenBuffer, GL_DYNAMIC_DRAW);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
}

void sig8_InitGLES(void)
{
#ifdef SIG8_USE_GLAD
    if (!gladLoadGLES2Loader((GLADloadproc)SDL_GL_GetProcAddress)) {
        printf("ERROR: Failed to load GLES3 loader\n");
        Finalize();
        exit(EXIT_FAILURE);
    }
#endif
    glGenBuffers(1, &screenVBO);
    glGenBuffers(1, &screenPBO);
    glGenTextures(1, &screenTexture);
    glGenVertexArrays(1, &screenVAO);

    // Configure buffer objects

    glBindVertexArray(screenVAO);

    glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof screenRect, screenRect, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Configure texture

    glBindTexture(GL_TEXTURE_2D, screenTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    sig8_InitGLESPixelBuffer();

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
    sig8_EmitEvent(FRAME_EVENT, NULL);
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

        if (sig8_EmitEvent(event.type, &event)) {
            anyEventsHappened = true;
        }
    }
}

static void RedrawScreen(void)
{
    sig8_UpdateScreen();
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

static bool Tick(void)
{
    RedrawScreen();

    // previous frame ends here

#ifdef SIG8_COMPILE_EDITORS
    if (pendingEditorEnter) {
        userScreenWidth = screenWidth;
        userScreenHeight = screenHeight;
        sig8_EmitEvent(EDITOR_ENTER_EVENT, NULL);
        ResizeScreen(EDITOR_WIDTH, EDITOR_HEIGHT);
        pendingEditorEnter = false;
    }

    if (pendingEditorLeave) {
        ResizeScreen(userScreenWidth, userScreenHeight);
        sig8_EmitEvent(EDITOR_LEAVE_EVENT, NULL);
        pendingEditorLeave = false;
    }
#endif

    bool q = shouldQuit;
    HandleEvents();
    return !q;
}

#ifdef __EMSCRIPTEN__
static void EmscriptenAnimationFrame(void) {
    if (!Tick()) {
        emscripten_cancel_main_loop();
        Finalize();
        return;
    }

#ifdef SIG8_COMPILE_EDITORS
    if (editorLoop) {
        editorLoop();
    } else {
        mainLoop();
    }
#else
    mainLoop();
#endif
}
#endif

void RunMainLoop(void (*function)(void))
{
    mainLoop = function;

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(EmscriptenAnimationFrame, -1, 1);
#else
    while (Tick()) {
#ifdef SIG8_COMPILE_EDITORS
        if (editorLoop) {
            editorLoop();
        } else {
            mainLoop();
        }
#else
        mainLoop();
#endif
    }
    Finalize();
#endif
}

void SetVSyncEnabled(bool enabled)
{
    SDL_GL_SetSwapInterval((int)enabled);
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

void EnableEditors(void)
{
#ifdef SIG8_COMPILE_EDITORS
    editorsEnabled = true;
#endif
}

#ifdef SIG8_COMPILE_EDITORS
void sig8_LeaveEditor(void)
{
    editorLoop = NULL;
    pendingEditorLeave = true;
}
#endif

void EditResource(uint8_t *resource)
{
#ifdef SIG8_COMPILE_EDITORS
    if (!editorsEnabled) {
        return;
    }

    ManagedResource *res = sig8_GetManagedResource(resource);

    switch (res->type) {
    case RESOURCE_SPRITESHEET:
        editorLoop = sig8_SpriteEditorTick;
        sig8_SpriteEditorInit(res);
        break;

    case RESOURCE_TILEMAP:
        editorLoop = sig8_TileEditorTick;
        sig8_TileEditorInit(res);
        break;

    default:
        return;
    }

    pendingEditorEnter = true;
#else
    (void)resource;
#endif
}
