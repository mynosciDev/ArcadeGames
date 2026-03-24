cmake_minimum_required(VERSION 4.1)
project(BreakoutInCpp)

set(CMAKE_CXX_STANDARD 20)
set(SFML_DIR "sfml/lib/cmake/SFML")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out)

add_executable(BreakoutInCpp main.cpp)



find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)

target_link_libraries(BreakoutInCpp
        sfml-graphics
        sfml-window
        sfml-system
)