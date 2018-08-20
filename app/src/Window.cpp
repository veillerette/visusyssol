#include <iostream>
#include "../include/Window.h"

Window::Window(const char *argv[], uint32_t width, uint32_t height, std::string windowName, Uint32 goalFPS)
        : manager(goalFPS),
          glFilled(true),
          lastTabKey(0),
          fps(nullptr),
          textRendering(nullptr) {
    if (0 != SDL_Init(SDL_INIT_EVERYTHING)) {
        std::cerr << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    if (nullptr == (window = SDL_SetVideoMode(width, height, 32, SDL_OPENGL | SDL_DOUBLEBUF))) {
        std::cerr << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

    SDL_WM_SetCaption(windowName.c_str(), nullptr);

    if (-1 == TTF_Init()) {
        std::cerr << TTF_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

    font = TTF_OpenFont("assets/Anonymous.ttf", 16);
    if (nullptr == font) {
        std::cerr << "Unable to load font" << std::endl;
        exit(EXIT_FAILURE);
    }

    initGlew();

    textRendering = new TextRendering(getWidth(), getHeight(), "assets/textures/Arial.bmp", argv);

    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

Window::~Window() {
    if (window != nullptr) {
        SDL_FreeSurface(window);
        window = nullptr;
    }
    if(font != nullptr) {
        TTF_CloseFont(font);
    }
    if(fps != nullptr) {
        SDL_FreeSurface(fps);
    }
    if(textRendering != nullptr) {
        delete textRendering;
    }
    TTF_Quit();
    SDL_Quit();
}

uint32_t Window::getWidth() const {
    return (uint32_t) window->w;
}

uint32_t Window::getHeight() const {
    return (uint32_t) window->h;
}

bool Window::initGlew() {
    GLenum glewInitError = glewInit();
    if (GLEW_OK != glewInitError) {
        std::cerr << glewGetErrorString(glewInitError) << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "OpenGL Version : " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLEW Version : " << glewGetString(GLEW_VERSION) << std::endl;

    return true;
}

void Window::handlerTab(SDL_Event &event) {
    if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_TAB) {
        if (SDL_GetTicks() - lastTabKey < 150) {
            return;
        }
        glFilled = !glFilled;
        handlerTab();
        lastTabKey = SDL_GetTicks();
    }
}

void Window::handlerTab() {
    if (glFilled) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
}

bool Window::activeFilled() {
    return glFilled;
}

void Window::handlerEscape(SDL_Event &event, bool &cont) {
    if (event.key.keysym.sym == SDLK_ESCAPE) {
        cont = false;
    }
}

void Window::run(std::function<bool(SDL_Event *)> play, std::function<void()> draw) {
    bool cont = true;
    SDL_Event event;
    while (cont) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    cont = false;
                    break;

                case SDL_KEYDOWN:
                    handlerEscape(event, cont);
                    break;
            }
            handlerTab(event);
            cont = cont && play(&event);
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        draw();
        flip();
    }
}

glm::mat4 Window::getProjectionMatrix() {
    return glm::perspective(glm::radians(70.f), window->w * 1.0f / window->h, 0.01f, 200.f);
}

void Window::flip() {
    manager.waitUntilNextFrame();
    textRendering->drawText((manager.toString() + " FPS"), 25, getHeight()-34, 34);

    if (manager.canMaj()) {
        manager.maj();
    }


    SDL_GL_SwapBuffers();
}


