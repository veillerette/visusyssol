#include "glimac/SDLWindowManager.hpp"
#include <iostream>

Uint32 timer;
Uint32 last;
Uint32 counter;

namespace glimac {

SDLWindowManager::SDLWindowManager(uint32_t width, uint32_t height, const char* title) {
    if(0 != SDL_Init(SDL_INIT_EVERYTHING)) {
        std::cerr << SDL_GetError() << std::endl;
        return;
    }
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
    if(!SDL_SetVideoMode(width, height, 32, SDL_OPENGL /*| SDL_FULLSCREEN */)) {
        std::cerr << SDL_GetError() << std::endl;
        return;
    }
    SDL_WM_SetCaption(title, nullptr);
    counter = 0;
    timer = SDL_GetTicks();
}

SDLWindowManager::~SDLWindowManager() {
    SDL_Quit();
}

bool SDLWindowManager::pollEvent(SDL_Event * e) {
    return SDL_PollEvent(e);
}

bool SDLWindowManager::pollEvent(SDL_Event & e) {
    return SDL_PollEvent(&e);
}

bool SDLWindowManager::isKeyPressed(SDLKey key) const {
    return SDL_GetKeyState(nullptr)[key];
}

// button can SDL_BUTTON_LEFT, SDL_BUTTON_RIGHT and SDL_BUTTON_MIDDLE
bool SDLWindowManager::isMouseButtonPressed(uint32_t button) const {
    return SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(button);
}

glm::ivec2 SDLWindowManager::getMousePosition() const {
    glm::ivec2 mousePos;
    SDL_GetMouseState(&mousePos.x, &mousePos.y);
    return mousePos;
}

void SDLWindowManager::swapBuffers() {
    char buffer[100];
    SDL_GL_SwapBuffers();
    timer = SDL_GetTicks();
    counter++;
    
    if(timer - last >= 1000)
    {
	sprintf(buffer, "%.1f FPS", counter * 1000.0 / (timer - last));
	//printf("%s\n", buffer);
	SDL_WM_SetCaption(buffer, "");
	last = timer;
	counter = 0;
    }
}

float SDLWindowManager::getTime() const {
    return 0.001f * SDL_GetTicks();
}

}
