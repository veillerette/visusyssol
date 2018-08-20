#ifndef DEF_HEADER_FPSMANAGER
#define DEF_HEADER_FPSMANAGER

#include <SDL/SDL.h>
#include <string>
#include <time.h>
#include <unistd.h>

class FPSManager {
public:
    static const Uint32 interval = 1000000;

    FPSManager(Uint32 _control);

    ~FPSManager();

    std::string toString();

    double fps();

    bool canMaj();

    void maj();

    void waitUntilNextFrame();

private:
    long control; // microseconds for each frame (goal)
    long last;    // last maj
    long frame;
    struct timespec timer;
    Uint32 counter;

    long getTime();
};


#endif //DEF_HEADER_FPSMANAGER
