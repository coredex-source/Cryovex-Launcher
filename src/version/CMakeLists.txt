# Version module - Minecraft version manifest fetching
add_library(CryovexVersion STATIC
    VersionManager.cpp
    VersionManager.h
    MinecraftVersion.cpp
    MinecraftVersion.h
)

target_link_libraries(CryovexVersion
    Qt6::Core
    Qt6::Network
)

target_link_libraries(CryovexLauncher CryovexVersion)