cmake_minimum_required(VERSION 2.8)

project(VisuSysSol)

add_definitions(-std=c++17 -O3)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake)
set(OpenGL_GL_PREFERENCE LEGACY)

find_package(SDL REQUIRED)
find_package(SDL_ttf REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

# Pour gérer un bug a la fac, a supprimer sur machine perso:
# set(OPENGL_LIBRARIES /usr/lib/x86_64-linux-gnu/libGL.so.1)

include_directories(${SDL_INCLUDE_DIR} ${SDL_TTF_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} glimac/include third-party/include)

set(ALL_LIBRARIES glimac ${SDL_LIBRARY} ${SDL_TTF_LIBRARY} ${OPENGL_LIBRARIES} ${GLEW_LIBRARY})

add_subdirectory(glimac)
add_subdirectory(app)

file(COPY assets DESTINATION ./)
