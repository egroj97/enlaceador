cmake_minimum_required(VERSION 3.16)
project(enlaceador VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_PREFIX_PATH AND DEFINED ENV{CMAKE_PREFIX_PATH})
    set(CMAKE_PREFIX_PATH "$ENV{CMAKE_PREFIX_PATH}")
endif()

if(NOT CMAKE_PREFIX_PATH)
    file(GLOB QT_VERSION_CANDIDATES LIST_DIRECTORIES true "C:/Qt/*/mingw_64")
    if(QT_VERSION_CANDIDATES)
        list(SORT QT_VERSION_CANDIDATES COMPARE NATURAL ORDER DESCENDING)
        list(GET QT_VERSION_CANDIDATES 0 QT_DETECTED)
        set(CMAKE_PREFIX_PATH "${QT_DETECTED}" CACHE PATH "Qt6 installation prefix")
    endif()
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
find_package(Qt6 COMPONENTS LinguistTools)

find_program(WINDEPLOYQT_EXECUTABLE
    NAMES windeployqt6 windeployqt
    PATHS "${Qt6_DIR}/bin" "${Qt6CoreTools_DIR}"
    NO_DEFAULT_PATH
)

if(NOT WINDEPLOYQT_EXECUTABLE)
    find_program(WINDEPLOYQT_EXECUTABLE NAMES windeployqt6 windeployqt)
endif()

if(WINDEPLOYQT_EXECUTABLE)
    message(STATUS "Found windeployqt: ${WINDEPLOYQT_EXECUTABLE}")
endif()

if(MINGW)
    add_compile_definitions(
        __USE_MINGW_ANSI_STDIO=1
        _POSIX_C_SOURCE=200809L
        __STDC_FORMAT_MACROS
        QT_NO_SESSIONMANAGER
        QT_NO_PRINTER
        QT_NO_CLIPBOARD
        QT_NO_DRAGANDDROP
        QT_NO_STYLE_ANDROID
        QT_NO_STYLE_FUSION
        QT_NO_STYLE_MAC
    )
    
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -finput-charset=UTF-8 -fexec-charset=UTF-8")
    set(CMAKE_CXX_FLAGS_RELEASE "-Os -DNDEBUG -march=x86-64 -ffunction-sections -fdata-sections -flto")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG -march=x86-64")
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
    
    add_link_options(
        -lpthread
        -Wl,--gc-sections
    )
endif()

add_subdirectory(main)
add_subdirectory(register)
