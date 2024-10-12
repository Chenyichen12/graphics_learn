cmake_minimum_required(VERSION 3.30)
project(t1)

add_subdirectory(glm)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_GENERATOR "Ninja")
set(AUTOMOC ON)
set(AUTORCC ON)
set(AUTOGEN ON)

find_package(Qt6 COMPONENTS Core OpenGLWidgets REQUIRED)

qt_add_executable(t1
    main.cpp
)
target_link_libraries(t1 PRIVATE Qt6::Core Qt6::OpenGLWidgets glm::glm)