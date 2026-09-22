add_executable(enlaceador WIN32
    main.cpp
    BrowserChooser.hpp
    BrowserChooser.cpp
    BrowserDetector.hpp
    BrowserDetector.cpp
    BrowserButton.hpp
    Logger.hpp
)

target_compile_features(enlaceador PRIVATE cxx_std_17)

target_link_libraries(enlaceador PRIVATE Qt6::Widgets user32)

if(Qt6LinguistTools_FOUND)
    set(TS_FILES
        ${CMAKE_SOURCE_DIR}/translations/en.ts
        ${CMAKE_SOURCE_DIR}/translations/es.ts
    )
    qt_add_translations(enlaceador TS_FILES ${TS_FILES} RESOURCE_PREFIX "/i18n")
endif()

if(MINGW)
    set_target_properties(enlaceador PROPERTIES
        LINK_FLAGS "-mwindows"
    )
endif()

install(TARGETS enlaceador DESTINATION bin)

if(WINDEPLOYQT_EXECUTABLE)
    add_custom_command(TARGET enlaceador POST_BUILD
        COMMAND "${WINDEPLOYQT_EXECUTABLE}" 
            --no-translations
            --no-opengl-sw
            --no-system-d3d-compiler
            "$<TARGET_FILE:enlaceador>"
        COMMAND ${CMAKE_COMMAND} -E remove -f 
            "$<TARGET_FILE_DIR:enlaceador>/Qt6Network.dll"
            "$<TARGET_FILE_DIR:enlaceador>/Qt6Svg.dll"
            "$<TARGET_FILE_DIR:enlaceador>/opengl32sw.dll"
            "$<TARGET_FILE_DIR:enlaceador>/D3Dcompiler_47.dll"
        COMMAND ${CMAKE_COMMAND} -E remove_directory
            "$<TARGET_FILE_DIR:enlaceador>/imageformats"
        COMMAND ${CMAKE_COMMAND} -E remove_directory
            "$<TARGET_FILE_DIR:enlaceador>/networkinformation"
        COMMAND ${CMAKE_COMMAND} -E remove_directory
            "$<TARGET_FILE_DIR:enlaceador>/tls"
    )
endif()
