add_executable(enlaceadorRegister
    main.cpp
)

target_compile_features(enlaceadorRegister PRIVATE cxx_std_17)

target_link_libraries(enlaceadorRegister PRIVATE Qt6::Core)

if(WIN32)
    target_link_libraries(enlaceadorRegister PRIVATE advapi32)
endif()

if(MINGW)
    set_target_properties(enlaceadorRegister PROPERTIES
        LINK_FLAGS "-mwindows -mconsole"
    )
endif()

install(TARGETS enlaceadorRegister DESTINATION bin)

if(WINDEPLOYQT_EXECUTABLE)
    add_custom_command(TARGET enlaceadorRegister POST_BUILD
        COMMAND "${WINDEPLOYQT_EXECUTABLE}" --no-translations "$<TARGET_FILE:enlaceadorRegister>"
    )
endif()
