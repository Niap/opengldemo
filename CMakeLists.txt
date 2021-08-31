cmake_minimum_required (VERSION 2.8)

project (opengldemo)

include_directories (${CMAKE_CURRENT_LIST_DIR}/deps/include)

add_executable(main main.cpp)

#target_link_libraries (main opengl32.lib ${CMAKE_CURRENT_LIST_DIR}/deps/lib/glew32s.lib ${CMAKE_CURRENT_LIST_DIR}/deps/lib/glfw3_mt.lib)


target_link_libraries (main "-framework OpenGL" ${CMAKE_CURRENT_LIST_DIR}/deps/lib/libGLEW.a ${CMAKE_CURRENT_LIST_DIR}/deps/lib/libglfw.3.dylib)

