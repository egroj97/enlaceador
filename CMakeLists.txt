cmake_minimum_required(VERSION 3.16)
project(enlaceador VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(Qt6_DIR "C:\\Qt\\6.11.2\\mingw_64\\lib\\cmake\\Qt6")
find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_subdirectory(main)
add_subdirectory(register)
