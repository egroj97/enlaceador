add_executable(EnlaceadorRegister
    main.cpp
)

target_link_libraries(EnlaceadorRegister PRIVATE Qt6::Core advapi32)
