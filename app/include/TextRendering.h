//
// Created by veillerette on 12/01/19.
//

#ifndef DEF_HEADER_TEXTRENDERING_H
#define DEF_HEADER_TEXTRENDERING_H

#include <string>
#include <utility>
#include <unordered_map>

#include "TextureRender.h"


struct hashPair {
    template<class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2> &p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ h2;
    }
};

class TextRendering {
public:
    TextRendering() : tex(0), shader(nullptr), map() {}

    TextRendering(int screenWidth, int screenHeight, const char *bitmapPath, const char *argv[]);

    ~TextRendering();

    void drawText(std::string text, int x, int y, unsigned int size);

    template<typename T>
    static std::string customPrecision(T num, unsigned int n = 2);

    static std::string to_string(bool value);

private:
    GLuint tex;
    Shader *shader;
    int width;
    int height;

    std::unordered_map<std::pair<std::string, unsigned int>, Geometry *, hashPair> map;

    Geometry *getGeo(std::pair<std::string, unsigned int> pair);
};

template<typename T>
std::string TextRendering::customPrecision(T num, unsigned int n) {
    std::ostringstream streamObj;
    streamObj << std::fixed;
    streamObj << std::setprecision(n);
    streamObj << num;
    return streamObj.str();
}


#endif // DEF_HEADER_TEXTRENDERING_H
