add_executable(Enlaceador WIN32
    main.cpp
    BrowserChooser.h
    BrowserChooser.cpp
    BrowserDetector.h
    BrowserDetector.cpp
)

target_link_libraries(Enlaceador PRIVATE Qt6::Widgets advapi32)
