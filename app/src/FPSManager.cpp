#include <iostream>
#include "../include/FPSManager.h"

FPSManager::FPSManager(Uint32 _control) : last(0), counter(0), frame(0), control(1000000 / _control) {
}

FPSManager::~FPSManager() {
}

long FPSManager::getTime() {
    clock_gettime(CLOCK_REALTIME, &timer);
    return timer.tv_sec * 1000000 + timer.tv_nsec / 1000;
}

double FPSManager::fps() {
    return counter * 1000000.0 / (getTime() - last);
}

bool FPSManager::canMaj() {
    if (getTime() > last + interval) {
        return true;
    }
    return false;
}

void FPSManager::maj() {
    last = frame = getTime();
    counter = 0;
}

std::string FPSManager::toString() {
    return std::to_string((int)fps());
}

void FPSManager::waitUntilNextFrame() {
    while (getTime() < frame + control - 1) {
        usleep((frame + control) - getTime());
    }
    frame = getTime();
    counter++;
}

