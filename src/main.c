#include "sig8_internal.h"

void die()
{
    fprintf(stderr, "Something went wrong.\n");
    fprintf(stderr, "SDL Error: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    srand(time(0));
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        die();
    }

    window = SDL_CreateWindow(
            WINDOW_TITLE,
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    if (!window) {
        die();
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GLContext context = SDL_GL_CreateContext(window);

    if (!context) {
        die();
    }

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        die();
    }
    printf("OpenGL Vendor:   %s\n", glGetString(GL_VENDOR));
    printf("OpenGL Renderer: %s\n", glGetString(GL_RENDERER));
    printf("OpenGL Version:  %s\n", glGetString(GL_VERSION));

    InitializeScreen();
    InitializeOpenGL();

    int quit = 0;

    ClearScreen(DARK_BLUE);
    DrawString(5, 5, PEACH, "Hello, world!");

    while (!quit) {
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }

            if (e.type == SDL_WINDOWEVENT) {
                if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                    OnResize();
                }
            }
        }
        DrawScreen();
        SDL_GL_SwapWindow(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
