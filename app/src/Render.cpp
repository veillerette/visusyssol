#include "../include/Render.h"

Render::Render(Geometry *_geometry) : geometry(_geometry) {
}

Render::~Render() {
}

void Render::enable() {
    geometry->enable();
}

void Render::disable() {
    geometry->disable();
}