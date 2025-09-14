# Launcher module - JVM process manager
add_library(CryovexLauncher_Core STATIC
    GameLauncher.cpp
    GameLauncher.h
    JvmArgumentBuilder.cpp
    JvmArgumentBuilder.h
)

target_link_libraries(CryovexLauncher_Core
    Qt6::Core
)

target_link_libraries(CryovexLauncher CryovexLauncher_Core)