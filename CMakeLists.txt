cmake_minimum_required(VERSION 3.0.2)
project(qt_shell)

set(CMAKE_INCLUDE_PATH "")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

set(CMAKE_PREFIX_PATH /usr/local/opt/qt5)
set(CMAKE_PREFIX_PATH E:/Qt/qt5.4.0/5.4/msvc2013_opengl)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(OpenGL REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_BUILD_TYPE Release)

set(SOURCE_FILES
    ObjLoader.h
    ObjLoader.cpp
    ObjModel.h
    window.h
    mainwindow.h
    glwidget.h
    mainwindow.cpp
    window.cpp
    glwidget.cpp
    main.cpp
    scene_loader.h
    scene_loader.cpp
    )

add_executable(qt_rtrac ${SOURCE_FILES})

qt5_use_modules(qt_rtrac Core Gui Widgets OpenGL)

target_link_libraries(qt_rtrac Qt5::Widgets Qt5::OpenGL ${OPENGL_LIBRARIES})

