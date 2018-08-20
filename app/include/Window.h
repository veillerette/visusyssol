#ifndef DEF_HEADER_WINDOW
#define DEF_HEADER_WINDOW


/*
 * Ici gestion de la fenêtre SDL,
 * des FPS, de la gestion
 * clavier/souris ...
 */

#include <string>
#include <GL/glew.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <glimac/glm.hpp>
#include <functional>
#include <iomanip>
#include <sstream>

#include "FPSManager.h"
#include "TextRendering.h"

class Window {
public:
    Window(const char *argv[], uint32_t width, uint32_t height, std::string windowName, Uint32 goalFPS);

    ~Window();

    uint32_t getWidth() const;

    uint32_t getHeight() const;

    void run(std::function<bool(SDL_Event *)> play, std::function<void()> draw);

    glm::mat4 getProjectionMatrix();

    void flip();

    bool activeFilled();

    void handlerTab();

private:
    SDL_Surface *window;
    FPSManager manager;
    bool glFilled;
    Uint32 lastTabKey;

    TTF_Font *font;
    SDL_Surface *fps;
    TextRendering *textRendering;

    bool initGlew();
    void handlerTab(SDL_Event & event);
    void handlerEscape(SDL_Event & event, bool & cont);
};


#endif
