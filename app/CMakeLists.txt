file(GLOB HEADER_FILES include/*.h)
file(GLOB SRC_FILES src/*.cpp)
file(GLOB_RECURSE SHADER_FILES *.glsl)

add_executable(main ${SRC_FILES} ${HEADER_FILES} ${SHADER_FILES})
target_link_libraries(main ${ALL_LIBRARIES})

file(COPY shaders DESTINATION ./)
