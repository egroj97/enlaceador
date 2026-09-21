add_executable(enlaceador WIN32
    main.cpp
    BrowserChooser.hpp
    BrowserChooser.cpp
    BrowserDetector.hpp
    BrowserDetector.cpp
    BrowserButton.hpp
)

target_compile_features(enlaceador PRIVATE cxx_std_17)

target_link_libraries(enlaceador PRIVATE Qt6::Widgets)

if(MINGW)
    set_target_properties(enlaceador PROPERTIES
        LINK_FLAGS "-mwindows"
    )
endif()

install(TARGETS enlaceador DESTINATION bin)

if(WINDEPLOYQT_EXECUTABLE)
    add_custom_command(TARGET enlaceador POST_BUILD
        COMMAND "${WINDEPLOYQT_EXECUTABLE}" --no-translations "$<TARGET_FILE:enlaceador>"
    )
endif()
