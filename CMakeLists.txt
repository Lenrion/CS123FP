cmake_minimum_required(VERSION 3.16)

# Sets project name
project(CS123FP LANGUAGES CXX C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Sets C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specifies required Qt components
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Gui)
find_package(Qt6 REQUIRED COMPONENTS OpenGL)
find_package(Qt6 REQUIRED COMPONENTS OpenGLWidgets)
find_package(Qt6 REQUIRED COMPONENTS Xml)

# Allows you to include files from within those directories, without prefixing their filepaths
include_directories(src)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/camera.cpp
        src/camera.h
        src/debugger.h

        src/mainwindow.cpp
        src/mainwindow.h
        src/realtime.cpp
        src/realtime.h
        src/settings.cpp
        src/settings.h
        src/utils/scenedata.h
        src/utils/scenefilereader.cpp
        src/utils/scenefilereader.h
        src/utils/sceneparser.cpp
        src/utils/sceneparser.h
        src/utils/shaderloader.h
        src/utils/aspectratiowidget/aspectratiowidget.hpp
        src/shapes/Cone.cpp
        src/shapes/Cube.cpp
        src/shapes/Water.cpp
        src/shapes/Cylinder.cpp
        src/shapes/Sphere.cpp
        src/shapes/Cone.h
        src/shapes/Cube.h
        src/shapes/Cylinder.h
        src/shapes/Water.h
        src/shapes/Sphere.h
        src/terraingenerator.h src/terraingenerator.cpp
        src/shapes/skybox.h src/shapes/skybox.cpp
        src/stb_image.h




)

# GLM: this creates its library and allows you to `#include "glm/..."`
add_subdirectory(glm)

# GLEW: this creates its library and allows you to `#include "GL/glew.h"`
add_library(StaticGLEW STATIC glew/src/glew.c

)
include_directories(${PROJECT_NAME} PRIVATE glew/include)

# Specifies libraries to be linked (Qt components, glew, etc)
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt::Core
    Qt::Gui
    Qt::OpenGL
    Qt::OpenGLWidgets
    Qt::Xml
    StaticGLEW
)

# Specifies other files
qt6_add_resources(${PROJECT_NAME} "Resources"
    PREFIX
        "/"
    FILES
        resources/shaders/default.frag
        resources/shaders/default.vert
        resources/shaders/pixel.vert
        resources/shaders/pixel.frag
        resources/shaders/terrain.frag
        resources/shaders/terrain.vert
        resources/shaders/water.frag
        resources/shaders/water.vert
        resources/shaders/skybox.vert
        resources/shaders/skybox.frag


)

target_sources(CS123FP_other_files
  PRIVATE


)

# GLEW: this provides support for Windows (including 64-bit)
#if (WIN32)
#  add_compile_definitions(GLEW_STATIC)
#  target_link_libraries(${PROJECT_NAME} PRIVATE
#    opengl32
#    glu32
#  )
#endif()

## Set this flag to silence warnings on Windows
#if (MSVC OR MSYS OR MINGW)
#  set(CMAKE_CXX_FLAGS "-Wno-volatile")
#endif()
# Set this flag to silence warnings on MacOS
if (APPLE)
  set(CMAKE_CXX_FLAGS "-Wno-deprecated-volatile")
endif()
