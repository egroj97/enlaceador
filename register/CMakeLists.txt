add_executable(enlaceadorRegister
    main.cpp
)

target_link_libraries(enlaceadorRegister PRIVATE Qt6::Core advapi32)
