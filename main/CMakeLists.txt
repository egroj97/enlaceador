add_executable(enlaceador WIN32
    main.cpp
    BrowserChooser.hpp
    BrowserChooser.cpp
    BrowserDetector.hpp
    BrowserDetector.cpp
    BrowserButton.hpp
)

target_link_libraries(enlaceador PRIVATE Qt6::Widgets advapi32)
