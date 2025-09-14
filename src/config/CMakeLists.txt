# Config module - Profiles & settings
add_library(CryovexConfig STATIC
    ConfigManager.cpp
    ConfigManager.h
    Profile.cpp
    Profile.h
)

target_link_libraries(CryovexConfig
    Qt6::Core
)

target_link_libraries(CryovexLauncher CryovexConfig)